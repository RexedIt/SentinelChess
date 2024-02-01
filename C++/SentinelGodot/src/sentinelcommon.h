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
    ~ChessCoord();

    void set_x(const int x);
    int get_x() const;
    void set_y(const int y);
    int get_y() const;
    coord_s get() { return m_coord; }

private:
    coord_s m_coord;

protected:
    static void _bind_methods();
};

class ChessMove : public Object
{
    GDCLASS(ChessMove, Object)

public:
    ChessMove();
    ChessMove(move_s &c);
    ~ChessMove();

    void set_p0(const Ref<ChessCoord> &c);
    Ref<ChessCoord> get_p0() const;
    void set_p1(const Ref<ChessCoord> &c);
    Ref<ChessCoord> get_p1() const;

    move_s get() { return m_move; }

private:
    Ref<ChessCoord> p0;
    Ref<ChessCoord> p1;
    move_s m_move;

protected:
    static void _bind_methods();
};

#endif // GDSENTINELCOMMON_H