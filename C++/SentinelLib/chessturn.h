#pragma once

#include "chessboard.h"
#include "chessmove.h"

#include "nlohmann/json.hpp"
using namespace nlohmann;

namespace chess
{

    class chessturn
    {
    public:
        chessturn();
        chessturn(const chessturn &oth);
        chessturn(int16_t _t, chessmove &_m, bool _ch, chessboard &_b, color_e _c, game_state_e _g, color_e _wc, int32_t _wt, int32_t _bt);

        int16_t t;
        chessmove m;
        bool ch;
        chessboard b;
        color_e c;
        game_state_e g;
        color_e wc;
        int32_t wt;
        int32_t bt;
        std::string cm;

        error_e load(json &turn);
        error_e save(json &turn);
    };

    // related functions not necessary to be static members
    chessturn new_turn(int16_t _t, chessmove &_m, bool _ch, chessboard &_b, color_e _c, game_state_e _g, color_e _wc, int32_t _wt, int32_t _bt);

}
