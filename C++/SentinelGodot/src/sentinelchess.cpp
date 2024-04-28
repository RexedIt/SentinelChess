#include "sentinelchess.h"

#include "chessengine.h"
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
    ClassDB::bind_method(D_METHOD("gamestatestr", "state"), &SentinelChess::gamestatestr);
    ClassDB::bind_method(D_METHOD("movestr", "n", "m"), &SentinelChess::movestr);
    ClassDB::bind_method(D_METHOD("new_game", "white", "black"), &SentinelChess::new_game);
    ClassDB::bind_method(D_METHOD("save_game", "filename"), &SentinelChess::save_game);
    ClassDB::bind_method(D_METHOD("load_game", "filename"), &SentinelChess::load_game);
    ClassDB::bind_method(D_METHOD("load_puzzle", "player", "rating"), &SentinelChess::load_puzzle);
    ClassDB::bind_method(D_METHOD("save_xfen"), &SentinelChess::save_xfen);
    ClassDB::bind_method(D_METHOD("load_xfen", "contents"), &SentinelChess::load_xfen);
    ClassDB::bind_method(D_METHOD("turn_color"), &SentinelChess::turn_color);
    ClassDB::bind_method(D_METHOD("win_color"), &SentinelChess::win_color);
    ClassDB::bind_method(D_METHOD("computer_moving"), &SentinelChess::computer_moving);
    ClassDB::bind_method(D_METHOD("has_local"), &SentinelChess::has_local);
    ClassDB::bind_method(D_METHOD("is_local"), &SentinelChess::is_local);

    ClassDB::bind_method(D_METHOD("is_local_turn"), &SentinelChess::is_local_turn);
    ClassDB::bind_method(D_METHOD("is_local_active", "col"), &SentinelChess::is_local_active);
    ClassDB::bind_method(D_METHOD("player", "col"), &SentinelChess::player);
    ClassDB::bind_method(D_METHOD("players"), &SentinelChess::players);
    ClassDB::bind_method(D_METHOD("local_players"), &SentinelChess::local_players);
    ClassDB::bind_method(D_METHOD("player_name", "col"), &SentinelChess::player_name);
    ClassDB::bind_method(D_METHOD("player_names"), &SentinelChess::player_names);
    ClassDB::bind_method(D_METHOD("preferred_board_color"), &SentinelChess::preferred_board_color);
    ClassDB::bind_method(D_METHOD("lone_local_color"), &SentinelChess::lone_local_color);

    ClassDB::bind_method(D_METHOD("state"), &SentinelChess::state);
    ClassDB::bind_method(D_METHOD("check_state"), &SentinelChess::check_state);
    ClassDB::bind_method(D_METHOD("comment", "turn", "cmt"), &SentinelChess::comment);

    ClassDB::bind_method(D_METHOD("puzzle"), &SentinelChess::puzzle);
    ClassDB::bind_method(D_METHOD("hints"), &SentinelChess::hints);
    ClassDB::bind_method(D_METHOD("hint"), &SentinelChess::hint);
    ClassDB::bind_method(D_METHOD("hintstr"), &SentinelChess::hintstr);
    ClassDB::bind_method(D_METHOD("win_points", "col"), &SentinelChess::win_points);
    ClassDB::bind_method(D_METHOD("initialize", "f"), &SentinelChess::initialize);
    ClassDB::bind_method(D_METHOD("get_meta"), &SentinelChess::get_meta);
    ClassDB::bind_method(D_METHOD("eco"), &SentinelChess::eco);
    ClassDB::bind_method(D_METHOD("open_title"), &SentinelChess::open_title);
    ClassDB::bind_method(D_METHOD("forfeit"), &SentinelChess::forfeit);
    ClassDB::bind_method(D_METHOD("move_m", "col", "m"), &SentinelChess::move_m);
    ClassDB::bind_method(D_METHOD("move_c", "col", "p0", "p1", "promote"), &SentinelChess::move_c);
    ClassDB::bind_method(D_METHOD("move_s", "s"), &SentinelChess::move_s);
    ClassDB::bind_method(D_METHOD("possible_moves", "col"), &SentinelChess::possible_moves);
    ClassDB::bind_method(D_METHOD("play_game"), &SentinelChess::play_game);
    ClassDB::bind_method(D_METHOD("pause_game"), &SentinelChess::pause_game);
    ClassDB::bind_method(D_METHOD("rewind_game"), &SentinelChess::rewind_game);
    ClassDB::bind_method(D_METHOD("advance_game"), &SentinelChess::advance_game);
    ClassDB::bind_method(D_METHOD("goto_turn", "turn_no"), &SentinelChess::goto_turn);
    ClassDB::bind_method(D_METHOD("remove_piece", "p0"), &SentinelChess::remove_piece);
    ClassDB::bind_method(D_METHOD("add_piece", "p0", "col", "piece"), &SentinelChess::add_piece);

    ClassDB::bind_method(D_METHOD("cell_interactive", "y", "x"), &SentinelChess::cell_interactive);
    ClassDB::bind_method(D_METHOD("get_board"), &SentinelChess::board);
    ClassDB::bind_method(D_METHOD("hasevent"), &SentinelChess::hasevent);
    ClassDB::bind_method(D_METHOD("popevent"), &SentinelChess::popevent);

    ClassDB::bind_method(D_METHOD("lastmove"), &SentinelChess::lastmove);
    ClassDB::bind_method(D_METHOD("lastcolor"), &SentinelChess::lastcolor);
    ClassDB::bind_method(D_METHOD("playno"), &SentinelChess::playno);
    ClassDB::bind_method(D_METHOD("playmax"), &SentinelChess::playmax);

    // Chessengine helpers
    ClassDB::bind_method(D_METHOD("hub_usernames", "ptype", "elo"), &SentinelChess::hub_usernames, DEFVAL(0));
    ClassDB::bind_method(D_METHOD("hub_players", "ptype", "include_avatars", "elo", "sort_elo"), &SentinelChess::hub_players, DEFVAL(false), DEFVAL(0), DEFVAL(false));
    ClassDB::bind_method(D_METHOD("hub_update_player", "pdata"), &SentinelChess::hub_update_player);
    ClassDB::bind_method(D_METHOD("hub_unregister", "guid"), &SentinelChess::hub_unregister);

    // Colors
    BIND_ENUM_CONSTANT(cNone);
    BIND_ENUM_CONSTANT(White);
    BIND_ENUM_CONSTANT(Black);

    // Game State
    BIND_ENUM_CONSTANT(sNone);
    BIND_ENUM_CONSTANT(Idle);
    BIND_ENUM_CONSTANT(Play);
    BIND_ENUM_CONSTANT(Terminate);
    BIND_ENUM_CONSTANT(Forfeit);
    BIND_ENUM_CONSTANT(TimeUp);
    BIND_ENUM_CONSTANT(PuzzleSolution);
    BIND_ENUM_CONSTANT(CheckMate);
    BIND_ENUM_CONSTANT(StaleMate);
    BIND_ENUM_CONSTANT(FiveFold);
    BIND_ENUM_CONSTANT(FiftyMove);
    BIND_ENUM_CONSTANT(InsuffMaterial);

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

