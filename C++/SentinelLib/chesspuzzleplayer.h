#pragma once

#include <thread>

#include "chessplayer.h"
#include "chessgamelistener.h"

namespace chess
{

    class chesspuzzleplayer : public chessplayer, public chessgamelistener
    {

    public:
        chesspuzzleplayer();
        chesspuzzleplayer(color_e color, chessplayerdata data);
        ~chesspuzzleplayer();

    protected:
        virtual void signal_on_turn(int16_t, chessmove, bool, chessboard &, color_e, game_state_e, color_e, int32_t, int32_t, std::string);
        virtual void signal_on_state(game_state_e, color_e);
        virtual void stop_listening();

    private:
        error_e puzzle_move(chessboard &board, std::map<int16_t, chessmove> &moves, int16_t turn_no);
        error_e puzzle_solved();
        void cancel_execution();

        void start_time(int32_t wt, int32_t bt);
        void pad_time();
        int32_t elapsed();

        chessboard m_board;
        std::map<int16_t, chessmove> m_moves;
        std::thread::id m_thread_id;
        bool m_thread_running;
        volatile bool m_cancel;
        int32_t m_min_time;
        int8_t m_last_pct;
        bool m_human_opponent;
        std::chrono::steady_clock::time_point m_start_tp;
    };
}
