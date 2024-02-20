#include "sentinelchess.h"

#include "chesspiece.h"

#include <godot_cpp/core/class_db.hpp>
#include <sstream>

using namespace godot;

SentinelChess::SentinelChess()
{
    mp_listener = std::shared_ptr<chessgamelistener_queue>(new chessgamelistener_queue());
    m_lobby.set_listener(mp_listener);
}

SentinelChess::~SentinelChess()
{
    m_lobby.set_listener(NULL);
    mp_listener = NULL;
    mp_game = m_lobby.game();
}

void SentinelChess::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("errorstr", "num"), &SentinelChess::errorstr);
    ClassDB::bind_method(D_METHOD("new_game", "white", "black"), &SentinelChess::new_game);
    ClassDB::bind_method(D_METHOD("save_game", "filename"), &SentinelChess::save_game);
    ClassDB::bind_method(D_METHOD("load_game", "filename"), &SentinelChess::load_game);
    ClassDB::bind_method(D_METHOD("save_xfen"), &SentinelChess::save_xfen);
    ClassDB::bind_method(D_METHOD("load_xfen", "contents"), &SentinelChess::load_xfen);
    ClassDB::bind_method(D_METHOD("turn_color"), &SentinelChess::turn_color);
    ClassDB::bind_method(D_METHOD("win_color"), &SentinelChess::win_color);
    ClassDB::bind_method(D_METHOD("computer_moving"), &SentinelChess::computer_moving);
    ClassDB::bind_method(D_METHOD("is_local"), &SentinelChess::is_local);

    ClassDB::bind_method(D_METHOD("is_local_turn"), &SentinelChess::is_local_turn);
    ClassDB::bind_method(D_METHOD("is_local_active", "col"), &SentinelChess::is_local_active);
    ClassDB::bind_method(D_METHOD("player", "col"), &SentinelChess::player);
    ClassDB::bind_method(D_METHOD("players"), &SentinelChess::players);
    ClassDB::bind_method(D_METHOD("local_players"), &SentinelChess::local_players);
    ClassDB::bind_method(D_METHOD("player_name", "col"), &SentinelChess::player_name);
    ClassDB::bind_method(D_METHOD("player_names"), &SentinelChess::player_names);
    ClassDB::bind_method(D_METHOD("preferred_board_color"), &SentinelChess::preferred_board_color);

    ClassDB::bind_method(D_METHOD("state"), &SentinelChess::state);
    ClassDB::bind_method(D_METHOD("check_state"), &SentinelChess::check_state);
    ClassDB::bind_method(D_METHOD("forfeit"), &SentinelChess::forfeit);
    ClassDB::bind_method(D_METHOD("move_m", "col", "m"), &SentinelChess::move_m);
    ClassDB::bind_method(D_METHOD("move_c", "col", "p0", "p1", "promote"), &SentinelChess::move_c);
    ClassDB::bind_method(D_METHOD("possible_moves", "col"), &SentinelChess::possible_moves);
    ClassDB::bind_method(D_METHOD("rewind_game", "move_no"), &SentinelChess::rewind_game);
    ClassDB::bind_method(D_METHOD("remove_piece", "p0"), &SentinelChess::remove_piece);
    ClassDB::bind_method(D_METHOD("add_piece", "p0", "col", "piece"), &SentinelChess::add_piece);

    ClassDB::bind_method(D_METHOD("cell_interactive", "y", "x"), &SentinelChess::cell_interactive);
    ClassDB::bind_method(D_METHOD("cell_color", "y", "x"), &SentinelChess::cell_color);
    ClassDB::bind_method(D_METHOD("cell_piece", "y", "x"), &SentinelChess::cell_piece);
    ClassDB::bind_method(D_METHOD("cell_dark", "y", "x"), &SentinelChess::cell_dark);
    ClassDB::bind_method(D_METHOD("cell_kill", "col", "y", "x"), &SentinelChess::cell_kill);

    ClassDB::bind_method(D_METHOD("hasevent"), &SentinelChess::hasevent);
    ClassDB::bind_method(D_METHOD("popevent"), &SentinelChess::popevent);

    ClassDB::bind_method(D_METHOD("lastmove"), &SentinelChess::lastmove);
    ClassDB::bind_method(D_METHOD("lastcolor"), &SentinelChess::lastcolor);
    ClassDB::bind_method(D_METHOD("lastturnno"), &SentinelChess::lastturnno);
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
    BIND_ENUM_CONSTANT(Time);
    BIND_ENUM_CONSTANT(Terminate);

    // Pieces
    BIND_ENUM_CONSTANT(pNone);
    BIND_ENUM_CONSTANT(Pawn);
    BIND_ENUM_CONSTANT(Bishop);
    BIND_ENUM_CONSTANT(Knight);
    BIND_ENUM_CONSTANT(Rook);
    BIND_ENUM_CONSTANT(Queen);
    BIND_ENUM_CONSTANT(King);
}

