#include "chessgamelistener.h"

namespace chess
{

    int id_ctr = 0;

    chessevent::chessevent(const chessevent &e)
    {
        etype = e.etype;
        check = e.check;
        color = e.color;
        turn_no = e.turn_no;
        check = e.check;
        turn_color = e.turn_color;
        win_color = e.win_color;
        game_state = e.game_state;
        move = e.move;
        board = e.board;
        percent = e.percent;
        msg = e.msg;
    }

    chessgamelistener::chessgamelistener(chessgamelistenertype listenertype)
    {
        m_listenertype = listenertype;
        m_id = ++id_ctr;
    }

    chessgamelistener::~chessgamelistener()
    {
    }

    chessgamelistenertype chessgamelistener::listenertype()
    {
        return m_listenertype;
    }

    int chessgamelistener::id()
    {
        return m_id;
    }

    chessgamelistener_direct::chessgamelistener_direct(
        chessgamelistenertype listenertype,
        refresh_board_cb p_refresh_board_cb,
        on_consider_cb p_on_consider_cb,
        on_turn_cb p_on_turn_cb,
        on_end_cb p_on_end_cb,
        chat_cb p_chat_cb)
    {
        m_listenertype = listenertype;
        mp_refresh_board_cb = p_refresh_board_cb;
        mp_on_consider_cb = p_on_consider_cb;
        mp_on_turn_cb = p_on_turn_cb;
        mp_on_end_cb = p_on_end_cb;
        mp_chat_cb = p_chat_cb;
    }

    void chessgamelistener_direct::signal_refresh_board(int16_t n, chessboard &b)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        if (mp_refresh_board_cb)
            (mp_refresh_board_cb)(n, b);
    }

    void chessgamelistener_direct::signal_on_consider(move_s m, color_e c, int8_t p)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        if (mp_on_consider_cb)
            (*mp_on_consider_cb)(m, c, p);
    }

    void chessgamelistener_direct::signal_on_turn(int16_t n, move_s m, bool ch, chessboard &b, color_e c)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        if (mp_on_turn_cb)
            (*mp_on_turn_cb)(n, m, ch, b, c);
    }

    void chessgamelistener_direct::signal_on_end(game_state_e g, color_e w)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        if (mp_on_end_cb)
            (*mp_on_end_cb)(g, w);
    }

    void chessgamelistener_direct::signal_chat(std::string msg, color_e c)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        if (mp_chat_cb)
            (*mp_chat_cb)(msg, c);
    }

    chessgamelistener_queue::chessgamelistener_queue(chessgamelistenertype listenertype)
    {
        m_listenertype = listenertype;
    }

    bool chessgamelistener_queue::has_event()
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        return m_events.size() > 0;
    }

    chessevent chessgamelistener_queue::pop_event()
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        chessevent e;
        if (m_events.size() > 0)
        {
            e = m_events.front();
            m_events.pop();
        }
        return e;
    }

    void chessgamelistener_queue::signal_refresh_board(int16_t n, chessboard &b)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        chessevent e(ce_refresh_board);
        e.turn_no = n;
        e.board = b;
        m_events.push(e);
    }

    void chessgamelistener_queue::signal_on_consider(move_s m, color_e c, int8_t p)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        if (m_events.size() == 0)
        {
            chessevent e(ce_consider);
            e.move = m;
            e.color = c;
            e.percent = p;
            m_events.push(e);
        }
    }

    void chessgamelistener_queue::signal_on_turn(int16_t n, move_s m, bool ch, chessboard &b, color_e c)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        chessevent e(ce_turn);
        e.turn_no = n;
        e.move = m;
        e.check = ch;
        e.board = b;
        e.color = c;
        m_events.push(e);
    }

    void chessgamelistener_queue::signal_on_end(game_state_e g, color_e w)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        chessevent e(ce_end);
        e.game_state = g;
        e.win_color = w;
        m_events.push(e);
    }

    void chessgamelistener_queue::signal_chat(std::string msg, color_e c)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        chessevent e(ce_chat);
        e.msg = msg;
        e.color = c;
        m_events.push(e);
    }

}
