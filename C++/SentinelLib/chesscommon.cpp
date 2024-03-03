#include <ctime>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>

#include "chesscommon.h"
#include "chesspiece.h"

namespace chess
{

    std::string errorstr(error_e num)
    {
        switch (num)
        {
        case e_busy:
            return "Engine Busy";
        case e_out_of_turn:
            return "Out of Turn";
        case e_invalid_game_state:
            return "Invalid Game State";
        case e_invalid_range:
            return "Invalid Range";
        case e_missing_move:
            return "Must include move";
        case e_invalid_move:
            return "Invalid Move, should be in form XX-XX with first digit A-H, second 1-8 in each";
        case e_failed_move:
            return "Move generation failed";
        case e_missing_filename:
            return "Must include filename";
        case e_loading:
            return "Loading";
        case e_saving:
            return "Saving";
        case e_removing:
            return "Removing";
        case e_adding:
            return "Adding";
        case e_rewind_failed:
            return "Engine failed to rewind";
        case e_advance_failed:
            return "Engine failed to advance";
        case e_missing_coord:
            return "Must include coordinate";
        case e_invalid_coord:
            return "Invalid Move, should be in form XX with first digit A-H, second 1-8";
        case e_missing_piece:
            return "Must include piece";
        case e_missing_coord_piece:
            return "Require coordinate XX and piece P arguments";
        case e_invalid_piece:
            return "Invalid Piece, must be single digit in prnbkqPRNBQK";
        case e_xfen_read:
            return "Invalid XFEN format board";
        case e_check:
            return "You are in Check";
        case e_no_piece_there:
            return "There is no piece there";
        case e_piece_undefined:
            return "Piece is undefined";
        case e_cannot_add_another_king:
            return "Cannot add another king";
        case e_cannot_remove_a_king:
            return "Cannot remove a king";
        case e_cannot_add_over_king:
            return "Cannot add over an existing king";
        case e_invalid_reference:
            return "Invalid Reference";
        case e_invalid_player:
            return "Invalid Player";
        case e_player_already_registered:
            return "Player already registered";
        case e_player_not_created:
            return "Player not created";
        case e_player_not_found:
            return "Player not found";
        case e_no_game:
            return "No Game";
        case e_interrupted:
            return "Interrupted";
        case e_invalid_move_needs_promote:
            return "Invalid Move, needs Promote piece set";
        case e_invalid_listener:
            return "Invalid listener";
        case e_listener_already_registered:
            return "Listener already registered";
        case e_listener_not_found:
            return "Listener not found";
        default:
            return "Unknown Error";
        }
    }

    std::string move_s::to_string()
    {
        std::string s = "";
        s += coord_str(p0) + " to " + coord_str(p1);
        if (cx != -1)
        {
            s += " Castle ";
            s += (cx == 0) ? "Queen Side" : "King Side";
        }
        if (en_passant)
            s += " En Passant";
        if (promote != p_none)
        {
            chesspiece p((unsigned char)promote);
            s += " Promote to " + p.name;
        }
        return s;
    }

    bool move_s::is_valid()
    {
        return ((p0.y != -1) && (error == e_none));
    }

    void move_s::invalidate()
    {
        p0.x = -1;
        p0.y = -1;
        p1.x = -1;
        p1.y = -1;
        cx = -1;
        en_passant = false;
        promote = p_none;
        error = e_none;
    }

    bool move_s::matches(const move_s &m)
    {
        if (p0 == m.p0)
            if (p1 == m.p1)
                return true;
        return false;
    }

    chessclock_s::chessclock_s()
    {
        ctype = cc_none;
        for (int i = 0; i < 0; i++)
        {
            allowedms[i] = 0;
            remainms[i] = 0;
            addms[i] = 0;
        }
    }

    chessclock_s::chessclock_s(const chessclock_s &c)
    {
        ctype = c.ctype;
        for (int i = 0; i < 0; i++)
        {
            allowedms[i] = c.allowedms[i];
            remainms[i] = c.remainms[i];
            addms[i] = c.addms[i];
        }
    }

    void chessclock_s::operator=(const chessclock_s &c)
    {
        ctype = c.ctype;
        for (int i = 0; i < 0; i++)
        {
            allowedms[i] = c.allowedms[i];
            remainms[i] = c.remainms[i];
            addms[i] = c.addms[i];
        }
    }

