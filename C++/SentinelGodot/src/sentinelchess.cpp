#include "sentinelchess.h"

#include "chesspiece.h"

#include <godot_cpp/core/class_db.hpp>
#include <sstream>

using namespace godot;

SentinelChess *instance = NULL;

// Callbacks
void traces(std::string msg)
{
    if (instance)
        instance->emit_signal("trace", instance, String(msg.c_str()));
}

void draw_board(int n, chessboard &b)
{
    if (instance)
        instance->emit_signal("draw_board", instance, n);
}

void draw_move(int n, move_s &m, color_e c)
{
    if (instance)
    {
        Ref<ChessMove> cm(memnew(ChessMove(m)));
        instance->emit_signal("draw_move", instance, n, cm, (SentinelChess::ChessColor)c);
    }
}

void game_over(game_state_e state, color_e win_color)
{
    if (instance)
        instance->emit_signal("game_over", instance, (SentinelChess::ChessGameState)state, (SentinelChess::ChessColor)win_color);
}

void computer_moved(int n, chessturn_s &t)
{
    if (instance)
    {
        Ref<ChessMove> cm(memnew(ChessMove(t.m)));
        // *** NOTE - Handler must draw the board and animate the move ***
        instance->emit_signal("computer_moved", instance, n, cm, (SentinelChess::ChessColor)t.c);
    }
}

piece_e request_promote_piece()
{
    return p_none;
}

void thinking(move_s m, int pct)
{
    if (instance)
    {
        Ref<ChessMove> cm(memnew(ChessMove(m)));
        instance->emit_signal("thinking", instance, cm, pct);
    }
}

