#include "chessturn.h"

namespace chess
{

    chessturn::chessturn()
    {
        t = 0;
        c = c_none;
        g = none_e;
        wc = c_none;
        wt = 0;
        bt = 0;
    }

    chessturn::chessturn(int16_t _t, chessmove &_m, bool _ch, chessboard &_b, color_e _c, game_state_e _g, color_e _wc, int32_t _wt, int32_t _bt)
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

    chessturn::chessturn(const chessturn &oth)
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

    error_e chessturn::save(json &turn)
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
        move["kill"] = m.kill;

        turn["move"] = move;

        turn["turn_no"] = t;
        turn["check"] = ch;
        turn["color"] = color_str(c);
        turn["game_state"] = game_state_str(g);
        turn["win_color"] = color_str(wc);
        turn["white_time_ms"] = wt;
        turn["black_time_ms"] = bt;
        turn["comment"] = cm;

        return err;
    }

    error_e chessturn::load(json &turn)
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
        m.kill = JSON_GET(move, "kill", false);

        t = turn["turn_no"];
        ch = turn["check"];
        c = str_color(turn["color"]);
        g = str_game_state(turn["game_state"]);
        wc = str_color(turn["win_color"]);
        wt = turn["white_time_ms"];
        bt = turn["black_time_ms"];
        cm = JSON_GET(turn, "comment", "");
        return err;
    }

    // static type implementations
    chessturn new_turn(int16_t _t, chessmove &_m, bool _ch, chessboard &_b, color_e _c, game_state_e _g, color_e _wc, int32_t _wt, int32_t _bt)
    {
        chessturn t(_t, _m, _ch, _b, _c, _g, _wc, _wt, _bt);
        return t;
    }

}