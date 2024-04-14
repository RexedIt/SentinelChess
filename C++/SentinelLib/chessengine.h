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
        static error_e initialize(std::string f);
        static std::string data_folder();
        static std::string user_folder();
        static std::string data_file(std::string f);
        static std::string user_file(std::string f);
        // static calls dependent on instantiation of eco db
        static error_e chessopenings(std::vector<chessopening> &openings);
        // static calls dependent on instantiation of player hub
        static error_e get_or_register_player(std::string username, int32_t elo, chessplayertype_e ptype, chessplayerdata &data);
        static error_e get_or_register_player(chessplayerdata &data);
        static error_e get_matching_computer_player(int32_t elo, chessplayerdata &data);
    };
}