void SentinelChess::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("errorstr", "num"), &SentinelChess::errorstr);
    ClassDB::bind_method(D_METHOD("new_game", "user_color", "level"), &SentinelChess::new_game);
    ClassDB::bind_method(D_METHOD("save_game", "filename"), &SentinelChess::save_game);
    ClassDB::bind_method(D_METHOD("load_game", "filename"), &SentinelChess::load_game);
    ClassDB::bind_method(D_METHOD("save_xfen"), &SentinelChess::save_xfen);
    ClassDB::bind_method(D_METHOD("load_xfen", "contents"), &SentinelChess::load_xfen);
    ClassDB::bind_method(D_METHOD("user_color"), &SentinelChess::user_color);
    ClassDB::bind_method(D_METHOD("turn_color"), &SentinelChess::turn_color);
    ClassDB::bind_method(D_METHOD("computer_color"), &SentinelChess::computer_color);
    ClassDB::bind_method(D_METHOD("win_color"), &SentinelChess::win_color);
    ClassDB::bind_method(D_METHOD("state"), &SentinelChess::state);
    ClassDB::bind_method(D_METHOD("computer_move", "col"), &SentinelChess::computer_move);
    ClassDB::bind_method(D_METHOD("computer_move_cancel"), &SentinelChess::computer_move_cancel);
    ClassDB::bind_method(D_METHOD("computer_moving"), &SentinelChess::computer_moving);
    ClassDB::bind_method(D_METHOD("forfeit"), &SentinelChess::forfeit);
    ClassDB::bind_method(D_METHOD("user_move_m", "col", "m"), &SentinelChess::user_move_m);
    ClassDB::bind_method(D_METHOD("user_move_c", "col", "p0", "p1", "promote"), &SentinelChess::user_move_c);
    ClassDB::bind_method(D_METHOD("possible_moves", "col"), &SentinelChess::possible_moves);
    ClassDB::bind_method(D_METHOD("suggest_move", "m"), &SentinelChess::suggest_move);
    ClassDB::bind_method(D_METHOD("rewind_game", "move_no"), &SentinelChess::rewind_game);
    ClassDB::bind_method(D_METHOD("remove_piece", "p0"), &SentinelChess::remove_piece);
    ClassDB::bind_method(D_METHOD("add_piece", "p0", "col", "piece"), &SentinelChess::add_piece);

    ClassDB::bind_method(D_METHOD("cell_color", "y", "x"), &SentinelChess::cell_color);
    ClassDB::bind_method(D_METHOD("cell_piece", "y", "x"), &SentinelChess::cell_piece);
    ClassDB::bind_method(D_METHOD("cell_dark", "y", "x"), &SentinelChess::cell_dark);
    ClassDB::bind_method(D_METHOD("cell_user_kill", "y", "x"), &SentinelChess::cell_user_kill);
    ClassDB::bind_method(D_METHOD("cell_computer_kill", "y", "x"), &SentinelChess::cell_computer_kill);

    ClassDB::bind_method(D_METHOD("lastmove"), &SentinelChess::lastmove);
    ClassDB::bind_method(D_METHOD("turnno"), &SentinelChess::turnno);

    // Colors
    BIND_ENUM_CONSTANT(cNone);
    BIND_ENUM_CONSTANT(White);
    BIND_ENUM_CONSTANT(Black);

    // Game State
    BIND_ENUM_CONSTANT(Play);
    BIND_ENUM_CONSTANT(CheckMate);
    BIND_ENUM_CONSTANT(StaleMate);
    BIND_ENUM_CONSTANT(Forfeit);

    // Pieces
    BIND_ENUM_CONSTANT(pNone);
    BIND_ENUM_CONSTANT(Pawn);
    BIND_ENUM_CONSTANT(Bishop);
    BIND_ENUM_CONSTANT(Knight);
    BIND_ENUM_CONSTANT(Rook);
    BIND_ENUM_CONSTANT(Queen);
    BIND_ENUM_CONSTANT(King);

    // Signals
    ADD_SIGNAL(MethodInfo("trace", PropertyInfo(Variant::OBJECT, "node"), PropertyInfo(Variant::STRING, "msg")));
    ADD_SIGNAL(MethodInfo("game_over", PropertyInfo(Variant::OBJECT, "node"), PropertyInfo(Variant::INT, "game_state"), PropertyInfo(Variant::INT, "win_color")));
    ADD_SIGNAL(MethodInfo("draw_move", PropertyInfo(Variant::OBJECT, "node"), PropertyInfo(Variant::INT, "n"), PropertyInfo(Variant::OBJECT, "m", PROPERTY_HINT_RESOURCE_TYPE, "ChessMove"), PropertyInfo(Variant::INT, "c", PROPERTY_HINT_RESOURCE_TYPE, "ChessColor")));
    ADD_SIGNAL(MethodInfo("computer_moved", PropertyInfo(Variant::OBJECT, "node"), PropertyInfo(Variant::INT, "n"), PropertyInfo(Variant::OBJECT, "m", PROPERTY_HINT_RESOURCE_TYPE, "ChessMove"), PropertyInfo(Variant::INT, "c", PROPERTY_HINT_RESOURCE_TYPE, "ChessColor")));
    ADD_SIGNAL(MethodInfo("draw_board", PropertyInfo(Variant::OBJECT, "node"), PropertyInfo(Variant::INT, "n")));
    ADD_SIGNAL(MethodInfo("thinking", PropertyInfo(Variant::OBJECT, "node"), PropertyInfo(Variant::OBJECT, "m", PROPERTY_HINT_RESOURCE_TYPE, "ChessMove"), PropertyInfo(Variant::INT, "pct")));
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

String SentinelChess::errorstr(int num)
{
    return String(::errorstr((error_e)num).c_str());
}

Ref<ChessMove> SentinelChess::lastmove()
{
    chessturn_s t = m_game.last_turn();
    Ref<ChessMove> cm(memnew(ChessMove(t.m)));
    return cm;
}

int SentinelChess::turnno()
{
    return m_game.turnno();
}

void SentinelChess::new_game(ChessColor user_color, int level)
{
    m_game.new_game((color_e)user_color, level);
}

int SentinelChess::save_game(String filename)
{
    return m_game.save_game(filename.ascii().get_data());
}

int SentinelChess::load_game(String filename)
{
    return m_game.load_game(filename.ascii().get_data());
}

