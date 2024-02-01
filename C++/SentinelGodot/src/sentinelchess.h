#ifndef GDSENTINELCHESS_H
#define GDSENTINELCHESS_H

#include <godot_cpp/classes/node.hpp>
#include "chessgame.h"

using namespace chess;
using namespace godot;

class SentinelChess : public Node
{
    GDCLASS(SentinelChess, Node)

public:

    enum ChessColor
    {
        ColorNone = c_none,
        ColorWhite = c_white,
        ColorBlack = c_black
    };

    enum ChessGameState
    {
        GameStatePlay = play_e,
        GameStateCheckMate = checkmate_e,
        GameStateStaleMate = stalemate_e,
        GameStateForfeit = forfeit_e
    };

    SentinelChess();
    ~SentinelChess();

private:

    String errorstr(int num);
    void new_game(ChessColor user_color, int level);
    int save_game(String filename);
    int load_game(String filename);
    ChessGameState state();
    ChessColor turn_color();

    chessgame m_game;

protected:
    static void _bind_methods();
};

VARIANT_ENUM_CAST(SentinelChess::ChessColor);
VARIANT_ENUM_CAST(SentinelChess::ChessGameState);

#endif // GDSENTINELCHESS_H