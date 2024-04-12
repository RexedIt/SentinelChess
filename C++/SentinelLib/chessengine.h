#pragma once

#include <string>
#include "chessopenings.h"

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
        static error_e preferredecos(color_e col, std::vector<std::string> &ecos);
    };
}