#include "chessboard.h"
#include "chesspiece.h"

namespace chess
{

    chesspiece::chesspiece()
    {
    }

    chesspiece::chesspiece(unsigned char c)
    {
        ptype = (piece_e)(c & piece_mask);
        color = (color_e)(c & color_mask);
        value = c;
        switch (ptype)
        {
        case p_pawn:
            abbr = "p";
            name = "Pawn";
            break;
        case p_bishop:
            abbr = "B";
            name = "Bishop";
            break;
        case p_knight:
            abbr = "k";
            name = "Knight";
            break;
        case p_rook:
            abbr = "R";
            name = "Rook";
            break;
        case p_queen:
            abbr = "Q";
            name = "Queen";
            break;
        case p_king:
            abbr = "K";
            name = "King";
            break;
        }
        if (color != c_none)
        {
            code = color == c_white ? 'W' : 'B';
            code += abbr;
        }
    }

    chesspiece::chesspiece(std::string code0)
    {
        if (code0.length() == 2)
        {
            code = code0;
            color = c_none;
            if (code0[0] == 'W')
                color = c_white;
            else if (code0[0] == 'B')
                color = c_black;
            abbr = code0[1];

            if (abbr == "p")
            {
                ptype = p_pawn;
                name = "Pawn";
            }
            else if (abbr == "B")
            {
                ptype = p_bishop;
                name = "Bishop";
            }
            else if (abbr == "k")
            {
                ptype = p_knight;
                name = "Knight";
            }
            else if (abbr == "R")
            {
                ptype = p_rook;
                name = "Rook";
            }
            else if (abbr == "Q")
            {
                ptype = p_queen;
                name = "Queen";
            }
            else if (abbr == "K")
            {
                ptype = p_king;
                name = "King";
            }
            value = (unsigned char)ptype + (unsigned char)color;
        }
    }

    void chesspiece::possible_moves(std::vector<move_s> &possible, short y0, short x0, unsigned char (&cells)[8][8])
    {
        switch (ptype)
        {
        case p_pawn:
            pawn_moves(possible, y0, x0, cells);
            break;
        case p_bishop:
            bishop_moves(possible, y0, x0, cells);
            break;
        case p_knight:
            knight_moves(possible, y0, x0, cells);
            break;
        case p_rook:
            rook_moves(possible, y0, x0, cells);
            break;
        case p_queen:
            queen_moves(possible, y0, x0, cells);
            break;
        case p_king:
            king_moves(possible, y0, x0, cells);
            break;
        }
    }

    move_s new_move(short y0, short x0, short y1, short x1, short cy = -1, short cx = -1)
    {
        move_s m(y0, x0, y1, x1, cy, cx);
        return m;
    }

    void chesspiece::pawn_moves(std::vector<move_s> &possible, short y0, short x0, unsigned char (&cells)[8][8])
    {
        // black is on top so dy =  1;
        short dy = color == c_black ? -1 : 1;
        short hy = color == c_black ? 6 : 1;

        if (in_range(y0 + dy, x0))
        {
            if (cells[y0 + dy][x0] == 0)
            {
                possible.push_back(new_move(y0, x0, y0 + dy, x0));
                // what about + 2 now
                if (y0 == hy) // home row
                    if (in_range(y0 + dy * 2, x0))
                        if (cells[y0 + dy * 2][x0] == 0)
                            possible.push_back(new_move(y0, x0, y0 + dy * 2, x0));
            }
        }
        // Now, what about kills
        color_e enemy_col = color == c_black ? c_white : c_black;
        if (in_range(y0 + dy, x0 - 1))
            if (is_color(cells[y0 + dy][x0 - 1], enemy_col))
                possible.push_back(new_move(y0, x0, y0 + dy, x0 - 1));
        if (in_range(y0 + dy, x0 + 1))
            if (is_color(cells[y0 + dy][x0 + 1], enemy_col))
                possible.push_back(new_move(y0, x0, y0 + dy, x0 + 1));
    }

    void chesspiece::pawn_kills(short y0, short x0, unsigned char (&cells)[8][8])
    {
        // black is on top so dy =  1;
        short dy = color == c_black ? -1 : 1;
        // Now, what about kills
        color_e enemy_col = color == c_black ? c_white : c_black;
        if (in_range(y0 + dy, x0 - 1))
            cells[y0 + dy][x0 - 1] |= kill_mask;
        if (in_range(y0 + dy, x0 + 1))
            cells[y0 + dy][x0 + 1] |= kill_mask;
    }

    void chesspiece::calc_direction_moves(std::vector<move_s> &possible, short y0, short x0, short dy, short dx, short max, unsigned char (&cells)[8][8])
    {
        color_e enemy_col = color == c_black ? c_white : c_black;
        short y = y0 + dy;
        short x = x0 + dx;
        short moved = 0;
        while (in_range(y, x) && (moved < max))
        {
            color_e cell_color = (color_e)(cells[y][x] & color_mask);
            if (cell_color == c_none)
                possible.push_back(new_move(y0, x0, y, x));
            else if (cell_color == color)
                break;
            else
            {
                possible.push_back(new_move(y0, x0, y, x));
                break;
            }
            y += dy;
            x += dx;
            moved++;
        }
    }

