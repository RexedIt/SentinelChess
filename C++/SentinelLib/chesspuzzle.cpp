#include "chesspuzzle.h"

#include <algorithm>
#include <fstream>

#define LICHESS_COLUMNS 9
#define LICHESS_RATING_COLUMN 3

namespace chess
{
    chesspuzzle::chesspuzzle()
    {
        rating = 0;
        ratingdeviation = 0;
        popularity = 0;
        nbplays = 0;
    }

    chesspuzzle::chesspuzzle(const chesspuzzle &oth)
    {
        copy(oth);
    }

    void chesspuzzle::operator=(const chesspuzzle &oth)
    {
        copy(oth);
    }

    void chesspuzzle::copy(const chesspuzzle &oth)
    {
        puzzleid = oth.puzzleid;
        fen = oth.fen;
        moves = oth.moves;
        rating = oth.rating;
        ratingdeviation = oth.ratingdeviation;
        popularity = oth.popularity;
        nbplays = oth.nbplays;
        themes = oth.themes;
        openingtags = oth.openingtags;
    }

    error_e chesspuzzle::load_line(std::string line)
    {
        std::vector<std::string> vals = split_string(line, ',');
        if (vals.size() != LICHESS_COLUMNS)
            return e_loading;
        int idx = 0;
        puzzleid = vals[idx++];
        fen = vals[idx++];
        moves = vals[idx++];
        rating = atoi(vals[idx++].c_str());
        ratingdeviation = atoi(vals[idx++].c_str());
        popularity = atoi(vals[idx++].c_str());
        nbplays = atoi(vals[idx++].c_str());
        themes = vals[idx++];
        openingtags = vals[idx++];

        return e_none;
    }

    error_e chesspuzzle::load_random(std::string filename, int rating)
    {
        try
        {
            uintmax_t fl = get_file_size(filename);
            std::ifstream pf(filename);
            uintmax_t sp = (uintmax_t)(get_rand() * 0.9 * (float)fl);
            pf.seekg(sp);
            std::string line;
            std::getline(pf, line);
            int tries = 0;
            int delta = 9999;
            std::string best = "";
            while (tries++ < 1000)
            {
                std::getline(pf, line);
                std::vector<std::string> vals = split_string(line, ',');
                if (vals.size() != LICHESS_COLUMNS)
                    return e_loading;
                int r = atoi(vals[LICHESS_RATING_COLUMN].c_str());
                if (r >= rating)
                {
                    if (r < delta)
                    {
                        delta = r;
                        best = line;
                        if (r - rating < 50)
                            break;
                    }
                }
            }
            pf.close();
            if (best != "")
                return load_line(line);
        }
        catch (const std::exception &)
        {
            return e_loading;
        }
        return e_none_found;
    }

    std::string chesspuzzle::title()
    {
        if (openingtags != "")
            return puzzleid + "- " + openingtags;
        return puzzleid + "- " + themes;
    }

}