#include "sentinelcommon.h"
#include "chessengine.h"
#include "chesslobby.h"

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

ChessMove::ChessMove(chessmove &c)
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
    ClassDB::bind_method(D_METHOD("get_promote"), &ChessMove::get_promote);
    ClassDB::bind_method(D_METHOD("set_promote"), &ChessMove::set_promote);
    ClassDB::add_property("ChessMove", PropertyInfo(Variant::INT, "p"), "set_promote", "get_promote");
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
        chessmove mm = m->get();
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

ChessPiece ChessMove::get_promote() const
{
    return (ChessPiece)m_move.promote;
}

void ChessMove::set_promote(const ChessPiece p)
{
    m_move.promote = (piece_e)p;
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
    m_playercolor = c_none;
}

ChessPlayer::ChessPlayer(std::shared_ptr<chessplayer> p)
{
    if (p)
    {
        m_playerdata = p->playerdata();
        m_playercolor = p->playercolor();
    }
}

ChessPlayer::ChessPlayer(const chessplayerdata &pd)
{
    m_playerdata = pd;
    m_playercolor = c_none;
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
    ClassDB::bind_method(D_METHOD("get_playercolor"), &ChessPlayer::get_playercolor);
    ClassDB::bind_method(D_METHOD("set_playercolor", "t"), &ChessPlayer::set_playercolor);
    ClassDB::add_property("ChessPlayer", PropertyInfo(Variant::INT, "PlayerColor"), "set_playercolor", "get_playercolor");
    ClassDB::bind_method(D_METHOD("guid"), &ChessPlayer::guid);
    ClassDB::bind_method(D_METHOD("fullname"), &ChessPlayer::fullname);
    ClassDB::bind_method(D_METHOD("puzzlepoints"), &ChessPlayer::puzzlepoints);
    ClassDB::bind_method(D_METHOD("get_persistent"), &ChessPlayer::get_persistent);
    ClassDB::bind_method(D_METHOD("set_persistent"), &ChessPlayer::set_persistent);
    ClassDB::add_property("ChessPlayer", PropertyInfo(Variant::BOOL, "Persistent"), "set_persistent", "get_persistent");
    ClassDB::bind_method(D_METHOD("get_avatar"), &ChessPlayer::get_avatar);
    ClassDB::bind_method(D_METHOD("set_avatar", "uuenc"), &ChessPlayer::set_avatar);
    ClassDB::add_property("ChessPlayer", PropertyInfo(Variant::STRING, "Avatar"), "set_avatar", "get_avatar");
    ClassDB::bind_method(D_METHOD("meta"), &ChessPlayer::meta);
    ClassDB::bind_method(D_METHOD("copy"), &ChessPlayer::copy);

    // ChessPlayerType
    BIND_ENUM_CONSTANT(tNone);
    BIND_ENUM_CONSTANT(Human);
    BIND_ENUM_CONSTANT(Computer);
    BIND_ENUM_CONSTANT(Puzzle);
}

void ChessPlayer::set_name(String s)
{
    m_playerdata.username = std::string(s.ascii().get_data());
}

String ChessPlayer::get_name()
{
    return String(m_playerdata.username.c_str());
}

void ChessPlayer::set_skill(const int s)
{
    m_playerdata.elo = s;
}

int ChessPlayer::get_skill()
{
    return m_playerdata.elo;
}

void ChessPlayer::set_playertype(ChessPlayerType t)
{
    m_playerdata.ptype = (chessplayertype_e)t;
}

ChessPlayerType ChessPlayer::get_playertype()
{
    return (ChessPlayerType)m_playerdata.ptype;
}

void ChessPlayer::set_playercolor(ChessColor c)
{
    m_playercolor = (color_e)c;
}

ChessColor ChessPlayer::get_playercolor()
{
    return (ChessColor)m_playercolor;
}

String ChessPlayer::guid()
{
    return String(m_playerdata.guid.c_str());
}

String ChessPlayer::fullname()
{
    return String(m_playerdata.fullname.c_str());
}

int ChessPlayer::puzzlepoints()
{
    return m_playerdata.puzzlepoints;
}

bool ChessPlayer::get_persistent()
{
    return m_playerdata.persistent;
}

void ChessPlayer::set_persistent(const bool b)
{
    m_playerdata.persistent = b;
}

