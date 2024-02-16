#pragma once

#include "chessgame.h"

namespace chess
{

    // class functions as a factory and game manager.
    class chesslobby
    {

    public:
        chesslobby();
        ~chesslobby();

        error_e new_game(int def_skill = 600);
        error_e new_game(color_e user_color, std::string name, int skill, chessplayertype_e ptype, int def_skill = 600);
        error_e load_game(std::string filename);
        error_e save_game(std::string filename);

        error_e add_player(color_e color, std::string name, int skill, chessplayertype_e ptype);
        error_e drop_player(color_e color);
        error_e clear_players();

        std::shared_ptr<chessplayer> player(color_e col);

    private:

        std::mutex m_mutex;
        std::map<color_e, std::shared_ptr<chessplayer>> mp_players;
        std::shared_ptr<chessgame> mp_game;

        error_e load_players(std::ifstream &is);
        error_e save_players(std::ofstream &os);
        
        void backup();
        error_e restore(error_e err=e_none);

        std::map<color_e, std::shared_ptr<chessplayer>> mp_players_backup;
        std::shared_ptr<chessgame> mp_game_backup;


    };
}