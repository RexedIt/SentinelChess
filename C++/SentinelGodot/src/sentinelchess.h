#ifndef GDSENTINELCHESS_H
#define GDSENTINELCHESS_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/variant/typed_array.hpp>

#include "chesslobby.h"
#include "chessgame.h"
#include "chessgamelistener.h"

#include "sentinelcommon.h"

using namespace chess;
using namespace godot;

class ChessEvent;

class SentinelChess : public Node
{
    GDCLASS(SentinelChess, Node)

public:
    enum ChessColor
    {
        cNone = c_none,
        White = c_white,
        Black = c_black
    };

    enum ChessGameState
    {
        Play = play_e,
        CheckMate = checkmate_e,
        StaleMate = stalemate_e,
        Forfeit = forfeit_e,
        Time = time_e,
        Terminate = terminate_e
    };

    enum ChessPiece
    {
        pNone = p_none,
        Pawn = p_pawn,
        Bishop = p_bishop,
        Knight = p_knight,
        Rook = p_rook,
        Queen = p_queen,
        King = p_king
    };

    SentinelChess();
    ~SentinelChess();

private:
    String errorstr(int num);
    int new_game(const Ref<ChessPlayer> &white, const Ref<ChessPlayer> &black);
    int save_game(String filename);
    int load_game(String filename);
    int load_xfen(String content);
    String save_xfen();
    ChessGameState state();
    ChessColor turn_color();
    ChessColor win_color();
    bool check_state(ChessColor col);

    int forfeit(ChessColor col);
    int move_c(ChessColor col, const Ref<ChessCoord> &p0, const Ref<ChessCoord> &p1, ChessPiece promote);
    int move_m(ChessColor col, const Ref<ChessMove> &m);
    Array possible_moves(ChessColor col);

    int rewind_game(int move_no);
    int remove_piece(const Ref<ChessCoord> &p0);
    int add_piece(const Ref<ChessCoord> &p0, ChessColor col, ChessPiece piece);

    bool hasevent();
    Ref<ChessEvent> popevent();

    Ref<ChessMove> lastmove();
    ChessColor lastcolor();
    int lastturnno();
    int turnno();

    bool computer_moving();
    bool is_local(ChessColor col);
    bool is_local_turn();
    bool is_local_active(ChessColor col);

    Ref<ChessPlayer> player(ChessColor col);
    Array players();
    Array local_players();
    String player_name(ChessColor col);
    Dictionary player_names();
    ChessColor preferred_board_color();

    // Board helpers
    bool cell_interactive(int y, int x);
    ChessColor cell_color(int y, int x);
    ChessPiece cell_piece(int y, int x);
    bool cell_dark(int y, int x);
    bool cell_kill(ChessColor col, int y, int x);

protected:
    static void _bind_methods();

private:
    void refresh_data();
    std::shared_ptr<chessgamelistener_queue> mp_listener;
    chesslobby m_lobby;
    std::shared_ptr<chessgame> mp_game;
};

VARIANT_ENUM_CAST(SentinelChess::ChessColor);
VARIANT_ENUM_CAST(SentinelChess::ChessGameState);
VARIANT_ENUM_CAST(SentinelChess::ChessPiece);

class ChessEvent : public RefCounted
{
    GDCLASS(ChessEvent, RefCounted)

public:
    ChessEvent();
    ChessEvent(chessevent &e);
    ~ChessEvent();

    enum ChessEventType
    {
        ceNone = ce_empty,
        ceRefreshBoard = ce_refresh_board,
        ceConsider = ce_consider,
        ceMove = ce_move,
        ceTurn = ce_turn,
        ceEnd = ce_end,
        ceChat = ce_chat
    };

    ChessEventType event_type();
    int move_no();
    bool check();
    SentinelChess::ChessColor color();
    SentinelChess::ChessColor turn_color();
    SentinelChess::ChessColor win_color();
    SentinelChess::ChessGameState game_state();
    Ref<ChessMove> move();
    int percent();
    String msg();

    chessevent get() { return m_event; }

private:
    chessevent m_event;

protected:
    static void _bind_methods();
};

VARIANT_ENUM_CAST(ChessEvent::ChessEventType);

#endif // GDSENTINELCHESS_H