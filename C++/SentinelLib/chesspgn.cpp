#include "chesspgn.h"

#include <fstream>
#include <iostream>

namespace chess
{
    chesspgn::chesspgn()
    {
        whiteelo = 0;
        blackelo = 0;
    }

    error_e chesspgn::read_tag(std::string line)
    {
        size_t l = line.length();
        if (l == 0)
            return e_pgn_parse;
        if ((line[0] == '[') && (line[l - 1] = ']'))
        {
            std::string mid = trim(line.substr(1, l - 2));
            l = line.find(' ');
            if (l == std::string::npos)
                return e_pgn_parse;
            std::string lhs = trim(mid.substr(0, l));
            std::string rhs = trim(mid.substr(l));
            // rhs must be quoted
            l = rhs.length();
            if (l == 0)
                return e_pgn_parse;
            if ((rhs[0] == '"') && (rhs[l - 1] == '"'))
            {
                rhs = trim(rhs.substr(1, l - 2));
                tags[lhs] = rhs;
                if (lhs == "Event")
                    event = rhs;
                else if (lhs == "White")
                    white = rhs;
                else if (lhs == "Black")
                    black = rhs;
                else if (lhs == "WhiteElo")
                    whiteelo = atoi(rhs.c_str());
                else if (lhs == "BlackElo")
                    blackelo = atoi(rhs.c_str());
                else if (lhs == "ECO")
                    eco = rhs;
                else if (lhs == "Opening")
                    opening = rhs;
                return e_none;
            }
            return e_pgn_parse;
        }
        return e_pgn_parse;
    }

    error_e chesspgn::load(std::string filename)
    {
        try
        {
            std::ifstream pf(filename);
            if (!pf.is_open())
                return e_loading;

            std::string line;
            error_e err = e_none;

            event = "";
            white = "";
            black = "";
            whiteelo = 0;
            blackelo = 0;
            eco = "";
            opening = "";
            tags.clear();
            moves = "";
            bool has_moves = false;

            while (std::getline(pf, line))
            {
                line = trim(line);
                if (line == "")
                    continue;
                if (starts_with(line, "["))
                {
                    if (has_moves)
                        break; // may be multi file break here
                    err = read_tag(line);
                    if (err != e_none)
                        return err;
                }
                else
                {
                    has_moves = true;
                    moves = line;
                }
            }
            return e_none;
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
            return e_loading;
        }
    }

    std::string chesspgn::operator[](const char *key)
    {
        if (tags.count(key))
            return tags[key];
        return "";
    }

    error_e chesspgn::save(std::string filename)
    {
        // *** REM *** TODO
        return e_none;
    }

}