    bool chessclock_s::operator==(const chessclock_s &c)
    {
        if (ctype != c.ctype)
            return false;
        for (int i = 0; i < 0; i++)
        {
            if (allowedms[i] != c.allowedms[i])
                return false;
            if (remainms[i] != c.remainms[i])
                return false;
            if (addms[i] != c.addms[i])
                return false;
        }
        return true;
    }

    int color_idx(color_e c)
    {
        return (c == c_white) ? 0 : 1;
    }

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

    color_e str_color(std::string col)
    {
        std::string colu = uppercase(col);
        if (colu == "BLACK")
            return c_black;
        else if (colu == "WHITE")
            return c_white;
        return c_none;
    }

    std::string piece_str(piece_e p)
    {
        switch (p)
        {
        case p_pawn:
            return "Pawn";
        case p_knight:
            return "Knight";
        case p_bishop:
            return "Bishop";
        case p_rook:
            return "Rook";
        case p_queen:
            return "Queen";
        case p_king:
            return "King";
        default:
            return "None";
        }
    }

    piece_e str_piece(std::string p)
    {
        std::string pu = uppercase(p);
        if (pu == "NONE")
            return p_none;
        if (pu == "PAWN")
            return p_pawn;
        if (pu == "KNIGHT")
            return p_knight;
        if (pu == "BISHOP")
            return p_bishop;
        if (pu == "ROOK")
            return p_rook;
        if (pu == "QUEEN")
            return p_queen;
        if (pu == "KING")
            return p_king;
        return p_none;
    }

    char abbr_char(piece_e p, color_e c)
    {
        char abbr = ' ';
        switch (p)
        {
        case p_pawn:
            abbr = 'p';
            break;
        case p_bishop:
            abbr = 'b';
            break;
        case p_knight:
            abbr = 'n';
            break;
        case p_rook:
            abbr = 'r';
            break;
        case p_queen:
            abbr = 'q';
            break;
        case p_king:
            abbr = 'k';
            break;
        }
        if (c != c_none)
        {
            if (c == c_white)
                abbr -= 32;
        }
        return abbr;
    }

    piece_e char_abbr(char c)
    {
        switch (c)
        {
        case 'p':
        case 'P':
            return p_pawn;
        case 'b':
        case 'B':
            return p_bishop;
        case 'n':
        case 'N':
            return p_knight;
        case 'r':
        case 'R':
            return p_rook;
        case 'k':
        case 'K':
            return p_king;
        case 'q':
        case 'Q':
            return p_queen;
        }
        return p_none;
    }

    std::string game_state_str(game_state_e g)
    {
        switch (g)
        {
        case idle_e:
            return "Idle";
        case play_e:
            return "Play";
        case checkmate_e:
            return "CheckMate";
        case forfeit_e:
            return "Forfeit";
        case time_up_e:
            return "Time Up";
        case terminate_e:
            return "Terminated";
        case draw_stalemate_e:
            return "Stalemate";
        case draw_fivefold_e:
            return "Draw - Fivefold Repetition";
        case draw_fiftymove_e:
            return "Draw - Fifty Move Rule";
        case draw_insuff_material_e:
            return "Draw - Dead Position";
        }
        return "None";
    }

    game_state_e str_game_state(std::string g)
    {
        std::string gu = uppercase(g);
        if (gu == "IDLE")
            return idle_e;
        if (gu == "PLAY")
            return play_e;
        if (gu == "CHECKMATE")
            return checkmate_e;
        if (gu == "FORFEIT")
            return forfeit_e;
        if (gu == "TIME UP")
            return time_up_e;
        if (gu == "TERMINATED")
            return terminate_e;
        if (gu == "STALEMATE")
            return draw_stalemate_e;
        if (gu == "DRAW - FIVEFOLD REPETITION")
            return draw_fivefold_e;
        if (gu == "DRAW - FIFTY MOVE RULE")
            return draw_fiftymove_e;
        if (gu == "DRAW - DEAD POSITION")
            return draw_insuff_material_e;
        return none_e;
    }

    std::string coord_str(coord_s s)
    {
        char pos[3];
        pos[0] = 'a' + s.x;
        pos[1] = '1' + s.y;
        pos[2] = 0;
        return std::string(pos);
    }

    std::string move_str(move_s m)
    {
        return coord_str(m.p0) + "-" + coord_str(m.p1);
    }

