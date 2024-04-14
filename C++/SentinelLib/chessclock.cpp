#include "chessclock.h"

#include <chrono>
#include <thread>
#include <iostream>

namespace chess
{

    chessclock::chessclock(const chessclock_s &clock, chessgame *p_game)
    {
        m_clock = clock;
        m_listenertype = cl_clock;
        m_current = c_none;
        m_active = false;
        m_cancel = false;
        m_thread_running = false;
        m_ending = false;
        mp_game = p_game;
        m_pause_ms = 0;
        start_execution();
    }

    chessclock::~chessclock()
    {
        cancel_execution();
    }

    void chessclock::start_execution()
    {
        if ((m_clock.ctype != cc_none) && (mp_game))
        {
            m_thread_running = true;
            std::thread background(&chessclock::time_keep, this);
            m_thread_id = background.get_id();
            background.detach();
        }
    }

    void chessclock::cancel_execution()
    {
        while (m_thread_running)
        {
            m_cancel = true;
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    }

    void chessclock::clear()
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        cancel_execution();
        chessclock_s c;
        m_clock = c;
        m_current = c_none;
        m_active = false;
    }

    error_e chessclock::time_remaining(int32_t &wt, int32_t &bt)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        wt = 0;
        bt = 0;
        if (m_clock.ctype != cc_none)
        {
            int32_t cur_elapsed = current_elapsed_nolock();
            wt = m_clock.remainms[color_idx(c_white)];
            bt = m_clock.remainms[color_idx(c_black)];
            if (m_current == c_white)
                wt -= cur_elapsed;
            else if (m_current == c_black)
                bt -= cur_elapsed;
        }
        return e_none;
    }

    error_e chessclock::save(json &clock)
    {
        clock["Type"] = clock_type_str(m_clock.ctype);
        clock["AllowedWhite"] = m_clock.allowedms[0];
        clock["AllowedBlack"] = m_clock.allowedms[1];
        clock["RemainWhite"] = m_clock.remainms[0];
        clock["RemainBlack"] = m_clock.remainms[1];
        clock["AddWhite"] = m_clock.addms[0];
        clock["AddBlack"] = m_clock.addms[1];
        return e_none;
    }

    std::shared_ptr<chessclock> chessclock::load(json &clock, chessgame *p_game)
    {
        if (clock.is_null())
            return nullptr;
        chessclock_s cc;
        std::string clocktype;
        JSON_LOAD(clock, "Type", clocktype, "None");
        cc.ctype = str_clock_type(clocktype);
        JSON_LOAD(clock, "AllowedWhite", cc.allowedms[0], 0);
        JSON_LOAD(clock, "AllowedBlack", cc.allowedms[1], 0);
        JSON_LOAD(clock, "RemainWhite", cc.remainms[0], 0);
        JSON_LOAD(clock, "RemainBlack", cc.remainms[1], 0);
        JSON_LOAD(clock, "AddWhite", cc.addms[0], 0);
        JSON_LOAD(clock, "AddBlack", cc.addms[1], 0);
        return std::shared_ptr<chessclock>(new chessclock(cc, p_game));
    }

    void chessclock::signal_on_turn(int16_t turn_no, chessmove m, bool check, chessboard &board, color_e color, game_state_e game_state, color_e win_color, int32_t wt, int32_t bt, std::string cmt)
    {
        // We will make our timekeeping adjustments here
        if (mp_game)
        {
            game_state_e cur_state = mp_game->state();
            if ((m_thread_running) && (cur_state == play_e))
                switch_player(color);
        }
    }

    void chessclock::signal_on_state(game_state_e game_state, color_e win_color)
    {
        if (game_state > play_e)
        {
            if (!m_ending)
                cancel_execution();
        }
        else
        {
            std::lock_guard<std::mutex> guard(m_mutex);
            if ((game_state == play_e) && !m_active)
            {
                end_pause();
                m_active = true;
            }
            else if ((game_state == idle_e) && m_active)
            {
                start_pause();
                m_active = false;
            }
        }
    }

    void chessclock::time_keep()
    {
        m_cancel = false;

        while (!m_cancel)
        {
            if (m_active)
                time_evaluate();
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        m_thread_running = false;
    }

    void chessclock::time_evaluate()
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        color_e wincolor = c_none;
        int elapsedms = current_elapsed_nolock();
        int32_t remainms = m_current == c_white ? m_clock.remainms[0] : m_clock.remainms[1];
        // *** NATHANAEL *** Apply clock evaluation here
        switch (m_clock.ctype)
        {
        case cc_suddendeath:
            if (remainms - elapsedms <= 0)
                wincolor = other(m_current);
            break;
        }
        // Flag for calling the game (wincolor)
        if (wincolor != c_none)
        {
            m_mutex.unlock();
            m_ending = true;
            if (mp_game)
                mp_game->end_game(time_up_e, wincolor);
            m_cancel = true;
        }
    }

    void chessclock::switch_player(color_e col)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        if (col != m_current)
        {
            if (m_current != c_none)
                end_turn(other(col));
            start_turn(col);
        }
    }

    void chessclock::end_turn(color_e col)
    {
        int othidx = color_idx(col);
        int elapsedms = current_elapsed_nolock();
        m_clock.remainms[othidx] -= elapsedms;
        // *** NATHANAEL *** Apply end of turn logic here
        switch (m_clock.ctype)
        {
        case cc_suddendeath:
            m_clock.remainms[othidx] += m_clock.addms[othidx];
            break;
        }
        m_clock.remainms[othidx] += m_clock.addms[othidx];
    }

    void chessclock::start_turn(color_e col)
    {
        m_current = col;
        m_pause_ms = 0;
        m_current_tp = std::chrono::steady_clock::now();
        // *** NATHANAEL *** Apply beginning of turn specific logic here
        switch (m_clock.ctype)
        {
        case cc_suddendeath:
            break;
        }
    }

    int32_t chessclock::current_elapsed()
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        return current_elapsed_nolock();
    }

    int32_t chessclock::current_elapsed_nolock()
    {
        std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
        std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_current_tp);
        return (int32_t)ms.count() - m_pause_ms;
    }

    void chessclock::start_pause()
    {
        m_pause_tp = std::chrono::steady_clock::now();
    }

    void chessclock::end_pause()
    {
        std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
        std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_pause_tp);
        m_pause_ms += (int32_t)ms.count();
    }

}