String ChessPlayer::get_avatar()
{
    return String(m_playerdata.avatar.c_str());
}

void ChessPlayer::set_avatar(const String s)
{
    m_playerdata.avatar = s.ascii().get_data();
}

String ChessPlayer::meta()
{
    return String(m_playerdata.meta.c_str());
}

Ref<ChessPlayer> ChessPlayer::copy()
{
    Ref<ChessPlayer> cp(memnew(ChessPlayer(m_playerdata)));
    return cp;
}

int ChessPlayer::refresh()
{
    return chessengine::hub_get_or_register_player(m_playerdata);
}

chessplayerdata ChessPlayer::get()
{
    return m_playerdata;
}

ChessClock::ChessClock()
{
}

ChessClock::ChessClock(chessclock_s &cc)
{
    m_clock = cc;
}

ChessClock::~ChessClock()
{
}

void ChessClock::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("set_ctype", "c"), &ChessClock::set_ctype);
    ClassDB::bind_method(D_METHOD("get_ctype"), &ChessClock::get_ctype);
    ClassDB::add_property("ChessClock", PropertyInfo(Variant::INT, "ClockType"), "set_ctype", "get_ctype");
    ClassDB::bind_method(D_METHOD("set_allowed_white", "c"), &ChessClock::set_allowed_white);
    ClassDB::bind_method(D_METHOD("get_allowed_white"), &ChessClock::get_allowed_white);
    ClassDB::add_property("ChessClock", PropertyInfo(Variant::INT, "AllowedWhite"), "set_allowed_white", "get_allowed_white");
    ClassDB::bind_method(D_METHOD("set_allowed_black", "c"), &ChessClock::set_allowed_black);
    ClassDB::bind_method(D_METHOD("get_allowed_black"), &ChessClock::get_allowed_black);
    ClassDB::add_property("ChessClock", PropertyInfo(Variant::INT, "AllowedBlack"), "set_allowed_black", "get_allowed_black");
    ClassDB::bind_method(D_METHOD("set_add_white", "c"), &ChessClock::set_add_white);
    ClassDB::bind_method(D_METHOD("get_add_white"), &ChessClock::get_add_white);
    ClassDB::add_property("ChessClock", PropertyInfo(Variant::INT, "AddWhite"), "set_add_white", "get_add_white");
    ClassDB::bind_method(D_METHOD("set_add_black", "c"), &ChessClock::set_add_black);
    ClassDB::bind_method(D_METHOD("get_add_black"), &ChessClock::get_add_black);
    ClassDB::add_property("ChessClock", PropertyInfo(Variant::INT, "AddBlack"), "set_add_black", "get_add_black");
    ClassDB::bind_method(D_METHOD("set_remain_white", "c"), &ChessClock::set_remain_white);
    ClassDB::bind_method(D_METHOD("get_remain_white"), &ChessClock::get_remain_white);
    ClassDB::add_property("ChessClock", PropertyInfo(Variant::INT, "RemainWhite"), "set_remain_white", "get_remain_white");
    ClassDB::bind_method(D_METHOD("set_remain_black", "c"), &ChessClock::set_remain_black);
    ClassDB::bind_method(D_METHOD("get_remain_black"), &ChessClock::get_remain_black);
    ClassDB::add_property("ChessClock", PropertyInfo(Variant::INT, "RemainBlack"), "set_remain_black", "get_remain_black");

    // ChessPlayerType
    BIND_ENUM_CONSTANT(ccNone);
    BIND_ENUM_CONSTANT(ccSuddenDeath);
    BIND_ENUM_CONSTANT(ccIncrement);
    BIND_ENUM_CONSTANT(ccBronsteinDelay);
    BIND_ENUM_CONSTANT(ccSimpleDelay);
}

void ChessClock::set_ctype(ChessClockType t)
{
    m_clock.ctype = (chessclock_e)t;
}

ChessClockType ChessClock::get_ctype()
{
    return (ChessClockType)m_clock.ctype;
}

void ChessClock::set_allowed_white(const int ms)
{
    m_clock.allowedms[0] = ms;
}

int ChessClock::get_allowed_white()
{
    return m_clock.allowedms[0];
}

void ChessClock::set_allowed_black(const int ms)
{
    m_clock.allowedms[1] = ms;
}

