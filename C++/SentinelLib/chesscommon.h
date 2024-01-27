#pragma once

#include <string>
#include <cstdlib>
#include <vector>

namespace chess
{

    class chessboard;
    struct move_s;

    typedef enum piece_e
    {
        p_none = 0,
        p_pawn = 1,
        p_bishop = 3,
        p_knight = 4,
        p_rook = 5,
        p_queen = 8,
        p_king = 15
    } piece_e;

    typedef enum color_e // a mask
    {
        c_none = 0,
        c_white = 16,
        c_black = 32
    } color_e;

    const unsigned char piece_mask = 15;
    const unsigned char color_mask = 48;
    const unsigned char piece_and_color_mask = 63;
    const unsigned char white_kill_mask = 64;
    const unsigned char black_kill_mask = 128;
    const unsigned char color_kill_mask_mult = 4;
    const int16_t request_promote = -1;

    const float piece_weight = 1.0f;
    const float kill_count_weight = 0.025f;
    const float board_position_weight = 0.05f;
    const int check_points = 60;

#pragma pack(push, 1)

    typedef struct coord_s
    {
        coord_s() { ; }
        coord_s(int8_t _y, int8_t _x)
        {
            y = _y;
            x = _x;
        }
        bool operator==(const coord_s &c1)
        {
            return (c1.y == y && c1.x == x);
        }
        coord_s operator+(const coord_s &c1)
        {
            return coord_s(y + c1.y, x + c1.x);
        }
        int8_t y = -1;
        int8_t x = -1;
    } coord_s;

    typedef struct move_s
    {
        move_s()
        {
            promote = p_none;
            terminal = false;
        }
        move_s(coord_s _p0, coord_s _p1, int8_t _cx = -1, bool _en_passant = false)
        {
            p0 = _p0;
            p1 = _p1;
            cx = _cx;
            en_passant = _en_passant;
            promote = p_none;
            terminal = false;
        }
        move_s(coord_s _p0, coord_s _p1, piece_e _promote)
        {
            p0 = _p0;
            p1 = _p1;
            promote = _promote;
            terminal = false;
        }
        coord_s p0;
        coord_s p1;
        int8_t cx = -1;
        float weight = -999;
        // Packed data - we want to hit 16 exactly
        int8_t promote;
        bool cache = false;
        bool en_passant = false;
        bool check = false;
        bool mate = false;
        // bit masked fields need to be init in constructor
        bool terminal : 1;
        // Note our total struct size must hit 16 for some reason
        std::string to_string();
        bool is_valid();
        void invalidate();
        bool matches(const move_s &);
    } move_s;

    typedef struct weight_metric_s
    {
        weight_metric_s()
        {
            weight = 0;
            pc = 0;
            kc = 0;
            bp = 0;
        }
        weight_metric_s(float _w, int _pc, int _kc, int _bp)
        {
            weight = _w;
            pc = _pc;
            kc = _kc;
            bp = _bp;
        }
        float weight;
        int pc;
        int kc;
        int bp;
    } weight_metric_s;

#pragma pack(pop)

    typedef enum game_state_e
    {
        play_e = 0,
        checkmate_e = 1,
        stalemate_e = 2
    } game_state_e;

    game_state_e is_game_over(color_e col, move_s &m);
    bool contains_move(std::vector<move_s> possible_moves, move_s &m, bool inherit = false);
    std::string color_str(color_e col);
    std::string coord_str(coord_s c);
    bool coord_int(std::string s, coord_s &c);
    bool in_range(int8_t y, int8_t x);
    bool in_range(coord_s c);
    bool is_color(unsigned char cell, color_e color);
    color_e other(color_e c);
    move_s new_move(coord_s p0, coord_s p1, int8_t cx = -1, bool en_passant = false);
    move_s new_move(coord_s p0, coord_s p1, piece_e promote);

    float get_rand();
    std::vector<std::string> split_string(std::string cmd, char div);
    void write_hex_uchar(std::ofstream &file1, unsigned char c);
    unsigned char read_hex_uchar(std::string line);
    std::string uppercase(std::string l);
    std::string lowercase(std::string u);

}