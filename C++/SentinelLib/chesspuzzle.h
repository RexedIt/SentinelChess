#pragma once

#include "chesscommon.h"
#include <set>

namespace chess
{
    class chesspuzzle
    {
    public:
        chesspuzzle();
        chesspuzzle(const chesspuzzle &);
        void copy(const chesspuzzle &);
        void operator=(const chesspuzzle &);

        error_e load_line(std::string line);
        error_e load_random(std::string filename, std::string keywords, int rating, std::set<std::string> visited);
        error_e load_specific(std::string filename, std::string puzzleid);

        std::string puzzleid;
        std::string fen;
        std::string moves;
        int rating;
        int ratingdeviation;
        int popularity;
        int nbplays;
        std::string themes;
        std::string openingtags;
    };
}