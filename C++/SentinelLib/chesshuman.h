#pragma once

#include "chessplayer.h"

namespace chess
{

    // for direct callbacks
    typedef void (*refresh_board_cb)(int16_t, chessboard &);
    typedef void (*on_consider_cb)(move_s &, color_e, int8_t pct);
    typedef void (*on_move_cb)(int16_t, move_s &, color_e);
    typedef void (*on_turn_cb)(int16_t, bool, chessboard &);
    typedef void (*on_end_cb)(game_state_e, color_e);
    typedef void (*chat_cb)(std::string, color_e);

    typedef enum chesseventtype
    {
        ce_empty,
        ce_refresh_board,
        ce_consider,
        ce_move,
        ce_turn,
        ce_end,
        ce_chat
    } chesseventtype;

    typedef struct chessevent
    {
        chessevent() { ; }
        chessevent(chesseventtype e) { etype = e; }
        chessevent(const chessevent &e);
        chesseventtype etype = ce_empty;
        int16_t move_no = 0;
        bool check = false;
        color_e color = c_none;
        color_e turn_color = c_none;
        color_e win_color = c_none;
        game_state_e game_state = none_e;
        move_s move;
        int8_t percent = 0;
        std::string msg;
    } chessevent;

    class chesshuman : public chessplayer
    {

    public:

        chesshuman();
        chesshuman(std::string name, int skill);
        ~chesshuman();

        // if set, direct callbacks to functions will occur
        // versus chessevents fired for each signal.
        void set_callbacks(
            refresh_board_cb p_refresh_board_cb = NULL,
            on_consider_cb p_on_consider_cb = NULL,
            on_move_cb p_on_move_cb = NULL,
            on_turn_cb p_on_turn_cb = NULL,
            on_end_cb p_on_end_cb = NULL,
            chat_cb p_chat_cb = NULL
        );

        bool has_event();
        chessevent pop_event();

    protected:

        virtual void signal_refresh_board(int16_t, chessboard &);
        virtual void signal_on_consider(move_s &, color_e, int8_t pct = -1);
        virtual void signal_on_move(int16_t, move_s &, color_e);
        virtual void signal_on_turn(int16_t, bool, chessboard &);
        virtual void signal_on_end(game_state_e, color_e);
        virtual void signal_chat(std::string, color_e);        

        refresh_board_cb mp_refresh_board_cb;
        on_consider_cb mp_on_consider_cb;
        on_move_cb mp_on_move_cb;
        on_turn_cb mp_on_turn_cb;
        on_end_cb mp_on_end_cb;
        chat_cb mp_chat_cb;

    private:

        void push_event(chessevent);
        std::queue<chessevent> m_events;        

    };

}