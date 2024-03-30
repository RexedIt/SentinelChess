#include "chessopenings.h"
#include "chessboard.h"

#include <fstream>
#include <iostream>

#include <set>

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

    bool chessopening::matches(std::vector<chessmove> filter)
    {
        if (moves.size() != filter.size())
            return false;
        size_t n = moves.size();
        for (size_t i = 0; i < n; i++)
            if (!filter[i].matches(moves[i]))
                return false;
        return true;
    }

    bool chessopening::startswith(std::vector<chessmove> filter)
    {
        if (filter.size() > moves.size())
            return false;
        size_t n = filter.size();
        for (size_t i = 0; i < n; i++)
            if (!filter[i].matches(moves[i]))
                return false;
        return true;
    }

    bool chessopening::get_move(size_t index, chessmove &m)
    {
        if (moves.size() > index)
        {
            m = moves[index];
            return true;
        }
        return false;
    }

    uint32_t chessopening::hash()
    {
        if (m_hash == 0)
            m_hash = chess::hash(moves);
        return m_hash;
    }

    chessecodb *p_eco = NULL;

    error_e get_chessopenings(std::vector<chessopening> &openings)
    {
        if (p_eco)
            return p_eco->chessopenings(openings);
        return e_no_openings;
    }

    chessecodb::chessecodb()
    {
        p_eco = this;
        initialize();
    }

    chessecodb::~chessecodb()
    {
        m_openings.clear();
        p_eco = NULL;
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

    error_e chessecodb::save_binary(std::string filename)
    {
        try
        {
            std::ofstream outf(filename, std::ios::binary);
            // Save in hash order or m_op_hash
            // begin tag
            std::string tag = "\\/\\/\\/\\v1";
            _save_binary(outf, tag);
            _save_binary(outf, m_openings.size());
            for (size_t i = 0; i < m_openings.size(); i++)
            {
                chessopening opening = m_openings[i];
                outf.put((char)255); // Demark each
                _save_binary(outf, opening.hash());
                _save_binary(outf, opening.eco);
                _save_binary(outf, opening.title);
                _save_binary(outf, opening.moves);
            }
            outf.close();
            return e_none;
        }
        catch (const std::exception &)
        {
            return e_saving;
        }
    }

    error_e chessecodb::chessopenings(std::vector<chessopening> &openings)
    {
        if (m_openings.size() == 0)
            return e_no_openings;
        openings = m_openings;
        return e_none;
    }

    error_e chessecodb::preferredecos(color_e col, std::set<std::string> &ecos)
    {
        if (m_preferred.count(col) == 0)
            return e_no_openings;
        ecos = m_preferred[col];
        return e_none;
    }

    error_e chessecodb::load_binary(std::string filename)
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
            m_openings.clear();
            size_t num = 0;
            _load_binary(inf, num);
            for (size_t i = 0; i < num; i++)
            {
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
                    chessopening opening(eco, title, move_vec, hash);
                    m_openings.push_back(opening);
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

    void chessecodb::initialize()
    {
        m_preferred[c_white] = std::set<std::string>({"D00", "C21", "C22", "C23", "C25", "C26", "C27", "C28", "D06", "D07", "D08", "D09", "C60", "C44", "C45", "A00", "A10", "A04"});
        m_preferred[c_black] = std::set<std::string>({"B20", "B00", "B03", "B04", "B05", "A00", "A42", "E12", "E13", "E14", "E15", "E16", "E17", "E18", "E19"});
    }

    error_e chessecodb::load_scid_eco(std::string filename)
    {
        try
        {
            m_openings.clear();
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

    std::string clean_title(std::string orig)
    {
        // comma
        size_t c = orig.find_last_of(',');
        if (c != std::string::npos)
            return orig.substr(0, c);
        c = orig.find('.'); // has moves, we will now truncate to :
        if (c != std::string::npos)
        {
            c = orig.find(':');
            if (c != std::string::npos)
                return orig.substr(0, c);
        }
        return orig;
    }

    error_e chessecodb::load_scid_line(std::string line, std::string &errextra)
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
        eco = trim(line.substr(0, q > 3 ? 3 : q));
        title = line.substr(q + 1);
        q = title.find('\"');
        if (q == std::string::npos)
        {
            errextra = "Title not enclosed in quotes";
            return e_loading;
        }
        moves = trim(title.substr(q + 1));
        if (moves == "*")
            return e_none;

        title = clean_title(trim(title.substr(0, q)));

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
            chessopening opening(eco, title, move_vec);
            m_openings.push_back(opening);
        }
        return e_none;
    }

    error_e chessecodb::load_move(std::string s, color_e tc, chessboard &b, chessmove &m, std::string &errextra)
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

    chessopenfilter::chessopenfilter()
    {
        get_chessopenings(m_openings);
        m_filtered = m_openings;
        m_last_match_size = 0;
    }

    chessopenfilter::~chessopenfilter()
    {
    }

    void chessopenfilter::reset()
    {
        m_filtered = m_openings;
        m_last.clear();
        m_eco = "";
        m_title = "";
        m_last_match_size = 0;
    }

    error_e chessopenfilter::narrow(std::vector<chessmove> &filter)
    {
        // if we are rewinding reset
        if (filter.size() < m_last.size())
            reset();
        if (equals(filter, m_last))
            return e_none;
        // Okay we can refilter
        std::vector<chessopening> refilter;
        for (size_t i = 0; i < m_filtered.size(); i++)
            if (m_filtered[i].startswith(filter))
                refilter.push_back(m_filtered[i]);
        m_filtered = refilter;
        m_last = filter;
        return e_none;
    }

    std::string chessopenfilter::eco()
    {
        find_best_opening_match();
        return m_eco;
    }

    std::string chessopenfilter::title()
    {
        find_best_opening_match();
        return m_title;
    }

    std::vector<chessopening> chessopenfilter::possible_openings()
    {
        return m_filtered;
    }

    size_t chessopenfilter::possible_opening_count()
    {
        return m_filtered.size();
    }

    error_e chessopenfilter::next_opening_moves(color_e col, std::string eco, std::vector<chessmove> &possible)
    {
        size_t index = m_last.size();
        if ((col == c_black) && (index % 2 == 0))
            return e_out_of_turn;
        if ((col == c_white) && (index % 2 == 1))
            return e_out_of_turn;
        possible.clear();
        std::set<int32_t> move_set;
        for (size_t i = 0; i < m_filtered.size(); i++)
        {
            if (eco != "")
                if (!starts_with(m_filtered[i].eco, eco))
                    continue;
            chessmove m;
            if (m_filtered[i].get_move(index, m))
            {
                int32_t mi = m.intval();
                if (move_set.count(mi) == 0)
                {
                    possible.push_back(m);
                    move_set.insert(mi);
                }
            }
        }
        // reload possible with actual moves
        return e_none;
    }

    void chessopenfilter::find_best_opening_match()
    {
        // Don't do this repeatedly
        if (m_last_match_size == m_filtered.size())
            return;
        if (m_filtered.size() > 0)
        {
            m_last_match_size = m_filtered.size();
            std::map<std::string, int> titles;
            std::map<std::string, int> ecos;
            for (size_t i = 0; i < m_filtered.size(); i++)
            {
                std::string title = m_filtered[i].title;
                std::string eco = m_filtered[i].eco;
                if (titles.count(title))
                    titles[title] = titles[title] + 1;
                else
                    titles[title] = 1;
                if (ecos.count(eco))
                    ecos[eco] = ecos[eco] + 1;
                else
                    ecos[eco] = 1;
            }
            // We could do this with a transform but more readable and about the same time?
            std::map<int, std::string> titles_sorted;
            std::map<int, std::string> ecos_sorted;
            for (auto kv : titles)
                titles_sorted[kv.second] = kv.first;
            for (auto kv : ecos)
                ecos_sorted[kv.second] = kv.first;
            std::map<int, std::string>::reverse_iterator mi;
            mi = titles_sorted.rbegin();
            if (mi != titles_sorted.rend())
                m_title = mi->second;
            mi = ecos_sorted.rbegin();
            if (mi != ecos_sorted.rend())
                m_eco = mi->second;
        }
    }
}