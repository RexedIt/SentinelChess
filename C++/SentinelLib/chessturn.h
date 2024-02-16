#pragma once

#include "chessboard.h"

namespace chess
{
    typedef struct chessturn_s
    {
        chessboard b;
        move_s m;
        color_e c;
        int16_t r;
        chessturn_s()
        {
            c = c_white;
            r = 0;
        }
        chessturn_s(const chessturn_s &oth);
        chessturn_s(chessboard &_b, move_s &_m, color_e _c, int16_t _r = 0);
        bool load(std::ifstream &);
        bool save(std::ofstream &);
    } chessturn_s;

    chessturn_s new_turn(chessboard &_b, move_s &_m, color_e _c, int16_t _r = 0);

}
