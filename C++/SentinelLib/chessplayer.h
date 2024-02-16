#pragma once

#include "chesscommon.h"
#include "chessgame.h"
#include "chessboard.h"

#include <mutex>
#include <queue>
#include <memory>

namespace chess
{

    std::vector<std::string> playertypes();
    chessplayertype_e playertypefromstring(std::string);
    std::string playertypetostring(chessplayertype_e);

    class chessplayer
    {
    public:
        chessplayer();
        chessplayer(std::string name, int32_t skill);
        ~chessplayer();

        bool is(color_e);
        error_e forfeit();
        error_e move(move_s m0);
        error_e move(coord_s p0, coord_s p1, piece_e promote = p_none);
        chessboard board();
        std::vector<move_s> possible_moves();
        error_e chat(std::string msg);

        // Signals
        virtual void signal_refresh_board(int16_t, chessboard &) = 0;
        virtual void signal_on_consider(move_s &m, color_e c, int8_t pct = -1) = 0;
        virtual void signal_on_move(int16_t, move_s &, color_e) = 0;
        virtual void signal_on_turn(int16_t, bool, chessboard &b) = 0;
        virtual void signal_on_end(game_state_e, color_e) = 0;
        virtual void signal_chat(std::string, color_e) = 0;

        color_e playercolor();
        chessplayertype_e playertype();
        std::string playername();
        int32_t playerskill();

        friend class chessgame;

    protected:
        virtual void _register(chessgame *pgame, color_e);
        virtual void _unregister();
        std::mutex m_mutex;
        color_e m_color;
        std::string m_name;
        int32_t m_skill;
        chessplayertype_e m_playertype;
        // we do not use a shared pointer here as we do not want the
        // player to be able to dispose of the game inadvertently.
        chessgame *mp_game;
    };

}