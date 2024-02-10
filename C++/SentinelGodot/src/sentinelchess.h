#ifndef GDSENTINELCHESS_H
#define GDSENTINELCHESS_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/variant/typed_array.hpp>

#include "chessgame.h"

#include "sentinelcommon.h"

using namespace chess;
using namespace godot;

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
        Forfeit = forfeit_e
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
    void new_game(ChessColor user_color, int level);
    int save_game(String filename);
    int load_game(String filename);
    int load_xfen(String content);
    String save_xfen();
    ChessGameState state();
    ChessColor user_color();
    ChessColor turn_color();
    ChessColor computer_color();
    ChessColor win_color();
    bool check_state(ChessColor col);

    // moving
    int computer_move(ChessColor col);
    void computer_move_cancel();
    bool computer_moving();
    int forfeit();

    int user_move_c(ChessColor col, const Ref<ChessCoord> &p0, const Ref<ChessCoord> &p1, ChessPiece promote);
    int user_move_m(ChessColor col, const Ref<ChessMove> &m);
    Array possible_moves(ChessColor col);

    int suggest_move(const Ref<ChessMove> &m);
    int rewind_game(int move_no);
    int remove_piece(const Ref<ChessCoord> &p0);
    int add_piece(const Ref<ChessCoord> &p0, ChessColor col, ChessPiece piece);

    Ref<ChessMove> lastmove();
    ChessColor lastcolor();
    int lastturnno();
    int turnno();

    // Board helpers
    ChessColor cell_color(int y, int x);
    ChessPiece cell_piece(int y, int x);
    bool cell_dark(int y, int x);
    bool cell_user_kill(int y, int x);
    bool cell_computer_kill(int y, int x);

    chessgame m_game;

protected:
    static void _bind_methods();
};

VARIANT_ENUM_CAST(SentinelChess::ChessColor);
VARIANT_ENUM_CAST(SentinelChess::ChessGameState);
VARIANT_ENUM_CAST(SentinelChess::ChessPiece);

#endif // GDSENTINELCHESS_H