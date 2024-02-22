#pragma once

#include "chessplayer.h"
#include "chessgamelistener.h"

namespace chess
{
    class chesscomputer : public chessplayer, public chessgamelistener
    {

    public:
        chesscomputer();
        chesscomputer(color_e color, std::string name, int32_t skill);
        ~chesscomputer();

    protected:
        virtual void signal_on_turn(int16_t, move_s, bool, chessboard &, color_e);
        virtual void signal_on_end(game_state_e, color_e);
        virtual void signal_refresh_board(int16_t, chessboard &) { ; }
        virtual void signal_on_consider(move_s, color_e, int8_t pct = -1) { ; }
        virtual void signal_chat(std::string, color_e) { ; }

    private:
        float weight(chessboard &board, color_e col);
        error_e computer_move(chessboard &board);
        float computer_move_max(chessboard &board, color_e turn_col, float _alpha, float _beta, int32_t rec);
        float computer_move_min(chessboard &board, color_e turn_col, float _alpha, float _beta, int32_t rec);
        void cancel_execution();

        int32_t m_level;

        chessboard m_board;
        std::thread::id m_thread_id;
        bool m_thread_running;
        volatile bool m_cancel;
    };
}
