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
        chesspuzzleplayer(color_e color, std::string name, int32_t skill);
        ~chesspuzzleplayer();

    protected:
        virtual void signal_on_turn(int16_t, chessmove, bool, chessboard &, color_e, game_state_e, color_e, int32_t, int32_t);
        virtual void signal_on_state(game_state_e, color_e) { ; }
        virtual void signal_refresh_board(int16_t, chessboard &) { ; }
        virtual void signal_on_consider(chessmove, color_e, int8_t pct = -1) { ; }
        virtual void signal_chat(std::string, color_e) { ; }
    };
}
