#include "chesspgn.h"
#include "chessboard.h"

#include <fstream>
#include <iostream>

namespace chess
{
    chesspgn::chesspgn()
    {
    }

    std::string chesspgn::event()
    {
        return tag("Event");
    }

    std::string chesspgn::white()
    {
        return tag("White");
    }

    std::string chesspgn::black()
    {
        return tag("Black");
    }

    int chesspgn::whiteelo()
    {
        return atoi(tag("WhiteElo").c_str());
    }

    int chesspgn::blackelo()
    {
        return atoi(tag("BlackElo").c_str());
    }

    std::string chesspgn::eco()
    {
        return tag("ECO");
    }

    std::string chesspgn::opening()
    {
        return tag("Opening");
    }

    std::string chesspgn::moves_str()
    {
        return m_moves_str;
    }

    std::vector<chessmove> chesspgn::moves()
    {
        return m_moves;
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
                m_tags[lhs] = rhs;
                return e_none;
            }
            return e_pgn_parse;
        }
        return e_pgn_parse;
    }

    error_e chesspgn::load(std::string filename, std::string &errextra)
    {
        try
        {
            std::ifstream pf(filename);
            if (!pf.is_open())
            {
                errextra = "Unable to open file - " + filename;
                return e_loading;
            }
            std::string line;
            error_e err = e_none;

            m_tags.clear();
            m_moves.clear();
            m_moves_str = "";

            bool has_moves = false;

            while (std::getline(pf, line))
            {
                if (has_moves)
                    break; // may be multi file break here
                line = trim(line);
                if (line == "")
                    continue;
                if (starts_with(line, "["))
                {
                    err = read_tag(line);
                    if (err != e_none)
                    {
                        errextra = "Unable to parse tag: " + line;
                        return err;
                    }
                }
                else
                {
                    has_moves = true;
                    m_moves_str = line;
                    error_e err = read_pgn_moves(m_moves_str, m_moves, errextra);
                    if (err != e_none)
                    {
                        // remove
                        std::cout << "chesspgn::load error " << errextra << std::endl;
                        return err;
                    }
                    if (m_moves.size() == 0)
                        return e_loading;
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
        return tag(key);
    }

    std::string chesspgn::tag(std::string key)
    {
        if (m_tags.count(key))
            return m_tags[key];
        return "";
    }

    error_e chesspgn::save(std::string filename)
    {
        // *** REM *** TODO
        return e_none;
    }

    error_e load_move(std::string s, color_e tc, chessboard &b, chessmove &m, std::string &errextra)
    {
        error_e err = str_move(s, tc, b, m);
        if (err != e_none)
        {
            errextra = "Move parse error - " + s;
            return err;
        }
        m = b.attempt_move(tc, m);
        if (m.error != e_none)
        {
            errextra = "Move execute error - " + s;
            return err;
        }
        return e_none;
    }

    bool terminal(std::string move)
    {
        if ((move == "") || (move == "*"))
            return true;
        if ((move[0] == '1') || (move[0] == '0'))
        {
            if (starts_with(move, "1/2-"))
                return true;
            if (starts_with(move, "0-"))
                return true;
            if (starts_with(move, "1-"))
                return true;
        }
        return false;
    }
    error_e read_pgn_moves(std::string moves_str, std::vector<chessmove> &move_vec, std::string &errextra)
    {
        chessboard b;
        b.load_xfen(c_open_board);
        color_e tc = b.turn_color();

        move_vec.clear();
        errextra = "";
        std::string moves = moves_str;

        // We want to move through the file in move pairs.
        int ft = 0;
        std::string fts = std::to_string(++ft) + ".";
        size_t q = moves.find(fts);
        if (q == std::string::npos)
            return e_none;
        moves = trim(moves.substr(q + fts.length()));

        while (moves != "")
        {
            std::string move_pair = moves;
            std::string fts = std::to_string(++ft) + ".";
            q = moves.find(fts);
            if (q != std::string::npos)
            {
                move_pair = trim(moves.substr(0, q));
                moves = trim(moves.substr(q + fts.length()));
            }
            else
            {
                moves = "";
            }
            std::vector<std::string> pair = split_string(move_pair, ' ');
            if ((pair.size() == 0) || (pair.size() > 3))
            {
                errextra = "Full move sequence error";
                return e_loading;
            }
            std::string white = trim(pair[0]);
            if (terminal(white))
                return e_none;
            if (tc != c_white)
                return e_out_of_turn;
            chessmove m;
            error_e err = load_move(white, tc, b, m, errextra);
            if (err != e_none)
                return err;
            move_vec.push_back(m);
            tc = b.turn_color();
            std::string black;
            if (pair.size() > 1)
                black = pair[1];
            if (!terminal(black))
            {
                err = load_move(black, tc, b, m, errextra);
                if (err != e_none)
                    return err;
                move_vec.push_back(m);
                tc = b.turn_color();
            }
        }
        return e_none;
    }

}