int ChessClock::get_allowed_black()
{
    return m_clock.allowedms[1];
}

void ChessClock::set_add_white(const int ms)
{
    m_clock.addms[0] = ms;
}

int ChessClock::get_add_white()
{
    return m_clock.addms[0];
}

void ChessClock::set_add_black(const int ms)
{
    m_clock.addms[1] = ms;
}

int ChessClock::get_add_black()
{
    return m_clock.addms[1];
}

void ChessClock::set_remain_white(const int ms)
{
    m_clock.remainms[0] = ms;
}

int ChessClock::get_remain_white()
{
    return m_clock.remainms[0];
}

void ChessClock::set_remain_black(const int ms)
{
    m_clock.remainms[1] = ms;
}

int ChessClock::get_remain_black()
{
    return m_clock.remainms[1];
}

chessclock_s ChessClock::get()
{
    return m_clock;
}

ChessMeta::ChessMeta()
{
    m_puzzle = false;
    m_hints = 0;
    m_turns = 0;
    m_playno = 0;
}

std::string winlosedraw(color_e col, bool puzzle, chesslobby &l)
{
    int32_t win = 0;
    int32_t lose = 0;
    int32_t draw = 0;
    l.potential_points(col, win, lose, draw);
    std::string ret = std::to_string(win) + "/" + std::to_string(lose);
    if (!puzzle)
        ret += "/" + std::to_string(draw);
    return ret;
}

ChessMeta::ChessMeta(std::shared_ptr<chessgame> g, chesslobby &l)
{
    bool puzzle = false;
    if (g != NULL)
    {
        m_title = g->title();
        m_puzzle = g->puzzle();
        m_hints = g->hints();
        m_turns = g->playmax();
        m_playno = g->playno();
        m_eco = g->eco();
        m_open_title = g->open_title();
        puzzle = g->puzzle();
    }
    m_w_points = winlosedraw(c_white, puzzle, l);
    m_b_points = winlosedraw(c_black, puzzle, l);
    std::map<color_e, std::shared_ptr<chessplayer>> pl = l.players();
    if (pl.count(c_white))
        m_white = pl[c_white]->playerdata();
    if (pl.count(c_black))
        m_black = pl[c_black]->playerdata();
}

ChessMeta::~ChessMeta()
{
}

void ChessMeta::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("title"), &ChessMeta::title);
    ClassDB::bind_method(D_METHOD("white"), &ChessMeta::white);
    ClassDB::bind_method(D_METHOD("black"), &ChessMeta::black);
    ClassDB::bind_method(D_METHOD("puzzle"), &ChessMeta::puzzle);
    ClassDB::bind_method(D_METHOD("hints"), &ChessMeta::hints);
    ClassDB::bind_method(D_METHOD("white_points"), &ChessMeta::white_points);
    ClassDB::bind_method(D_METHOD("black_points"), &ChessMeta::black_points);
    ClassDB::bind_method(D_METHOD("turns"), &ChessMeta::turns);
    ClassDB::bind_method(D_METHOD("playno"), &ChessMeta::playno);
    ClassDB::bind_method(D_METHOD("eco"), &ChessMeta::eco);
    ClassDB::bind_method(D_METHOD("open_title"), &ChessMeta::open_title);
}

String ChessMeta::title()
{
    return String(m_title.c_str());
}

Ref<ChessPlayer> ChessMeta::white()
{
    Ref<ChessPlayer> cp(memnew(ChessPlayer(m_white)));
    return cp;
}

Ref<ChessPlayer> ChessMeta::black()
{
    Ref<ChessPlayer> cp(memnew(ChessPlayer(m_black)));
    return cp;
}

bool ChessMeta::puzzle()
{
    return m_puzzle;
}

int ChessMeta::hints()
{
    return m_hints;
}

String ChessMeta::white_points()
{
    return String(m_w_points.c_str());
}

String ChessMeta::black_points()
{
    return String(m_b_points.c_str());
}

int ChessMeta::turns()
{
    return m_turns;
}

int ChessMeta::playno()
{
    return m_playno;
}

String ChessMeta::eco()
{
    return String(m_eco.c_str());
}

String ChessMeta::open_title()
{
    return String(m_open_title.c_str());
}
