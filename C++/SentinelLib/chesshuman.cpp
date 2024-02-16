#include "chesshuman.h"

namespace chess
{

    chessevent::chessevent(const chessevent &e)
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


    chesshuman::chesshuman()
    {
        m_playertype = t_human;
        m_name = "Human";
        m_skill = 0;
        // Initializes them to all NULL
        set_callbacks();
    }

    chesshuman::chesshuman(std::string name, int skill)
    {
        m_playertype = t_human;
        m_name = name;
        m_skill = skill;
        // Initializes them to all NULL
        set_callbacks();
    }

    chesshuman::~chesshuman()
    {
    }

    void chesshuman::set_callbacks(
            refresh_board_cb p_refresh_board_cb,
            on_consider_cb p_on_consider_cb,
            on_move_cb p_on_move_cb,
            on_turn_cb p_on_turn_cb,
            on_end_cb p_on_end_cb,
            chat_cb p_chat_cb
        )
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        mp_refresh_board_cb = p_refresh_board_cb;
        mp_on_consider_cb = p_on_consider_cb;
        mp_on_move_cb = p_on_move_cb;
        mp_on_turn_cb = p_on_turn_cb;
        mp_on_end_cb = p_on_end_cb;
        mp_chat_cb = p_chat_cb;
    }

    void chesshuman::signal_refresh_board(int16_t n, chessboard &b)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        if (mp_refresh_board_cb)
        {
            (mp_refresh_board_cb)(n, b);
        }
        else
        {
            chessevent e(ce_refresh_board);
            e.move_no = n;
            push_event(e);
        }
    }

    void chesshuman::signal_on_consider(move_s &m, color_e c, int8_t p)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        if (mp_on_consider_cb)
        {
            (*mp_on_consider_cb)(m, c, p);
        }
        else
        {
            chessevent e(ce_consider);
            e.move = m;
            e.color = c;
            e.percent = p;
            push_event(e);
        }
    }

    void chesshuman::signal_on_move(int16_t n, move_s &m, color_e c)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        if (mp_on_move_cb)
        {
            (*mp_on_move_cb)(n, m, c);
        }
        else
        {
            chessevent e(ce_move);
            e.move_no = n;
            e.move = m;
            e.color = c;
            push_event(e);
        }
    }

    void chesshuman::signal_on_turn(int16_t n, bool c, chessboard &b)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        if (mp_on_turn_cb)
        {
            (*mp_on_turn_cb)(n, c, b);
        }
        else
        {
            chessevent e(ce_turn);
            e.move_no = n;
            e.check = c;
            push_event(e);
        }
    }

    void chesshuman::signal_on_end(game_state_e g, color_e w)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        if (mp_on_end_cb)
        {
            (*mp_on_end_cb)(g, w);
        }
        else
        {
            chessevent e(ce_end);
            e.game_state = g;
            e.win_color = w;
        }
    }

    void chesshuman::signal_chat(std::string msg, color_e c)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        if (mp_chat_cb)
        {
            (*mp_chat_cb)(msg,c);
        }
        else
        {
            chessevent e(ce_chat);
            e.msg = msg;
            e.color = c;
            push_event(e);
        }
    }

    void chesshuman::push_event(chessevent e)
    {
        // dont lock here as callers should have locked
        m_events.push(e);
    }

    bool chesshuman::has_event()
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        return m_events.size()>0;
    }

    chessevent chesshuman::pop_event()
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

}