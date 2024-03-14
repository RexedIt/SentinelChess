#include "chessboard.h"
#include "chesspiece.h"
#include "chessturn.h"

#include <memory.h>
#include <cstdlib>
#include <fstream>
#include <cstring>
#include <sstream>

namespace chess
{

    chessboard::chessboard()
    {
        memset(&m_cells, 0, sizeof(m_cells));
        memset(&m_captured, 0, sizeof(m_captured));
        m_castled_left = 0;
        m_castled_right = 0;
        m_turn = c_white;
        m_hash = 0;
        m_halfmove = 0;
        m_fullmove = 1;
        m_kill_updated = false;
        m_check[0] = m_check[1] = false;
    }

    chessboard::chessboard(const chessboard &other)
    {
        copy(other);
    }

    void chessboard::operator=(const chessboard &other)
    {
        copy(other);
    }

    void chessboard::copy(const chessboard &other)
    {
        memcpy(&m_cells, other.m_cells, sizeof(m_cells));
        memcpy(&m_captured, other.m_captured, sizeof(m_captured));
        m_castled_left = other.m_castled_left;
        m_castled_right = other.m_castled_right;
        m_ep = other.m_ep;
        m_check[0] = other.m_check[0];
        m_check[1] = other.m_check[1];
        m_king_pos[0] = other.m_king_pos[0];
        m_king_pos[1] = other.m_king_pos[1];
        m_turn = other.m_turn;
        m_halfmove = other.m_halfmove;
        m_fullmove = other.m_fullmove;
        m_hash = 0;
        m_kill_updated = false;
    }

    error_e chessboard::save(json &o)
    {
        o["xfen"] = save_xfen();
        o["white_check"] = m_check[0];
        o["black_check"] = m_check[1];
        o["turn"] = color_str(m_turn);
        o["captured"] = captured_pieces_abbr(c_none);
        return e_none;
    }

    std::string chessboard::save_xfen()
    {
        std::stringstream ss;
        for (int8_t y = 7; y >= 0; y--)
        {
            int blanks = 0;
            for (int8_t x = 0; x < 8; x++)
            {
                chesspiece p(m_cells[y][x]);
                if (p.ptype == p_none)
                    blanks++;
                else
                {
                    if (blanks)
                        ss << blanks;
                    ss << p.abbr;
                    blanks = 0;
                }
            }
            if (blanks)
                ss << blanks;
            if (y > 0)
                ss << '/';
        }
        ss << ' ' << ((m_turn == c_white) ? 'w' : 'b');

        if ((m_castled_left == 0) || (m_castled_right == 0))
        {
            ss << ' ';
            if ((m_castled_right & c_white) == 0)
                ss << 'K';
            if ((m_castled_left & c_white) == 0)
                ss << 'Q';
            if ((m_castled_right & c_black) == 0)
                ss << 'k';
            if ((m_castled_left & c_black) == 0)
                ss << 'q';
        }

        if (m_ep.y == -1)
        {
            ss << " -";
        }
        else
        {
            ss << " " << coord_str(m_ep);
        }

        ss << " " << m_halfmove << " " << m_fullmove;

        return ss.str();
    }

    error_e chessboard::load(json &o)
    {
        if (o.is_null())
            return e_loading;
        load_xfen(o["xfen"], false);
        update_kill_bits();
        m_check[0] = o["white_check"];
        m_check[1] = o["black_check"];
        m_turn = str_color(o["turn"]);
        return set_captured_pieces(o["captured"]);
    }

