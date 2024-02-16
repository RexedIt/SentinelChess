#include "chessplayer.h"

namespace chess
{

    chessplayerasync::eventdata::eventdata(const eventdata &e)
    {
        etype = e.etype;
        move_no = e.move_no;
        check = e.check;
        color = e.color;
        turn_color = e.turn_color;
        win_color = e.win_color;
        game_state = e.game_state;
        move = e.move;
        msg = e.msg;
    }

    chessplayer::chessplayer()
    {
        m_color = c_none;
        m_playertype = t_none;
        mp_game = NULL;
        m_skill = 0;
    }

    chessplayer::chessplayer(std::string name, int skill)
    {
        m_name = name;
        m_skill = skill;
        m_playertype = t_none;
        mp_game = NULL;
    }

    chessplayer::~chessplayer()
    {
        _unregister();
    }

    chessplayertype_e chessplayer::playertype()
    {
        return m_playertype;
    }

    std::string chessplayer::playername()
    {
        return m_name;
    }

    int chessplayer::playerskill()
    {
        return m_skill;
    }

    void chessplayer::_register(chessgame *p_game, color_e color)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        mp_game = p_game;
        m_color = color;
    }

    void chessplayer::_unregister()
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        if (mp_game != NULL)
            mp_game->_unregister(m_color);
        mp_game = NULL;
        m_color = c_none;
    }

    bool chessplayer::is(color_e c)
    {
        return c == m_color;
    }

    error_e chessplayer::forfeit()
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        if (mp_game != NULL)
            return e_no_game;
        return mp_game->forfeit(m_color);
    }

    error_e chessplayer::move(move_s m0)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        if (mp_game != NULL)
            return e_no_game;
        return mp_game->move(m_color, m0);
    }

    error_e chessplayer::move(coord_s p0, coord_s p1, piece_e promote)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        if (mp_game != NULL)
            return e_no_game;
        return mp_game->move(m_color, p0, p1, promote);
    }

    chessboard chessplayer::board()
    {
        chessboard b;
        if (mp_game != NULL)
            b.copy(mp_game->board());
        return b;
    }

    std::vector<move_s> chessplayer::possible_moves()
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        std::vector<move_s> p;
        if (mp_game != NULL)
            p = mp_game->possible_moves(m_color);
        return p;
    }

    void chessplayerasync::signal_refresh_board(int n, chessboard &b)
    {
        eventdata e(refresh_board);
        e.move_no = n;
        push_event(e);
    }

    void chessplayerasync::signal_on_consider(move_s &m, color_e c, int p)
    {
        eventdata e(on_consider);
        e.move = m;
        e.color = c;
        e.percent = p;
        push_event(e);
    }

    void chessplayerasync::signal_on_move(int n, move_s &m, color_e c)
    {
        eventdata e(on_move);
        e.move_no = n;
        e.move = m;
        e.color = c;
        push_event(e);
    }

    void chessplayerasync::signal_on_turn(int n, bool c, chessboard &b)
    {
        eventdata e(on_turn);
        e.move_no = n;
        e.check = c;
        push_event(e);
    }

    void chessplayerasync::signal_on_end(game_state_e g, color_e w)
    {
        eventdata e(on_end);
        e.game_state = g;
        e.win_color = w;
    }

    void chessplayerasync::signal_chat(std::string msg, color_e c)
    {
        eventdata e(on_chat);
        e.msg = msg;
        e.color = c;
        push_event(e);
    }

    void chessplayerasync::push_event(eventdata e)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        m_events.push(e);
    }

    chessplayerasync::eventdata chessplayerasync::pop_event()
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        eventdata e;
        if (m_events.size() > 0)
        {
            e = m_events.front();
            m_events.pop();
        }
        return e;
    }

}