String SentinelChess::gamestatestr(ChessGameState state)
{
    return String(::game_state_str((game_state_e)state).c_str());
}

String SentinelChess::movestr(int t, const Ref<ChessMove> &m)
{
    if ((m.is_valid()) && (mp_game))
    {
        chessmove cm = m->get();
        return String(::move_str(cm, mp_game->board(t - 1)).c_str());
    }
    return "";
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
    chessturn t;
    if (mp_game)
        t = mp_game->play_turn();
    Ref<ChessMove> cm(memnew(ChessMove(t.m)));
    return cm;
}

Ref<ChessMeta> SentinelChess::get_meta()
{
    if (mp_game)
    {
        Ref<ChessMeta> cm(memnew(ChessMeta(mp_game, m_lobby)));
        return cm;
    }
    return nullptr;
}

String SentinelChess::eco()
{
    if (mp_game)
        return String(mp_game->eco().c_str());
    return "";
}

String SentinelChess::open_title()
{
    if (mp_game)
        return String(mp_game->open_title().c_str());
    return "";
}

ChessColor SentinelChess::lastcolor()
{
    chessturn t;
    if (mp_game)
        t = mp_game->play_turn();
    return (ChessColor)t.c;
}

int SentinelChess::playno()
{
    if (mp_game)
        return mp_game->playno();
    return -1;
}

int SentinelChess::playmax()
{
    if (mp_game)
        return mp_game->playmax();
    return -1;
}

void SentinelChess::refresh_data()
{
    mp_game = m_lobby.game();
}

