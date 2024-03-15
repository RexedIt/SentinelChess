#pragma once

#include "chesscommon.h"

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
        error_e load_random(std::string filename, int rating);

        std::string puzzleid;
        std::string fen;
        std::string moves;
        int rating;
        int ratingdeviation;
        int popularity;
        int nbplays;
        std::string themes;
        std::string gameurl;
        std::string openingtags;

        std::string title();

    private:
        std::string interpret_opening(std::string);
    };
}