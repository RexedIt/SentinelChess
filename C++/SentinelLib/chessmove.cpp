#include "chessmove.h"
#include "chessboard.h"
#include "chesspiece.h"

namespace chess
{

    chessmove::chessmove()
    {
        cx = -1;
        en_passant = false;
        promote = p_none;
        check = false;
        mate = false;
        kill = false;
        error = e_none;
    }

    chessmove::chessmove(int32_t _i)
    {
        int8_t *pi = (int8_t *)&_i;
        p0.y = pi[0];
        p0.x = pi[1];
        p1.y = pi[2];
        p1.x = pi[3];
        cx = -1;
        en_passant = false;
        promote = p_none;
        check = false;
        mate = false;
        kill = false;
        error = e_none;
    }

    chessmove::chessmove(coord_s _p0, coord_s _p1, int8_t _cx, bool _en_passant)
    {
        p0 = _p0;
        p1 = _p1;
        cx = _cx;
        en_passant = _en_passant;
        promote = p_none;
        check = false;
        mate = false;
        kill = false;
        error = e_none;
    }
    chessmove::chessmove(coord_s _p0, coord_s _p1, piece_e _promote)
    {
        p0 = _p0;
        p1 = _p1;
        cx = -1;
        en_passant = false;
        promote = _promote;
        check = false;
        mate = false;
        kill = false;
        error = e_none;
    }

