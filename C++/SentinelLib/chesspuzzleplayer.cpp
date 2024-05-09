#include "chesspuzzleplayer.h"

#include <chrono>
#include <thread>

namespace chess
{

    chesspuzzleplayer::chesspuzzleplayer()
    {
        m_data.ptype = t_puzzle;
        m_data.username = "Puzzle";
        m_listenertype = cl_computer;
        m_cancel = false;
        m_thread_running = false;
        m_min_time = 1000;
        m_last_pct = -1;
        m_human_opponent = false;
    }

    chesspuzzleplayer::chesspuzzleplayer(color_e color, chessplayerdata data)
    {
        m_color = color;
        m_data = data;
        m_data.ptype = t_puzzle;
        m_listenertype = cl_computer;
        m_cancel = false;
        m_thread_running = false;
        m_min_time = 1000;
        m_last_pct = -1;
        m_human_opponent = false;
    }

    chesspuzzleplayer::~chesspuzzleplayer()
    {
        cancel_execution();
    }

    void chesspuzzleplayer::cancel_execution()
    {
        while (m_thread_running)
        {
            m_cancel = true;
            std::this_thread::sleep_for(std::chrono::milliseconds(25));
        }
    }

    void chesspuzzleplayer::signal_on_turn(int16_t turn_no, chessmove m, bool check, chessboard &board, color_e color, game_state_e game_state, color_e win_color, int32_t wt, int32_t bt, std::string cmt)
    {
        // This is where we will determine game state, move, or forfeit.
        // move:
        game_state_e cur_state = mp_game->state();
        if ((!m_thread_running) && (cur_state == play_e) && (color == m_color))
        {
            m_human_opponent = human_opponent();
            start_time(wt, bt);
            m_board = board;
            m_moves = mp_game->player_moves(m_color);
            m_thread_running = true;
            std::thread background(&chesspuzzleplayer::puzzle_move, this, std::ref(m_board), std::ref(m_moves), turn_no);
            m_thread_id = background.get_id();
            background.detach();
        }
    }

    void chesspuzzleplayer::signal_on_state(game_state_e game_state, color_e win_color)
    {
        // if (game_state != play_e)
        //     cancel_execution();
    }

    void chesspuzzleplayer::stop_listening()
    {
        if (mp_game)
            mp_game->unlisten(id());
    }

    error_e chesspuzzleplayer::puzzle_move(chessboard &board, std::map<int16_t, chessmove> &moves, int16_t turn_no)
    {
        error_e err = e_none;
        if (moves.count(turn_no + 1) == 0)
            err = puzzle_solved(); // out of moves, player wins
        else
        {
            pad_time();
            err = move(moves[turn_no + 1]);
        }
        m_thread_running = false;
        return err;
    }

    error_e chesspuzzleplayer::puzzle_solved()
    {
        std::lock_guard<std::mutex> guard(chessplayer::m_mutex);
        if (mp_game == NULL)
            return e_no_game;
        return mp_game->puzzle_solved(m_color);
    }

    int32_t chesspuzzleplayer::elapsed()
    {
        std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
        std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_start_tp);
        return (int32_t)ms.count();
    }

    void chesspuzzleplayer::start_time(int32_t wt, int32_t bt)
    {
        m_min_time = 0;
        m_last_pct = -1;
        if (m_human_opponent)
        {
            m_min_time = 750;
            int32_t game_time = m_color == c_white ? wt : bt;
            game_time -= 500;
            if (game_time < 0)
                game_time = 0;
            if ((game_time > 0) && (game_time < m_min_time))
                m_min_time = game_time;
            if (m_min_time > 0)
                m_start_tp = std::chrono::steady_clock::now();
        }
    }

    void chesspuzzleplayer::pad_time()
    {
        if (m_min_time > 0)
        {
            while ((elapsed() < m_min_time) && (!m_cancel))
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                int8_t tpct = (int8_t)(elapsed() * 100 / m_min_time);
                if (tpct > 100)
                    tpct = 100;
                if (tpct != m_last_pct)
                {
                    m_last_pct = tpct;
                    chessplayer::consider(tpct);
                }
            }
        }
    }

}