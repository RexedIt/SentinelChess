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

    chessopening::chessopening(std::string _eco, std::string _title, std::vector<chessmove> _moves, uint32_t _hash)
    {
        eco = _eco;
        title = _title;
        moves = _moves;
        m_hash = _hash;
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

    bool _save_binary(std::ofstream &of, size_t v)
    {
        of.write((char *)&v, sizeof(v));
        return true;
    }

    bool _load_binary(std::ifstream &inf, size_t &v)
    {
        inf.read((char *)&v, sizeof(v));
        return true;
    }

    bool _load_binary_u(std::ifstream &inf, uint32_t &v)
    {
        inf.read((char *)&v, sizeof(v));
        return true;
    }

    bool _save_binary(std::ofstream &of, uint32_t &v)
    {
        of.write((char *)&v, sizeof(v));
        return true;
    }

    bool _save_binary(std::ofstream &of, std::string &v)
    {
        _save_binary(of, v.length());
        of.write(v.c_str(), v.length());
        return true;
    }

    bool _load_binary(std::ifstream &inf, std::string &v)
    {
        size_t l = 0;
        _load_binary(inf, l);
        char buf[256];
        if ((l < 0) || (l > 255))
            return false;
        inf.read(buf, l);
        buf[l] = 0;
        v = std::string(buf);
        return true;
    }

    bool _save_binary(std::ofstream &of, chessmove v)
    {
        int8_t buf[5];
        buf[0] = v.p0.y;
        buf[1] = v.p0.x;
        buf[2] = v.p1.y;
        buf[3] = v.p1.x;
        of.write((char *)buf, 4);
        return true;
    }

    bool _load_binary(std::ifstream &inf, chessmove &v)
    {
        int8_t buf[5];
        inf.read((char *)&buf, 4);
        v = new_move(buf[0], buf[1], buf[2], buf[3]);
        return true;
    }

    bool _save_binary(std::ofstream &of, std::vector<chessmove> &v)
    {
        _save_binary(of, v.size());
        for (size_t i = 0; i < v.size(); i++)
            _save_binary(of, v[i]);
        return true;
    }

    bool _load_binary(std::ifstream &inf, std::vector<chessmove> &v)
    {
        size_t l = 0;
        _load_binary(inf, l);
        if ((l < 0) || (l > 64))
            return false;
        for (size_t i = 0; i < l; i++)
        {
            chessmove m;
            if (!_load_binary(inf, m))
                return false;
            v.push_back(m);
        }
        return true;
    }

    error_e chessopenings::save_binary(std::string filename)
    {
        try
        {
            std::ofstream outf(filename, std::ios::binary);
            // Save in hash order or m_op_hash
            // begin tag
            std::string tag = "\\/\\/\\/\\v1";
            _save_binary(outf, tag);
            _save_binary(outf, m_op_hash.size());
            std::map<uint32_t, std::shared_ptr<chessopening>>::iterator kv;
            for (kv = m_op_hash.begin(); kv != m_op_hash.end(); ++kv)
            {
                outf.put((char)255); // Demark each
                std::shared_ptr<chessopening> p_opening = kv->second;
                _save_binary(outf, kv->first);
                _save_binary(outf, p_opening->eco);
                _save_binary(outf, p_opening->title);
                _save_binary(outf, p_opening->moves);
            }
            outf.close();
            return e_none;
        }
        catch (const std::exception &)
        {
            return e_saving;
        }
    }

    error_e chessopenings::load_binary(std::string filename)
    {
        try
        {
            std::ifstream inf(filename, std::ios::binary);
            // Save in hash order or m_op_hash
            // begin tag
            std::string tag = "\\/\\/\\/\\v1";
            std::string ptag;
            if ((!_load_binary(inf, ptag) || (ptag != tag)))
            {
                inf.close();
                return e_loading;
            }
            m_op_hash.clear();
            m_op_name.clear();
            size_t num = 0;
            _load_binary(inf, num);
            for (size_t i = 0; i < num; i++)
            {
                uint32_t pos = inf.tellg();
                if (inf.get() != 255)
                {
                    inf.close();
                    return e_loading;
                }
                uint32_t hash = 0;
                std::string eco;
                std::string title;
                std::vector<chessmove> move_vec;
                _load_binary_u(inf, hash);
                _load_binary(inf, eco);
                _load_binary(inf, title);
                if (!_load_binary(inf, move_vec))
                {
                    inf.close();
                    return e_loading;
                }
                if ((hash != 0) && (move_vec.size() > 0))
                {
                    std::shared_ptr<chessopening> p_opening(new chessopening(eco, title, move_vec, hash));
                    m_op_hash[hash] = p_opening;
                    m_op_name[title] = p_opening;
                }
            }
            inf.close();
            return e_none;
        }
        catch (const std::exception &)
        {
            return e_saving;
        }
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
                            std::cout << "Error, line " << lines << " : " << errorstr(err) << " - " << errextra << std::endl;
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
        b.load_xfen(c_open_board);
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
            err = load_move(white, tc, b, m, errextra);
            if (err != e_none)
                return err;
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
                err = load_move(black, tc, b, m, errextra);
                if (err != e_none)
                    return err;
                move_vec.push_back(m);
                tc = b.turn_color();
            }
            // for next
        }
        if (move_vec.size())
        {
            std::shared_ptr<chessopening> p_opening = std::shared_ptr<chessopening>(new chessopening(eco, title, move_vec));
            uint32_t hash = p_opening->hash();
            if (m_op_hash.count(hash) > 0)
            {
                errextra = "Hash already found! " + std::to_string(hash);
                return e_loading;
            }
            else
            {
                m_op_hash[hash] = p_opening;
                m_op_name[title] = p_opening;
            }
        }
        return e_none;
    }

    error_e chessopenings::load_move(std::string s, color_e tc, chessboard &b, chessmove &m, std::string &errextra)
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

}