#pragma once

#include <thread>

#include "chessplayer.h"
#include "chessgamelistener.h"

namespace chess
{

    class chesscomputer : public chessplayer, public chessgamelistener
    {

    public:
        chesscomputer();
        chesscomputer(color_e color, chessplayerdata data);
        ~chesscomputer();

    protected:
        virtual void signal_on_turn(int16_t, chessmove, bool, chessboard &, color_e, game_state_e, color_e, int32_t, int32_t, std::string);
        virtual void signal_on_state(game_state_e, color_e);
        virtual void stop_listening();
        virtual error_e consider(int8_t pct);

    private:
        float weight(chessboard &board, color_e col);
        error_e computer_move(chessboard &board, int32_t wt, int32_t bt);
        error_e computer_move_calc(chessboard &board, std::vector<chessmove> &possible, int rec, chessmove &best, float &best_score, int &processed, bool watch_clock);
        float computer_move_max(chessboard &board, color_e turn_col, float _alpha, float _beta, int32_t rec);
        float computer_move_min(chessboard &board, color_e turn_col, float _alpha, float _beta, int32_t rec);
        void cancel_execution();
        error_e load_meta(std::string);
        std::string save_meta();
        void initialize_opening();
        float opening_weight(chessmove &m);

        void start_time(int32_t wt, int32_t bt);
        bool out_of_time();
        void pad_time();
        int32_t elapsed();

        int32_t m_level;

        chessboard m_board;
        int m_turn_no;
        std::thread::id m_thread_id;
        bool m_thread_running;
        volatile bool m_cancel;

        // opening
        std::string m_opening;
        std::vector<chessmove> m_next_opening_moves;
        int m_opening_weight;
        bool m_opening_in_play;
        float m_kc_weight;
        float m_bp_weight;
        float m_chaos;
        int m_eco_weight;
        int32_t m_turn_time;
        int32_t m_min_turn;
        int32_t m_max_time;
        int32_t m_min_time;
        int8_t m_last_pct;
        bool m_human_opponent;
        std::vector<std::string> m_eco_favorites;
        std::chrono::steady_clock::time_point m_start_tp;
    };
}
