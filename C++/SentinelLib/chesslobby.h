#pragma once

#include "chessgame.h"

namespace chess
{

    // class functions as a factory and game manager.
    class chesslobby
    {

    public:
        error_e new_game(chessgame &game);
        error_e new_game(chessgame &game, color_e user_color, std::string name, int skill, chessplayertype_e ptype);
        error_e load_game(chessgame &game, std::string filename);

        error_e add_player(color_e color, std::string name, int skill, chessplayertype_e ptype);
        error_e drop_player(color_e color);
        error_e clear_players();

        std::shared_ptr<chessplayer> player(color_e col);

    private:
        std::mutex m_mutex;
        std::map<color_e, std::shared_ptr<chessplayer>> m_players;
    };
}