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

    error_e chessturn_s::save(json &turn)
    {
        auto board = json::object();
        error_e err = b.save(board);
        if (err != e_none)
            return err;
        turn["board"] = board;

        auto move = json::object();
        move["coords"] = move_str(m);
        move["promote"] = piece_str((piece_e)m.promote);
        move["check"] = m.check;
        move["cx"] = m.cx;
        move["en_passant"] = m.en_passant;
        move["mate"] = m.mate;

        turn["move"] = move;

        turn["turn_no"] = t;
        turn["check"] = ch;
        turn["color"] = color_str(c);
        turn["game_state"] = game_state_str(g);
        turn["win_color"] = color_str(wc);
        turn["white_time_ms"] = wt;
        turn["black_time_ms"] = bt;

        return err;
    }

    error_e chessturn_s::load(json &turn)
    {
        if (turn.is_null())
            return e_loading;

        error_e err = b.load(turn["board"]);
        if (err != e_none)
            return err;

        auto move = turn["move"];
        if (move.is_null())
            return e_loading;

        str_move(move["coords"], m);
        m.promote = str_piece(move["promote"]);
        m.check = move["check"];
        m.cx = move["cx"];
        m.en_passant = move["en_passant"];
        m.mate = move["mate"];

        t = turn["turn_no"];
        ch = turn["check"];
        c = str_color(turn["color"]);
        g = str_game_state(turn["game_state"]);
        wc = str_color(turn["win_color"]);
        wt = turn["white_time_ms"];
        bt = turn["black_time_ms"];

        return err;
    }

}