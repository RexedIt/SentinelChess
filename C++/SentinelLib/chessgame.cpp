#include "chessgame.h"
#include "chesscomputer.h"

namespace chess
{

    chessgame::chessgame()
    {
        m_state = none_e;
        m_win_color = c_none;
    }

    chessboard chessgame::board()
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        chessboard b(m_board);
        return b;
    }

    game_state_e chessgame::state()
    {
        return m_state;
    }

    color_e chessgame::turn_color()
    {
        return m_board.turn_color();
    }

    color_e chessgame::win_color()
    {
        return m_win_color;
    }

    chessturn_s chessgame::last_turn()
    {
        if (m_turn.size() == 0)
        {
            chessturn_s m;
            return m;
        }
        return m_turn[m_turn.size() - 1];
    }

    int16_t chessgame::turnno()
    {
        return (int16_t)m_turn.size();
    }

    game_state_e chessgame::is_game_over(color_e col, move_s m)
    {
        game_state_e g = m_state;
        if (g == play_e)
        {
            color_e winner_col = col == c_white ? c_black : c_white;
            if (m.mate)
            {
                if (m.check)
                {
                    g = checkmate_e;
                    m_win_color = winner_col;
                }
                else
                    g = stalemate_e;
            }
        }
        return g;
    }

    void chessgame::clock_remaining(color_e col, int32_t &wt, int32_t &bt)
    {
        // *** NATHANAEL ***
        // This can be the event that gets called 'on turn' that would cause you
        // to return the current remaining times as well as internally call
        // your method to start counting in behalf of the player designated
        // as color_e col (c_white or c_black)
        wt = 0;
        bt = 0;
    }

    error_e chessgame::forfeit(color_e col)
    {
        return end_game(forfeit_e, other(col));
    }

    error_e chessgame::move(color_e col, move_s m0)
    {
        std::unique_lock<std::mutex> guard(m_mutex);
        if (m_state != play_e)
            return e_invalid_game_state;
        if (col == m_board.turn_color())
        {
            move_s m = m_board.attempt_move(col, m0);
            guard.unlock();
            set_state(is_game_over(col, m));
            push_new_turn(m);
        }
        return e_none;
    }

    error_e chessgame::move(color_e col, coord_s p0, coord_s p1, piece_e promote)
    {
        move_s m(p0, p1, promote);
        return move(col, m);
    }

    std::vector<move_s> chessgame::possible_moves(color_e col)
    {
        return m_board.possible_moves(col);
    }

    bool chessgame::check_state(color_e col)
    {
        return m_board.check_state(col);
    }

    std::string chessgame::check_state()
    {
        return m_board.check_state();
    }

    // Lobby or Administrative Functions
    error_e chessgame::remove_piece(coord_s p0)
    {
        if (m_board.remove(p0) == e_none)
        {
            signal_refresh_board();
            return e_none;
        }
        return e_removing;
    }

    error_e chessgame::add_piece(coord_s p0, chesspiece &p1)
    {
        if (m_board.add(p0, p1) == e_none)
        {
            signal_refresh_board();
            return e_none;
        }
        return e_adding;
    }

    error_e chessgame::rewind_game(int move_no)
    {
        int idx = move_no - 1;
        if (idx == turnno())
            return e_none;
        if ((idx < -1) || (idx >= turnno()))
            return e_rewind_failed;
        if (idx >= 0)
        {
            m_board.copy(m_turn[idx].b);
            m_turn.resize(idx + 1);
        }
        else
        {
            m_turn.clear();
            m_board.new_board();
        }
        signal_refresh_board();
        return e_none;
    }

    error_e chessgame::new_game()
    {
        m_win_color = c_none;
        // *** NATHANAEL ***
        // I recommend you act sort of like
        // the board, and just have a method to reset
        // the clock to default settings
        // maybe something like
        // m_clock.new();
        m_board.new_board();
        m_turn.clear();
        set_state(play_e);
        signal_refresh_board();
        signal_on_turn();
        return e_none;
    }

    error_e chessgame::load_game(std::ifstream &is)
    {
        try
        {
            // *** NATHANAEL ***
            // Read in to your class would be done here, for load game.
            // I recommend using something like my chessclock_s structure (common.h)
            // at any rate, your function signature might look like
            // if (m_clock.load(is) != e_none)
            //     return e_loading;
            // note you would immediately begin functioning your clock
            // as per the settings loaded.
            is.read((char *)&m_state, sizeof(m_state));
            is.read((char *)&m_win_color, sizeof(m_win_color));
            int16_t num_turns = 0;
            is.read((char *)&num_turns, sizeof(num_turns));
            m_turn.clear();
            // If there are no turns ... we will just load the board
            chessturn_s t;
            for (int i = 0; i < num_turns; i++)
            {
                if (t.load(is) != e_none)
                {
                    is.close();
                    return e_loading;
                }
                m_turn.push_back(t);
            }
            if (m_board.load(is) != e_none)
                return e_loading;
            is.close();
            set_state(play_e);
            signal_refresh_board();
            signal_on_turn();
            return e_none;
        }
        catch (const std::exception &)
        {
            return e_loading;
        }
    }

    error_e chessgame::save_game(std::ofstream &os)
    {
        try
        {
            // *** NATHANAEL ***
            // Here is where your class would write settings to a save
            // game file, this would be consistent with that of
            // the load function in terms of content strategy.  I recommend
            // utilizing chessclock_s structure but within your class
            // this code might look like
            // if (m_clock.save(os) != e_none)
            // {
            //     os.close();
            //     return e_saving;
            // }
            os.write((char *)&m_state, sizeof(m_state));
            os.write((char *)&m_win_color, sizeof(m_win_color));
            int16_t num_turns = turnno();
            os.write((char *)&num_turns, sizeof(num_turns));
            for (int i = 0; i < num_turns; i++)
            {
                if (m_turn[i].save(os) != e_none)
                {
                    os.close();
                    return e_saving;
                }
            }
            if (m_board.save(os) != e_none)
            {
                os.close();
                return e_saving;
            }
            os.close();
            return e_none;
        }
        catch (const std::exception &)
        {
            return e_saving;
        }
    }

    error_e chessgame::load_xfen(std::string contents)
    {
        error_e ret = m_board.load_xfen(contents);
        signal_refresh_board();
        return ret;
    }

    std::string chessgame::save_xfen()
    {
        return m_board.save_xfen();
    }

    error_e chessgame::listen(std::shared_ptr<chessgamelistener> plistener)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        if (plistener == NULL)
            return e_invalid_listener;
        int id = plistener->id();
        mp_listeners[id] = plistener;
        return e_none;
    }

    error_e chessgame::unlisten(int id)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        if (mp_listeners.count(id))
            mp_listeners.erase(id);
        return e_none;
    }

    error_e chessgame::end_game(game_state_e end_state, color_e win_color)
    {
        // *** NATHANAEL ***
        // You are going to need to be able to call this function
        // with a time_up_e value for end_state, and declare the winner.
        // It is a bit of a conundrum with how to get the reference
        // to the game object for the clock.  I suppose it would be
        // OK to use a traditional pointer with a set_game function
        // Alternatively we could do all of this up above at the lobby level
        // where a shared pointer to the game exists.  A third option
        // that might be cleanest, is to give the clock a pointer to
        // this function as a callback.
        std::unique_lock<std::mutex> guard(m_mutex);
        m_win_color = win_color;
        set_state(end_state);
        guard.unlock();
        signal_on_state();
        return e_none;
    }

    error_e chessgame::chat(std::string msg, color_e col)
    {
        signal_chat(msg, col);
        return e_none;
    }

    error_e chessgame::consider(move_s m, color_e c, int8_t p)
    {
        signal_on_consider(m, c, p);
        return e_none;
    }

    void chessgame::set_state(game_state_e g)
    {
        if (g != m_state)
        {
            m_state = g;
            signal_on_state();
        }
    }

    void chessgame::push_new_turn(move_s m)
    {
        if (m.is_valid())
        {
            int16_t n = turnno() + 1;
            color_e col = turn_color();
            bool ch = check_state(col);
            int32_t wt = 0;
            int32_t bt = 0;
            clock_remaining(col, wt, bt);
            m_turn.push_back(new_turn(n, m, ch, m_board, col, wt, bt));
            signal_on_turn();
        }
    }

    void chessgame::signal_refresh_board()
    {
        int16_t n = turnno();
        for (const auto &kv : mp_listeners)
            kv.second->signal_refresh_board(n, m_board);
    }

    void chessgame::signal_on_consider(move_s m, color_e c, int8_t p)
    {
        for (const auto &kv : mp_listeners)
            kv.second->signal_on_consider(m, c, p);
    }

    void chessgame::signal_on_turn()
    {
        chessturn_s l = last_turn();
        int16_t n = turnno();
        color_e c = turn_color();
        bool ch = check_state(c);
        if (l.t == 0)
            clock_remaining(c, l.wt, l.bt);
        for (const auto &kv : mp_listeners)
            kv.second->signal_on_turn(n, l.m, ch, m_board, c, l.wt, l.bt);
    }

    void chessgame::signal_on_state()
    {
        for (const auto &kv : mp_listeners)
            kv.second->signal_on_state(m_state, m_win_color);
        // *** NATHANAEL ***
        // See above, you will need to react to this event.  later
        // we will add a pause or resume event but for now it's just
        // this
    }

    void chessgame::signal_chat(std::string msg, color_e c)
    {
        for (const auto &kv : mp_listeners)
            kv.second->signal_chat(msg, c);
    }

}