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
    ClassDB::bind_method(D_METHOD("set_yx", "y", "x"), &ChessCoord::set_yx);
    ClassDB::bind_method(D_METHOD("matches", "c"), &ChessCoord::matches);
    ClassDB::bind_method(D_METHOD("matches_yx", "y", "x"), &ChessCoord::matches_yx);
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

void ChessCoord::set_yx(const int y, const int x)
{
    m_coord.y = y;
    m_coord.x = x;
}

bool ChessCoord::matches(const Ref<ChessCoord> &c)
{
    if (c.is_valid())
        return c->get() == m_coord;
    return false;
}

bool ChessCoord::matches_yx(const int y, const int x)
{
    if (m_coord.y == y)
        if (m_coord.x == x)
            return true;
    return false;
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
    ClassDB::add_property("ChessMove", PropertyInfo(Variant::OBJECT, "p0", PROPERTY_HINT_RESOURCE_TYPE, "ChessCoord"), "set_p0", "get_p0");
    ClassDB::bind_method(D_METHOD("get_p1"), &ChessMove::get_p1);
    ClassDB::bind_method(D_METHOD("set_p1", "c"), &ChessMove::set_p1);
    ClassDB::add_property("ChessMove", PropertyInfo(Variant::OBJECT, "p1", PROPERTY_HINT_RESOURCE_TYPE, "ChessCoord"), "set_p1", "get_p1");
    ClassDB::bind_method(D_METHOD("get_cx"), &ChessMove::get_cx);
    ClassDB::bind_method(D_METHOD("set_y0x0y1x1", "y0", "x0", "y1", "x1"), &ChessMove::set_y0x0y1x1);
    ClassDB::bind_method(D_METHOD("set_cx", "c"), &ChessMove::set_cx);
    ClassDB::bind_method(D_METHOD("matches", "m"), &ChessMove::matches);
    ClassDB::bind_method(D_METHOD("matches_p0p1", "p0", "p1"), &ChessMove::matches_p0p1);
    ClassDB::bind_method(D_METHOD("matches_y0x0p1", "y0", "x0", "p0"), &ChessMove::matches_y0x0p1);
    ClassDB::add_property("ChessMove", PropertyInfo(Variant::OBJECT, "c", PROPERTY_HINT_RESOURCE_TYPE, "ChessCoord"), "set_cx", "get_cx");
    ClassDB::bind_method(D_METHOD("en_passant"), &ChessMove::en_passant);
    ClassDB::bind_method(D_METHOD("promote"), &ChessMove::promote);
    ClassDB::bind_method(D_METHOD("check"), &ChessMove::check);
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

void ChessMove::set_y0x0y1x1(const int y0, const int x0, const int y1, const int x1)
{
    m_move.p0.y = y0;
    m_move.p0.x = x0;
    m_move.p1.y = y1;
    m_move.p1.x = x1;
}

bool ChessMove::matches(const Ref<ChessMove> &m)
{
    if (m.is_valid())
    {
        move_s mm = m->get();
        if (mm.p0 == m_move.p0)
            if (mm.p1 == m_move.p1)
                return true;
    }
    return false;
}

bool ChessMove::matches_p0p1(const Ref<ChessCoord> &p0, const Ref<ChessCoord> &p1)
{
    if (p0.is_valid() && p1.is_valid())
        return (m_move.p0 == p0->get()) && (m_move.p1 == p1->get());
    return false;
}

bool ChessMove::matches_y0x0p1(const int y0, const int x0, const Ref<ChessCoord> &p1)
{
    if (m_move.p0.y == y0)
        if (m_move.p0.x == x0)
            if (p1.is_valid())
                return (m_move.p1 == p1->get());
    return false;
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

bool ChessMove::is_valid()
{
    return m_move.is_valid();
}

ChessPlayer::ChessPlayer()
{
    m_name = "";
    m_skill = 600;
    m_playertype = t_none;
}

ChessPlayer::ChessPlayer(std::shared_ptr<chessplayer> p)
{
    if (p)
    {
        m_name = p->playername();
        m_skill = p->playerskill();
        m_playertype = p->playertype();
    }
}

ChessPlayer::~ChessPlayer()
{
}

void ChessPlayer::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("get_name"), &ChessPlayer::get_name);
    ClassDB::bind_method(D_METHOD("set_name", "s"), &ChessPlayer::set_name);
    ClassDB::add_property("ChessPlayer", PropertyInfo(Variant::STRING, "Name"), "set_name", "get_name");
    ClassDB::bind_method(D_METHOD("get_skill"), &ChessPlayer::get_skill);
    ClassDB::bind_method(D_METHOD("set_skill", "y"), &ChessPlayer::set_skill);
    ClassDB::add_property("ChessPlayer", PropertyInfo(Variant::INT, "Skill"), "set_skill", "get_skill");
    ClassDB::bind_method(D_METHOD("get_playertype"), &ChessPlayer::get_playertype);
    ClassDB::bind_method(D_METHOD("set_playertype", "t"), &ChessPlayer::set_playertype);
    ClassDB::add_property("ChessPlayer", PropertyInfo(Variant::INT, "PlayerType"), "set_playertype", "get_playertype");

    // ChessPlayerType
    BIND_ENUM_CONSTANT(tNone);
    BIND_ENUM_CONSTANT(Human);
    BIND_ENUM_CONSTANT(Computer);
}

void ChessPlayer::set_name(String s)
{
    m_name = std::string(s.ascii().get_data());
}

String ChessPlayer::get_name()
{
    return String(m_name.c_str());
}

void ChessPlayer::set_skill(const int s)
{
    m_skill = s;
}

int ChessPlayer::get_skill()
{
    return m_skill;
}

void ChessPlayer::set_playertype(ChessPlayerType t)
{
    m_playertype = (chessplayertype_e)t;
}

ChessPlayer::ChessPlayerType ChessPlayer::get_playertype()
{
    return (ChessPlayerType)m_playertype;
}

void ChessPlayer::get(std::string &n, int &s, chessplayertype_e &t)
{
    n = m_name;
    s = m_skill;
    t = m_playertype;
}
