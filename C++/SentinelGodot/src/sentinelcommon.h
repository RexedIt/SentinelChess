#ifndef GDSENTINELCOMMON_H
#define GDSENTINELCOMMON_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/ref_counted.hpp>

#include "chesscommon.h"
#include "chessgamelistener.h"

using namespace chess;
using namespace godot;

typedef enum ChessColor
{
    cNone = c_none,
    White = c_white,
    Black = c_black
} ChessColor;

typedef enum ChessGameState
{
    Play = play_e,
    CheckMate = checkmate_e,
    StaleMate = stalemate_e,
    Forfeit = forfeit_e,
    TimeUp = time_up_e,
    Terminate = terminate_e
} ChessGameState;

typedef enum ChessPiece
{
    pNone = p_none,
    Pawn = p_pawn,
    Bishop = p_bishop,
    Knight = p_knight,
    Rook = p_rook,
    Queen = p_queen,
    King = p_king
} ChessPiece;

typedef enum ChessPlayerType
{
    tNone = t_none,
    Human = t_human,
    Computer = t_computer
} ChessPlayerType;

typedef enum ChessEventType
{
    ceNone = ce_empty,
    ceRefreshBoard = ce_refresh_board,
    ceConsider = ce_consider,
    ceTurn = ce_turn,
    ceState = ce_state,
    ceChat = ce_chat
} ChessEventType;

typedef enum ChessClockType
{
    ccNone = cc_none,
    ccSuddenDeath = cc_suddendeath,
    ccIncrement = cc_increment,
    ccBronsteinDelay = cc_bronstein_delay,
    ccSimpleDelay = cc_simple_delay
} ChessClockType;

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
    bool is_valid();

    move_s get() { return m_move; }

private:
    Ref<ChessCoord> p0;
    Ref<ChessCoord> p1;

    move_s m_move;

protected:
    static void _bind_methods();
};

class ChessClock : public RefCounted
{
    GDCLASS(ChessClock, RefCounted)

public:
    ChessClock();
    ChessClock(chessclock_s &cc);
    ~ChessClock();

    void set_ctype(ChessClockType);
    ChessClockType get_ctype();
    void set_allowed_white(const int);
    int get_allowed_white();
    void set_allowed_black(const int);
    int get_allowed_black();
    void set_add_white(const int);
    int get_add_white();
    void set_add_black(const int);
    int get_add_black();
    void set_remain_white(const int);
    int get_remain_white();
    void set_remain_black(const int);
    int get_remain_black();

    chessclock_s get();

private:
    chessclock_s m_clock;

protected:
    static void _bind_methods();
};

class ChessPlayer : public RefCounted
{
    GDCLASS(ChessPlayer, RefCounted)

public:
    ChessPlayer();
    ChessPlayer(std::shared_ptr<chessplayer>);
    ~ChessPlayer();

    void set_name(String s);
    String get_name();
    void set_skill(const int s);
    int get_skill();
    void set_playertype(ChessPlayerType t);
    ChessPlayerType get_playertype();
    void get(std::string &, int &, chessplayertype_e &);

private:
    std::string m_name;
    chessplayertype_e m_playertype;
    int m_skill;

protected:
    static void _bind_methods();
};

VARIANT_ENUM_CAST(ChessColor);
VARIANT_ENUM_CAST(ChessGameState);
VARIANT_ENUM_CAST(ChessPiece);
VARIANT_ENUM_CAST(ChessPlayerType);
VARIANT_ENUM_CAST(ChessEventType);
VARIANT_ENUM_CAST(ChessClockType);

#endif // GDSENTINELCOMMON_H