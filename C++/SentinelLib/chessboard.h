#pragma once

#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <fstream>

#include "chesscommon.h"

namespace chess
{

    class chesspiece;

    typedef void (*thinking_callback)(move_s, int);
    typedef void (*traces_callback)(std::string);

    class chessboard
    {
        // first index is a-h, second index is 1-8.  Note
        // our arrangement is flipped with white on top for index
        // convenience in Y

    public:
        chessboard();
        chessboard(chessboard &other);

        void copy(chessboard &other);
        error_e load(std::ifstream &is);
        error_e save(std::ofstream &os);

        void new_board();
        std::string save_xfen();
        error_e load_xfen(std::string);

        // The board weight or value for the supplied color
        weight_metric_s weight(color_e col);

        unsigned char get(int8_t y, int8_t x);
        unsigned char get(coord_s c);
        piece_e get_piece(coord_s c);

        bool find_piece(piece_e pc, color_e col, coord_s &c);

        // User Move
        move_s user_move(color_e col, move_s m);
        move_s user_move(color_e col, coord_s p0, coord_s p1, piece_e promote = p_none);
        std::vector<move_s> possible_moves(color_e col);

        // Best Move
        move_s computer_move(color_e col, int rec);
        error_e suggest_move(move_s m);

        move_s is_game_over(color_e color);
        // Remove a piece
        error_e remove(coord_s p0);
        error_e add(coord_s p0, chesspiece &p1);

        // Check?
        bool check_state(color_e col);
        std::string check_state();
        color_e turn_color();
        uint32_t hash(int rec);

        void set_callbacks(thinking_callback _thinking, traces_callback _traces);
        void cancel(bool c) { m_cancel = c; }
        void trace(std::string msg);

    protected:
        unsigned char m_cells[8][8];
        unsigned char m_castled_left;
        unsigned char m_castled_right;
        coord_s m_ep;
        std::map<color_e, coord_s> m_king_pos;
        std::map<color_e, bool> m_check;
        move_s m_suggestion;
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

        // Move
        move_s move(const move_s &m);
        // move_s computer_move(std::vector<move_s> &possible, color_e col, coord_s p0, int rec, bool root = true);
        float computer_move_max(color_e turn_col, float alpha, float beta, int rec);
        float computer_move_min(color_e turn_col, float alpha, float beta, int rec);

        void possible_moves(std::vector<move_s> &possible, coord_s c);
        void update_kill_bits();
        void update_check(color_e col);
        void thinking(move_s, int pct);
        uint32_t m_hash;
        bool m_kill_updated;
        volatile bool m_cancel;
        thinking_callback mp_cb_thinking;
        traces_callback mp_cb_traces;
    };

}