    void chesspiece::calc_single_move(std::vector<move_s> &possible, short y0, short x0, short y, short x, unsigned char (&cells)[8][8])
    {
        if (!in_range(y, x))
            return;
        color_e cell_color = (color_e)(cells[y][x] & color_mask);
        if (cell_color != color)
            possible.push_back(new_move(y0, x0, y, x));
    }

    void chesspiece::bishop_moves(std::vector<move_s> &possible, short y0, short x0, unsigned char (&cells)[8][8])
    {
        calc_direction_moves(possible, y0, x0, -1, -1, 8, cells);
        calc_direction_moves(possible, y0, x0, -1, 1, 8, cells);
        calc_direction_moves(possible, y0, x0, 1, -1, 8, cells);
        calc_direction_moves(possible, y0, x0, 1, 1, 8, cells);
    }

    void chesspiece::knight_moves(std::vector<move_s> &possible, short y0, short x0, unsigned char (&cells)[8][8])
    {
        calc_single_move(possible, y0, x0, y0 - 1, x0 - 2, cells);
        calc_single_move(possible, y0, x0, y0 - 1, x0 + 2, cells);
        calc_single_move(possible, y0, x0, y0 - 2, x0 - 1, cells);
        calc_single_move(possible, y0, x0, y0 - 2, x0 + 1, cells);
        calc_single_move(possible, y0, x0, y0 + 1, x0 - 2, cells);
        calc_single_move(possible, y0, x0, y0 + 1, x0 + 2, cells);
        calc_single_move(possible, y0, x0, y0 + 2, x0 - 1, cells);
        calc_single_move(possible, y0, x0, y0 + 2, x0 + 1, cells);
    }

    void chesspiece::rook_moves(std::vector<move_s> &possible, short y0, short x0, unsigned char (&cells)[8][8])
    {
        calc_direction_moves(possible, y0, x0, -1, 0, 8, cells);
        calc_direction_moves(possible, y0, x0, 0, -1, 8, cells);
        calc_direction_moves(possible, y0, x0, 1, 0, 8, cells);
        calc_direction_moves(possible, y0, x0, 0, 1, 8, cells);
    }

    void chesspiece::queen_moves(std::vector<move_s> &possible, short y0, short x0, unsigned char (&cells)[8][8])
    {
        calc_direction_moves(possible, y0, x0, -1, 0, 8, cells);
        calc_direction_moves(possible, y0, x0, 0, -1, 8, cells);
        calc_direction_moves(possible, y0, x0, 1, 0, 8, cells);
        calc_direction_moves(possible, y0, x0, 0, 1, 8, cells);
        calc_direction_moves(possible, y0, x0, -1, -1, 8, cells);
        calc_direction_moves(possible, y0, x0, -1, 1, 8, cells);
        calc_direction_moves(possible, y0, x0, 1, -1, 8, cells);
        calc_direction_moves(possible, y0, x0, 1, 1, 8, cells);
    }

    void chesspiece::king_moves(std::vector<move_s> &possible, short y0, short x0, unsigned char (&cells)[8][8])
    {
        calc_single_move(possible, y0, x0, y0 - 1, x0 - 1, cells);
        calc_single_move(possible, y0, x0, y0 - 1, x0, cells);
        calc_single_move(possible, y0, x0, y0 - 1, x0 + 1, cells);
        calc_single_move(possible, y0, x0, y0, x0 - 1, cells);
        calc_single_move(possible, y0, x0, y0, x0 + 1, cells);
        calc_single_move(possible, y0, x0, y0 + 1, x0 - 1, cells);
        calc_single_move(possible, y0, x0, y0 + 1, x0, cells);
        calc_single_move(possible, y0, x0, y0 + 1, x0 + 1, cells);
    }

    void chesspiece::update_kill_bits(short y0, short x0, unsigned char (&cells)[8][8])
    {
        switch (ptype)
        {
        case p_pawn:
            pawn_kills(y0, x0, cells);
            break;
        case p_bishop:
            bishop_kills(y0, x0, cells);
            break;
        case p_knight:
            knight_kills(y0, x0, cells);
            break;
        case p_rook:
            rook_kills(y0, x0, cells);
            break;
        case p_queen:
            queen_kills(y0, x0, cells);
            break;
        case p_king:
            king_kills(y0, x0, cells);
            break;
        }
    }

    void chesspiece::bishop_kills(short y0, short x0, unsigned char (&cells)[8][8])
    {
    }

    void chesspiece::knight_kills(short y0, short x0, unsigned char (&cells)[8][8])
    {
    }

    void chesspiece::rook_kills(short y0, short x0, unsigned char (&cells)[8][8])
    {
    }

    void chesspiece::queen_kills(short y0, short x0, unsigned char (&cells)[8][8])
    {
    }

    void chesspiece::king_kills(short y0, short x0, unsigned char (&cells)[8][8])
    {
    }

}