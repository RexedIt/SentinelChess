#pragma once

#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <fstream>

#include "chesscommon.h"

#include "nlohmann/json.hpp"
using namespace nlohmann;

namespace chess
{

#define CHESSBOARD_CAPTURE_MAX 32

    class chesspiece;

    typedef struct board_metric_s
    {
        board_metric_s()
        {
            kc = 0;
            bp = 0;
            ch = false;
            och = false;
            for (int i = 0; i < 7; i++)
                pc[i] = opc[i] = 0;
        }
        int8_t pc[7];  // Piece Count, own Color
        int8_t opc[7]; // Piece Count, other color
        int kc;        // Kill Coverage
        int bp;        // Board Position
        bool ch;       // Me in Check
        bool och;      // Opponent in Check
    } board_metric_s;

    class chessboard
    {
        // first index is a-h, second index is 1-8.  Note
        // our arrangement is flipped with white on top for index
        // convenience in Y

    public:
        chessboard();
        chessboard(const chessboard &other);

        void copy(const chessboard &other);
        void operator=(const chessboard &other);

        error_e load(json &o);
        error_e save(json &o);

        std::string save_xfen();
        error_e load_xfen(std::string, bool recalc_captured = true);

        // The board weight or value for the supplied color
        board_metric_s board_metric(color_e col);

        unsigned char get(int8_t y, int8_t x);
        unsigned char get(coord_s c);
        piece_e get_piece(coord_s c);

        bool find_piece(piece_e pc, color_e col, coord_s &c);

        // User Move
        chessmove attempt_move(color_e col, chessmove m);
        chessmove attempt_move(color_e col, std::string s);
        chessmove attempt_move(color_e col, coord_s p0, coord_s p1, piece_e promote = p_none);
        std::vector<chessmove> possible_moves(color_e col, bool allowcheck = true);
        std::vector<chessmove> possible_moves(color_e col, piece_e piece, int8_t yc = -1, int8_t xc = -1, bool allowcheck = true);
        std::vector<piece_e> captured_pieces(color_e col);
        std::string captured_pieces_abbr(color_e col);
        error_e set_captured_pieces(std::string pieces);
        void calc_captured_pieces();

        chessmove is_game_over(color_e color);
        // Remove a piece
        error_e remove(coord_s p0);
        error_e add(coord_s p0, chesspiece &p1);

        // Check?
        bool check_state(color_e col);
        std::string check_state();
        color_e turn_color();
        uint32_t hash();

        friend class chesscomputer;

    protected:
        // Move
        chessmove execute_move(const chessmove &m);

        unsigned char m_cells[8][8];
        unsigned char m_castled_left;
        unsigned char m_castled_right;
        coord_s m_ep;
        coord_s m_king_pos[2];
        bool m_check[2];
        unsigned char m_captured[CHESSBOARD_CAPTURE_MAX];
        color_e m_turn;
        int m_halfmove;
        int m_fullmove;

    private:
        // Special
        bool find_check(color_e col);

        void evaluate_check_and_mate(color_e col, std::vector<chessmove> &possible, chessmove &m);
        void possible_moves(std::vector<chessmove> &possible, coord_s c);
        void move_piece(chessmove &m);
        bool capture_piece(int8_t y, int8_t x);
        void add_captured(unsigned char dest);
        void update_kill_bits();
        void update_check(color_e col);
        uint32_t m_hash;
        bool m_kill_updated;
    };

}