    error_e str_move(std::string s, move_s &m)
    {
        // Simple parse only!
        std::vector<std::string> c = split_string(s, '-');
        if ((c.size() < 2) || (c.size() > 3))
            return e_invalid_move;
        if ((!coord_int(c[0], m.p0)) ||
            (!coord_int(c[1], m.p1)))
            return e_invalid_move;
        if (c.size() == 3)
            m.promote = char_abbr(c[2][0]);
        return e_none;
    }

    error_e str_move(std::string s, chessboard &b, move_s &m)
    {
        // *** NATHANAEL ***
        // This is intended for more advanced
        // advanced move parsing (PGN)
        return e_invalid_move;
    }

    std::string time_str(int32_t t)
    {
        char tbuf[32];
        int32_t tenths = (t % 100) / 10;
        int32_t seconds = (t / 1000) % 60;
        int32_t minutes = (t / 60000) % 60;
        int32_t hours = (t / 360000);
        sprintf_s(tbuf, 32, "%d:%2.2d:%2.2d.%d", hours, minutes, seconds, tenths);
        std::string ret(tbuf);
        return ret;
    }

    bool coord_int(std::string s, coord_s &c)
    {
        if (s.length() != 2)
            return false;
        s = lowercase(s);
        c.x = (int8_t)(s[0] - 'a');
        c.y = (int8_t)(s[1] - '1');
        return true;
    }

    bool in_range(coord_s s)
    {
        if ((s.y < 0) || (s.y > 7))
            return false;
        if ((s.x < 0) || (s.x > 7))
            return false;
        return true;
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

    bool contains_move(std::vector<move_s> possible_moves, move_s &m, bool inherit)
    {
        for (size_t i = 0; i < possible_moves.size(); i++)
        {
            if (possible_moves[i].p0 == m.p0)
                if (possible_moves[i].p1 == m.p1)
                {
                    // inherit is allowing a user move to pick up the en passant and
                    // castle flags the CPU would have determined possible for the
                    // square so the user move need only be a coordinate not additional
                    // instructions.
                    if (inherit)
                        if (possible_moves[i].promote == 0) // don't do this for promotion
                            m = possible_moves[i];
                    return true;
                }
        }
        return false;
    }

    move_s new_move(coord_s p0, coord_s p1, int8_t cx, bool en_passant)
    {
        move_s m(p0, p1, cx, en_passant);
        return m;
    }

    move_s new_move(coord_s p0, coord_s p1, piece_e promote)
    {
        move_s m(p0, p1, promote);
        return m;
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

    std::string uppercase(std::string l)
    {
        std::string cmdu = l;
        std::transform(cmdu.begin(), cmdu.end(), cmdu.begin(), ::toupper);
        return cmdu;
    }

    std::string lowercase(std::string u)
    {
        std::string cmdu = u;
        std::transform(cmdu.begin(), cmdu.end(), cmdu.begin(), ::tolower);
        return cmdu;
    }

    const char *ws = " \t\n\r\f\v";

    // trim from end of string (right)
    inline std::string rtrim(std::string s, const char *t = ws)
    {
        s.erase(s.find_last_not_of(t) + 1);
        return s;
    }

    // trim from beginning of string (left)
    inline std::string ltrim(std::string s, const char *t = ws)
    {
        s.erase(0, s.find_first_not_of(t));
        return s;
    }

    // trim from both ends of string (right then left)
    inline std::string trim(std::string s, const char *t = ws)
    {
        return ltrim(rtrim(s, t), t);
    }

    std::vector<std::string> split_string(std::string cmd, char div)
    {
        std::vector<std::string> ret;
        std::string rem = cmd;
        size_t pos = rem.find(div);
        while (pos != std::string::npos)
        {
            ret.push_back(trim(rem.substr(0, pos)));
            rem = rem.substr(pos + 1);
            pos = rem.find(div);
        }
        if (rem != "")
            ret.push_back(trim(rem));
        return ret;
    }

    void write_hex_uchar(std::ofstream &file1, unsigned char c)
    {
        file1 << std::hex << std::setw(2) << std::setfill('0') << (int)c;
        file1 << std::endl;
    }

    unsigned char read_hex_uchar(std::string line)
    {
        return (unsigned char)std::stoi(line, 0, 16);
    }
}