int SentinelChess::new_game(String title, const Ref<ChessPlayer> &white, const Ref<ChessPlayer> &black, const Ref<ChessClock> &clock)
{
    m_lobby.clear_players();
    std::string n;
    int s = 600;
    chessplayertype_e t = t_none;
    error_e err = e_none;
    if (white.is_valid())
    {
        white->refresh();
        err = m_lobby.add_player(c_white, white->get());
        if (err != e_none)
            return err;
    }
    if (black.is_valid())
    {
        black->refresh();
        err = m_lobby.add_player(c_black, black->get());
        if (err != e_none)
            return err;
    }

    chessclock_s sclock;
    if (clock.is_valid())
        sclock = clock->get();
    sclock.remainms[0] = sclock.allowedms[0];
    sclock.remainms[1] = sclock.allowedms[1];

    err = m_lobby.new_game(title.ascii().get_data(), sclock);
    refresh_data();
    return err;
}

int SentinelChess::load_puzzle(const Ref<ChessPlayer> &player, String keywords, const int rating)
{
    if (!player.is_valid())
        return -1;
    player->refresh();
    chessplayerdata pd = player->get();
    std::string k = keywords.ascii().get_data();
    std::string f = chessengine::data_file("db_puzzles.csv");
    int err = m_lobby.load_puzzle(player->get(), f, k, rating);
    refresh_data();
    return err;
}

int SentinelChess::save_game(String filename)
{
    return m_lobby.save_game(filename.ascii().get_data());
}

int SentinelChess::load_game(String filename)
{
    std::string sfilename = filename.ascii().get_data();
    std::string errorstr;
    error_e err = m_lobby.load_game(sfilename, errorstr);
    refresh_data();
    return err;
}

int SentinelChess::load_xfen(String content)
{
    if (mp_game)
        return mp_game->load_xfen(content.ascii().get_data());
    return e_invalid_reference;
}

String SentinelChess::save_xfen()
{
    if (mp_game)
        return String(mp_game->save_xfen().c_str());
    return "";
}

ChessColor SentinelChess::turn_color()
{
    if (mp_game)
        return (ChessColor)mp_game->turn_color();
    return cNone;
}

ChessColor SentinelChess::win_color()
{
    if (mp_game)
        return (ChessColor)mp_game->win_color();
    return cNone;
}

bool SentinelChess::computer_moving()
{
    if (mp_game)
        if (mp_game->state() == play_e)
            return !m_lobby.is_local_turn();
    return false;
}