    error_e chessboard::load_xfen(std::string contents, bool recalc_captured)
    {
        std::vector<std::string> args = split_string(contents, ' ');
        if (args.size() < 4)
            return e_xfen_read;
        std::string board = args[0];
        std::string col = args[1];
        std::string KQkq = args[2];
        std::string ep = args[3];

        // others are game state level
        unsigned char ucboard[64];
        int ucidx = 0;
        for (size_t i = 0; i < board.size(); i++)
        {
            char c = board[i];
            if ((c >= '0') && (c <= '8'))
            {
                int b = c - '0';
                for (int j = 0; j < b; j++)
                {
                    if (ucidx > 63)
                        return e_xfen_read;
                    ucboard[ucidx++] = 0;
                }
            }
            else if ((c >= 'A') && (c <= 'z'))
            {
                if (ucidx > 63)
                    return e_xfen_read;
                chesspiece p(c);
                ucboard[ucidx++] = p.value;
            }
        }

        if (ucidx != 64)
            return e_xfen_read;

        ucidx = 0;
        for (int y = 7; y >= 0; y--)
        {
            for (int x = 0; x < 8; x++)
            {
                m_cells[y][x] = ucboard[ucidx++];
            }
        }

        // turn color
        m_turn = (col == "b") ? c_black : c_white;

        // Castling
        m_castled_left = c_white + c_black;
        m_castled_right = c_white + c_black;

        for (size_t i = 0; i < KQkq.size(); i++)
        {
            char c = KQkq[i];
            if (c == 'K')
                m_castled_right &= (255 - c_white);
            if (c == 'k')
                m_castled_right &= (255 - c_black);
            if (c == 'Q')
                m_castled_left &= (255 - c_white);
            if (c == 'q')
                m_castled_left &= (255 - c_black);
        }

        // En passant
        if (ep == "-")
        {
            m_ep.y = -1;
            m_ep.x = -1;
        }
        else
        {
            if (!coord_int(ep, m_ep))
                return e_xfen_read;
        }

        // we need to read half move and full move
        if (args.size() >= 5)
            m_halfmove = atoi(args[4].c_str());
        if (args.size() >= 6)
            m_fullmove = atoi(args[5].c_str());

        m_hash = 0;

        if (recalc_captured)
            calc_captured_pieces();

        return e_none;
    }

    board_metric_s chessboard::board_metric(color_e col)
    {
        board_metric_s bm;

        color_e enemy_col = other(col);
        unsigned char kill_mask = col * color_kill_mask_mult;

        for (int8_t y = 0; y < 8; y++)
        {
            for (int8_t x = 0; x < 8; x++)
            {
                piece_e piece = (piece_e)(m_cells[y][x] & piece_mask);
                if (piece != p_none)
                {
                    if (m_cells[y][x] & col)
                    {
                        bm.pc[piece]++;
                        bm.bp += (col == c_white) ? y + 1 : 8 - y;
                    }
                    else if (m_cells[y][x] & enemy_col)
                    {
                        bm.opc[piece]++;
                        bm.bp -= (enemy_col == c_white) ? y + 1 : 8 - y;
                    }
                }
                if ((m_cells[y][x] & kill_mask) == kill_mask)
                    bm.kc++;
            }
        }

        // opponent king capture?
        bm.ch = m_check[color_idx(col)];
        bm.och = m_check[color_idx(other(col))];

        return bm;
    }

    unsigned char chessboard::get(coord_s s)
    {
        if (in_range(s))
            return m_cells[s.y][s.x];
        return 0;
    }

    unsigned char chessboard::get(int8_t y, int8_t x)
    {
        if (in_range(y, x))
            return m_cells[y][x];
        return 0;
    }

    piece_e chessboard::get_piece(coord_s s)
    {
        return (piece_e)(get(s) & piece_mask);
    }

    bool chessboard::find_piece(piece_e pc, color_e col, coord_s &s)
    {
        unsigned char cv = (unsigned char)pc + (unsigned char)col;
        for (s.y = 0; s.y < 8; s.y++)
            for (s.x = 0; s.x < 8; s.x++)
                if ((m_cells[s.y][s.x] & piece_and_color_mask) == cv)
                    return true;
        s.y = -1;
        s.x = -1;
        return false;
    }

    bool chessboard::find_check(color_e turn_col)
    {
        // Are we in check?  If so let's see if mate.
        if (!m_kill_updated)
            update_kill_bits();
        unsigned char kill_mask = other(turn_col) * color_kill_mask_mult;
        coord_s k = m_king_pos[color_idx(turn_col)];
        bool ret = (m_cells[k.y][k.x] & kill_mask) == kill_mask;
        return ret;
    }

    chessmove chessboard::is_game_over(color_e col)
    {
        chessmove m;
        std::vector<chessmove> possible = possible_moves(col);
        if (find_check(col))
            m.check = true;
        m.mate = true;
        for (size_t i = 0; i < possible.size(); i++)
        {
            chessboard b(*this);
            b.execute_move(possible[i]);
            if (!b.find_check(col))
            {
                m = possible[i];
                m.mate = false;
                break;
            }
        }
        return m;
    }

