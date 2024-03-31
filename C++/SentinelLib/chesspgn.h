#pragma once

#include <map>
#include "chesscommon.h"

namespace chess
{

    class chesspgn
    {
    public:
        chesspgn();
        std::string event;
        std::string white;
        std::string black;
        int whiteelo;
        int blackelo;
        std::string eco;
        std::string opening;
        std::string moves;
        std::map<std::string, std::string> tags;

        error_e load(std::string filename);
        error_e save(std::string filename);

        std::string operator[](const char *);

    private:
        error_e read_tag(std::string line);
    };
}