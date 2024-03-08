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
class ChessBoard;

class SentinelChess : public Node
{
    GDCLASS(SentinelChess, Node)

public:
    SentinelChess();
    ~SentinelChess();

private:
    String errorstr(int num);
    String gamestatestr(ChessGameState state);

    int new_game(const Ref<ChessPlayer> &white, const Ref<ChessPlayer> &black, const Ref<ChessClock> &clock);
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

    int play_game();
    int pause_game();
    int rewind_game();
    int advance_game();
    int goto_turn(int turn_no);

    int remove_piece(const Ref<ChessCoord> &p0);
    int add_piece(const Ref<ChessCoord> &p0, ChessColor col, ChessPiece piece);

    bool hasevent();
    Ref<ChessEvent> popevent();

    Ref<ChessMove> lastmove();
    ChessColor lastcolor();
    int playno();
    int playmax();

    bool computer_moving();
    bool has_local();
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
    Ref<ChessBoard> board();

protected:
    static void _bind_methods();

private:
    void refresh_data();
    std::shared_ptr<chessgamelistener_queue> mp_listener;
    chesslobby m_lobby;
    std::shared_ptr<chessgame> mp_game;
};

class ChessBoard : public RefCounted
{
    GDCLASS(ChessBoard, RefCounted)

public:
    ChessBoard();
    ChessBoard(const chessboard &b);
    ~ChessBoard();

    ChessColor color_(int y, int x);
    ChessPiece piece(int y, int x);
    bool dark(int y, int x);
    bool kill(ChessColor col, int y, int x);
    bool check_state(ChessColor col);
    String save_xfen();
    Array captured_pieces(ChessColor col);
    String captured_pieces_abbr(ChessColor col);

private:
    chessboard m_board;

protected:
    static void _bind_methods();
};

class ChessEvent : public RefCounted
{
    GDCLASS(ChessEvent, RefCounted)

public:
    ChessEvent();
    ChessEvent(chessevent &e);
    ~ChessEvent();

    ChessEventType event_type();
    int turn_no();
    bool check();
    ChessColor color();
    ChessColor turn_color();
    ChessColor win_color();
    ChessGameState game_state();
    Ref<ChessMove> move();
    Ref<ChessBoard> board();
    int white_time();
    int black_time();
    int percent();
    String msg();

    chessevent get() { return m_event; }

private:
    chessevent m_event;

protected:
    static void _bind_methods();
};

#endif // GDSENTINELCHESS_H