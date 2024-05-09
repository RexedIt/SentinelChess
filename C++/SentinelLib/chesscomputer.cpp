#include "chesscomputer.h"
#include "chessengine.h"

#include <chrono>
#include <thread>
#include <sstream>
#include <iostream>

#include "nlohmann/json.hpp"
using namespace nlohmann;

namespace chess
{

    chesscomputer::chesscomputer()
    {
        m_level = 3;
        m_data.username = "Computer";
        m_data.ptype = t_computer;
        m_listenertype = cl_computer;
        m_cancel = false;
        m_thread_running = false;
        m_opening_weight = 0;
        m_opening_in_play = false;
        m_turn_no = 0;
        m_kc_weight = 0.25f;
        m_bp_weight = 0.5f;
        m_eco_weight = 32;
        m_chaos = 0.0;
        m_min_turn = 750;
        m_turn_time = 4000;
        m_max_time = 0;
        m_min_time = 0;
        m_last_pct = -1;
        m_human_opponent = false;
    }

    chesscomputer::chesscomputer(color_e color, chessplayerdata data)
    {
        m_color = color;
        m_data.ptype = t_computer;
        m_data = data;
        m_level = m_data.elo / 400 + 1;
        if (m_level < 1)
            m_level = 1;
        if (m_level > 6)
            m_level = 6;
        m_listenertype = cl_computer;
        m_thread_running = false;
        m_cancel = false;
        m_opening_weight = 0;
        m_opening_in_play = false;
        m_turn_no = 0;
        m_kc_weight = 0.25f;
        m_bp_weight = 0.5f;
        m_eco_weight = 32;
        m_chaos = 0.0;
        m_min_turn = 750;
        m_turn_time = 4000;
        m_max_time = 0;
        m_min_time = 0;
        m_last_pct = -1;
        m_human_opponent = false;
        load_meta(data.meta);
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

    void chesscomputer::signal_on_turn(int16_t turn_no, chessmove m, bool check, chessboard &board, color_e color, game_state_e game_state, color_e win_color, int32_t wt, int32_t bt, std::string cmt)
    {
        // This is where we will determine game state, move, or forfeit.
        // move:
        game_state_e cur_state = mp_game->state();
        if ((!m_thread_running) && (cur_state == play_e) && (color == m_color))
        {
            m_human_opponent = human_opponent();
            start_time(wt, bt);
            m_turn_no = turn_no;
            m_board = board;
            m_thread_running = true;
            std::thread background(&chesscomputer::computer_move, this, std::ref(m_board), wt, bt);
            m_thread_id = background.get_id();
            background.detach();
        }
    }

    int32_t chesscomputer::elapsed()
    {
        std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
        std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_start_tp);
        return (int32_t)ms.count();
    }

    void chesscomputer::start_time(int32_t wt, int32_t bt)
    {
        m_min_time = 0;
        m_max_time = 0;
        m_last_pct = -1;
        if (m_human_opponent)
        {
            m_min_time = m_min_turn;
            m_max_time = m_turn_time;
            int32_t game_time = m_color == c_white ? wt : bt;
            game_time -= 500;
            if (game_time < 0)
                game_time = 0;
            if ((game_time > 0) && (game_time < m_max_time))
                m_max_time = game_time;
            if (m_max_time > m_turn_time)
                m_max_time = m_turn_time;
            if (m_min_time > m_max_time)
                m_min_time = m_max_time;
            if (m_max_time > 0)
                m_start_tp = std::chrono::steady_clock::now();
        }
    }

    bool chesscomputer::out_of_time()
    {
        // Do not watch the clock if low level, no human opponent, or no max time calculated
        if ((m_max_time == 0) || (m_level <= 3))
            return false;
        return elapsed() >= m_max_time;
    }

