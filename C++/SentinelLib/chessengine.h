#pragma once

#include <string>
#include "chesscommon.h"
#include "chessopenings.h"
#include "chessplayerhub.h"

namespace chess
{
    class chessengine
    {
    public:
        static error_e initialize(int argc, char **argv);
        static error_e initialize(std::string f, std::string t = "");
        static std::string data_folder();
        static std::string user_folder();
        static std::string data_file(std::string f);
        static std::string user_file(std::string f);
        static std::string test_folder();
        // static calls dependent on instantiation of eco db
        static error_e chessopenings(std::vector<chessopening> &openings);
        // static calls dependent on instantiation of player hub
        static error_e hub_register_data(chessplayerdata data);
        static error_e hub_register_player(chessplayerdata data);
        static error_e hub_register_computer(chessplayerdata data);
        static error_e hub_unregister(std::string guid);
        // common getters
        static error_e hub_get_or_register_player(chessplayerdata &data, std::string username, int32_t elo, chessplayertype_e ptype = t_human);
        static error_e hub_get_or_register_player(chessplayerdata &data);
        static error_e hub_get_matching_computer_player(chessplayerdata &data, std::string username = "", int32_t elo = 0);
        // Lists
        static error_e hub_usernames(std::vector<std::string> &vec, chessplayertype_e ptype, int32_t elo = 0);
        static error_e hub_players(std::vector<chessplayerdata> &vec, chessplayertype_e ptype, bool include_avatars = false, int32_t elo = 0, bool sort_elo = false);
        static error_e hub_refresh_player(chessplayerdata &data);
        static error_e hub_update_player(chessplayerdata data);
        static error_e hub_update_points(std::string guid, int32_t pts, std::string puzzle);
    };
}