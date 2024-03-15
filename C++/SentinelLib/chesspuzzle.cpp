#include "chesspuzzle.h"

#include <algorithm>
#include <fstream>

#define LICHESS_COLUMNS 10
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
        gameurl = oth.gameurl;
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
        gameurl = vals[idx++];
        openingtags = interpret_opening(vals[idx++]);

        return e_none;
    }

    std::string chesspuzzle::interpret_opening(std::string s)
    {
        // example: Kings_Pawn_Game Kings_Pawn_Game_Leonardis_Variation
        std::vector<std::string> vals = split_string(s, ' ');
        std::string ret = s;
        if (vals.size() == 2)
        {
            std::string title = vals[0];
            std::string subtitle = vals[1];
            size_t tl = title.length();
            size_t sl = subtitle.length();
            if (tl && sl > tl)
            {
                if (subtitle.substr(0, tl + 1) == title + "_")
                    subtitle = subtitle.substr(tl);
            }
            std::replace(title.begin(), title.end(), '_', ' ');
            std::replace(subtitle.begin(), subtitle.end(), '_', ' ');
            ret = title + ": " + subtitle;
        }
        return ret;
    }

    error_e chesspuzzle::load_random(std::string filename, int rating)
    {
        try
        {
            uintmax_t fl = get_file_size(filename);
            std::ifstream pf(filename);
            uintmax_t sp = (uintmax_t)(get_rand() * 0.75 * (float)fl);
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