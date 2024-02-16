#pragma once

#include "chessplayer.h"

namespace chess
{

    class chessconsole : public chessplayerasync
    {

    public:
        chessconsole();
        chessconsole(std::string name, int skill);
        ~chessconsole();

        virtual void signal_refresh_board(int, chessboard &) { ; }
        virtual void signal_on_move(int, move_s &, color_e) { ; }
        virtual void signal_on_turn(int, bool, chessboard &) { ; }
        virtual void signal_on_end(game_state_e, color_e) { ; }
        virtual void signal_chat(std::string, color_e) { ; }
    };

}