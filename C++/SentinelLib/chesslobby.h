#pragma once

#include "chessgame.h"
#include "chessgamelistener.h"
#include "chessplayer.h"
#include "chesspuzzle.h"

#include <mutex>
#include <map>
#include <set>

#include "nlohmann/json.hpp"
using namespace nlohmann;

namespace chess
{

    // class functions as a factory and game manager.
    class chesslobby
    {

    public:
        chesslobby(std::shared_ptr<chessgamelistener> p_listener = NULL);
        ~chesslobby();

        error_e new_game(std::string title, const chessclock_s &clock);
        error_e new_game(std::string title, color_e user_color, std::string user_name, int elo, const chessclock_s &clock);
        error_e load_game(std::string filename, std::string &errextra);
        error_e save_game(std::string filename);
        error_e load_puzzle(chessplayerdata user, chesspuzzle p);
        error_e load_puzzle(chessplayerdata user, std::string filename, std::string keywords, int rating);
        error_e load_puzzle(chessplayerdata user, std::string contents);
        // error_e add_player(color_e color, std::string name, int skill, chessplayertype_e ptype);
        error_e add_player(color_e color, chessplayerdata data);
        error_e drop_player(color_e color);
        error_e clear_players();

        void set_listener(std::shared_ptr<chessgamelistener>);
        std::shared_ptr<chessgame> game();
        std::shared_ptr<chessplayer> player(color_e col);
        std::map<color_e, std::shared_ptr<chessplayer>> players();

        std::set<color_e> local_players();
        bool has_local();
        bool is_local(color_e col);
        bool is_local_turn();
        bool is_local_active(color_e col);
        void potential_points(color_e col, int32_t &win, int32_t &lose, int32_t &draw);

        std::string player_name(color_e col);
        std::map<color_e, std::string> player_names();

    private:
        std::mutex m_mutex;
        std::shared_ptr<chessgamelistener> mp_listener;
        std::map<color_e, std::shared_ptr<chessplayer>> mp_players;
        std::shared_ptr<chessgame> mp_game;
        std::set<color_e> m_locals;

        error_e load_chs(std::string filename, std::string &errextra);
        error_e load_pgn(std::string filename, std::string &errextra);
        error_e save_chs(std::string filename);
        error_e save_pgn(std::string filename);

        void attach_to_game();
        void detach_from_game();
        void backup();
        void write_player_meta(bool puzzle = false);
        error_e restore(error_e err = e_none);
        std::map<color_e, int32_t> player_skills_nolock();

        std::map<color_e, std::shared_ptr<chessplayer>> mp_players_backup;
        std::shared_ptr<chessgame> mp_game_backup;
    };
}