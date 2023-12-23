#pragma once

#include <stdio.h>
#include <memory.h>
#include <string>
#include <vector>
#include <map>

namespace chess
{

    typedef enum piece_e
    {
        p_none = 0,
        p_pawn = 1,
        p_bishop = 3,
        p_knight = 4,
        p_rook = 6,
        p_queen = 15,
        p_king = 30
    } piece_e;

    typedef enum color_e // a mask
    {
        c_none = 0,
        c_white = 32,
        c_black = 64
    } color_e;

    const unsigned char piece_mask = 31;
    const unsigned char color_mask = 96;
    const unsigned char kill_mask = 128;

    std::string
    color_str(color_e col);
    std::string coord_str(int8_t y, int8_t x);
    bool coord_int(std::string s, int8_t &y, int8_t &x);
    bool in_range(int8_t y, int8_t x);
    bool is_color(unsigned char cell, color_e color);

    color_e other(color_e c);

#pragma pack(1)
    typedef struct move_s
    {
        move_s() { ; }
        move_s(int8_t _y0, int8_t _x0, int8_t _y1, int8_t _x1, int8_t _cy = -1, int8_t _cx = -1)
        {
            y0 = _y0;
            x0 = _x0;
            y1 = _y1;
            x1 = _x1;
            cy = _cy;
            cx = _cx;
        }

        int8_t y0 = -1;
        int8_t x0 = -1;
        int8_t y1 = -1;
        int8_t x1 = -1;
        int8_t cy = -1; // castle
        int8_t cx = -1; // castle
        short weight = -999;
        bool check = false;
        bool mate = false;
        std::string to_string();
        bool is_valid();

    } move_s;

    class chessboard
    {
        // first index is a-h, second index is 1-8.  Note
        // our arrangement is flipped with white on top for index
        // convenience in Y

    public:
        chessboard();
        chessboard(chessboard &other);

        void new_game();
        bool save_game(std::string filename);
        bool load_game(std::string filename);

        // The board weight or value for the supplied color
        short weight(color_e col);
        unsigned char get(int8_t y0, int8_t x0);
        piece_e get_piece(int8_t y0, int8_t x0);
        bool find_piece(piece_e pc, color_e col, int8_t &y, int8_t &x);

        // User Move
        move_s user_move(color_e col, int8_t y0, int8_t x0, int8_t y1, int8_t x1);
        // Best Move
        move_s computer_move(color_e col, int rec, bool root = true);
        move_s is_game_over(color_e color);

        // Check?
        std::string check_state();
        color_e turn_color();
        std::string cache_stats();

    protected:
        unsigned char m_cells[8][8];
        unsigned char m_castled_left;
        unsigned char m_castled_right;
        unsigned char m_check;
        color_e m_turn;

    private:
        // Initialization
        void set_kings_row(int8_t y, color_e col);
        void set_pawns_row(int8_t y, color_e col);
        void set_empty_rows();

        // Special
        bool find_check(color_e col);
        void evaluate_check_and_mate(color_e col, std::vector<move_s> &possible, move_s &m);

        // Move
        move_s move(int8_t y0, int8_t x0, int8_t y1, int8_t x1, int8_t cy, int8_t cx);
        move_s move(const move_s &m);
        move_s computer_move(std::vector<move_s> &possible, color_e col, int8_t y0, int8_t x0, int rec, bool root = true);

        std::vector<move_s> possible_moves(color_e col);
        void possible_moves(std::vector<move_s> &possible, int8_t y0, int8_t x0);
        void update_kill_bits(color_e col);

        uint32_t hash();
        uint32_t m_hash = 0;
        move_s cached_move(color_e col);
        void cache_move(color_e col, move_s &m);
    };

}