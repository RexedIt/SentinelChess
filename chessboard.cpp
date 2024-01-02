#include "chessboard.h"
#include "chesspiece.h"

#include <memory.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
#include <sstream>

namespace chess
{

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

    std::string coord_str(int8_t y, int8_t x)
    {
        char pos[3];
        pos[0] = '1' + y;
        pos[1] = 'A' + x;
        pos[2] = 0;
        return std::string(pos);
    }

    bool coord_int(std::string s, int8_t &y, int8_t &x)
    {
        if (s.length() != 2)
            return false;
        x = (int8_t)(s[0] - 'A');
        y = (int8_t)(s[1] - '1');
        return true;
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

    std::string move_s::to_string()
    {
        std::string s = "";
        s += coord_str(y0, x0) + " to " + coord_str(y1, x1);
        if (cx != -1)
            s += " Castle " + (cx == 0) ? "Queen Side" : "King Side";
        if (en_passant)
            s += " En Passant";
        if (promote != p_none)
        {
            chesspiece p(promote);
            s += " Promote to " + p.name;
        }
        return s;
    }

    bool move_s::is_valid()
    {
        return (y0 != -1);
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

    bool contains_move(std::vector<move_s> possible_moves, move_s m)
    {
        for (size_t i = 0; i < possible_moves.size(); i++)
        {
            if (possible_moves[i].y1 == m.y1)
                if (possible_moves[i].x1 == m.x1)
                    if (possible_moves[i].y0 == m.y0)
                        if (possible_moves[i].x0 == m.x0)
                            return true;
        }
        return false;
    }

    chessboard::chessboard()
    {
        memset(&m_cells, 0, sizeof(m_cells));
        m_castled_left = 0;
        m_castled_right = 0;
        m_ep_y = -1;
        m_ep_x = -1;
        m_check = 0;
        m_turn = c_white;
        m_hash = 0;
    }

    chessboard::chessboard(chessboard &other)
    {
        memcpy(&m_cells, other.m_cells, sizeof(m_cells));
        m_castled_left = other.m_castled_left;
        m_castled_right = other.m_castled_right;
        m_ep_y = other.m_ep_y;
        m_ep_x = other.m_ep_x;
        m_check = other.m_check;
        m_turn = other.m_turn;
        m_hash = 0;
    }

    void chessboard::new_game()
    {
        set_kings_row(0, c_white);
        set_pawns_row(1, c_white);
        set_empty_rows();
        set_pawns_row(6, c_black);
        set_kings_row(7, c_black);
        m_castled_left = 0;
        m_castled_right = 0;
        m_ep_y = 0;
        m_ep_x = 0;
        m_check = 0;
        m_turn = c_white;
        m_hash = 0;
    }

    void write_hex_uchar(std::ofstream &file1, unsigned char c)
    {
        file1 << std::hex << std::setw(2) << std::setfill('0') << (int)c;
        file1 << std::endl;
    }

    bool chessboard::save_game(std::string filename)
    {
        std::ofstream file1;
        file1.open(filename);
        if (!file1.is_open())
            return false;
        for (int8_t y = 0; y < 8; y++)
        {
            for (int8_t x = 0; x < 8; x++)
            {
                chesspiece p(m_cells[y][x]);
                file1 << p.code + " ";
            }
            file1 << std::endl;
        }
        file1 << std::endl;
        write_hex_uchar(file1, m_castled_left);
        write_hex_uchar(file1, m_castled_right);
        write_hex_uchar(file1, m_check);
        write_hex_uchar(file1, m_turn);
        file1 << std::to_string(m_ep_y) << std::endl;
        file1 << std::to_string(m_ep_x) << std::endl;
        file1.close();
        return true;
    }

    unsigned char read_hex_uchar(std::string line)
    {
        return (unsigned char)std::stoi(line, 0, 16);
    }

    bool chessboard::load_game(std::string filename)
    {
        std::ifstream file1;
        file1.open(filename);
        if (!file1.is_open())
            return false;
        std::string line;
        int lineno = 0;
        while (std::getline(file1, line))
        {
            if (lineno < 8)
            {
                int8_t y = lineno;
                for (int8_t x = 0; x < 8; x++)
                {
                    std::string piecestr = line.substr(x * 3, 2);
                    chesspiece p(piecestr);
                    m_cells[y][x] = p.value;
                }
            }
            else if (lineno == 9)
                m_castled_left = read_hex_uchar(line);
            else if (lineno == 10)
                m_castled_right = read_hex_uchar(line);
            else if (lineno == 11)
                m_check = read_hex_uchar(line);
            else if (lineno == 12)
                m_turn = (color_e)read_hex_uchar(line);
            else if (lineno == 13)
                m_ep_y = std::stoi(line);
            else if (lineno == 14)
                m_ep_x = std::stoi(line);
            lineno++;
        }
        m_hash = 0;
        file1.close();
        return lineno == 13;
    }

    void chessboard::set_kings_row(int8_t y, color_e col)
    {
        m_cells[y][0] = col + p_rook;
        m_cells[y][1] = col + p_knight;
        m_cells[y][2] = col + p_bishop;
        m_cells[y][3] = col + p_queen;
        m_cells[y][4] = col + p_king;
        m_cells[y][5] = col + p_bishop;
        m_cells[y][6] = col + p_knight;
        m_cells[y][7] = col + p_rook;
    }

    void chessboard::set_pawns_row(int8_t y, color_e col)
    {
        for (int8_t j = 0; j < 8; j++)
            m_cells[y][j] = col + p_pawn;
    }

    void chessboard::set_empty_rows()
    {
        for (int i = 2; i < 6; i++)
            for (int j = 0; j < 8; j++)
                m_cells[i][j] = 0;
    }

    float chessboard::weight(color_e col)
    {
        float w = 0;
        for (int8_t y = 0; y < 8; y++)
            for (int8_t x = 0; x < 8; x++)
                if (m_cells[y][x] != 0)
                {
                    if (m_cells[y][x] & col)
                        w += (float)(m_cells[y][x] & piece_mask);
                    else
                        w -= (float)(m_cells[y][x] & piece_mask);
                }
        // 120 is full set
        // 50 is king only (the real minimum)
        return w;
    }

    unsigned char chessboard::get(int8_t y, int8_t x)
    {
        if ((y < 0) || (y > 7))
            return 0;
        if ((x < 0) || (x > 7))
            return 0;
        return m_cells[y][x];
    }

    piece_e chessboard::get_piece(int8_t y, int8_t x)
    {
        return (piece_e)(get(y, x) & piece_mask);
    }

    bool chessboard::find_piece(piece_e pc, color_e col, int8_t &y, int8_t &x)
    {
        unsigned char cv = (unsigned char)pc + (unsigned char)col;
        for (y = 0; y < 8; y++)
            for (x = 0; x < 8; x++)
                if ((m_cells[y][x] & piece_and_color_mask) == cv)
                    return true;
        y = -1;
        x = -1;
        return false;
    }

    bool chessboard::find_check(color_e turn_col)
    {
        // Are we in check?  If so let's see if mate.
        int8_t ky = 0;
        int8_t kx = 0;
        find_piece(p_king, turn_col, ky, kx);
        bool ret = (m_cells[ky][kx] & kill_mask) == kill_mask;
        if (ret)
            m_check |= turn_col;
        else
            m_check &= 255 - turn_col;
        return ret;
    }

    move_s chessboard::is_game_over(color_e col)
    {
        move_s m;
        std::vector<move_s> possible = possible_moves(col);
        if (find_check(col))
            m.check = true;
        m.mate = true;
        for (size_t i = 0; i < possible.size(); i++)
        {
            chessboard b(*this);
            b.move(possible[i]);
            if (!b.find_check(col))
            {
                m = possible[i];
                m.mate = false;
                break;
            }
        }
        return m;
    }

    void chessboard::evaluate_check_and_mate(color_e col, std::vector<move_s> &possible, move_s &m)
    {
        // If we are in check currently, our move MUST remove check
        if (m.is_valid())
        {
            chessboard b(*this);
            b.move(m);
            if (b.find_check(col))
                m.check = true;
        }
        else
        {
            update_kill_bits(other(col));
            if (find_check(col))
                m.check = true;
        }
        m.mate = true;
        for (size_t i = 0; i < possible.size(); i++)
        {
            chessboard b(*this);
            b.move(possible[i]);
            if (!b.find_check(col))
            {
                m.mate = false;
                break;
            }
        }
    }

    std::string chessboard::check_state()
    {
        std::string s = "";
        if (m_check & c_white)
            s += "White in Check ";
        if (m_check & c_black)
            s += "Black in Check ";
        return s;
    }

    color_e chessboard::turn_color()
    {
        return m_turn;
    }

    move_s chessboard::user_move(color_e col, int8_t y0, int8_t x0, int8_t y1, int8_t x1)
    {
        move_s empty;
        if ((!in_range(y0, x0)) || (!in_range(y1, x1)))
            return empty;
        if ((y0 == y1) && (x0 == x1))
            return empty;
        chesspiece p(get(y0, x0));
        if (p.color != col)
            return empty;
        std::vector<move_s> possible;
        possible_moves(possible, y0, x0);
        move_s m(y0, x0, y1, x1);
        if (!contains_move(possible, m))
            return empty;
        // Castling etc?
        evaluate_check_and_mate(col, possible, m);
        if (m.is_valid())
        {
            m_turn = other(col);
            return move(m);
        }
        return empty;
    }

    move_s chessboard::computer_move(color_e turn_col, int rec, bool root)
    {
        move_s best = cached_move(turn_col);
        if (rec <= 0)
            return best;
        m_turn = turn_col;
        std::vector<move_s> possible;
        if (!best.is_valid())
        {
            for (int8_t y = 0; y < 8; y++)
            {
                for (int8_t x = 0; x < 8; x++)
                {
                    unsigned char content = m_cells[y][x];
                    if (content != 0)
                    {
                        color_e content_col = (color_e)(content & color_mask);
                        if (content_col == turn_col)
                        {
                            std::vector<move_s> this_possible;
                            move_s candidate = computer_move(this_possible, turn_col, y, x, rec, root);
                            if (this_possible.size()>0)
                                possible.insert(possible.end(), this_possible.begin(), this_possible.end());
                            if (candidate.is_valid())
                            {
                                if (candidate.weight > best.weight)
                                {
                                    best = candidate;
                                }
                                else if (candidate.weight == best.weight)
                                {
                                    if (get_rand() > 0.5)
                                        best = candidate;
                                }
                            }
                        }
                    }
                }
            }
        }
        if (root)
            evaluate_check_and_mate(turn_col, possible, best);
        if (best.is_valid())
        {
            cache_move(m_turn, best);
            m_turn = other(m_turn);
            return move(best);
        }
        return best;
    }

    move_s chessboard::computer_move(std::vector<move_s> &possible, color_e turn_col, int8_t y0, int8_t x0, int rec, bool root)
    {
        move_s best;
        if (rec <= 0)
            return best;
        color_e enemy_col = other(turn_col);
        std::vector<move_s> this_possible;
        possible_moves(this_possible, y0, x0);
        for (size_t i = 0; i < this_possible.size(); i++)
        {
            chessboard b(*this);
            // Execute the move
            move_s candidate = b.move(this_possible[i]);
            if (b.find_check(turn_col))
                continue;
            possible.push_back(this_possible[i]);
            move_s response = b.computer_move(enemy_col, rec - 1, false);
            if (response.is_valid())
            {
                candidate.weight = weight(turn_col);
                move_s recurse = b.computer_move(turn_col, rec - 1, false);
                candidate.weight += recurse.weight / 2.0f;
            }
            if (candidate.weight > best.weight)
            {
                best = candidate;
            }
            else if (candidate.weight == best.weight)
            {
                if (get_rand() > 0.5)
                    best = candidate;
            }
        }
        return best;
    }

    std::vector<move_s> chessboard::possible_moves(color_e turn_col)
    {
        std::vector<move_s> possible;
        for (int8_t y = 0; y < 8; y++)
        {
            for (int8_t x = 0; x < 8; x++)
            {
                unsigned char content = m_cells[y][x];
                if (content != 0)
                {
                    color_e content_col = (color_e)(content & color_mask);
                    if (content_col == turn_col)
                        possible_moves(possible, y, x);
                }
            }
        }
        return possible;
    }

    void chessboard::possible_moves(std::vector<move_s> &possible, int8_t y0, int8_t x0)
    {
        chesspiece piece(m_cells[y0][x0]);
        piece.possible_moves(possible, y0, x0, m_cells, m_castled_left, m_castled_right, m_ep_y, m_ep_x);
    }

    void chessboard::update_kill_bits(color_e turn_col)
    {
        // Clear all of the bits
        for (int8_t y = 0; y < 8; y++)
            for (int8_t x = 0; x < 8; x++)
                m_cells[y][x] &= 127;
        // Set the ones of our color
        for (int8_t y = 0; y < 8; y++)
            for (int8_t x = 0; x < 8; x++)
            {
                unsigned char content = m_cells[y][x];
                if (content != 0)
                {
                    color_e content_col = (color_e)(content & color_mask);
                    if (content_col == turn_col)
                    {
                        chesspiece piece(m_cells[y][x]);
                        piece.update_kill_bits(y, x, m_cells);
                    }
                }
            }
        m_hash = 0;
    }

    move_s chessboard::move(int8_t y0, int8_t x0, int8_t y1, int8_t x1, int8_t cx)
    {
        move_s m0(y0, x0, y1, x1, cx);
        return move(m0);
    }

    move_s chessboard::move(const move_s &m0)
    {
        move_s m1 = m0;
        chesspiece piece(m_cells[m0.y0][m0.x0]);
        m_cells[m0.y1][m0.x1] = m_cells[m0.y0][m0.x0];
        m_cells[m0.y0][m0.x0] = 0;
        // Clear en passant flag - set if appropriate under pawn
        m_ep_x = -1;
        m_ep_y = -1;
        // These handle castling and flagging for castle.
        if (piece.ptype == p_king)
        {
            if (m0.cx != -1)
            {
                // King cell has already moved.
                if (m0.cx == 0)
                    m_cells[m0.y1][m0.x0 - 1] = m_cells[m0.y1][m0.cx];
                else
                    m_cells[m0.y1][m0.x0 + 1] = m_cells[m0.y1][m0.cx];
                m_cells[m0.y1][m0.cx] = 0;
            }
            // No castling after king move
            m_castled_left |= piece.color;
            m_castled_right |= piece.color;
        }
        else if (piece.ptype == p_rook)
        {
            // will catch on back row, set flag to
            if (m0.x0 == 0)
                m_castled_left |= piece.color;
            else if (m0.x0 == 7)
                m_castled_right |= piece.color;
        }
        else if (piece.ptype == p_pawn)
        {
            // en passant 
            if (((m0.y0 == 1) && (m0.y1 == 3)) || ((m0.y0 == 6) && (m0.y1 == 4)))
            {
                m_ep_x = m0.x1;
                m_ep_y = m0.y1;
            }
            if (m0.en_passant)
            {
                m_cells[m0.y0][m0.x1] = 0;
            }
            if (m0.promote != p_none)
            {
                m_cells[m0.y1][m0.x1] = m0.promote + piece.color;
            }
        }
        // Adjust weight
        m1.weight = weight(piece.color);
        m_hash = 0;
        update_kill_bits(other(piece.color));
        return m1;
    }

    uint32_t chessboard::hash()
    {
        if (m_hash != 0)
            return m_hash;

        for (int8_t y = 0; y < 8; y++)
            for (int8_t x = 0; x < 8; x++)
            {
                m_hash += (m_hash << 1) + (m_hash << 4) + (m_hash << 7) + (m_hash << 8) + (m_hash << 24);
                m_hash ^= (uint32_t)m_cells[y][x];
            }

        return m_hash;
    }

    int m_move_hits = 0;
    int m_move_misses = 0;
    std::map<color_e, std::map<uint32_t, move_s>> m_move_cache;
    move_s chessboard::cached_move(color_e col)
    {
        move_s best;
        std::map<uint32_t, move_s>::iterator it = m_move_cache[col].find(hash());
        if (it != m_move_cache[col].end())
        {
            m_move_hits++;
            return it->second;
        }
        m_move_misses++;
        return best;
    }

    void chessboard::cache_move(color_e col, move_s &m)
    {
        if (m_move_cache.size() == 0)
        {
            std::map<uint32_t, move_s> empty;
            m_move_cache[c_white] = empty;
            m_move_cache[c_black] = empty;
        }
        m_move_cache[col][hash()] = m;
    }

    std::string chessboard::cache_stats()
    {
        std::stringstream s;
        if (m_move_cache.size() == 0)
            return "No Cache";
        s << "Hits: " << m_move_hits << " Misses: " << m_move_misses;
        double pct = 100.0;
        if (m_move_misses > 0)
            pct = (double)m_move_hits * 100.0 / (double)m_move_misses;
        s << " " << pct << "%";
        return s.str();
    }

}