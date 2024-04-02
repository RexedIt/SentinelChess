#include "chessboard.h"
#include "chesspiece.h"
#include "chessmove.h"

namespace chess
{

    chesspiece::chesspiece()
    {
        ptype = p_none;
        color = c_none;
        abbr = ' ';
        value = 0;
        kill_mask = 0;
        enemy_kill_mask = 0;
    }

    chesspiece::chesspiece(color_e c, piece_e p)
    {
        ptype = p;
        color = c;
        value = (unsigned char)(p + c);
        kill_mask = color * color_kill_mask_mult;
        enemy_kill_mask = (color == c_white) ? black_kill_mask : white_kill_mask;
        name = piece_str(ptype);
        abbr = abbr_char(ptype, color);
    }

    chesspiece::chesspiece(unsigned char c)
    {
        ptype = (piece_e)(c & piece_mask);
        color = (color_e)(c & color_mask);
        value = c;
        kill_mask = color * color_kill_mask_mult;
        enemy_kill_mask = (color == c_white) ? black_kill_mask : white_kill_mask;
        name = piece_str(ptype);
        abbr = abbr_char(ptype, color);
    }

    chesspiece::chesspiece(char code)
    {
        ptype = p_none;
        color = c_none;
        if (code != ' ')
        {
            color = (code >= 97) ? c_black : c_white;
            kill_mask = color * color_kill_mask_mult;
            enemy_kill_mask = (color == c_white) ? black_kill_mask : white_kill_mask;
            abbr = code;
            ptype = char_abbr(abbr);
        }
        value = (unsigned char)ptype + (unsigned char)color;
    }

    chesspiece::chesspiece(const chesspiece &oth)
    {
        copy(oth);
    }

    void chesspiece::copy(const chesspiece &oth)
    {
        ptype = oth.ptype;
        color = oth.color;
        abbr = oth.abbr;
        name = oth.name;
        value = oth.value;
        kill_mask = oth.kill_mask;
        enemy_kill_mask = oth.enemy_kill_mask;
    }

    void chesspiece::operator=(const chesspiece &oth)
    {
        copy(oth);
    }

    void chesspiece::possible_moves(std::vector<chessmove> &possible, coord_s p0, unsigned char (&cells)[8][8], unsigned char castled_left, unsigned char castled_right, coord_s ep)
    {
        switch (ptype)
        {
        case p_pawn:
            pawn_moves(possible, p0, cells, ep);
            break;
        case p_bishop:
            bishop_moves(possible, p0, cells);
            break;
        case p_knight:
            knight_moves(possible, p0, cells);
            break;
        case p_rook:
            rook_moves(possible, p0, cells);
            break;
        case p_queen:
            queen_moves(possible, p0, cells);
            break;
        case p_king:
            king_moves(possible, p0, cells, castled_left, castled_right);
            break;
        }
    }

    void chesspiece::pawn_moves(std::vector<chessmove> &possible, coord_s p0, unsigned char (&cells)[8][8], coord_s ep)
    {
        // black is on top so dy =  1;
        int8_t dy = color == c_black ? -1 : 1;
        int8_t hy = color == c_black ? 6 : 1;
        int8_t opp_y = color == c_black ? 0 : 7;

        if (in_range(p0.y + dy, p0.x))
        {
            if ((cells[p0.y + dy][p0.x] & piece_mask) == 0)
            {
                possible_pawn_move(possible, p0, dy, 0);
                // what about + 2 now
                if (p0.y == hy) // home row
                    if (in_range(p0.y + dy * 2, p0.x))
                        if ((cells[p0.y + dy * 2][p0.x] & piece_mask) == 0)
                            possible.push_back(new_move(p0, coord_s(p0.y + dy * 2, p0.x)));
            }
        }
        // Now, what about kills
        color_e enemy_col = color == c_black ? c_white : c_black;
        if (in_range(p0.y + dy, p0.x - 1))
        {
            if (is_color(cells[p0.y + dy][p0.x - 1], enemy_col))
                possible_pawn_move(possible, p0, dy, -1);
            // en passant
            else if ((ep.y == p0.y) && (ep.x == p0.x - 1) && (is_color(cells[p0.y + dy][p0.x - 1], c_none)))
                possible.push_back(new_move(p0, coord_s(p0.y + dy, p0.x - 1), -1, true));
        }
        if (in_range(p0.y + dy, p0.x + 1))
        {
            if (is_color(cells[p0.y + dy][p0.x + 1], enemy_col))
                possible_pawn_move(possible, p0, dy, 1);
            // en passant
            else if ((ep.y == p0.y) && (ep.x == p0.x + 1) && (is_color(cells[p0.y + dy][p0.x + 1], c_none)))
                possible.push_back(new_move(p0, coord_s(p0.y + dy, p0.x + 1), -1, true));
        }
    }

