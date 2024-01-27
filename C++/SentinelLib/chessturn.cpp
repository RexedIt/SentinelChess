#include "chessturn.h"

namespace chess
{

    chessturn_s new_turn(chessboard &_b, move_s &_m, color_e _c, int _r)
    {
        chessturn_s t;
        t.b = _b;
        t.m = _m;
        t.c = _c;
        t.r = _r;
        return t;
    }

    chessturn_s::chessturn_s(chessboard &_b, move_s &_m, color_e _c, int _r)
    {
        b = _b;
        m = _m;
        c = _c;
        r = _r;
    }

    chessturn_s::chessturn_s(const chessturn_s &oth)
    {
        b = oth.b;
        m = oth.m;
        c = oth.c;
        r = oth.r;
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