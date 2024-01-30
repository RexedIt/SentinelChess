#include "chessgame.h"
#include "chesscache.h"

namespace chess
{
    chessgame::chessgame()
    {
        mp_cb_draw_board = NULL;
        mp_cb_game_end = NULL;
        mp_cb_draw_move = NULL;
        mp_cb_request_promote = NULL;
        m_state = play_e;
        m_user_color = c_white;
        m_win_color = c_none;
        m_level = 4;
        m_trace_level = -1;
        m_thread_running = false;
    }

    void chessgame::new_game(color_e user_color, int level)
    {
        m_user_color = user_color;
        m_level = level;
        m_state = play_e;
        m_board.new_board();
        m_turn.clear();
        draw_board(1, m_board);
    }

    error_e chessgame::load_game(std::string filename)
    {
        try
        {
            std::ifstream is;
            is.open(filename, std::ios::binary | std::ios::in);
            if (!is.is_open())
                return e_loading;
            is.read((char *)&m_state, sizeof(m_state));
            is.read((char *)&m_user_color, sizeof(m_user_color));
            is.read((char *)&m_win_color, sizeof(m_win_color));
            is.read((char *)&m_level, sizeof(m_level));
            is.read((char *)&m_trace_level, sizeof(m_trace_level));
            int num_turns = 0;
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
            draw_board(turnno(), m_board);
            return e_none;
        }
        catch (const std::exception &)
        {
            return e_loading;
        }
    }

    error_e chessgame::save_game(std::string filename)
    {
        try
        {
            std::ofstream os;
            os.open(filename, std::ios::binary | std::ios::out);

            os.write((char *)&m_state, sizeof(m_state));
            os.write((char *)&m_user_color, sizeof(m_user_color));
            os.write((char *)&m_win_color, sizeof(m_win_color));
            os.write((char *)&m_level, sizeof(m_level));
            os.write((char *)&m_trace_level, sizeof(m_trace_level));
            int num_turns = turnno() - 1;
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
        draw_board(turnno(), m_board);
        return ret;
    }

    std::string chessgame::save_xfen()
    {
        return m_board.save_xfen();
    }

    game_state_e chessgame::state()
    {
        return m_state;
    }

    color_e chessgame::user_color()
    {
        return m_user_color;
    }

    color_e chessgame::turn_color()
    {
        return m_board.turn_color();
    }

    color_e chessgame::win_color()
    {
        return m_win_color;
    }

    int chessgame::level()
    {
        return m_level;
    }

    int chessgame::turnno()
    {
        return (int)m_turn.size() + 1;
    }

    void chessgame::set_callbacks(draw_board_callback _draw_board, game_end_callback _game_end, draw_move_callback _draw_move, request_promote_callback _request_promote, thinking_callback _thinking, traces_callback _traces)
    {
        mp_cb_draw_board = _draw_board;
        mp_cb_game_end = _game_end;
        mp_cb_draw_move = _draw_move;
        mp_cb_request_promote = _request_promote;
        m_board.set_callbacks(_thinking, _traces);
    }

    void chessgame::draw_board(int n, chessboard &b)
    {
        if (mp_cb_draw_board)
            (mp_cb_draw_board)(n, b);
    }

    void chessgame::game_over(game_state_e state, color_e win_color)
    {
        m_state = state;
        m_win_color = win_color;
        if (mp_cb_game_end)
            (mp_cb_game_end)(state, win_color);
    }

    void chessgame::draw_move(int n, move_s &m, color_e c)
    {
        if (mp_cb_draw_move)
            (mp_cb_draw_move)(n, m, c);
    }

    void chessgame::draw_turn(int n, chessboard &b, move_s &m, color_e c)
    {
        if (mp_cb_draw_board)
            (mp_cb_draw_board)(n, b);
        if (mp_cb_draw_move)
            (mp_cb_draw_move)(n, m, c);
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

    error_e chessgame::computer_move(color_e col)
    {
        if (m_state != play_e)
            return e_invalid_game_state;
        if (col == m_board.turn_color())
        {
            move_s m = m_board.computer_move(col, m_level);
            m_state = is_game_over(col, m);
            m_turn.push_back(new_turn(m_board, m, col));
            draw_turn(turnno(), m_board, m, col);
            if (m_state != play_e)
                game_over(m_state, m_win_color);
            return e_none;
        }
        return e_out_of_turn;
    }

    error_e chessgame::computer_move_async(color_e col)
    {
        if (computer_moving())
            return e_busy;
        if (m_state != play_e)
            return e_invalid_game_state;
        if (col != m_board.turn_color())
            return e_out_of_turn;
        m_thread_running = true;
        std::thread background(&chessgame::computer_move_background, this, col);
        m_thread_id = background.get_id();
        background.detach();
        return e_none;
    }

    void chessgame::computer_move_background(color_e col)
    {
        move_s m = m_board.computer_move(col, m_level);
        m_state = is_game_over(col, m);
        m_thread_result = new_turn(m_board, m, col);
        m_thread_result.r = (int)m_state;
        m_turn.push_back(m_thread_result);
        m_thread_running = true;
    }

    void chessgame::computer_move_cancel()
    {
        m_board.cancel(true);
    }

    bool chessgame::computer_moving()
    {
        return m_thread_running;
    }

    error_e chessgame::forfeit()
    {
        m_state = forfeit_e;
        m_win_color = other(m_user_color);
        return e_none;
    }

    error_e chessgame::user_move(color_e col, coord_s p0, coord_s p1, piece_e promote)
    {
        if (m_state != play_e)
            return e_invalid_game_state;
        if (col == m_board.turn_color())
        {
            move_s m = m_board.user_move(col, p0, p1, promote);
            if (!m.is_valid())
                return e_invalid_move;
            if (m.promote == request_promote)
            {
                piece_e promote = p_queen;
                if (mp_cb_request_promote)
                    promote = (mp_cb_request_promote)();
                m = m_board.user_move(col, p0, p1, promote);
            }
            m_state = is_game_over(col, m);
            m_turn.push_back(new_turn(m_board, m, col));
            draw_turn(turnno(), m_board, m, col);
            if (m_state != play_e)
                game_over(m_state, m_win_color);
        }
        return e_none;
    }

    error_e chessgame::suggest_move(coord_s p0, coord_s p1, int cx, bool en_passant, piece_e promote)
    {
        move_s m(p0, p1, cx, en_passant);
        if (!m.is_valid())
            return e_invalid_move;
        m.promote = promote;
        return m_board.suggest_move(m);
    }

    error_e chessgame::remove_piece(coord_s p0)
    {
        if (m_board.remove(p0) == e_none)
        {
            draw_board(turnno(), m_board);
            return e_none;
        }
        return e_removing;
    }

    error_e chessgame::add_piece(coord_s p0, chesspiece &p1)
    {
        if (m_board.add(p0, p1) == e_none)
        {
            draw_board(turnno(), m_board);
            return e_none;
        }
        return e_adding;
    }

    error_e chessgame::rewind_game(int move_no)
    {
        int idx = move_no - 2;
        if ((idx < -1) || (idx >= turnno() - 2))
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
        draw_board(turnno(), m_board);
        clear_cache();
        return e_none;
    }

    bool chessgame::check_state(color_e col)
    {
        return m_board.check_state(col);
    }

    std::string chessgame::check_state()
    {
        return m_board.check_state();
    }

}