    void chesspiece::pawn_kills(coord_s p0, unsigned char (&cells)[8][8], int &kc)
    {
        // black is on top so dy =  1;
        int8_t dy = color == c_black ? -1 : 1;
        // Now, what about kills
        color_e enemy_col = color == c_black ? c_white : c_black;
        if (in_range(p0.y + dy, p0.x - 1))
        {
            cells[p0.y + dy][p0.x - 1] |= kill_mask;
            kc++;
        }
        if (in_range(p0.y + dy, p0.x + 1))
        {
            cells[p0.y + dy][p0.x + 1] |= kill_mask;
            kc++;
        }
    }

    void chesspiece::calc_direction_moves(std::vector<chessmove> &possible, coord_s p0, int8_t dy, int8_t dx, int8_t max, unsigned char (&cells)[8][8])
    {
        color_e enemy_col = other(color);
        int8_t y = p0.y + dy;
        int8_t x = p0.x + dx;
        int8_t moved = 0;
        while (in_range(y, x) && (moved < max))
        {
            color_e cell_color = (color_e)(cells[y][x] & color_mask);
            if (cell_color == c_none)
                possible.push_back(new_move(p0, coord_s(y, x)));
            else if (cell_color == color)
                break;
            else
            {
                possible.push_back(new_move(p0, coord_s(y, x)));
                break;
            }
            y += dy;
            x += dx;
            moved++;
        }
    }

    void chesspiece::calc_single_move(std::vector<chessmove> &possible, coord_s p0, int8_t dy, int8_t dx, unsigned char (&cells)[8][8])
    {
        int8_t y = p0.y + dy;
        int8_t x = p0.x + dx;
        if (!in_range(y, x))
            return;
        color_e cell_color = (color_e)(cells[y][x] & color_mask);
        if (cell_color != color)
            possible.push_back(new_move(p0, coord_s(y, x)));
    }

    void chesspiece::calc_king_move(std::vector<chessmove> &possible, coord_s p0, int8_t dy, int8_t dx, unsigned char (&cells)[8][8])
    {
        int8_t y = p0.y + dy;
        int8_t x = p0.x + dx;
        if (!in_range(y, x))
            return;
        color_e cell_color = (color_e)(cells[y][x] & color_mask);
        if (cell_color != color)
        {
            if ((cells[y][x] & enemy_kill_mask) != enemy_kill_mask)
                possible.push_back(new_move(p0, coord_s(y, x)));
        }
    }

    void chesspiece::bishop_moves(std::vector<chessmove> &possible, coord_s p0, unsigned char (&cells)[8][8])
    {
        calc_direction_moves(possible, p0, -1, -1, 8, cells);
        calc_direction_moves(possible, p0, -1, 1, 8, cells);
        calc_direction_moves(possible, p0, 1, -1, 8, cells);
        calc_direction_moves(possible, p0, 1, 1, 8, cells);
    }

    void chesspiece::calc_direction_kills(coord_s p0, int8_t dy, int8_t dx, int8_t max, unsigned char (&cells)[8][8], int &kc)
    {
        int8_t y = p0.y + dy;
        int8_t x = p0.x + dx;
        int8_t moved = 1;
        while (in_range(y, x) && (moved <= max))
        {
            cells[y][x] |= kill_mask;
            kc++;
            color_e cell_color = (color_e)(cells[y][x] & color_mask);
            if (cell_color != c_none)
                break;
            y += dy;
            x += dx;
            moved++;
        }
    }

