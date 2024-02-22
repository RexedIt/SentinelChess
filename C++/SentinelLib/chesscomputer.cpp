#include "chesscomputer.h"

#include <chrono>
#include <thread>

namespace chess
{

    chesscomputer::chesscomputer()
    {
        m_level = 3;
        m_cancel = false;
        m_playertype = t_computer;
        m_listenertype = cl_computer;
        m_thread_running = false;
        m_name = "Computer";
    }

    chesscomputer::chesscomputer(color_e color, std::string name, int32_t skill)
    {
        m_color = color;
        m_level = skill / 500 + 2;
        if (m_level < 1)
            m_level = 1;
        if (m_level > 6)
            m_level = 6;
        m_cancel = false;
        m_playertype = t_computer;
        m_listenertype = cl_computer;
        m_thread_running = false;
        m_name = name;
        m_skill = skill;
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

    void chesscomputer::signal_on_turn(int16_t turn_no, move_s m, bool check, chessboard &board, color_e color)
    {
        // This is where we will determine game state, move, or forfeit.
        // move:
        if ((!m_thread_running) && (color == m_color))
        {
            m_board.copy(board);
            m_thread_running = true;
            std::thread background(&chesscomputer::computer_move, this, std::ref(m_board));
            m_thread_id = background.get_id();
            background.detach();
        }
    }

    void chesscomputer::signal_on_end(game_state_e game_state, color_e win_color)
    {
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

    error_e chesscomputer::computer_move(chessboard &board)
    {
        m_cancel = false;
        int rec = m_level;
        move_s best;
        std::vector<move_s> possible = board.possible_moves(m_color);
        // Figure move?
        float alpha = -9999;
        float beta = 9999;
        for (size_t i = 0; i < possible.size(); i++)
        {
            if (m_cancel)
            {
                m_thread_running = false;
                return e_interrupted;
            }
            chessboard b(board);
            move_s candidate = b.execute_move(possible[i]);
            if (candidate.is_valid())
            {
                float score = computer_move_min(b, other(m_color), alpha, beta, rec - 1);
                if (score >= beta)
                {
                    best = candidate;
                    break;
                }
                else if (score > alpha)
                {
                    best = candidate;
                    alpha = score;
                }
            }
            consider(candidate, (int8_t)(i * 100 / possible.size()));
        }
        // We call move on the best outcome as it will not actually move if not valid but evaluate end of game.
        error_e err = move(best);
        m_thread_running = false;
        return err;
    }

    float chesscomputer::computer_move_max(chessboard &board, color_e turn_col, float _alpha, float _beta, int32_t rec)
    {
        if ((rec == 0) || (m_cancel))
            return weight(board, turn_col);
        float alpha = _alpha;
        float beta = _beta;
        std::vector<move_s> possible = board.possible_moves(turn_col);
        for (size_t i = 0; i < possible.size(); i++)
        {
            chessboard b(board);
            // Execute the move
            move_s candidate = b.execute_move(possible[i]);
            if (candidate.is_valid())
            {
                float score = computer_move_min(b, other(turn_col), alpha, beta, rec - 1);
                if (score >= beta)
                    return beta;
                if (score > alpha)
                    alpha = score;
            }
        }
        return alpha;
    }

    float chesscomputer::computer_move_min(chessboard &board, color_e turn_col, float _alpha, float _beta, int32_t rec)
    {
        if ((rec == 0) || (m_cancel))
            return -1.0f * weight(board, turn_col);
        float alpha = _alpha;
        float beta = _beta;
        std::vector<move_s> possible = board.possible_moves(turn_col);
        for (size_t i = 0; i < possible.size(); i++)
        {
            chessboard b(board);
            // Execute the move
            move_s candidate = b.execute_move(possible[i]);
            if (candidate.is_valid())
            {
                float score = computer_move_max(b, other(turn_col), alpha, beta, rec - 1);
                if (score <= alpha)
                    return alpha;
                if (score < beta)
                    beta = score;
            }
        }
        return beta;
    }

}