String SentinelChess::errorstr(int num)
{
    return String(::errorstr((error_e)num).c_str());
}

bool SentinelChess::hasevent()
{
    if (mp_listener)
        return mp_listener->has_event();
    return false;
}

Ref<ChessEvent> SentinelChess::popevent()
{
    chessevent e;
    if (mp_listener)
        e = mp_listener->pop_event();
    Ref<ChessEvent> ce(memnew(ChessEvent(e)));
    return ce;
}

Ref<ChessMove> SentinelChess::lastmove()
{
    chessturn_s t = mp_game->last_turn();
    Ref<ChessMove> cm(memnew(ChessMove(t.m)));
    return cm;
}

SentinelChess::ChessColor SentinelChess::lastcolor()
{
    chessturn_s t = mp_game->last_turn();
    return (ChessColor)t.c;
}

int SentinelChess::lastturnno()
{
    if (turnno() == 0)
        return 0;
    return turnno() - 1;
}

int SentinelChess::turnno()
{
    return mp_game->turnno();
}

void SentinelChess::refresh_data()
{
    mp_game = m_lobby.game();
}

int SentinelChess::new_game(const Ref<ChessPlayer> &white, const Ref<ChessPlayer> &black)
{
    m_lobby.clear_players();
    std::string n;
    int s = 600;
    chessplayertype_e t = t_none;
    error_e err = e_none;
    if (white.is_valid())
    {
        white->get(n, s, t);
        err = m_lobby.add_player(c_white, n, s, t);
        if (err != e_none)
            return err;
    }
    if (black.is_valid())
    {
        black->get(n, s, t);
        err = m_lobby.add_player(c_black, n, s, t);
        if (err != e_none)
            return err;
    }
    err = m_lobby.new_game();
    refresh_data();
    return err;
}

int SentinelChess::save_game(String filename)
{
    return m_lobby.save_game(filename.ascii().get_data());
}

int SentinelChess::load_game(String filename)
{
    error_e err = m_lobby.load_game(filename.ascii().get_data());
    refresh_data();
    return err;
}

int SentinelChess::load_xfen(String content)
{
    return mp_game->load_xfen(content.ascii().get_data());
}

String SentinelChess::save_xfen()
{
    return String(mp_game->save_xfen().c_str());
}

SentinelChess::ChessColor SentinelChess::turn_color()
{
    return (SentinelChess::ChessColor)mp_game->turn_color();
}

SentinelChess::ChessColor SentinelChess::win_color()
{
    return (SentinelChess::ChessColor)mp_game->win_color();
}

bool SentinelChess::computer_moving()
{
    if (state() == play_e)
        return !m_lobby.is_local_turn();
    return false;
}

bool SentinelChess::is_local(ChessColor col)
{
    return m_lobby.is_local((color_e)col);
}

bool SentinelChess::is_local_turn()
{
    return m_lobby.is_local_turn();
}

bool SentinelChess::is_local_active(ChessColor col)
{
    return m_lobby.is_local_active((color_e)col);
}

Ref<ChessPlayer> SentinelChess::player(ChessColor col)
{
    Ref<ChessPlayer> cp(memnew(ChessPlayer(m_lobby.player((color_e)col))));
    return cp;
}

Array SentinelChess::players()
{
    Array a;
    for (const auto &kv : m_lobby.players())
    {
        Ref<ChessPlayer> cp(memnew(ChessPlayer(kv.second)));
        a.push_back(cp);
    }
    return a;
}

Array SentinelChess::local_players()
{
    Array a;
    for (const auto &kv : m_lobby.local_players())
        a.push_back(kv);
    return a;
}

String SentinelChess::player_name(ChessColor col)
{
    return String(m_lobby.player_name((color_e)col).c_str());
}

Dictionary SentinelChess::player_names()
{
    Dictionary d;
    for (const auto &kv : m_lobby.player_names())
    {
        String s(kv.second.c_str());
        d[kv.first] = s;
    }
    return d;
}

SentinelChess::ChessColor SentinelChess::preferred_board_color()
{
    std::set<color_e> locals = m_lobby.local_players();
    // if both players local and white, return it.
    if (locals.count(c_white))
        return (ChessColor)c_white;
    // Okay apparently one of the locals is not white, so return black
    // if that is local
    if (locals.count(c_black))
        return (ChessColor)c_black;
    // Other wise, return white.
    return (ChessColor)c_white;
}

bool SentinelChess::check_state(ChessColor col)
{
    return mp_game->check_state((color_e)col);
}

SentinelChess::ChessGameState SentinelChess::state()
{
    return (SentinelChess::ChessGameState)mp_game->state();
}

int SentinelChess::forfeit(ChessColor col)
{
    return mp_game->forfeit((color_e)col);
}

int SentinelChess::move_c(ChessColor col, const Ref<ChessCoord> &p0, const Ref<ChessCoord> &p1, ChessPiece promote)
{
    if ((p0.is_valid()) && (p1.is_valid()))
        return mp_game->move((color_e)col, p0->get(), p1->get(), (piece_e)promote);
    else
        return e_invalid_reference;
}

