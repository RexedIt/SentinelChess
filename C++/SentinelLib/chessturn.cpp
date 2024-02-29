#include "chessturn.h"

namespace chess
{

    chessturn_s new_turn(int16_t _t, move_s &_m, bool _ch, chessboard &_b, color_e _c, game_state_e _g, color_e _wc, int32_t _wt, int32_t _bt)
    {
        chessturn_s t;
        t.t = _t;
        t.m = _m;
        t.ch = _ch;
        t.b = _b;
        t.c = _c;
        t.g = _g;
        t.wc = _wc;
        t.wt = _wt;
        t.bt = _bt;
        return t;
    }

    chessturn_s::chessturn_s(int16_t _t, move_s &_m, bool _ch, chessboard &_b, color_e _c, game_state_e _g, color_e _wc, int32_t _wt, int32_t _bt)
    {
        t = _t;
        m = _m;
        ch = _ch;
        b = _b;
        c = _c;
        g = _g;
        wc = _wc;
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
        g = oth.g;
        wc = oth.wc;
        wt = oth.wt;
        bt = oth.bt;
    }

    error_e chessturn_s::load(std::ifstream &is)
    {
        error_e err = b.load(is);
        if (err == e_none)
        {
            is.read((char *)&t, sizeof(t));
            is.read((char *)&m, sizeof(m));
            is.read((char *)&ch, sizeof(ch));
            is.read((char *)&c, sizeof(c));
            is.read((char *)&g, sizeof(g));
            is.read((char *)&wc, sizeof(wc));
            is.read((char *)&wt, sizeof(wt));
            is.read((char *)&bt, sizeof(bt));
        }
        return err;
    }

    error_e chessturn_s::save(std::ofstream &os)
    {
        error_e err = b.save(os);
        if (err == e_none)
        {
            os.write((char *)&t, sizeof(t));
            os.write((char *)&m, sizeof(m));
            os.write((char *)&ch, sizeof(ch));
            os.write((char *)&c, sizeof(c));
            os.write((char *)&g, sizeof(g));
            os.write((char *)&wc, sizeof(wc));
            os.write((char *)&wt, sizeof(wt));
            os.write((char *)&bt, sizeof(bt));
        }
        return err;
    }

}