bool SentinelChess::has_local()
{
    return m_lobby.has_local();
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

Array SentinelChess::hub_usernames(ChessPlayerType ptype, int elo)
{
    Array a;
    std::vector<std::string> vec;
    int err = chessengine::hub_usernames(vec, (chessplayertype_e)ptype, elo);
    if (err == 0)
    {
        for (const std::string sn : vec)
        {
            String s(sn.c_str());
            a.push_back(s);
        }
    }
    return a;
}

Array SentinelChess::hub_players(ChessPlayerType ptype, bool include_avatars, int elo, bool sort_elo)
{
    Array a;
    std::vector<chessplayerdata> vec;
    int err = chessengine::hub_players(vec, (chessplayertype_e)ptype, include_avatars, elo, sort_elo);
    if (err == 0)
    {
        for (const chessplayerdata pd : vec)
        {
            Ref<ChessPlayer> cp(memnew(ChessPlayer(pd)));
            a.push_back(cp);
        }
    }
    return a;
}

int SentinelChess::hub_update_player(const Ref<ChessPlayer> &pdata)
{
    if (pdata.is_valid())
        return chessengine::hub_update_player(pdata->get());
    return e_invalid_reference;
}

int SentinelChess::hub_unregister(String guid)
{
    return chessengine::hub_unregister(guid.ascii().get_data());
}

ChessColor SentinelChess::preferred_board_color()
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

ChessColor SentinelChess::lone_local_color()
{
    std::set<color_e> locals = m_lobby.local_players();
    if (locals.size() == 1)
        return (ChessColor)(*(locals.begin()));
    return (ChessColor)c_none;
}

bool SentinelChess::check_state(ChessColor col)
{
    if (mp_game)
        return mp_game->check_state((color_e)col);
    return false;
}

void SentinelChess::comment(int turn, String cmt)
{
    if (mp_game)
        mp_game->comment(turn, cmt.ascii().get_data());
}

ChessGameState SentinelChess::state()
{
    if (mp_game)
        return (ChessGameState)mp_game->state();
    return sNone;
}

bool SentinelChess::puzzle()
{
    if (mp_game)
        return mp_game->puzzle();
    return false;
}

int SentinelChess::hints()
{
    if (mp_game)
        return mp_game->hints();
    return 0;
}

Ref<ChessMove> SentinelChess::hint()
{
    if (mp_game)
    {
        Ref<ChessMove> cm(memnew(ChessMove(mp_game->hint())));
        return cm;
    }
    return nullptr;
}

String SentinelChess::hintstr()
{
    if (mp_game)
        return String(mp_game->hintstr().c_str());
    return "";
}

String SentinelChess::win_points(ChessColor col)
{
    int32_t win = 0;
    int32_t lose = 0;
    int32_t draw = 0;
    m_lobby.potential_points((color_e)col, win, lose, draw);
    std::string winlosedraw = std::to_string(win) + "/" + std::to_string(lose);
    if (mp_game)
        if (!mp_game->puzzle())
            winlosedraw += "/" + std::to_string(draw);
    return String(winlosedraw.c_str());
}

int SentinelChess::initialize(const String &d)
{
    return chessengine::initialize(d.ascii().get_data());
}

int SentinelChess::forfeit(ChessColor col)
{
    if (mp_game)
        return mp_game->forfeit((color_e)col);
    return e_invalid_reference;
}

int SentinelChess::move_c(ChessColor col, const Ref<ChessCoord> &p0, const Ref<ChessCoord> &p1, ChessPiece promote)
{
    if ((mp_game) && (p0.is_valid()) && (p1.is_valid()))
        return mp_game->move((color_e)col, p0->get(), p1->get(), (piece_e)promote);
    else
        return e_invalid_reference;
}

int SentinelChess::move_m(ChessColor col, const Ref<ChessMove> &m)
{
    if ((mp_game) && (m.is_valid()))
        return mp_game->move((color_e)col, m->get());
    else
        return e_invalid_reference;
}

int SentinelChess::move_s(ChessColor col, String s)
{
    if (mp_game)
        return mp_game->move((color_e)col, s.ascii().get_data());
    return e_invalid_reference;
}

Array SentinelChess::possible_moves(ChessColor col)
{
    Array a;

    if (mp_game)
    {
        std::vector<chessmove> sv = mp_game->possible_moves((color_e)col);
        for (int i = 0; i < sv.size(); i++)
        {
            Ref<ChessMove> cm(memnew(ChessMove(sv[i])));
            a.push_back(cm);
        }
    }

    return a;
}

int SentinelChess::play_game()
{
    if (mp_game)
        return mp_game->play_game();
    return e_invalid_reference;
}

int SentinelChess::pause_game()
{
    if (mp_game)
        return mp_game->pause_game();
    return e_invalid_reference;
}

int SentinelChess::rewind_game()
{
    if (mp_game)
        return mp_game->rewind_game();
    return e_invalid_reference;
}

int SentinelChess::advance_game()
{
    if (mp_game)
        return mp_game->advance_game();
    return e_invalid_reference;
}

int SentinelChess::goto_turn(int turn_no)
{
    if (mp_game)
        return mp_game->goto_turn(turn_no);
    return e_invalid_reference;
}

int SentinelChess::remove_piece(const Ref<ChessCoord> &p0)
{
    if ((p0.is_valid()) && (mp_game))
        return mp_game->remove_piece(p0->get());
    else
        return e_invalid_reference;
}

int SentinelChess::add_piece(const Ref<ChessCoord> &p0, ChessColor col, ChessPiece piece)
{
    if ((p0.is_valid()) && (mp_game))
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

Ref<ChessBoard> SentinelChess::board()
{
    Ref<ChessBoard> cb(memnew(ChessBoard(mp_game->board())));
    return cb;
}

// ChessBoard
ChessBoard::ChessBoard()
{
}

ChessBoard::ChessBoard(const chessboard &b)
{
    m_board = b;
}

ChessBoard::~ChessBoard()
{
}

void ChessBoard::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("color_", "y", "x"), &ChessBoard::color_);
    ClassDB::bind_method(D_METHOD("piece", "y", "x"), &ChessBoard::piece);
    ClassDB::bind_method(D_METHOD("dark", "y", "x"), &ChessBoard::dark);
    ClassDB::bind_method(D_METHOD("kill", "y", "x"), &ChessBoard::kill);
    ClassDB::bind_method(D_METHOD("check_state", "col"), &ChessBoard::check_state);
    ClassDB::bind_method(D_METHOD("save_xfen"), &ChessBoard::save_xfen);
    ClassDB::bind_method(D_METHOD("captured_pieces"), &ChessBoard::captured_pieces);
    ClassDB::bind_method(D_METHOD("captured_pieces_abbr"), &ChessBoard::captured_pieces_abbr);
}