int SentinelChess::move_m(ChessColor col, const Ref<ChessMove> &m)
{
    if (m.is_valid())
        return mp_game->move((color_e)col, m->get());
    else
        return e_invalid_reference;
}

Array SentinelChess::possible_moves(ChessColor col)
{
    Array a;

    std::vector<move_s> sv = mp_game->possible_moves((color_e)col);
    for (int i = 0; i < sv.size(); i++)
    {
        Ref<ChessMove> cm(memnew(ChessMove(sv[i])));
        a.push_back(cm);
    }
    return a;
}

int SentinelChess::rewind_game(int move_no)
{
    return mp_game->rewind_game(move_no);
}

int SentinelChess::remove_piece(const Ref<ChessCoord> &p0)
{
    if (p0.is_valid())
        return mp_game->remove_piece(p0->get());
    else
        return e_invalid_reference;
}

int SentinelChess::add_piece(const Ref<ChessCoord> &p0, ChessColor col, ChessPiece piece)
{
    if (p0.is_valid())
    {
        chesspiece p((color_e)col, (piece_e)piece);
        return mp_game->add_piece(p0->get(), p);
    }
    return e_invalid_reference;
}

// Board helpers
bool SentinelChess::cell_interactive(int y, int x)
{
    return m_lobby.is_local_active((color_e)(mp_game->board().get(y, x) & color_mask));
}

SentinelChess::ChessColor SentinelChess::cell_color(int y, int x)
{
    return (SentinelChess::ChessColor)(mp_game->board().get(y, x) & color_mask);
}

SentinelChess::ChessPiece SentinelChess::cell_piece(int y, int x)
{
    return (SentinelChess::ChessPiece)(mp_game->board().get(y, x) & piece_mask);
}

bool SentinelChess::cell_dark(int y, int x)
{
    int cell = y * 8 + x + y % 2;
    return (cell % 2 == 0);
}

bool SentinelChess::cell_kill(ChessColor col, int y, int x)
{
    unsigned char mask_to_use = ((color_e)col == c_white) ? white_kill_mask : black_kill_mask;
    return (SentinelChess::ChessColor)(mp_game->board().get(y, x) & mask_to_use);
}

ChessEvent::ChessEvent()
{
}

ChessEvent::ChessEvent(chessevent &e)
{
    m_event = e;
}

ChessEvent::~ChessEvent()
{
}

void ChessEvent::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("event_type"), &ChessEvent::event_type);
    ClassDB::bind_method(D_METHOD("move_no"), &ChessEvent::move_no);
    ClassDB::bind_method(D_METHOD("check"), &ChessEvent::check);
    ClassDB::bind_method(D_METHOD("color"), &ChessEvent::color);
    ClassDB::bind_method(D_METHOD("turn_color"), &ChessEvent::turn_color);
    ClassDB::bind_method(D_METHOD("win_color"), &ChessEvent::win_color);
    ClassDB::bind_method(D_METHOD("game_state"), &ChessEvent::game_state);
    ClassDB::bind_method(D_METHOD("move"), &ChessEvent::move);
    ClassDB::bind_method(D_METHOD("percent"), &ChessEvent::percent);
    ClassDB::bind_method(D_METHOD("msg"), &ChessEvent::msg);

    // ChessEventType
    BIND_ENUM_CONSTANT(ceNone);
    BIND_ENUM_CONSTANT(ceRefreshBoard);
    BIND_ENUM_CONSTANT(ceConsider);
    BIND_ENUM_CONSTANT(ceMove);
    BIND_ENUM_CONSTANT(ceTurn);
    BIND_ENUM_CONSTANT(ceEnd);
    BIND_ENUM_CONSTANT(ceChat);
}

ChessEvent::ChessEventType ChessEvent::event_type()
{
    return (ChessEventType)m_event.etype;
}

int ChessEvent::move_no()
{
    return m_event.move_no;
}

bool ChessEvent::check()
{
    return m_event.check;
}

SentinelChess::ChessColor ChessEvent::color()
{
    return (SentinelChess::ChessColor)m_event.color;
}

SentinelChess::ChessColor ChessEvent::turn_color()
{
    return (SentinelChess::ChessColor)m_event.turn_color;
}

SentinelChess::ChessColor ChessEvent::win_color()
{
    return (SentinelChess::ChessColor)m_event.win_color;
}

SentinelChess::ChessGameState ChessEvent::game_state()
{
    return (SentinelChess::ChessGameState)m_event.game_state;
}

Ref<ChessMove> ChessEvent::move()
{
    Ref<ChessMove> cm(memnew(ChessMove(m_event.move)));
    return cm;
}

int ChessEvent::percent()
{
    return m_event.percent;
}

String ChessEvent::msg()
{
    return String(m_event.msg.c_str());
}