    void chesspiece::bishop_kills(coord_s p0, unsigned char (&cells)[8][8], int &kc)
    {
        calc_direction_kills(p0, -1, -1, 8, cells, kc);
        calc_direction_kills(p0, -1, 1, 8, cells, kc);
        calc_direction_kills(p0, 1, -1, 8, cells, kc);
        calc_direction_kills(p0, 1, 1, 8, cells, kc);
    }

    void chesspiece::calc_single_kill(coord_s p0, int8_t dy, int8_t dx, unsigned char (&cells)[8][8], int &kc)
    {
        int8_t y = p0.y + dy;
        int8_t x = p0.x + dx;
        if (!in_range(y, x))
            return;
        cells[y][x] |= kill_mask;
        kc++;
    }

    void chesspiece::knight_moves(std::vector<chessmove> &possible, coord_s p0, unsigned char (&cells)[8][8])
    {
        calc_single_move(possible, p0, -1, 2, cells);
        calc_single_move(possible, p0, -1, -2, cells);
        calc_single_move(possible, p0, -2, 1, cells);
        calc_single_move(possible, p0, -2, -1, cells);
        calc_single_move(possible, p0, 1, -2, cells);
        calc_single_move(possible, p0, 1, 2, cells);
        calc_single_move(possible, p0, 2, -1, cells);
        calc_single_move(possible, p0, 2, 1, cells);
    }

    void chesspiece::knight_kills(coord_s p0, unsigned char (&cells)[8][8], int &kc)
    {
        calc_single_kill(p0, -1, 2, cells, kc);
        calc_single_kill(p0, -1, -2, cells, kc);
        calc_single_kill(p0, -2, 1, cells, kc);
        calc_single_kill(p0, -2, -1, cells, kc);
        calc_single_kill(p0, 1, -2, cells, kc);
        calc_single_kill(p0, 1, 2, cells, kc);
        calc_single_kill(p0, 2, -1, cells, kc);
        calc_single_kill(p0, 2, 1, cells, kc);
    }

    void chesspiece::rook_moves(std::vector<chessmove> &possible, coord_s p0, unsigned char (&cells)[8][8])
    {
        calc_direction_moves(possible, p0, -1, 0, 8, cells);
        calc_direction_moves(possible, p0, 0, -1, 8, cells);
        calc_direction_moves(possible, p0, 1, 0, 8, cells);
        calc_direction_moves(possible, p0, 0, 1, 8, cells);
    }

    void chesspiece::rook_kills(coord_s p0, unsigned char (&cells)[8][8], int &kc)
    {
        calc_direction_kills(p0, -1, 0, 8, cells, kc);
        calc_direction_kills(p0, 0, -1, 8, cells, kc);
        calc_direction_kills(p0, 1, 0, 8, cells, kc);
        calc_direction_kills(p0, 0, 1, 8, cells, kc);
    }

    void chesspiece::queen_moves(std::vector<chessmove> &possible, coord_s p0, unsigned char (&cells)[8][8])
    {
        calc_direction_moves(possible, p0, -1, 0, 8, cells);
        calc_direction_moves(possible, p0, 0, -1, 8, cells);
        calc_direction_moves(possible, p0, 1, 0, 8, cells);
        calc_direction_moves(possible, p0, 0, 1, 8, cells);
        calc_direction_moves(possible, p0, -1, -1, 8, cells);
        calc_direction_moves(possible, p0, -1, 1, 8, cells);
        calc_direction_moves(possible, p0, 1, -1, 8, cells);
        calc_direction_moves(possible, p0, 1, 1, 8, cells);
    }

