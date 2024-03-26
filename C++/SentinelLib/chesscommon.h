#pragma once

#include <stdint.h>
#include <string>
#include <cstdlib>
#include <vector>

namespace chess
{

#define JSON_LOAD(o, k, v, d) \
    if (o.contains(k))        \
        v = o[k];             \
    else                      \
        v = d;

#define JSON_GET(o, k, d) \
    (o.contains(k) ? o[k] : d)

    typedef enum error_e
    {
        e_none,
        e_busy,
        e_out_of_turn,
        e_invalid_game_state,
        e_invalid_range,
        e_missing_move,
        e_invalid_move,
        e_failed_move,
        e_missing_coord,
        e_invalid_coord,
        e_missing_filename,
        e_loading,
        e_saving,
        e_removing,
        e_adding,
        e_rewind_failed,
        e_advance_failed,
        e_missing_piece,
        e_invalid_piece,
        e_missing_coord_piece,
        e_xfen_read,
        e_check,
        e_no_piece_there,
        e_piece_undefined,
        e_cannot_add_another_king,
        e_cannot_remove_a_king,
        e_cannot_add_over_king,
        e_invalid_reference,
        e_invalid_player,
        e_player_already_registered,
        e_player_not_created,
        e_player_not_found,
        e_no_game,
        e_interrupted,
        e_invalid_move_needs_promote,
        e_invalid_listener,
        e_listener_already_registered,
        e_listener_not_found,
        e_none_found,
        e_incorrect_move
    } error_e;

    std::string errorstr(error_e num);

    extern const char *c_open_board;

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
        bool operator!=(const coord_s &c1)
        {
            return (c1.y != y || c1.x != x);
        }
        coord_s operator+(const coord_s &c1)
        {
            return coord_s(y + c1.y, x + c1.x);
        }
        void clear()
        {
            y = x = -1;
        }
        int8_t y = -1;
        int8_t x = -1;
    } coord_s;

    typedef enum chessclock_e
    {
        cc_none,
        cc_suddendeath,
        cc_increment,
        cc_bronstein_delay,
        cc_simple_delay
    } chessclock_e;

    typedef enum piece_e
    {
        p_none = 0,
        p_pawn = 1,
        p_bishop = 2,
        p_knight = 3,
        p_rook = 4,
        p_queen = 5,
        p_king = 6
    } piece_e;

    const int piece_default_weights[] =
        {
            0,  // no piece
            10, // pawn
            30, // bishop
            30, // knight
            50, // rook
            90, // queen
            900 // king
    };

    typedef enum color_e // a mask
    {
        c_none = 0,
        c_white = 16,
        c_black = 32
    } color_e;

    const unsigned char piece_mask = 7;
    const unsigned char unused_mask = 8;
    const unsigned char color_mask = 48;
    const unsigned char piece_and_color_mask = 63;
    const unsigned char white_kill_mask = 64;
    const unsigned char black_kill_mask = 128;
    const unsigned char color_kill_mask_mult = 4;

    typedef struct chessclock_s
    {
        chessclock_s();
        chessclock_s(const chessclock_s &);
        void operator=(const chessclock_s &);
        bool operator==(const chessclock_s &);
        chessclock_e ctype;
        // these are black[0] and white[1]
        // you can use the color_idx function to map from a color_e to the
        // appropriate index
        int32_t allowedms[2];
        int32_t remainms[2];
        int32_t addms[2];
    } gameclock_s;

    typedef enum game_state_e
    {
        none_e = -1,
        idle_e = 0,
        play_e = 1,
        checkmate_e = 2,
        terminate_e = 3,
        forfeit_e = 4,
        time_up_e = 5,
        puzzle_solution_e = 6,
        draw_stalemate_e = 7,
        draw_fivefold_e = 8,
        draw_fiftymove_e = 9,
        draw_insuff_material_e = 10,
    } game_state_e;

    typedef enum chessplayertype_e
    {
        t_none,
        t_human,
        t_computer,
        t_puzzle
    } chessplayertype_e;

    typedef enum chessgamelistenertype
    {
        cl_none,
        cl_computer,
        cl_user,
        cl_clock
    } chessgamelistenertype;

    class chessmove;

    game_state_e is_game_over(color_e col, chessmove &m);

    std::string coord_str(coord_s c);
    bool coord_int(std::string s, coord_s &c);
    bool is_coord(std::string s);
    bool in_range(int8_t y, int8_t x);
    bool in_range(coord_s c);

    int color_idx(color_e c);
    std::string color_str(color_e col);
    color_e str_color(std::string col);
    std::string piece_str(piece_e p);
    piece_e str_piece(std::string p);
    std::string clock_type_str(chessclock_e ct);
    chessclock_e str_clock_type(std::string ct);

    char abbr_char(piece_e p, color_e c = c_white);
    piece_e char_abbr(char c);
    std::string game_state_str(game_state_e g);
    game_state_e str_game_state(std::string g);
    std::string time_str(int32_t t);
    bool is_color(unsigned char cell, color_e color);
    color_e other(color_e c);

    float get_rand();

    std::string trim(std::string s);
    std::vector<std::string> split_string(std::string cmd, char div);
    void write_hex_uchar(std::ofstream &file1, unsigned char c);
    unsigned char read_hex_uchar(std::string line);
    std::string uppercase(std::string l);
    std::string lowercase(std::string u);
    uintmax_t get_file_size(std::string f);
    bool get_dir_exists(std::string dirname);

    std::string string_replace(std::string s, char o, char n);
    std::string fix_path(std::string f);
    std::string get_data_folder();
    bool set_data_folder(std::string f);
    std::string data_file(std::string f);
    uint32_t hash(unsigned char *, size_t);
    uint32_t hash(std::vector<chessmove>);
}