int SentinelChess::load_xfen(String content)
{
    return m_game.load_xfen(content.ascii().get_data());
}

String SentinelChess::save_xfen()
{
    return String(m_game.save_xfen().c_str());
}

SentinelChess::ChessColor SentinelChess::user_color()
{
    return (SentinelChess::ChessColor)m_game.user_color();
}

SentinelChess::ChessColor SentinelChess::turn_color()
{
    return (SentinelChess::ChessColor)m_game.turn_color();
}

SentinelChess::ChessColor SentinelChess::computer_color()
{
    if (m_game.user_color() == c_white)
        return Black;
    else
        return White;
}

SentinelChess::ChessColor SentinelChess::win_color()
{
    return (SentinelChess::ChessColor)m_game.win_color();
}

SentinelChess::ChessGameState SentinelChess::state()
{
    return (SentinelChess::ChessGameState)m_game.state();
}

int SentinelChess::computer_move(ChessColor col)
{
    return m_game.computer_move_async((color_e)col);
}

void SentinelChess::computer_move_cancel()
{
    m_game.computer_move_cancel();
}

bool SentinelChess::computer_moving()
{
    return m_game.computer_moving();
}

int SentinelChess::forfeit()
{
    return m_game.forfeit();
}

int SentinelChess::user_move_c(ChessColor col, const Ref<ChessCoord> &p0, const Ref<ChessCoord> &p1, ChessPiece promote)
{
    if ((p0.is_valid()) && (p1.is_valid()))
        return m_game.user_move((color_e)col, p0->get(), p1->get(), (piece_e)promote);
    else
        return e_invalid_reference;
}

int SentinelChess::user_move_m(ChessColor col, const Ref<ChessMove> &m)
{
    if (m.is_valid())
        return m_game.user_move((color_e)col, m->get());
    else
        return e_invalid_reference;
}

Array SentinelChess::possible_moves(ChessColor col)
{
    Array a;

    std::vector<move_s> sv = m_game.possible_moves((color_e)col);
    for (int i = 0; i < sv.size(); i++)
    {
        Ref<ChessMove> cm(memnew(ChessMove(sv[i])));
        a.push_back(cm);
    }
    return a;
}

int SentinelChess::suggest_move(const Ref<ChessMove> &m)
{
    if (m.is_valid())
        return m_game.suggest_move(m->get());
    else
        return e_invalid_reference;
}

int SentinelChess::rewind_game(int move_no)
{
    return m_game.rewind_game(move_no);
}

int SentinelChess::remove_piece(const Ref<ChessCoord> &p0)
{
    if (p0.is_valid())
        return m_game.remove_piece(p0->get());
    else
        return e_invalid_reference;
}

int SentinelChess::add_piece(const Ref<ChessCoord> &p0, ChessColor col, ChessPiece piece)
{
    if (p0.is_valid())
    {
        chesspiece p((color_e)col, (piece_e)piece);
        return m_game.add_piece(p0->get(), p);
    }
    return e_invalid_reference;
}

// Board helpers
SentinelChess::ChessColor SentinelChess::cell_color(int y, int x)
{
    return (SentinelChess::ChessColor)(m_game.board().get(y, x) & color_mask);
}

SentinelChess::ChessPiece SentinelChess::cell_piece(int y, int x)
{
    return (SentinelChess::ChessPiece)(m_game.board().get(y, x) & piece_mask);
}

bool SentinelChess::cell_dark(int y, int x)
{
    int cell = y * 8 + x + y % 2;
    return (cell % 2 == 0);
}

bool SentinelChess::cell_user_kill(int y, int x)
{
    unsigned char mask_to_use = (m_game.user_color() == c_white) ? white_kill_mask : black_kill_mask;
    return (SentinelChess::ChessColor)(m_game.board().get(y, x) & mask_to_use);
}

bool SentinelChess::cell_computer_kill(int y, int x)
{
    unsigned char mask_to_use = (m_game.user_color() == c_white) ? black_kill_mask : white_kill_mask;
    return (SentinelChess::ChessColor)(m_game.board().get(y, x) & mask_to_use);
}
