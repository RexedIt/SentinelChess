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
        chessplayerdata(json j);
        error_e save_json(json &j);
        std::string guid;
        std::string username;
        int32_t elo;
        chessplayertype_e playertype;
        int32_t puzzlepoints;
        int32_t gamepoints;
        bool persistent;
        std::string avatar;
        std::string meta;
    };

    class chessplayerhub
    {
    public:
        chessplayerhub();
        ~chessplayerhub();

        error_e load_json(std::string);

        error_e find_by_id(std::string guid, chessplayerdata &data);
        error_e find_by_name(std::string username, chessplayerdata &data);
        error_e register_data(chessplayerdata data);
        error_e unregister(std::string guid);
        error_e update_permanent_data(chessplayerdata data);
        error_e update_puzzle_points(std::string guid, int addpuzzle);
        error_e update_game_points(std::string guid, int addgame);

    private:
        void clear_players();
        void initialize();
        error_e save_json();

        std::mutex m_mutex;
        std::string m_filename;
        std::map<std::string, chessplayerdata> m_players;
    };

}