    error_e chessboard::remove(coord_s p0)
    {
        if (!in_range(p0))
            return e_invalid_coord;
        // Can't remove a king either.
        if ((p0 == m_king_pos[0]) || (p0 == m_king_pos[1]))
            return e_cannot_remove_a_king;
        if ((m_cells[p0.y][p0.x] & piece_mask) == 0)
            return e_no_piece_there;
        m_cells[p0.y][p0.x] = 0;
        update_kill_bits();
        m_hash = 0;
        return e_none;
    }

    error_e chessboard::add(coord_s p0, chesspiece &p1)
    {
        if (!in_range(p0))
            return e_invalid_coord;
        if (p1.ptype == p_king)
            return e_cannot_add_another_king;
        if (p1.ptype == p_none)
            return e_piece_undefined;
        // Can't remove a king either.
        if ((p0 == m_king_pos[0]) || (p0 == m_king_pos[1]))
            return e_cannot_add_over_king;
        m_cells[p0.y][p0.x] = p1.value;
        update_kill_bits();
        m_hash = 0;
        return e_none;
    }

    void chessboard::evaluate_check_and_mate(color_e col, std::vector<chessmove> &possible, chessmove &m)
    {
        // If we are in check currently, our move MUST remove check
        if (m.is_valid())
        {
            chessboard b(*this);
            m = b.execute_move(m);
        }
        m.mate = true;
        for (size_t i = 0; i < possible.size(); i++)
        {
            chessboard b(*this);
            b.execute_move(possible[i]);
            if (!b.find_check(col))
            {
                m.mate = false;
                break;
            }
        }
    }

    bool chessboard::check_state(color_e col)
    {
        return m_check[color_idx(col)];
    }

    std::string chessboard::check_state()
    {
        std::string s = "";
        if (m_check[0])
            s += "White in Check ";
        if (m_check[1])
            s += "Black in Check ";
        return s;
    }

    color_e chessboard::turn_color()
    {
        return m_turn;
    }

    chessmove chessboard::attempt_move(color_e col, chessmove m)
    {
        chessmove empty;
        empty.error = e_invalid_move;
        coord_s p0 = m.p0;
        coord_s p1 = m.p1;
        if ((!in_range(p0)) || (!in_range(p1)))
            return empty;
        if (p1 == p0)
            return empty;
        chesspiece p(get(p0));
        if (p.color != col)
            return empty;
        std::vector<chessmove> possible;
        possible_moves(possible, p0);
        if (!contains_move(possible, m, true))
            return empty;
        // Promotion?
        int ptarget = (col == c_white) ? 7 : 0;
        if ((p.ptype == p_pawn) && (p1.y == ptarget) && (m.promote == p_none))
        {
            empty.error = e_invalid_move_needs_promote;
            return empty;
        }
        // Castling etc?
        // For this we have to evaluate all possible moves
        // for our color.
        possible = possible_moves(col);
        evaluate_check_and_mate(col, possible, m);
        if (m.is_valid())
        {
            if (m.check)
            {
                m.error = e_check;
                return m;
            }
            m_turn = other(col);
            return execute_move(m);
        }
        return empty;
    }

    chessmove chessboard::attempt_move(color_e col, coord_s p0, coord_s p1, piece_e promote)
    {
        chessmove m(p0, p1, promote);
        return attempt_move(col, m);
    }

    // *** MUST GO ***
    float weight(board_metric_s bm)
    {
        const float kc_weight = 0.25f;
        const float bp_weight = 0.5f;
        int pw = 0;
        for (int i = 0; i < 7; i++)
            pw += bm.pc[i] * piece_default_weights[i] -
                  bm.opc[i] * piece_default_weights[i];
        return ((float)pw) +
               ((float)bm.kc) * kc_weight +
               ((float)bm.bp) * bp_weight;
    }

    std::vector<chessmove> chessboard::possible_moves(color_e turn_col)
    {
        std::vector<chessmove> possible;
        for (int8_t y = 0; y < 8; y++)
        {
            for (int8_t x = 0; x < 8; x++)
            {
                unsigned char content = m_cells[y][x];
                if (content != 0)
                {
                    color_e content_col = (color_e)(content & color_mask);
                    if (content_col == turn_col)
                        possible_moves(possible, coord_s(y, x));
                }
            }
        }
        return possible;
    }

