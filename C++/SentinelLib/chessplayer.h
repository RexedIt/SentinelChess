#pragma once

#include "chesscommon.h"
#include "chessgame.h"
#include "chessboard.h"

#include <mutex>
#include <memory>

namespace chess
{

    class chessgame;

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

        color_e playercolor();
        chessplayertype_e playertype();
        std::string playername();
        int32_t playerskill();

        friend class chesslobby;

    protected:
        void set_game(std::shared_ptr<chessgame> p_game);

        color_e m_color;
        std::string m_name;
        int32_t m_skill;
        chessplayertype_e m_playertype;
        // we do not use a shared pointer here as we do not want the
        // player to be able to dispose of the game inadvertently.
        std::mutex m_mutex;
        std::shared_ptr<chessgame> mp_game;
    };

}