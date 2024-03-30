#include "chesscomputer.h"

#include <chrono>
#include <thread>

namespace chess
{

    chesscomputer::chesscomputer()
    {
        m_level = 3;
        m_name = "Computer";
        m_playertype = t_computer;
        m_listenertype = cl_computer;
        m_cancel = false;
        m_thread_running = false;
        m_opening_weight = 0;
        m_opening_in_play = false;
        m_turn_no = 0;
    }

    chesscomputer::chesscomputer(color_e color, std::string name, int32_t skill)
    {
        m_color = color;
        m_level = skill / 500 + 2;
        if (m_level < 1)
            m_level = 1;
        if (m_level > 6)
            m_level = 6;
        m_playertype = t_computer;
        m_listenertype = cl_computer;
        m_name = name;
        m_skill = skill;
        m_cancel = false;
        m_opening_weight = 0;
        m_opening_in_play = false;
        m_turn_no = 0;
        m_thread_running = false;
    }

    chesscomputer::~chesscomputer()
    {
        cancel_execution();
    }

    void chesscomputer::cancel_execution()
    {
        while (m_thread_running)
        {
            m_cancel = true;
            std::this_thread::sleep_for(std::chrono::milliseconds(25));
        }
    }

    void chesscomputer::signal_on_turn(int16_t turn_no, chessmove m, bool check, chessboard &board, color_e color, game_state_e game_state, color_e win_color, int32_t wt, int32_t bt)
    {
        // This is where we will determine game state, move, or forfeit.
        // move:
        game_state_e cur_state = mp_game->state();
        if ((!m_thread_running) && (cur_state == play_e) && (color == m_color))
        {
            m_turn_no = turn_no;
            m_board = board;
            m_thread_running = true;
            std::thread background(&chesscomputer::computer_move, this, std::ref(m_board), wt, bt);
            m_thread_id = background.get_id();
            background.detach();
        }
    }

    void chesscomputer::signal_on_state(game_state_e game_state, color_e win_color)
    {
        if (game_state != play_e)
            cancel_execution();
    }

    float chesscomputer::weight(chessboard &board, color_e col)
    {
        const float kc_weight = 0.25f;
        const float bp_weight = 0.5f;
        board_metric_s bm = board.board_metric(col);
        int pw = 0;
        for (int i = 0; i < 7; i++)
            pw += bm.pc[i] * piece_default_weights[i] -
                  bm.opc[i] * piece_default_weights[i];
        return ((float)pw) +
               ((float)bm.kc) * kc_weight +
               ((float)bm.bp) * bp_weight;
    }

    error_e chesscomputer::computer_move(chessboard &board, int32_t wt, int32_t bt)
    {
        m_cancel = false;
        int rec = m_level;
        chessmove best;

        std::vector<chessmove> possible = board.possible_moves(m_color);
        initialize_opening();

        // Figure move?
        float maxval = -9999;
        for (size_t i = 0; i < possible.size(); i++)
        {
            if (m_cancel)
            {
                m_thread_running = false;
                return e_interrupted;
            }
            chessboard b(board);
            chessmove candidate = b.execute_move(possible[i]);
            if (candidate.is_valid())
            {
                float score = computer_move_min(b, other(m_color), -9999, 9999, rec - 1);
                score += opening_weight(candidate);
                if (score >= maxval)
                {
                    best = candidate;
                    maxval = score;
                }
            }
            consider(candidate, (int8_t)(i * 100 / possible.size()));
        }
        // We call move on the best outcome as it will not actually move if not valid but evaluate end of game.
        m_thread_running = false; // This is called FIRST so that the callback we get for state change does not loop
        error_e err = move(best);
        return err;
    }

    float chesscomputer::computer_move_max(chessboard &board, color_e turn_col, float _alpha, float beta, int32_t rec)
    {
        if ((rec == 0) || (m_cancel))
            return weight(board, turn_col);
        float alpha = _alpha;
        float maxeval = -9999;
        std::vector<chessmove> possible = board.possible_moves(turn_col);
        for (size_t i = 0; i < possible.size(); i++)
        {
            chessboard b(board);
            // Execute the move
            chessmove candidate = b.execute_move(possible[i]);
            if (candidate.is_valid())
            {
                float score = computer_move_min(b, other(turn_col), alpha, beta, rec - 1);
                if (score > maxeval)
                    maxeval = score;
                if (alpha >= score)
                    alpha = score;
                if (beta < alpha)
                    break;
            }
        }
        return maxeval;
    }

    float chesscomputer::computer_move_min(chessboard &board, color_e turn_col, float alpha, float _beta, int32_t rec)
    {
        if ((rec == 0) || (m_cancel))
            return -1.0f * weight(board, turn_col);
        float beta = _beta;
        float mineval = 9999;
        std::vector<chessmove> possible = board.possible_moves(turn_col);
        for (size_t i = 0; i < possible.size(); i++)
        {
            chessboard b(board);
            // Execute the move
            chessmove candidate = b.execute_move(possible[i]);
            if (candidate.is_valid())
            {
                float score = computer_move_max(b, other(turn_col), alpha, beta, rec - 1);
                if (score < mineval)
                    mineval = score;
                if (beta < score)
                    beta = score;
                if (beta <= alpha)
                    break;
            }
        }
        return mineval;
    }

    void chesscomputer::initialize_opening()
    {
        if (m_opening == "")
        {
            std::vector<std::string> ecos;
            if (get_preferredecos(m_color, ecos) == e_none)
            {
                // Select one from random
                size_t idx = (size_t)get_rand_int(0, (int)(ecos.size() - 1));
                m_opening = ecos[idx];
            }
        }
        if (m_turn_no < 2)
            m_opening_in_play = true;
        m_opening_weight = 32;
        for (int i = 0; i < (m_turn_no + 1) / 2; i++)
        {
            m_opening_weight /= 2;
            if (m_opening_weight == 0)
                return;
        }
        std::string filter = m_opening_in_play ? m_opening : "";
        if (mp_game->next_opening_moves(m_color, filter, m_next_opening_moves) != e_none)
        {
            m_opening_weight = 0;
            return;
        }
        if ((m_opening_in_play) && (m_next_opening_moves.size() == 0))
        {
            m_opening_in_play = false;
            filter = "";
            if (mp_game->next_opening_moves(m_color, filter, m_next_opening_moves) != e_none)
            {
                m_opening_weight = 0;
                return;
            }
        }
    }

    float chesscomputer::opening_weight(chessmove &m)
    {
        if (contains(m_next_opening_moves, m))
            return (float)m_opening_weight;
        return 0.0;
    }
}