ChessColor ChessBoard::color_(int y, int x)
{
    return (ChessColor)(m_board.get(y, x) & color_mask);
}

ChessPiece ChessBoard::piece(int y, int x)
{
    return (ChessPiece)(m_board.get(y, x) & piece_mask);
}

bool ChessBoard::dark(int y, int x)
{
    int cell = y * 8 + x + y % 2;
    return (cell % 2 == 0);
}

bool ChessBoard::kill(ChessColor col, int y, int x)
{
    unsigned char mask_to_use = ((color_e)col == c_white) ? white_kill_mask : black_kill_mask;
    return (ChessColor)(m_board.get(y, x) & mask_to_use);
}

bool ChessBoard::check_state(ChessColor col)
{
    return m_board.check_state((color_e)col);
}

String ChessBoard::save_xfen()
{
    return String(m_board.save_xfen().c_str());
}

Array ChessBoard::captured_pieces(ChessColor col)
{
    Array a;
    for (const auto &ki : m_board.captured_pieces((color_e)col))
        a.push_back(ki);
    return a;
}

String ChessBoard::captured_pieces_abbr(ChessColor col)
{
    return String(m_board.captured_pieces_abbr((color_e)col).c_str());
}

// Chess Event
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
    ClassDB::bind_method(D_METHOD("turn_no"), &ChessEvent::turn_no);
    ClassDB::bind_method(D_METHOD("check"), &ChessEvent::check);
    ClassDB::bind_method(D_METHOD("color"), &ChessEvent::color);
    ClassDB::bind_method(D_METHOD("turn_color"), &ChessEvent::turn_color);
    ClassDB::bind_method(D_METHOD("win_color"), &ChessEvent::win_color);
    ClassDB::bind_method(D_METHOD("game_state"), &ChessEvent::game_state);
    ClassDB::bind_method(D_METHOD("white_time"), &ChessEvent::white_time);
    ClassDB::bind_method(D_METHOD("black_time"), &ChessEvent::black_time);
    ClassDB::bind_method(D_METHOD("white_points"), &ChessEvent::white_points);
    ClassDB::bind_method(D_METHOD("black_points"), &ChessEvent::black_points);
    ClassDB::bind_method(D_METHOD("move"), &ChessEvent::move);
    ClassDB::bind_method(D_METHOD("board"), &ChessEvent::board);
    ClassDB::bind_method(D_METHOD("percent"), &ChessEvent::percent);
    ClassDB::bind_method(D_METHOD("msg"), &ChessEvent::msg);
    ClassDB::bind_method(D_METHOD("cmt"), &ChessEvent::cmt);

    // ChessEventType
    BIND_ENUM_CONSTANT(ceNone);
    BIND_ENUM_CONSTANT(ceRefreshBoard);
    BIND_ENUM_CONSTANT(ceConsider);
    BIND_ENUM_CONSTANT(ceTurn);
    BIND_ENUM_CONSTANT(ceState);
    BIND_ENUM_CONSTANT(cePoints);
    BIND_ENUM_CONSTANT(ceChat);
}

ChessEventType ChessEvent::event_type()
{
    return (ChessEventType)m_event.etype;
}

int ChessEvent::turn_no()
{
    return m_event.turn_no;
}

bool ChessEvent::check()
{
    return m_event.check;
}

ChessColor ChessEvent::color()
{
    return (ChessColor)m_event.color;
}

ChessColor ChessEvent::turn_color()
{
    return (ChessColor)m_event.turn_color;
}

ChessColor ChessEvent::win_color()
{
    return (ChessColor)m_event.win_color;
}

ChessGameState ChessEvent::game_state()
{
    return (ChessGameState)m_event.game_state;
}

int ChessEvent::white_time()
{
    return m_event.wt;
}

int ChessEvent::black_time()
{
    return m_event.bt;
}

int ChessEvent::white_points()
{
    return m_event.wp;
}

int ChessEvent::black_points()
{
    return m_event.bp;
}

Ref<ChessMove> ChessEvent::move()
{
    Ref<ChessMove> cm(memnew(ChessMove(m_event.move)));
    return cm;
}

Ref<ChessBoard> ChessEvent::board()
{
    Ref<ChessBoard> cb(memnew(ChessBoard(m_event.board)));
    return cb;
}

int ChessEvent::percent()
{
    return m_event.percent;
}

String ChessEvent::msg()
{
    return String(m_event.msg.c_str());
}

String ChessEvent::cmt()
{
    return String(m_event.cmt.c_str());
}
