#include "sentinelcommon.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

// *** Chess Coord
ChessCoord::ChessCoord(coord_s &c)
{
    m_coord = c;
}

ChessCoord::~ChessCoord()
{
}

void ChessCoord::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("get_x"), &ChessCoord::get_x);
    ClassDB::bind_method(D_METHOD("set_x", "x"), &ChessCoord::set_x);
    ClassDB::add_property("ChessCoord", PropertyInfo(Variant::INT, "x"), "set_x", "get_x");
    ClassDB::bind_method(D_METHOD("get_y"), &ChessCoord::get_y);
    ClassDB::bind_method(D_METHOD("set_y", "y"), &ChessCoord::set_y);
    ClassDB::add_property("ChessCoord", PropertyInfo(Variant::INT, "y"), "set_y", "get_y");
}

void ChessCoord::set_x(const int x)
{
    m_coord.x = x;
}

int ChessCoord::get_x() const
{
    return m_coord.x;
}

void ChessCoord::set_y(const int y)
{
    m_coord.y = y;
}

int ChessCoord::get_y() const
{
    return m_coord.y;
}

// *** Chess Move
ChessMove::ChessMove()
{
    p0.reference_ptr(memnew(ChessCoord()));
    p1.reference_ptr(memnew(ChessCoord()));
}

ChessMove::ChessMove(move_s &c)
{
    m_move = c;
    p0.reference_ptr(memnew(ChessCoord(c.p0)));
    p1.reference_ptr(memnew(ChessCoord(c.p1)));
}

ChessMove::~ChessMove()
{
}

void ChessMove::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("get_p0"), &ChessMove::get_p0);
    ClassDB::bind_method(D_METHOD("set_p0", "c"), &ChessMove::set_p0);
    ClassDB::add_property("ChessMove", PropertyInfo(Variant::OBJECT, "c", PROPERTY_HINT_RESOURCE_TYPE, "ChessCoord"), "set_p0", "get_p0");
    ClassDB::bind_method(D_METHOD("get_p1"), &ChessMove::get_p1);
    ClassDB::bind_method(D_METHOD("set_p1", "c"), &ChessMove::set_p1);
    ClassDB::add_property("ChessMove", PropertyInfo(Variant::OBJECT, "c", PROPERTY_HINT_RESOURCE_TYPE, "ChessCoord"), "set_p1", "get_p1");
    ClassDB::bind_method(D_METHOD("get_cx"), &ChessMove::get_cx);
    ClassDB::bind_method(D_METHOD("set_cx", "c"), &ChessMove::set_cx);
    ClassDB::add_property("ChessMove", PropertyInfo(Variant::OBJECT, "c", PROPERTY_HINT_RESOURCE_TYPE, "ChessCoord"), "set_cx", "get_cx");
    ClassDB::bind_method(D_METHOD("promote"), &ChessMove::promote);
    ClassDB::bind_method(D_METHOD("check"), &ChessMove::check);
    ClassDB::bind_method(D_METHOD("mate"), &ChessMove::mate);
    ClassDB::bind_method(D_METHOD("is_valid"), &ChessMove::is_valid);
}

void ChessMove::set_p0(const Ref<ChessCoord> &c)
{
    if (c.is_valid())
    {
        p0 = c;
        m_move.p0 = c->get();
    }
}

Ref<ChessCoord> ChessMove::get_p0() const
{
    return p0;
}

void ChessMove::set_p1(const Ref<ChessCoord> &c)
{
    if (c.is_valid())
    {
        p1 = c;
        m_move.p1 = c->get();
    }
}

Ref<ChessCoord> ChessMove::get_p1() const
{
    return p1;
}

void ChessMove::set_cx(const int c)
{
    m_move.cx = (int8_t)c;
}

int ChessMove::get_cx() const
{
    return m_move.cx;
}

int ChessMove::promote() const
{
    return m_move.promote;
}

bool ChessMove::en_passant() const
{
    return m_move.en_passant;
}

bool ChessMove::check() const
{
    return m_move.check;
}

bool ChessMove::mate() const
{
    return m_move.mate;
}

bool ChessMove::is_valid()
{
    return m_move.is_valid();
}

