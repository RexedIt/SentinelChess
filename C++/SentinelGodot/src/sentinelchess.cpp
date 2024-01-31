#include "sentinelchess.h"

#include <godot_cpp/core/class_db.hpp>
#include <map>

using namespace godot;

SentinelChess *instance = NULL;

// Callbacks
void draw_board(int n, chessboard &b)
{
}

void draw_move(int n, move_s &m, color_e c)
{
}

void game_over(game_state_e state, color_e win_color)
{
    if (instance)
        instance->emit_signal("game_over", "state", "win_color");
}

void computer_moved(int n, chessturn_s &t)
{
}

piece_e request_promote_piece()
{
    return p_none;
}

void thinking(move_s m, int pct)
{
}

void traces(std::string msg)
{
    if (instance)
        instance->emit_signal("trace", instance, String(msg.c_str()));
}

void SentinelChess::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("new_game", "user_color", "level"), &SentinelChess::new_game);

    // Colors
    BIND_ENUM_CONSTANT(ColorNone);
    BIND_ENUM_CONSTANT(ColorWhite);
    BIND_ENUM_CONSTANT(ColorBlack);

    // Game State
    BIND_ENUM_CONSTANT(GameStatePlay);
    BIND_ENUM_CONSTANT(GameStateCheckMate);
    BIND_ENUM_CONSTANT(GameStateStaleMate);
    BIND_ENUM_CONSTANT(GameStateForfeit);

    // Signals
    ADD_SIGNAL(MethodInfo("trace", PropertyInfo(Variant::STRING, "msg")));
    ADD_SIGNAL(MethodInfo("game_over", PropertyInfo(Variant::INT, "game_state"), PropertyInfo(Variant::INT, "win_color")));
}

SentinelChess::SentinelChess()
{
    // Initialize any variables here.
    instance = this;
    m_game.set_callbacks(
        &draw_board,
        &game_over,
        &computer_moved,
        &draw_move,
        &request_promote_piece,
        &thinking,
        &traces);
}

SentinelChess::~SentinelChess()
{
    // Add your cleanup here.
    instance = NULL;
}

void SentinelChess::new_game(ChessColor user_color, int level)
{
    m_game.new_game((color_e)user_color, level);
}
