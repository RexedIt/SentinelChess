#include <ctime>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>

#include "chesscommon.h"
#include "chesspiece.h"

namespace chess
{

    std::string move_s::to_string()
    {
        std::string s = "";
        s += coord_str(p0) + " to " + coord_str(p1);
        if (cx != -1)
        {
            s += " Castle ";
            s += (cx == 0) ? "Queen Side" : "King Side";
        }
        if (en_passant)
            s += " En Passant";
        if (promote != p_none)
        {
            chesspiece p((unsigned char)promote);
            s += " Promote to " + p.name;
        }
        return s;
    }

    bool move_s::is_valid()
    {
        return (p0.y != -1);
    }

    void move_s::invalidate()
    {
        p0.x = -1;
        p0.y = -1;
        p1.x = -1;
        p1.y = -1;
        cx = -1;
        en_passant = false;
        promote = p_none;
        terminal = false;
    }

    bool move_s::matches(const move_s &m)
    {
        if (p0 == m.p0)
            if (p1 == m.p1)
                return true;
        return false;
    }

    std::string color_str(color_e col)
    {
        switch (col)
        {
        case c_black:
            return "Black";
        case c_white:
            return "White";
        }
        return "None";
    }

    std::string coord_str(coord_s s)
    {
        char pos[3];
        pos[0] = 'a' + s.x;
        pos[1] = '1' + s.y;
        pos[2] = 0;
        return std::string(pos);
    }

    bool coord_int(std::string s, coord_s &c)
    {
        if (s.length() != 2)
            return false;
        s = lowercase(s);
        c.x = (int8_t)(s[0] - 'a');
        c.y = (int8_t)(s[1] - '1');
        return true;
    }

    bool in_range(coord_s s)
    {
        if ((s.y < 0) || (s.y > 7))
            return false;
        if ((s.x < 0) || (s.x > 7))
            return false;
        return true;
    }

    bool in_range(int8_t y, int8_t x)
    {
        if ((y < 0) || (y > 7))
            return false;
        if ((x < 0) || (x > 7))
            return false;
        return true;
    }

    bool is_color(unsigned char cell, color_e color)
    {
        return (color_e)(cell & color_mask) == color;
    }

    color_e other(color_e t)
    {
        return t == c_white ? c_black : c_white;
    }

    bool contains_move(std::vector<move_s> possible_moves, move_s &m, bool inherit)
    {
        for (size_t i = 0; i < possible_moves.size(); i++)
        {
            if (possible_moves[i].p0 == m.p0)
                if (possible_moves[i].p1 == m.p1)
                {
                    // inherit is allowing a user move to pick up the en passant and
                    // castle flags the CPU would have determined possible for the
                    // square so the user move need only be a coordinate not additional
                    // instructions.
                    if (inherit)
                        m = possible_moves[i];
                    return true;
                }
        }
        return false;
    }

    move_s new_move(coord_s p0, coord_s p1, int8_t cx, bool en_passant)
    {
        move_s m(p0, p1, cx, en_passant);
        return m;
    }

    move_s new_move(coord_s p0, coord_s p1, piece_e promote)
    {
        move_s m(p0, p1, promote);
        return m;
    }

    static volatile bool _sinit = false;
    float get_rand()
    {
        if (!_sinit)
        {
            srand((int)time(NULL));
            _sinit = true;
        }
        return (float)rand() / (float)RAND_MAX;
    }

    std::string uppercase(std::string l)
    {
        std::string cmdu = l;
        std::transform(cmdu.begin(), cmdu.end(), cmdu.begin(), ::toupper);
        return cmdu;
    }

    std::string lowercase(std::string u)
    {
        std::string cmdu = u;
        std::transform(cmdu.begin(), cmdu.end(), cmdu.begin(), ::tolower);
        return cmdu;
    }

    std::vector<std::string> split_string(std::string cmd, char div)
    {
        std::vector<std::string> ret;
        std::string rem = cmd;
        size_t pos = rem.find(div);
        while (pos != std::string::npos)
        {
            ret.push_back(rem.substr(0, pos));
            rem = rem.substr(pos + 1);
            pos = rem.find(div);
        }
        if (rem != "")
            ret.push_back(rem);
        return ret;
    }

    void write_hex_uchar(std::ofstream &file1, unsigned char c)
    {
        file1 << std::hex << std::setw(2) << std::setfill('0') << (int)c;
        file1 << std::endl;
    }

    unsigned char read_hex_uchar(std::string line)
    {
        return (unsigned char)std::stoi(line, 0, 16);
    }

}