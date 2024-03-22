#include "chessgame.h"
#include "chesscomputer.h"

namespace chess
{

    chessgame::chessgame()
    {
        m_state = none_e;
        m_win_color = c_none;
        m_play_pos = -1;
        m_puzzle = false;
        m_hints = 0;
        m_points = 0;
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

    chessturn chessgame::play_turn()
    {
        if ((m_turn.size() == 0) || (m_play_pos < 0))
        {
            chessturn m;
            m.t = -1;
            m.b.load_xfen(m_init_board);
            m.c = m.b.turn_color();
            m.ch = m.b.check_state(m.c);
            m.g = play_e;
            return m;
        }
        return m_turn[m_play_pos];
    }

    int16_t chessgame::playmax()
    {
        return (int16_t)m_turn.size();
    }

    int16_t chessgame::playno()
    {
        return (int16_t)m_play_pos + 1;
    }

    game_state_e chessgame::is_game_over(color_e col, chessmove m)
    {
        game_state_e g = m_state;
        if (g == play_e)
        {
            // Defensive Checkmate
            color_e winner_col = other(col);
            if (!m.is_valid())
            {
                // Player was unable to obtain a move
                // or an erroneous move was set.  Let's check
                m.mate = true;
                m.check = check_state(col);
                if (!m.check)
                    m.mate = false;
            }
            // Offensive Checkmate
            if (!m.mate)
            {
                color_e opponent = other(col);
                m.mate = true;
                m.check = check_state(opponent);
                if (!m.check)
                    m.mate = false;
                // Check all moves
                std::vector<chessmove> pm = possible_moves(opponent);
                for (size_t i = 0; i < pm.size(); i++)
                {
                    chessboard b(m_board);
                    chessmove m0 = b.attempt_move(opponent, pm[i]);
                    if (m0.is_valid())
                    {
                        m.mate = false;
                        break;
                    }
                }
                if (m.mate)
                    winner_col = col;
            }
            // Conclusion
            if (m.mate)
            {
                if (m.check)
                {
                    g = checkmate_e;
                    m_win_color = winner_col;
                }
                else
                    g = draw_stalemate_e;
            }
            // Fivefold Repetition
            if (prev_position_count() >= 4)
            {
                g = draw_fivefold_e;
                m_win_color = c_none;
            }
            // Puzzle mode
            if (m.error == e_incorrect_move)
            {
                if (m_hints <= 0)
                {
                    m_win_color = other(col);
                    g = puzzle_solution_e;
                }
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

    error_e chessgame::puzzle_solved(color_e col)
    {
        return end_game(puzzle_solution_e, other(col));
    }

    error_e chessgame::move(color_e col, chessmove m0)
    {
        std::unique_lock<std::mutex> guard(m_mutex);
        if (m_state != play_e)
        {
            guard.unlock();
            return e_invalid_game_state;
        }
        if (col == m_board.turn_color())
        {
            chessmove m;
            if (m_puzzle)
            {
                m = m0;
                std::map<int16_t, chessmove> moves = player_moves(col);
                if (moves.count(m_play_pos + 1) == 0)
                    m.error = e_incorrect_move;
                else
                {
                    chessmove pm = moves[m_play_pos + 1];
                    if ((pm.p0 != m0.p0) || (pm.p1 != m0.p1))
                        m.error = e_incorrect_move;
                }
                if (m.error == e_incorrect_move)
                {
                    // Consume a hint
                    if (m_hints > 0)
                    {
                        m_hints--;
                        m_points /= 2;
                        guard.unlock();
                        return m.error;
                    }
                }
            }
            if (m.error == e_none)
                m = m_board.attempt_move(col, m0);
            guard.unlock();
            set_state(is_game_over(col, m));
            if (m.error == e_none)
                push_new_turn(m);
            return m.error;
        }
        guard.unlock();
        return e_out_of_turn;
    }

    error_e chessgame::move(color_e col, coord_s p0, coord_s p1, piece_e promote)
    {
        chessmove m(p0, p1, promote);
        return move(col, m);
    }

    error_e chessgame::move(color_e col, std::string s)
    {
        if (col != turn_color())
            return e_out_of_turn;
        chessmove m0;
        error_e err = str_move(s, col, m_board, m0);
        if (err == e_none)
            return move(col, m0);
        return err;
    }

    std::vector<chessmove> chessgame::possible_moves(color_e col)
    {
        return m_board.possible_moves(col);
    }

    std::map<int16_t, chessmove> chessgame::player_moves(color_e col)
    {
        // as far as TURN is concerned, the records are whose turn
        // it IS rather than WAS, and the move is 'WAS'
        // So we search for the reverse color.
        std::map<int16_t, chessmove> ret;
        int16_t num_turns = playmax();
        for (int16_t i = 0; i < num_turns; i++)
            if (m_turn[i].c == other(col))
                ret[m_turn[i].t] = m_turn[i].m;
        return ret;
    }

    bool chessgame::puzzle()
    {
        return m_puzzle;
    }

    int chessgame::hints()
    {
        return m_hints;
    }

    int chessgame::points()
    {
        return m_points;
    }

    void chessgame::set_points(const int v)
    {
        m_points = v;
    }

    chessmove chessgame::hint()
    {
        if ((m_hints > 0) && (m_play_pos < playmax()))
        {
            m_hints--;
            m_points /= 2;
            return m_turn[m_play_pos + 1].m;
        }
        chessmove m;
        m.error = e_invalid_move;
        return m;
    }

    std::string chessgame::hintstr()
    {
        if ((m_hints > 0) && (m_play_pos < playmax()))
        {
            m_hints--;
            m_points /= 2;
            return move_str(m_turn[m_play_pos + 1].m);
        }
        return "No Hint Available";
    }

    std::string chessgame::title()
    {
        return m_title;
    }

    void chessgame::set_title(std::string t)
    {
        m_title = t;
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

    void chessgame::set_turn_to(int idx)
    {
        if (idx != m_play_pos)
        {
            bool turn_rw = (m_state == play_e && !m_puzzle);
            if ((idx >= 0) && (m_turn.size() > 0))
            {
                m_board.copy(m_turn[idx].b);
                if (turn_rw)
                    m_turn.resize(idx + 1);
                m_play_pos = idx;
            }
            else
            {
                m_board.load_xfen(m_init_board);
                if (turn_rw)
                    m_turn.clear();
                m_play_pos = -1;
            }
            if (turn_rw)
                refresh_board_positions();
        }
        signal_on_turn();
    }

    error_e chessgame::rewind_game()
    {
        int idx = m_play_pos - 1;
        if (idx < -1)
            return e_rewind_failed;
        set_turn_to(idx);
        return e_none;
    }

    error_e chessgame::advance_game()
    {
        int idx = m_play_pos + 1;
        if (idx >= playmax())
            return e_advance_failed;
        set_turn_to(idx);
        return e_none;
    }

    error_e chessgame::goto_turn(int turn_no)
    {
        int idx = turn_no - 1;
        if ((idx < -1) || (idx >= playmax()))
            return e_advance_failed;
        set_turn_to(idx);
        return e_none;
    }

    error_e chessgame::play_game()
    {
        if (m_state != play_e)
        {
            // accomplishes a trim
            m_win_color = c_none;
            set_state(play_e);
            set_turn_to(m_play_pos);
        }
        return e_none;
    }

    error_e chessgame::pause_game()
    {
        if (m_state != idle_e)
        {
            set_state(idle_e);
        }
        return e_none;
    }

    error_e chessgame::new_game(std::string title, const chessclock_s &clock)
    {
        m_win_color = c_none;
        // *** NATHANAEL ***
        // Your class could take clock and use it for it's
        // data storage and read and write a chessclock_s
        // in the load and save functions.
        // I recommend you act sort of like
        // the board, and just have a method to reset
        // the clock to default settings
        // maybe something like
        // m_clock.new();
        m_init_board = c_open_board;
        m_board.load_xfen(m_init_board);
        m_turn.clear();
        m_play_pos = -1;
        m_title = title;
        refresh_board_positions();
        set_state(play_e, true);
        signal_on_turn();
        return e_none;
    }

    error_e chessgame::load_game(json &jsonf)
    {
        try
        {
            if (jsonf.is_null())
                return e_loading;
            // *** NATHANAEL ***
            // Read in to your class would be done here, for load game.
            // I recommend using something like my chessclock_s structure (common.h)
            // at any rate, your function signature might look like
            // if (m_clock.load(jsonf) != e_none)
            //     return e_loading;
            // note you would immediately begin functioning your clock
            // as per the settings loaded.
            m_state = str_game_state(jsonf["state"]);
            m_win_color = str_color(jsonf["win_color"]);

            auto turns = jsonf["turns"];
            m_turn.clear();

            chessturn t;
            for (auto turn : turns)
            {
                if (t.load(turn) != e_none)
                    return e_loading;
                m_turn.push_back(t);
            }

            JSON_LOAD(jsonf, "init_board", m_init_board, c_open_board);
            JSON_LOAD(jsonf, "puzzle", m_puzzle, false);
            JSON_LOAD(jsonf, "hints", m_hints, 0);
            JSON_LOAD(jsonf, "title", m_title, "");
            JSON_LOAD(jsonf, "points", m_points, 0);

            m_play_pos = jsonf["play_pos"];
            if (m_play_pos < 0)
                m_play_pos = 0;
            if (m_play_pos > playmax() - 1)
                m_play_pos = playmax() - 1;

            auto board = jsonf["board"];
            if (m_board.load(board) != e_none)
                return e_loading;

            refresh_board_positions();
            // when we load any game, if it's state
            // is play we turn to idle (review mode)
            set_state(idle_e, true);
            signal_on_turn();
            return e_none;
        }
        catch (const std::exception &)
        {
            return e_loading;
        }
    }

    error_e chessgame::load_puzzle(chesspuzzle &p)
    {
        chessboard b;
        error_e err = b.load_xfen(p.fen);
        if (err != e_none)
            return err;
        std::vector<std::string> moves = split_string(p.moves, ' ');
        if (moves.size() < 2)
            return e_loading;

        color_e tc = b.turn_color();
        std::vector<chessturn> turns;

        int16_t n = 0;
        chessturn t;

        for (std::string move : moves)
        {
            chessmove m;
            err = str_move(move, tc, b, m);
            if (err != e_none)
                return err;
            m = b.attempt_move(tc, m);
            if (m.error != e_none)
                return err;
            tc = b.turn_color();
            t.t = n++;
            t.b = b;
            t.c = tc;
            t.m = m;
            t.ch = b.check_state(tc);
            t.g = play_e;
            t.wc = c_none;
            turns.push_back(t);
        }

        m_init_board = p.fen;
        m_turn = turns;
        m_play_pos = -1;
        m_board.load_xfen(p.fen);
        m_puzzle = true;
        m_title = p.title();
        m_hints = turns.size() / 4;
        refresh_board_positions();
        set_state(play_e, true);
        signal_on_turn();
        return e_none;
    }

    error_e chessgame::save_game(json &jsonf)
    {
        try
        {
            // *** NATHANAEL ***
            // Here is where your class would write settings to a save
            // game file, this would be consistent with that of
            // the load function in terms of content strategy.  I recommend
            // utilizing chessclock_s structure but within your class
            // this code might look like
            // auto clock = json::object();
            // if (m_clock.save(os) != e_none)
            //     return e_saving;
            // jsonf["clock"] = clock;
            //
            jsonf["state"] = game_state_str(m_state);
            jsonf["win_color"] = color_str(win_color());

            auto turns = json::array();
            int16_t num_turns = playmax();
            for (int i = 0; i < num_turns; i++)
            {
                auto turn = json::object();
                if (m_turn[i].save(turn) != e_none)
                    return e_saving;
                turns.push_back(turn);
            }
            jsonf["turns"] = turns;

            jsonf["init_board"] = m_init_board;
            jsonf["puzzle"] = m_puzzle;
            jsonf["hints"] = m_hints;
            jsonf["title"] = m_title;
            jsonf["play_pos"] = m_play_pos;
            jsonf["points"] = m_points;

            auto board = json::object();
            if (m_board.save(board) != e_none)
                return e_saving;

            jsonf["board"] = board;
            return e_none;
        }
        catch (const std::exception &)
        {
            return e_saving;
        }
    }

    error_e chessgame::load_xfen(std::string contents)
    {
        chessboard b;
        error_e ret = b.load_xfen(contents);
        if (ret != e_none)
            return ret;
        m_board.copy(b);
        m_win_color = c_none;
        // *** NATHANAEL ***
        // in the case of an XFEN paste, we should
        // remove the clock I think since the
        // game state is indeterminate.
        m_turn.clear();
        m_play_pos = -1;
        refresh_board_positions();
        // when we load any game, if it's state
        // is play we turn to idle (review mode)
        set_state(idle_e, true);
        signal_on_turn();
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

    error_e chessgame::consider(chessmove m, color_e c, int8_t p)
    {
        signal_on_consider(m, c, p);
        return e_none;
    }

    void chessgame::set_state(game_state_e g, bool force_notify)
    {
        if ((g != m_state) || (force_notify))
        {
            m_state = g;
            signal_on_state();
        }
    }

    chessturn chessgame::new_turn(chessmove m)
    {
        int16_t n = playmax() + 1;
        if ((m_turn.size() == 0) && (!m.is_valid()))
            n = 0;
        game_state_e g = state();
        color_e c = turn_color();
        bool ch = check_state(c);
        color_e wc = win_color();
        int32_t wt = 0;
        int32_t bt = 0;
        clock_remaining(c, wt, bt);
        return chess::new_turn(n, m, ch, m_board, c, g, wc, wt, bt);
    }

    void chessgame::push_new_turn(chessmove m)
    {
        if (m.is_valid())
        {
            if (!m_puzzle)
            {
                add_board_position();
                m_turn.push_back(new_turn(m));
                m_play_pos = (int)m_turn.size() - 1;
            }
            else
            {
                if (m_play_pos < (int)m_turn.size() - 1)
                    m_play_pos++;
            }
            signal_on_turn();
        }
    }

    void chessgame::refresh_board_positions()
    {
        m_board_positions.clear();
        for (size_t i = 0; i < m_turn.size(); i++)
        {
            uint32_t curpos = m_turn[i].b.hash();
            int c = m_board_positions[curpos];
            m_board_positions[curpos] = c + 1;
        }
    }

    int chessgame::prev_position_count()
    {
        uint32_t curpos = m_board.hash();
        std::map<uint32_t, int>::iterator it = m_board_positions.find(curpos);
        if (it != m_board_positions.end())
            return it->second;
        return 0;
    }

    void chessgame::add_board_position()
    {
        uint32_t curpos = m_board.hash();
        int c = m_board_positions[curpos];
        m_board_positions[curpos] = c + 1;
    }

    void chessgame::signal_refresh_board()
    {
        int16_t n = playno();
        for (const auto &kv : mp_listeners)
            kv.second->signal_refresh_board(n, m_board);
    }

    void chessgame::signal_on_consider(chessmove m, color_e c, int8_t p)
    {
        for (const auto &kv : mp_listeners)
            kv.second->signal_on_consider(m, c, p);
    }

    void chessgame::signal_on_turn()
    {
        chessmove m;
        chessturn l = m_turn.size() > 0 ? play_turn() : new_turn(m);
        for (const auto &kv : mp_listeners)
            kv.second->signal_on_turn(l.t, l.m, l.ch, l.b, l.c, l.g, l.wc, l.wt, l.bt);
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