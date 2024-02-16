#pragma once

#include "chesscommon.h"
#include "chessgame.h"
#include "chessboard.h"

#include <mutex>
#include <queue>
#include <memory>

namespace chess
{

    class chessplayer
    {
    public:
        chessplayer();
        chessplayer(std::string name, int skill);
        ~chessplayer();

        bool is(color_e);
        error_e forfeit();
        error_e move(move_s m0);
        error_e move(coord_s p0, coord_s p1, piece_e promote = p_none);
        chessboard board();
        std::vector<move_s> possible_moves();

        // Signals
        virtual void signal_refresh_board(int, chessboard &) = 0;
        virtual void signal_on_consider(move_s &m, color_e c, int pct = -1) = 0;
        virtual void signal_on_move(int, move_s &, color_e) = 0;
        virtual void signal_on_turn(int, bool, chessboard &b) = 0;
        virtual void signal_on_end(game_state_e, color_e) = 0;
        virtual void signal_chat(std::string, color_e) = 0;

        chessplayertype_e playertype();
        std::string playername();
        int playerskill();

        friend class chessgame;

    protected:
        virtual void _register(chessgame *pgame, color_e);
        virtual void _unregister();
        std::mutex m_mutex;
        color_e m_color;
        std::string m_name;
        int m_skill;
        chessplayertype_e m_playertype;
        // we do not use a shared pointer here as we do not want the
        // player to be able to dispose of the game inadvertently.
        chessgame *mp_game;
    };

    class chessplayerasync : public chessplayer
    {
        typedef enum eventtype
        {
            empty = -1,
            refresh_board = 1,
            on_consider,
            on_move,
            on_turn,
            on_end,
            on_chat
        } eventtype;

        typedef struct eventdata
        {
            eventdata() { ; }
            eventdata(eventtype e) { etype = e; }
            eventdata(const eventdata &e);
            eventtype etype = empty;
            int move_no = 0;
            bool check = false;
            color_e color = c_none;
            color_e turn_color = c_none;
            color_e win_color = c_none;
            game_state_e game_state = none_e;
            move_s move;
            int percent = 0;
            std::string msg;
        } eventdata;

    public:
        virtual void signal_refresh_board(int, chessboard &);
        virtual void signal_on_consider(move_s &, color_e, int pct = -1);
        virtual void signal_on_move(int, move_s &, color_e);
        virtual void signal_on_turn(int, bool, chessboard &);
        virtual void signal_on_end(game_state_e, color_e);
        virtual void signal_chat(std::string, color_e);

    protected:
        eventdata pop_event();

    private:
        void push_event(eventdata);
        std::queue<eventdata> m_events;
    };

}