    void chesscomputer::pad_time()
    {
        if (m_min_time > 0)
        {
            while ((elapsed() < m_min_time) && (!m_cancel))
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
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

    error_e chesscomputer::consider(int8_t pct)
    {
        if ((m_max_time == 0) && (pct != m_last_pct))
        {
            m_last_pct = pct;
            return chessplayer::consider(pct);
        }
        int8_t tpct = (int8_t)(elapsed() * 100 / m_max_time);
        if (tpct > 100)
            tpct = 100;
        if (tpct != m_last_pct)
        {
            m_last_pct = tpct;
            return chessplayer::consider(tpct);
        }
        return e_none;
    }

    void chesscomputer::signal_on_state(game_state_e game_state, color_e win_color)
    {
        if (game_state != play_e)
            cancel_execution();
    }

    void chesscomputer::stop_listening()
    {
        if (mp_game)
            mp_game->unlisten(id());
    }

    float chesscomputer::weight(chessboard &board, color_e col)
    {
        board_metric_s bm = board.board_metric(col);
        int pw = 0;
        for (int i = 0; i < 7; i++)
            pw += bm.pc[i] * piece_default_weights[i] -
                  bm.opc[i] * piece_default_weights[i];
        return ((float)pw) +
               ((float)bm.kc) * m_kc_weight +
               ((float)bm.bp) * m_bp_weight;
    }

    error_e chesscomputer::computer_move(chessboard &board, int32_t wt, int32_t bt)
    {
        m_cancel = false;
        int rec = m_level;
        m_percent = -1;

        chessmove best;
        float best_score = 0.0;

        std::vector<chessmove> possible = board.possible_moves(m_color);
        int processed = 0;

        initialize_opening();

        // Throttle back minmax if following an opening.
        if (m_opening_in_play)
            rec = 3;

        error_e err = computer_move_calc(board, possible, rec, best, best_score, processed, true);
        // If our difficulty level is > 3 and we ran out of time, let's alternate.
        if ((err == e_out_of_time) && (rec > 3))
        {
            chessmove second;
            float second_score = 0.0;
            err = computer_move_calc(board, possible, 3, second, second_score, processed, false);
            if (second_score > best_score)
            {
                // We found a better alternate than found so far in main loop
                best = second;
                // Make an adjustment back if our percent processed was less than 50% complete.
                if (processed < possible.size() / 2)
                {
                    m_level--;
                    if (m_level < 3)
                        m_level = 3;
                }
            }
        }

        // We call move on the best outcome as it will not actually move if not valid but evaluate end of game.
        pad_time();
        m_thread_running = false; // This is called FIRST so that the callback we get for state change does not loop
        err = move(best);
        return err;
    }

    error_e chesscomputer::computer_move_calc(chessboard &board, std::vector<chessmove> &possible, int rec, chessmove &best, float &best_score, int &processed, bool watch_clock)
    {
        // Figure move?
        float maxval = -9999;
        processed = 0;
        best_score = maxval;

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
                if (m_chaos > 0.0001)
                    score += get_rand() * m_chaos;
                if (score >= maxval)
                {
                    best = candidate;
                    maxval = score;
                    best_score = score;
                }
            }
            processed++;
            if (watch_clock)
                if (out_of_time())
                    return e_out_of_time;
            consider((int8_t)(processed * 100 / possible.size()));
        }
        return e_none;
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
            std::vector<std::string> ecos = m_eco_favorites;
            if (ecos.size())
            {
                // Select one from random
                size_t idx = (size_t)get_rand_int(0, (int)(ecos.size() - 1));
                m_opening = ecos[idx];
            }
            else
            {
                m_opening = "B21";
            }
        }
        if (m_turn_no < 2)
            m_opening_in_play = true;
        m_opening_weight = m_eco_weight;
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

    error_e chesscomputer::load_meta(std::string m)
    {
        if (m != "")
        {
            try
            {
                std::istringstream ifs(m);
                json jmeta;
                ifs >> jmeta;
                JSON_LOADIF(jmeta, "kc_weight", m_kc_weight);
                JSON_LOADIF(jmeta, "bp_weight", m_bp_weight);
                JSON_LOADIF(jmeta, "eco_weight", m_eco_weight);
                JSON_LOADIF(jmeta, "chaos", m_chaos);
                JSON_LOADIF(jmeta, "turn_time", m_turn_time);
                JSON_LOADIF(jmeta, "min_turn", m_min_turn);
                JSON_LOADIF(jmeta, "eco_favorites", m_eco_favorites);
                return e_none;
            }
            catch (const std::exception &)
            {
                return e_loading;
            }
        }
        return e_none;
    }

    std::string chesscomputer::save_meta()
    {
        try
        {
            json jmeta;
            jmeta["kc_weight"] = m_kc_weight;
            jmeta["bp_weight"] = m_bp_weight;
            jmeta["eco_weight"] = m_eco_weight;
            jmeta["chaos"] = m_chaos;
            jmeta["turn_time"] = m_turn_time;
            jmeta["min_turn"] = m_min_turn;
            jmeta["eco_favorites"] = m_eco_favorites;
            return jmeta.dump();
        }
        catch (const std::exception &)
        {
        }
        return "";
    }

}