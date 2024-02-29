#pragma once

#include "chessboard.h"

namespace chess
{
    typedef struct chessturn_s
    {
        int16_t t;
        move_s m;
        bool ch;
        chessboard b;
        color_e c;
        game_state_e g;
        color_e wc;
        int32_t wt;
        int32_t bt;
        chessturn_s()
        {
            t = 0;
            c = c_none;
            g = none_e;
            wc = c_none;
            wt = 0;
            bt = 0;
        }
        chessturn_s(const chessturn_s &oth);
        chessturn_s(int16_t _t, move_s &_m, bool _ch, chessboard &_b, color_e _c, game_state_e _g, color_e _wc, int32_t _wt, int32_t _bt);
        error_e load(std::ifstream &);
        error_e save(std::ofstream &);
    } chessturn_s;

    chessturn_s new_turn(int16_t _t, move_s &_m, bool _ch, chessboard &_b, color_e _c, game_state_e _g, color_e _wc, int32_t _wt, int32_t _bt);

}
