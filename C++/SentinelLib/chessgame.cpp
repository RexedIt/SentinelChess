#include "chessgame.h"
#include "chesscomputer.h"

namespace chess
{

    chessgame::chessgame()
    {
        m_state = play_e;
        m_win_color = c_none;
    }

    chessboard chessgame::board()
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        chessboard b(m_board);
        return b;
    }

    void chessgame::board_copy(chessboard &b)
    {
        b.copy(m_board);
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

    game_state_e chessgame::is_game_over(color_e col, move_s &m)
    {
        if (m_state == play_e)
        {
            color_e winner_col = col == c_white ? c_black : c_white;
            if (m.mate)
            {
                if (m.check)
                    m_state = checkmate_e;
                else
                    m_state = stalemate_e;
            }
        }
        return m_state;
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
            if (!m.is_valid())
                return m.error;
            m_state = is_game_over(col, m);
            m_turn.push_back(new_turn(m_board, m, col));
            guard.unlock();
            signal_on_move(m, col);
            signal_on_turn();
            if (m_state != play_e)
                return end_game(m_state, m_win_color);
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
        m_state = play_e;
        m_win_color = c_none;
        m_board.new_board();
        m_turn.clear();
        m_board.trace("cg:new_game");
        signal_refresh_board();
        signal_on_turn();
        return e_none;
    }

    error_e chessgame::load_game(std::ifstream &is)
    {
        try
        {
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
        std::unique_lock<std::mutex> guard(m_mutex);
        m_state = end_state;
        m_win_color = win_color;
        guard.unlock();
        signal_on_end();
        return e_none;
    }

    error_e chessgame::chat(std::string msg, color_e col)
    {
        signal_chat(msg, col);
        return e_none;
    }

    error_e chessgame::consider(move_s &m, color_e c, int8_t p)
    {
        signal_on_consider(m, c, p);
        return e_none;
    }

    void chessgame::signal_refresh_board()
    {
        int16_t n = turnno();
        for (const auto &kv : mp_listeners)
            kv.second->signal_refresh_board(n, m_board);
    }

    void chessgame::signal_on_consider(move_s &m, color_e c, int8_t p)
    {
        for (const auto &kv : mp_listeners)
            kv.second->signal_on_consider(m, c, p);
    }

    void chessgame::signal_on_move(move_s &m, color_e c)
    {
        int16_t n = turnno();
        for (const auto &kv : mp_listeners)
            kv.second->signal_on_move(n, m, c);
    }

    void chessgame::signal_on_turn()
    {
        int16_t n = turnno();
        color_e c = m_board.turn_color();
        bool ch = m_board.check_state(c);
        for (const auto &kv : mp_listeners)
            kv.second->signal_on_turn(n, ch, m_board, c);
    }

    void chessgame::signal_on_end()
    {
        for (const auto &kv : mp_listeners)
            kv.second->signal_on_end(m_state, m_win_color);
    }

    void chessgame::signal_chat(std::string msg, color_e c)
    {
        for (const auto &kv : mp_listeners)
            kv.second->signal_chat(msg, c);
    }

}