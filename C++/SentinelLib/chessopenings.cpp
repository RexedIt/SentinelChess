#include "chessopenings.h"
#include "chessboard.h"

#include <fstream>
#include <iostream>

namespace chess
{

    chessopening::chessopening()
    {
        m_hash = 0;
    }

    chessopening::chessopening(std::string _eco, std::string _title, std::vector<chessmove> _moves)
    {
        eco = _eco;
        title = _title;
        moves = _moves;
        m_hash = 0;
    }

    chessopening::chessopening(const chessopening &oth)
    {
        copy(oth);
    }

    void chessopening::copy(const chessopening &oth)
    {
        eco = oth.eco;
        title = oth.title;
        moves = oth.moves;
        m_hash = 0;
    }

    void chessopening::operator=(const chessopening &oth)
    {
        copy(oth);
    }

    uint32_t chessopening::hash()
    {
        if (m_hash == 0)
            m_hash = chess::hash(moves);
        return m_hash;
    }

    chessopenings::chessopenings()
    {
    }

    chessopenings::~chessopenings()
    {
        m_op_hash.clear();
        m_op_name.clear();
    }

    error_e chessopenings::load_scid_eco(std::string filename)
    {
        try
        {
            m_op_hash.clear();
            m_op_name.clear();
            int lines = 0;
            std::string partial_line;
            std::ifstream pf(filename);
            std::string line;
            std::string errextra;
            error_e err = e_loading;
            while (std::getline(pf, line))
            {
                lines++;
                if (line.length())
                {
                    if (line[0] == '#')
                        continue;
                    if (line[line.length() - 1] == '*')
                    {
                        if (partial_line != "")
                            line = trim(partial_line) + " " + trim(line);
                        partial_line = "";
                        err = load_scid_line(line, errextra);
                        if (err != e_none)
                        {
                            std::cout << "Error, line " << lines << " : " << errorstr(err) << " - " << errextra << std::endl;
                        }
                    }
                    else
                    {
                        partial_line += " " + trim(line);
                    }
                }
            }
            std::cout << lines << " Read." << std::endl;
            return e_none;
        }
        catch (const std::exception &)
        {
            std::cout << "Error opening " << filename << std::endl;
            return e_loading;
        }
    }

    error_e chessopenings::load_scid_line(std::string line, std::string &errextra)
    {
        std::string eco;
        std::string title;
        std::string moves;
        errextra = "";
        size_t q = line.find('\"');
        if (q == std::string::npos)
        {
            errextra = "Title not enclosed in quotes";
            return e_loading;
        }
        eco = trim(line.substr(0, q));
        title = line.substr(q + 1);
        q = title.find('\"');
        if (q == std::string::npos)
        {
            errextra = "Title not enclosed in quotes";
            return e_loading;
        }
        moves = trim(title.substr(q + 1));
        title = trim(title.substr(0, q));

        chessboard b;
        b.load_xfen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        color_e tc = b.turn_color();
        std::vector<chessmove> move_vec;
        error_e err = e_none;

        // We want to move through the file in move pairs.
        int ft = 0;
        std::string fts = std::to_string(++ft) + ".";
        q = moves.find(fts);
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
            if (white == "*")
                return e_none;
            if (tc != c_white)
                return e_out_of_turn;
            chessmove m;
            err = str_move(white, tc, b, m);
            if (err != e_none)
            {
                errextra = "Move parse error - " + white;
                return err;
            }
            m = b.attempt_move(tc, m);
            if (m.error != e_none)
            {
                errextra = "Move execute error - " + white;
                return err;
            }
            move_vec.push_back(m);
            tc = b.turn_color();
            std::string black;
            if (pair.size() > 1)
            {
                black = pair[1];
                if (black == "*")
                    black = "";
            }
            if (black != "")
            {
                err = str_move(black, tc, b, m);
                if (err != e_none)
                {
                    errextra = "Move parse error - " + black;
                    return err;
                }
                m = b.attempt_move(tc, m);
                if (m.error != e_none)
                {
                    errextra = "Move execute error - " + black;
                    return err;
                }
                move_vec.push_back(m);
                tc = b.turn_color();
            }
            // for next
        }
        if (move_vec.size())
        {
            std::shared_ptr<chessopening> p_opening = std::shared_ptr<chessopening>(new chessopening(eco, title, move_vec));
            uint32_t hash = p_opening->hash();
            m_op_hash[hash] = p_opening;
            m_op_name[title] = p_opening;
        }
        return e_none;
    }

}