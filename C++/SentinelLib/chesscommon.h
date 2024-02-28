#pragma once

#include <string>
#include <cstdlib>
#include <vector>

namespace chess
{

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
        e_rewind_missing,
        e_rewind_failed,
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
        e_listener_not_found
    } error_e;

    std::string errorstr(error_e num);

    typedef enum chessclock_e
    {
        cc_none,
        cc_suddendeath,
        cc_increment,
        cc_bronstein_delay,
        cc_simple_delay
    } chessclock_e;

    class chessboard;
    struct move_s;

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

#pragma pack(push, 1)

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
        void clear()
        {
            y = x = -1;
        }
        int8_t y = -1;
        int8_t x = -1;
    } coord_s;

    typedef struct move_s
    {
        move_s()
        {
            promote = p_none;
        }
        move_s(coord_s _p0, coord_s _p1, int8_t _cx = -1, bool _en_passant = false)
        {
            p0 = _p0;
            p1 = _p1;
            cx = _cx;
            en_passant = _en_passant;
            promote = p_none;
        }
        move_s(coord_s _p0, coord_s _p1, piece_e _promote)
        {
            p0 = _p0;
            p1 = _p1;
            promote = _promote;
        }
        coord_s p0;
        coord_s p1;
        int8_t cx = -1;
        // Packed data - we want to hit 16 exactly
        int8_t promote;
        bool en_passant = false;
        bool check = false;
        bool mate = false;
        error_e error = e_none;
        // Note our total struct size must hit 16 for some reason
        std::string to_string();
        bool is_valid();
        void invalidate();
        bool matches(const move_s &);
    } move_s;

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

#pragma pack(pop)

    typedef enum game_state_e
    {
        none_e = -1,
        idle_e = 0,
        play_e = 1, 
        checkmate_e = 2,
        terminate_e = 3,
        forfeit_e = 4,
        time_up_e = 5,
        draw_stalemate_e = 6,
        draw_fivefold_e = 7,
        draw_fiftymove_e = 8,
        draw_insuff_material_e = 9,
    } game_state_e;

    typedef enum chessplayertype_e
    {
        t_none,
        t_human,
        t_computer
    } chessplayertype_e;

    typedef enum chessgamelistenertype
    {
        cl_none,
        cl_computer,
        cl_user,
        cl_clock
    } chessgamelistenertype;

    game_state_e is_game_over(color_e col, move_s &m);
    bool contains_move(std::vector<move_s> possible_moves, move_s &m, bool inherit = false);
    int color_idx(color_e c);
    std::string color_str(color_e col);
    std::string game_state_str(game_state_e g);
    std::string coord_str(coord_s c);
    bool coord_int(std::string s, coord_s &c);
    bool in_range(int8_t y, int8_t x);
    bool in_range(coord_s c);
    bool is_color(unsigned char cell, color_e color);
    color_e other(color_e c);
    move_s new_move(coord_s p0, coord_s p1, int8_t cx = -1, bool en_passant = false);
    move_s new_move(coord_s p0, coord_s p1, piece_e promote);

    float get_rand();

    std::string load_string(std::ifstream &is);
    void save_string(std::string s, std::ofstream &os);

    std::vector<std::string> split_string(std::string cmd, char div);
    void write_hex_uchar(std::ofstream &file1, unsigned char c);
    unsigned char read_hex_uchar(std::string line);
    std::string uppercase(std::string l);
    std::string lowercase(std::string u);

}