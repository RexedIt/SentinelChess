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

        void new_board();
        std::string save_xfen();
        error_e load_xfen(std::string, bool recalc_captured = true);

        // The board weight or value for the supplied color
        board_metric_s board_metric(color_e col);

        unsigned char get(int8_t y, int8_t x);
        unsigned char get(coord_s c);
        piece_e get_piece(coord_s c);

        bool find_piece(piece_e pc, color_e col, coord_s &c);

        // User Move
        move_s attempt_move(color_e col, move_s m);
        move_s attempt_move(color_e col, coord_s p0, coord_s p1, piece_e promote = p_none);
        std::vector<move_s> possible_moves(color_e col);
        std::vector<piece_e> captured_pieces(color_e col);
        std::string captured_pieces_abbr(color_e col);
        error_e set_captured_pieces(std::string pieces);
        void calc_captured_pieces();

        move_s is_game_over(color_e color);
        // Remove a piece
        error_e remove(coord_s p0);
        error_e add(coord_s p0, chesspiece &p1);

        // Check?
        bool check_state(color_e col);
        std::string check_state();
        color_e turn_color();
        uint32_t hash();

        void cancel(bool c) { m_cancel = c; }

        friend class chesscomputer;

    protected:
        // Move
        move_s execute_move(const move_s &m);

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
        // Initialization
        void set_kings_row(int8_t y, color_e col);
        void set_pawns_row(int8_t y, color_e col);
        void set_empty_rows();

        // Special
        bool find_check(color_e col);

        void evaluate_check_and_mate(color_e col, std::vector<move_s> &possible, move_s &m);
        void possible_moves(std::vector<move_s> &possible, coord_s c);
        void move_piece(move_s m);
        void capture_piece(coord_s &c);
        void capture_piece(int8_t y, int8_t x);
        void add_captured(unsigned char dest);
        void update_kill_bits();
        void update_check(color_e col);
        void thinking(move_s, int pct);
        uint32_t m_hash;
        bool m_kill_updated;
        volatile bool m_cancel;
    };

}