    std::string chessmove::to_string()
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
            s += " Promote to " + p.name();
        }
        return s;
    }

    bool chessmove::is_valid()
    {
        return ((p0.y != -1) && (error == e_none));
    }

    void chessmove::invalidate()
    {
        p0.x = -1;
        p0.y = -1;
        p1.x = -1;
        p1.y = -1;
        cx = -1;
        mate = false;
        check = false;
        kill = false;
        en_passant = false;
        promote = p_none;
        error = e_none;
    }

    bool chessmove::matches(const chessmove &m)
    {
        if (p0 == m.p0)
            if (p1 == m.p1)
                return true;
        return false;
    }

    int32_t chessmove::intval() const
    {
        int32_t ret;
        int8_t *pret = (int8_t *)&ret;
        pret[0] = p0.y;
        pret[1] = p0.x;
        pret[2] = p1.y;
        pret[3] = p1.x;
        return ret;
    }

    std::string move_str(chessmove m)
    {
        return coord_str(m.p0) + "-" + coord_str(m.p1);
    }

    std::string move_str_c4(chessmove m)
    {
        return coord_str(m.p0) + coord_str(m.p1);
    }

    std::string move_str_c4(std::vector<chessmove> v)
    {
        std::string ret = "";
        for (chessmove m : v)
        {
            if (ret != "")
                ret += " ";
            ret += move_str_c4(m);
        }
        return ret;
    }

    std::string move_str(chessmove m, chessboard &b)
    {
        std::string bs = b.save_xfen();
        chesspiece p(b.get(m.p0));

        std::string ms = "";
        if (m.cx != -1) // castling moves
        {
            if (m.cx == 6)
                ms = "O-O-O";
            else
                ms = "O-O";
        }
        else
        {
            if (p.ptype != p_pawn)
            {
                char pc = p.abbr;
                if (pc >= 'a')
                    pc -= 32;
                ms += pc;
            }
            // disambiguate now.
            std::vector<chessmove> da_vec = b.possible_moves(p.color, false);
            std::string da = "";
            unsigned char my_piece = p.value & piece_and_color_mask;
            for (size_t i = 0; i < da_vec.size(); i++)
            {
                unsigned char other_piece = b.get(da_vec[i].p0) & piece_and_color_mask;
                if ((my_piece == other_piece) && (da_vec[i].p0 != m.p0) && (da_vec[i].p1 == m.p1))
                {
                    std::string ret = "";
                    if (da_vec[i].p0.x != m.p0.x)
                        da = 'a' + m.p0.x;
                    else if (da_vec[i].p0.y != m.p0.y)
                        da = '1' + m.p0.y;
                    break;
                }
            }
            if (p.ptype == p_pawn)
            {
                if ((m.kill) && (da == ""))
                    da = 'a' + m.p0.x;
            }
            ms += da;
            if (m.kill)
                ms += "x";
            ms += coord_str(m.p1);
            if (m.promote)
            {
                char pc = abbr_char(m.promote, p.color);
                if (pc >= 'a')
                    pc -= 32;
                ms += "=";
                ms += pc;
            }
            if (m.mate)
                ms += "#";
        }
        color_e tc = b.turn_color();
        chessmove m1 = b.attempt_move(tc, m);
        if (m1.error != e_none)
            return "";
        if (m1.mate)
            ms += "#";
        else if (b.check_state(other(tc)))
            ms += "+";
        return ms;
    }

    error_e str_move(std::string s, chessmove &m)
    {
        // Simple parse only!
        std::vector<std::string> c = split_string(s, '-');
        if ((c.size() == 1) && (s.length() >= 4))
        {
            if (s.length() > 5)
                return e_invalid_move;
            c.clear();
            c.push_back(s.substr(0, 2));
            c.push_back(s.substr(2, 2));
            if (s.length() == 5)
                c.push_back(s.substr(4, 1));
        }
        if ((c.size() < 2) || (c.size() > 3))
            return e_invalid_move;
        if ((!coord_int(c[0], m.p0)) ||
            (!coord_int(c[1], m.p1)))
            return e_invalid_move;
        if (c.size() == 3)
            m.promote = char_abbr(c[2][0]);
        return e_none;
    }

    piece_e char_abbr_an(char c)
    {
        switch (c)
        {
        case 'P':
            return p_pawn;
        case 'B':
            return p_bishop;
        case 'N':
            return p_knight;
        case 'R':
            return p_rook;
        case 'K':
            return p_king;
        case 'Q':
            return p_queen;
        }
        return p_none;
    }

    bool is_coordinate_pair(std::string s)
    {
        size_t l = s.length();
        if (l < 4)
            return false;
        if (l >= 5)
            if (s[2] == '-')
            {
                if (is_coord(lowercase(s.substr(0, 2))))
                    if (is_coord(lowercase(s.substr(3, 2))))
                        return true;
                return false;
            }
        // pure 4 digit move algebraic notation
        if (((l == 4) || (l == 5)) && (s == lowercase(s)))
        {
            if (is_coord(lowercase(s.substr(0, 2))))
                if (is_coord(lowercase(s.substr(2, 2))))
                    return true;
        }
        return false;
    }

    error_e str_move(std::string s0, color_e col, chessboard &b, chessmove &m)
    {
        // Is it a coordinate move?
        std::string s = s0;
        size_t l = s.length();
        if (l < 2)
            return e_invalid_move;
        if (is_coordinate_pair(s))
            return str_move(s, m);
        std::vector<chessmove> poss;
        // Handle castlingit is vastly different
        // note we are using 'contains move' to
        // find and conform a move with the necessary details for our engine
        bool check = ends_with(s, "+");
        m.check = false;
        if (check)
            s = s.substr(0, s.length() - 1);
        int8_t our_cx = -1;
        if ((s == "O-O") || (s == "0-0"))
            our_cx = 6;
        else if ((s == "O-O-O") || (s == "0-0-0"))
            our_cx = 2;
        if (our_cx != -1)
        {
            // Kingside Castling
            poss = b.possible_moves(col, p_king, -1, -1, false);
            int8_t kingsrow = (col == c_white) ? 0 : 7;
            m = new_move(kingsrow, 4, kingsrow, our_cx);
            if (contains_move(poss, m, true))
                return e_none;
        }
        // strip out any special characters, this includes
        // those for promotion assignment as well as check
        // and checkmate etc as those are determined by
        // our engine and can be rebuilt
        std::string si;
        bool kill = false;
        for (size_t i = 0; i < l; i++)
        {
            char c = s[i];
            bool inc = false;
            if ((c >= 'a') && (c <= 'h'))
                inc = true;
            if ((c >= '1') && (c <= '8'))
                inc = true;
            if ((c == 'N') || (c == 'B') || (c == 'R') || (c == 'Q') || (c == 'K'))
                inc = true;
            if (c == 'x')
                kill = true;
            if (inc)
                si += c;
        }
        l = si.length();
        if (l < 2)
            return e_invalid_move;
        // Now the string has been rebuilt, let's look at the
        // last character.  If it is a piece character,
        // let's assign promote and cut that off so that
        // the last 2 characters are expected to be the destination
        // coordinate.
        piece_e promote = char_abbr_an(si[l - 1]);
        if (promote != p_none)
        {
            m.promote = promote;
            si = si.substr(0, l - 1);
            l = si.length();
        }
        // If no piece indicated, it is possibly a pawn move
        piece_e p = p_pawn; // the default if none provided
        int8_t yc = -1;
        int8_t xc = -1;
        bool cap = false;
        // Determine the components based on their character ranges.
        // note we are expecting the last two characters to be destination
        for (size_t i = 0; i < l - 2; i++)
        {
            char c = si[i];
            if ((c >= 'a') && (c <= 'h'))
                xc = c - 'a';
            if ((c >= '1') && (c <= '8'))
                yc = c - '1';
            if (c == 'x')
                cap = true;
            piece_e p1 = char_abbr_an(c);
            if (p1 != p_none)
                p = p1;
        }
        // let's now observe the destination coordinate
        std::string ms = si.substr(l - 2);
        if (coord_int(ms, m.p1))
        {
            poss = b.possible_moves(col, p, yc, xc, false);
            if (contains_move_dest(poss, m))
            {
                // Double check capture
                if (cap)
                {
                    piece_e dp = b.get_piece(m.p1);
                    if (dp == p_none)
                        return e_invalid_move;
                }
                if (p == p_pawn)
                {
                    // last check for promote
                    int8_t promoterow = (col == c_white) ? 7 : 0;
                    if ((m.p1.y == promoterow) && (promote == p_none))
                        return e_invalid_move;
                    m.promote = promote;
                    // and en-passant
                    // *** REM *** TODO
                }
                m.check = check;
                m.kill = kill;
                return e_none;
            }
        }
        return e_invalid_move;
    }

    bool contains_move(std::vector<chessmove> possible_moves, chessmove &m, bool inherit)
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
                        if (possible_moves[i].promote == 0) // don't do this for promotion
                            m = possible_moves[i];
                    return true;
                }
        }
        return false;
    }

    bool contains_move_dest(std::vector<chessmove> possible_moves, chessmove &m)
    {
        // assumes that possible moves have been narrowed.
        int count = 0;
        for (size_t i = 0; i < possible_moves.size(); i++)
        {
            if (possible_moves[i].p1 == m.p1)
            {
                // inherit is allowing a user move to pick up the en passant and
                // castle flags the CPU would have determined possible for the
                // square so the user move need only be a coordinate not additional
                // instructions.  This is just an exception for pawns
                if ((m.promote == p_none) || (m.promote == possible_moves[i].promote))
                {
                    m = possible_moves[i];
                    count++;
                }
            }
        }
        return (count == 1);
    }

    bool equals(std::vector<chessmove> &a, std::vector<chessmove> &b)
    {
        if (a.size() != b.size())
            return false;
        for (size_t i = 0; i < a.size(); i++)
            if (!a[i].matches(b[i]))
                return false;
        return true;
    }

    bool contains(std::vector<chessmove> &a, chessmove &b)
    {
        for (size_t i = 0; i < a.size(); i++)
            if (a[i].matches(b))
                return true;
        return false;
    }

    chessmove new_move(int8_t y0, int8_t x0, int8_t y1, int8_t x1)
    {
        chessmove m(coord_s(y0, x0), coord_s(y1, x1));
        return m;
    }

    chessmove new_move(coord_s p0, coord_s p1, int8_t cx, bool en_passant)
    {
        chessmove m(p0, p1, cx, en_passant);
        return m;
    }

    chessmove new_move(coord_s p0, coord_s p1, piece_e promote)
    {
        chessmove m(p0, p1, promote);
        return m;
    }

    chessmove new_move(std::string s, color_e c, chessboard &b)
    {
        chessmove m;
        error_e err = str_move(s, c, b, m);
        m.error = err;
        return m;
    }
}