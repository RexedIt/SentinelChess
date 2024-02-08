#ifndef GDSENTINELCOMMON_H
#define GDSENTINELCOMMON_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/ref_counted.hpp>

#include "chesscommon.h"

using namespace chess;
using namespace godot;

class ChessCoord : public RefCounted
{
    GDCLASS(ChessCoord, RefCounted)

public:
    ChessCoord(){};
    ChessCoord(coord_s &c);
    ChessCoord(const int y, const int x);
    ~ChessCoord();

    void set_x(const int x);
    int get_x() const;
    void set_y(const int y);
    int get_y() const;
    void set_yx(const int y, const int x);
    bool matches(const Ref<ChessCoord> &c);
    bool matches_yx(const int y, const int x);
    coord_s get() { return m_coord; }

private:
    coord_s m_coord;

protected:
    static void _bind_methods();
};

class ChessMove : public RefCounted
{
    GDCLASS(ChessMove, RefCounted)

public:
    ChessMove();
    ChessMove(move_s &c);
    ~ChessMove();

    void set_p0(const Ref<ChessCoord> &c);
    Ref<ChessCoord> get_p0() const;
    void set_p1(const Ref<ChessCoord> &c);
    Ref<ChessCoord> get_p1() const;
    void set_cx(const int c);
    int get_cx() const;
    void set_y0x0y1x1(const int y0, const int x0, const int y1, const int x1);
    bool matches(const Ref<ChessMove> &m);
    bool matches_p0p1(const Ref<ChessCoord> &p0, const Ref<ChessCoord> &p1);
    bool matches_y0x0p1(const int y0, const int x0, const Ref<ChessCoord> &p1);
    int promote() const;
    bool en_passant() const;
    bool check() const;
    bool mate() const;
    bool is_valid();

    move_s get() { return m_move; }

private:
    Ref<ChessCoord> p0;
    Ref<ChessCoord> p1;

    move_s m_move;

protected:
    static void _bind_methods();
};

#endif // GDSENTINELCOMMON_H