#pragma once

#include "chessgame.h"

#include <mutex>
#include <queue>
#include <memory>

namespace chess
{

    // for direct callbacks
    typedef void (*refresh_board_cb)(int16_t, chessboard &);
    typedef void (*on_consider_cb)(move_s, color_e, int8_t pct);
    typedef void (*on_turn_cb)(int16_t, move_s, bool, chessboard &, color_e, game_state_e, color_e, int32_t, int32_t);
    typedef void (*on_state_cb)(game_state_e, color_e);
    typedef void (*chat_cb)(std::string, color_e);

    typedef enum chesseventtype
    {
        ce_empty,
        ce_refresh_board,
        ce_consider,
        ce_turn,
        ce_state,
        ce_chat
    } chesseventtype;

    typedef struct chessevent
    {
        chessevent() { ; }
        chessevent(chesseventtype e) { etype = e; }
        chessevent(const chessevent &e);
        chesseventtype etype = ce_empty;
        int16_t turn_no = 0;
        bool check = false;
        color_e color = c_none;
        color_e turn_color = c_none;
        color_e win_color = c_none;
        game_state_e game_state = none_e;
        move_s move;
        chessboard board;
        int8_t percent = 0;
        int32_t wt = 0;
        int32_t bt = 0;
        std::string msg;
    } chessevent;

    class chessgamelistener
    {

    public:
        chessgamelistener(chessgamelistenertype listenertype = cl_none);
        ~chessgamelistener();

        friend class chessgame;
        friend class chesslobby;

    protected:
        virtual void signal_refresh_board(int16_t, chessboard &) = 0;
        virtual void signal_on_consider(move_s, color_e, int8_t pct = -1) = 0;
        virtual void signal_on_turn(int16_t, move_s, bool, chessboard &, color_e, game_state_e, color_e, int32_t, int32_t) = 0;
        virtual void signal_on_state(game_state_e, color_e) = 0;
        virtual void signal_chat(std::string, color_e) = 0;

        int id();
        chessgamelistenertype listenertype();
        chessgamelistenertype m_listenertype;
        std::mutex m_mutex;
        int m_id;
    };

    class chessgamelistener_direct : public chessgamelistener
    {

    public:
        chessgamelistener_direct(
            chessgamelistenertype listenertype = cl_none,
            refresh_board_cb p_refresh_board_cb = NULL,
            on_consider_cb p_on_consider_cb = NULL,
            on_turn_cb p_on_turn_cb = NULL,
            on_state_cb p_on_state_cb = NULL,
            chat_cb p_chat_cb = NULL);

    protected:
        virtual void signal_refresh_board(int16_t, chessboard &);
        virtual void signal_on_consider(move_s, color_e, int8_t pct = -1);
        virtual void signal_on_turn(int16_t, move_s, bool, chessboard &, color_e, game_state_e, color_e, int32_t, int32_t);
        virtual void signal_on_state(game_state_e, color_e);
        virtual void signal_chat(std::string, color_e);

    private:
        refresh_board_cb mp_refresh_board_cb;
        on_consider_cb mp_on_consider_cb;
        on_turn_cb mp_on_turn_cb;
        on_state_cb mp_on_state_cb;
        chat_cb mp_chat_cb;
    };

    class chessgamelistener_queue : public chessgamelistener
    {

    public:
        chessgamelistener_queue(chessgamelistenertype listenertype = cl_none);

        bool has_event();
        chessevent pop_event();

    protected:
        virtual void signal_refresh_board(int16_t, chessboard &);
        virtual void signal_on_consider(move_s, color_e, int8_t pct = -1);
        virtual void signal_on_turn(int16_t, move_s, bool, chessboard &, color_e, game_state_e, color_e, int32_t, int32_t);
        virtual void signal_on_state(game_state_e, color_e);
        virtual void signal_chat(std::string, color_e);

    private:
        std::queue<chessevent> m_events;
    };

}