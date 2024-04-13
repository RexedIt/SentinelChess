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
        virtual void signal_refresh_board(int16_t, chessboard &) { ; }
        virtual void signal_on_consider(chessmove, color_e, int8_t pct = -1) { ; }
        virtual void signal_chat(std::string, color_e) { ; }
        virtual void stop_listening();

    private:
        float weight(chessboard &board, color_e col);
        error_e computer_move(chessboard &board, int32_t wt, int32_t bt);
        float computer_move_max(chessboard &board, color_e turn_col, float _alpha, float _beta, int32_t rec);
        float computer_move_min(chessboard &board, color_e turn_col, float _alpha, float _beta, int32_t rec);
        void cancel_execution();

        void initialize_opening();
        float opening_weight(chessmove &m);

        int32_t m_level;
        bool m_half_level;

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
    };
}
