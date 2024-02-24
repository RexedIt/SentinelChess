#include "chessturn.h"

namespace chess
{

    chessturn_s new_turn(int16_t _t, move_s &_m, bool _ch, chessboard &_b, color_e _c, int32_t _wt, int32_t _bt)
    {
        chessturn_s t;
        t.t = _t;
        t.m = _m;
        t.ch = _ch;
        t.c = _c;
        t.wt = _wt;
        t.bt = _bt;
        return t;
    }

    chessturn_s::chessturn_s(int16_t _t, move_s &_m, bool _ch, chessboard &_b, color_e _c, int32_t _wt, int32_t _bt)
    {
        t = _t;
        m = _m;
        ch = _ch;
        c = _c;
        wt = _wt;
        bt = _bt;
    }

    chessturn_s::chessturn_s(const chessturn_s &oth)
    {
        t = oth.t;
        m = oth.m;
        ch = oth.ch;
        b = oth.b;
        c = oth.c;
        wt = oth.wt;
        bt = oth.bt;
    }

    bool chessturn_s::load(std::ifstream &is)
    {
        if (!b.load(is))
            return false;
        is.read((char *)&m, sizeof(m));
        is.read((char *)&c, sizeof(c));
        return true;
    }

    bool chessturn_s::save(std::ofstream &os)
    {
        if (!b.save(os))
            return false;
        os.write((char *)&m, sizeof(m));
        os.write((char *)&c, sizeof(c));
        return true;
    }

}