    std::vector<chessmove> chessboard::possible_moves(color_e turn_col, piece_e piece, int8_t yc, int8_t xc)
    {
        std::vector<chessmove> possible;
        for (int8_t y = 0; y < 8; y++)
        {
            if ((yc != -1) && (y != yc))
                continue;
            for (int8_t x = 0; x < 8; x++)
            {
                if ((xc != -1) && (x != xc))
                    continue;
                unsigned char content = m_cells[y][x];
                if (content != 0)
                {
                    if ((content & piece_mask) != (unsigned char)piece)
                        continue;
                    color_e content_col = (color_e)(content & color_mask);
                    if (content_col == turn_col)
                        possible_moves(possible, coord_s(y, x));
                }
            }
        }
        return possible;
    }

    void chessboard::possible_moves(std::vector<chessmove> &possible, coord_s p0)
    {
        chesspiece piece(m_cells[p0.y][p0.x]);
        piece.possible_moves(possible, p0, m_cells, m_castled_left, m_castled_right, m_ep);
    }

    std::vector<piece_e> chessboard::captured_pieces(color_e col)
    {
        std::vector<piece_e> pieces;
        for (int i = 0; i < CHESSBOARD_CAPTURE_MAX; i++)
        {
            unsigned char upc = m_captured[i];
            unsigned char uc = (unsigned char)col;
            if (upc == 0)
                break;
            if ((col == c_none) || ((upc & uc) == uc))
            {
                piece_e pc = (piece_e)(upc & piece_mask);
                pieces.push_back(pc);
            }
        }
        return pieces;
    }

    std::string chessboard::captured_pieces_abbr(color_e col)
    {
        char ret[CHESSBOARD_CAPTURE_MAX + 1];
        int r = 0;
        for (int i = 0; i < CHESSBOARD_CAPTURE_MAX; i++)
        {
            unsigned char upc = m_captured[i];
            unsigned char uc = (unsigned char)col;
            if (upc == 0)
                break;
            if ((col == c_none) || ((upc & uc) == uc))
            {
                piece_e pc = (piece_e)(upc & piece_mask);
                ret[r++] = abbr_char(pc, (color_e)(upc & color_mask));
            }
        }
        ret[r++] = 0;
        return ret;
    }

    error_e chessboard::set_captured_pieces(std::string pieces)
    {
        memset(&m_captured, 0, sizeof(m_captured));
        int r = 0;
        for (size_t i = 0; i < pieces.length(); i++)
        {
            if (i >= CHESSBOARD_CAPTURE_MAX)
                return e_invalid_piece;
            chesspiece p(pieces[i]);
            m_captured[r++] = p.value;
        }
        return e_none;
    }

    void chessboard::calc_captured_pieces()
    {
        std::vector<char> poss = {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P',
                                  'B', 'B', 'N', 'N', 'R', 'R', 'Q', 'K',
                                  'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p',
                                  'b', 'b', 'n', 'n', 'r', 'r', 'q', 'k'};
        for (int8_t y = 0; y < 8; y++)
            for (int8_t x = 0; x < 8; x++)
            {
                unsigned char cell = m_cells[y][x] & piece_and_color_mask;
                if (cell != 0)
                {
                    chesspiece p(cell);
                    auto f = std::find(poss.begin(), poss.end(), p.abbr);
                    if (f != poss.end())
                        poss.erase(f);
                }
            }
        memset(&m_captured, 0, sizeof(m_captured));
        for (size_t i = 0; i < poss.size(); i++)
        {
            chesspiece p(poss[i]);
            m_captured[i] = p.value;
        }
    }

    void chessboard::update_check(color_e c)
    {
        coord_s k = m_king_pos[color_idx(c)];
        unsigned char kill_mask = other(c) * color_kill_mask_mult;
        m_check[color_idx(c)] = ((m_cells[k.y][k.x] & kill_mask) == kill_mask);
    }

    void chessboard::update_kill_bits()
    {
        for (int8_t y = 0; y < 8; y++)
            for (int8_t x = 0; x < 8; x++)
                m_cells[y][x] &= 63;
        m_king_pos[0].clear();
        m_king_pos[1].clear();
        // Set the ones of our color
        for (int8_t y = 0; y < 8; y++)
            for (int8_t x = 0; x < 8; x++)
            {
                unsigned char content = m_cells[y][x];
                if (content != 0)
                {
                    color_e content_col = (color_e)(content & color_mask);
                    if (content_col != c_none)
                    {
                        chesspiece piece(m_cells[y][x]);
                        int pc = 0;
                        piece.update_kill_bits(coord_s(y, x), m_cells, pc);
                        if ((m_cells[y][x] & piece_mask) == p_king)
                            m_king_pos[color_idx(content_col)] = coord_s(y, x);
                    }
                }
            }

        // update our check state
        update_check(c_white);
        update_check(c_black);

        m_hash = 0;
        m_kill_updated = true;
    }

