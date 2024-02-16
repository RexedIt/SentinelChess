#pragma once

#include "chessplayer.h"

namespace chess
{
    class chesscomputer : public chessplayer
    {

    public:
        chesscomputer();
        chesscomputer(std::string name, int skill);
        ~chesscomputer();

    protected:
        virtual void signal_refresh_board(int, chessboard &) { ; }
        virtual void signal_on_consider(move_s &, color_e, int pct = -1) { ; }
        virtual void signal_on_move(int, move_s &, color_e) { ; }
        virtual void signal_on_turn(int, bool, chessboard &);
        virtual void signal_on_end(game_state_e, color_e);
        virtual void signal_chat(std::string, color_e) { ; }

    private:
        float weight(chessboard &board, color_e col);
        error_e computer_move(chessboard &board);
        float computer_move_max(chessboard &board, color_e turn_col, float _alpha, float _beta, int rec);
        float computer_move_min(chessboard &board, color_e turn_col, float _alpha, float _beta, int rec);
        void cancel_execution();
        
        int m_level;

        std::thread::id m_thread_id;
        bool m_thread_running;
        volatile bool m_cancel;
    };
}
