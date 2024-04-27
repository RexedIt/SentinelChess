#pragma once

#include "chesscommon.h"
#include "chessgame.h"
#include "chessboard.h"
#include "chessplayerhub.h"

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
        chessplayer(color_e color, chessplayerdata data);
        chessplayer(color_e color, std::string name, int32_t skill, chessplayertype_e ptype);
        ~chessplayer();

        bool is(color_e);

        error_e forfeit();
        error_e move(chessmove m0);
        error_e move(coord_s p0, coord_s p1, piece_e promote = p_none);
        error_e move(std::string s);

        chessboard board();
        std::vector<chessmove> possible_moves();
        error_e chat(std::string msg);
        error_e consider(chessmove &, int8_t pct = -1);

        std::string playerguid();
        color_e playercolor();
        chessplayertype_e playertype();
        std::string playername();
        int32_t playerskill();
        int32_t playerpuzzlepoints();
        chessplayerdata playerdata();

        friend class chesslobby;

    protected:
        virtual void set_game(std::shared_ptr<chessgame> p_game);
        virtual void stop_listening() { ; }

        color_e m_color;
        chessplayerdata m_data;
        // we do not use a shared pointer here as we do not want the
        // player to be able to dispose of the game inadvertently.
        std::mutex m_mutex;
        std::shared_ptr<chessgame> mp_game;
    };

}