    chessmove chessboard::execute_move(const chessmove &m0)
    {
        chessmove m1 = m0;
        chesspiece piece(m_cells[m0.p0.y][m0.p0.x]);
        move_piece(m0);
        if (m_cells[m0.p0.y][m0.p0.x] & piece_mask)
        {
            m_halfmove = 0; // resets on any capture
            m_cells[m0.p0.y][m0.p0.x] = 0;
        }
        // Clear en passant flag - set if appropriate under pawn
        m_ep.x = -1;
        m_ep.y = -1;
        // These handle castling and flagging for castle.
        if (piece.ptype == p_king)
        {
            if (m0.cx != -1)
            {
                // King cell has already moved.
                if (m0.cx == 0)
                    m_cells[m0.p1.y][m0.p0.x - 1] = m_cells[m0.p1.y][m0.cx];
                else
                    m_cells[m0.p1.y][m0.p0.x + 1] = m_cells[m0.p1.y][m0.cx];
                m_cells[m0.p1.y][m0.cx] = 0;
            }
            // No castling after king move
            m_castled_left |= piece.color;
            m_castled_right |= piece.color;
        }
        else if (piece.ptype == p_rook)
        {
            // will catch on back row, set flag to
            if (m0.p0.x == 0)
                m_castled_left |= piece.color;
            else if (m0.p0.x == 7)
                m_castled_right |= piece.color;
        }
        else if (piece.ptype == p_pawn)
        {
            // en passant first condition sets the board 'ep' state so the board knows the
            // piece at position m0.p1 can be captured by en passant move
            if (((m0.p0.y == 1) && (m0.p1.y == 3)) || ((m0.p0.y == 6) && (m0.p1.y == 4)))
            {
                m_ep.x = m0.p1.x;
                m_ep.y = m0.p1.y;
            }
            if (m0.en_passant)
                capture_piece(m0.p0.y, m0.p1.x);
            if (m0.promote != p_none)
                capture_piece(m0.p1.y, m0.p1.x);
            m_halfmove = 0; // resets
        }
        // Adjust bitmask, returns balance of kill cells
        update_kill_bits();

        m_hash = 0;
        m1.check = m_check[color_idx(piece.color)];
        if (m1.check)
            m1.error = e_check;

        m_fullmove++;
        return m1;
    }

    void chessboard::move_piece(chessmove m)
    {
        unsigned char dest = m_cells[m.p1.y][m.p1.x] & piece_and_color_mask;
        m_cells[m.p1.y][m.p1.x] = m_cells[m.p0.y][m.p0.x];
        if (dest != 0)
            add_captured(dest);
    }

    void chessboard::capture_piece(coord_s &c)
    {
        unsigned char dest = m_cells[c.y][c.x] & piece_and_color_mask;
        m_cells[c.y][c.x] = 0;
        if (dest != 0)
            add_captured(dest);
    }

    void chessboard::capture_piece(int8_t y, int8_t x)
    {
        unsigned char dest = m_cells[y][x] & piece_and_color_mask;
        m_cells[y][x] = 0;
        if (dest != 0)
            add_captured(dest);
    }

    void chessboard::add_captured(unsigned char piece)
    {
        for (int i = 0; i < CHESSBOARD_CAPTURE_MAX; i++)
            if (m_captured[i] == 0)
            {
                m_captured[i] = piece;
                return;
            }
    }

    uint32_t chessboard::hash()
    {
        if (m_hash != 0)
            return m_hash;

        for (int8_t y = 0; y < 8; y++)
            for (int8_t x = 0; x < 8; x += 4)
            {
                m_hash += (m_hash << 1) + (m_hash << 4) + (m_hash << 7) + (m_hash << 8) + (m_hash << 24);
                m_hash ^= (((uint32_t)m_cells[y][x] << 24) +
                           ((uint32_t)m_cells[y][x + 1] << 16) +
                           ((uint32_t)m_cells[y][x + 2] << 8) +
                           ((uint32_t)m_cells[y][x + 3]));
            }
        return m_hash;
    }

}
