#pragma once

#include <map>
#include <mutex>

#include "chesscommon.h"

#include "nlohmann/json.hpp"
using namespace nlohmann;

namespace chess
{
    class chessplayerdata
    {
    public:
        chessplayerdata();
        chessplayerdata(std::string _guid, std::string _username, int32_t _elo, chessplayertype_e _playertype, bool _persistent, std::string _meta);
        chessplayerdata(json j, chessplayertype_e _playertype = t_none);
        error_e load_json(json j);
        error_e save_json(json &j, bool full = true);
        std::string guid;
        std::string username;
        std::string fullname;
        int32_t elo;
        chessplayertype_e ptype;
        int32_t puzzlepoints;
        int32_t gamepoints;
        bool persistent;
        std::string avatar;
        std::string meta;
    };

    std::string new_guid();

    chessplayerdata new_computer_player(std::string guid, std::string username, int32_t elo, std::string meta = "");
    chessplayerdata new_human_player(std::string username, int32_t elo, std::string meta = "");
    chessplayerdata new_puzzle_player(std::string username, int32_t elo, std::string meta = "");

    class chessplayerhub
    {
    public:
        chessplayerhub();
        ~chessplayerhub();

        error_e load_players(std::string);
        error_e load_computers(std::string);

        error_e find_by_id(chessplayerdata &data, std::string guid);
        error_e find_player_by_username(chessplayerdata &data, std::string username, chessplayertype_e ptype = t_human);
        error_e register_data(chessplayerdata data);
        error_e register_player(chessplayerdata data);
        error_e register_computer(chessplayerdata data);
        error_e unregister(std::string guid);

        error_e get_or_register_player(chessplayerdata &data, std::string username, int32_t elo, chessplayertype_e ptype = t_human);
        error_e get_or_register_player(chessplayerdata &data);
        error_e get_matching_computer_player(chessplayerdata &data, std::string username = "", int32_t elo = 0);

        error_e update_player(chessplayerdata data);
        error_e update_puzzle_points(std::string guid, int addpuzzle);
        error_e update_game_points(std::string guid, int addgame);

        std::vector<std::string> usernames(chessplayertype_e ptype, int32_t elo = 0);
        std::vector<chessplayerdata> players(chessplayertype_e ptype, bool include_avatars = false, int32_t elo = 0, bool sort_elo = false);

    private:
        void clear_players();
        error_e save_json(bool persistent = true);

        std::mutex m_mutex;
        std::string m_filename;
        std::map<std::string, chessplayerdata> m_humans;
        std::map<std::string, chessplayerdata> m_computers;
    };

}