    void chesspiece::queen_kills(coord_s p0, unsigned char (&cells)[8][8], int &kc)
    {
        calc_direction_kills(p0, -1, 0, 8, cells, kc);
        calc_direction_kills(p0, 0, -1, 8, cells, kc);
        calc_direction_kills(p0, 1, 0, 8, cells, kc);
        calc_direction_kills(p0, 0, 1, 8, cells, kc);
        calc_direction_kills(p0, -1, -1, 8, cells, kc);
        calc_direction_kills(p0, -1, 1, 8, cells, kc);
        calc_direction_kills(p0, 1, -1, 8, cells, kc);
        calc_direction_kills(p0, 1, 1, 8, cells, kc);
    }

    void chesspiece::king_moves(std::vector<chessmove> &possible, coord_s p0, unsigned char (&cells)[8][8], unsigned char castled_left, unsigned char castled_right)
    {
        // kill mask should be set so that should dictate
        // if this call returns a possible move (check)
        calc_king_move(possible, p0, -1, -1, cells);
        calc_king_move(possible, p0, -1, 0, cells);
        calc_king_move(possible, p0, -1, 1, cells);
        calc_king_move(possible, p0, 0, -1, cells);
        calc_king_move(possible, p0, 0, 1, cells);
        calc_king_move(possible, p0, 1, -1, cells);
        calc_king_move(possible, p0, 1, 0, cells);
        calc_king_move(possible, p0, 1, 1, cells);
        // Can we castle left?
        if ((castled_left & color) == 0)
            if (castle_path(p0, p0.y, 2, cells))
                possible.push_back(new_move(p0, coord_s(p0.y, 2), 0));
        // How about right?
        if ((castled_right & color) == 0)
            if (castle_path(p0, p0.y, 6, cells))
                possible.push_back(new_move(p0, coord_s(p0.y, 6), 7));
    }

    void chesspiece::king_kills(coord_s p0, unsigned char (&cells)[8][8], int &kc)
    {
        calc_single_kill(p0, -1, -1, cells, kc);
        calc_single_kill(p0, -1, 0, cells, kc);
        calc_single_kill(p0, -1, 1, cells, kc);
        calc_single_kill(p0, 0, -1, cells, kc);
        calc_single_kill(p0, 0, 1, cells, kc);
        calc_single_kill(p0, 1, -1, cells, kc);
        calc_single_kill(p0, 1, 0, cells, kc);
        calc_single_kill(p0, 1, 1, cells, kc);
    }

    bool chesspiece::castle_path(coord_s p0, int8_t y1, int8_t x1, unsigned char (&cells)[8][8])
    {
        int8_t dx = x1 - p0.x < 0 ? -1 : 1;
        int8_t x = p0.x;
        int8_t y = p0.y;
        unsigned char allowed = 255 - kill_mask;
        while (true)
        {
            if ((cells[y][x] & enemy_kill_mask) == enemy_kill_mask)
                return false;
            x += dx;
            if ((cells[y][x] & allowed) != 0)
                return false;
            if (x == x1)
                break;
        }
        return true;
    }

    void chesspiece::possible_pawn_move(std::vector<chessmove> &possible, coord_s p0, int8_t dy, int8_t dx)
    {
        int8_t y1 = p0.y + dy;
        int8_t x1 = p0.x + dx;
        coord_s p1 = coord_s(y1, x1);
        if ((y1 == 0) || (y1 == 7)) // promote
        {
            possible.push_back(new_move(p0, p1, p_knight));
            possible.push_back(new_move(p0, p1, p_bishop));
            possible.push_back(new_move(p0, p1, p_rook));
            possible.push_back(new_move(p0, p1, p_queen));
        }
        else
        {
            possible.push_back(new_move(p0, p1));
        }
    }

    void chesspiece::update_kill_bits(coord_s p0, unsigned char (&cells)[8][8], int &kc)
    {
        switch (ptype)
        {
        case p_pawn:
            pawn_kills(p0, cells, kc);
            break;
        case p_bishop:
            bishop_kills(p0, cells, kc);
            break;
        case p_knight:
            knight_kills(p0, cells, kc);
            break;
        case p_rook:
            rook_kills(p0, cells, kc);
            break;
        case p_queen:
            queen_kills(p0, cells, kc);
            break;
        case p_king:
            king_kills(p0, cells, kc);
            break;
        }
    }

}