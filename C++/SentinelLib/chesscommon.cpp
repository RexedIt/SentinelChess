#include <ctime>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <chrono>

#include "chesscommon.h"
#include "chesspiece.h"
#include "chessboard.h"
#include "chessmove.h"

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
            return "Invalid Move";
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
            return "Invalid Coordinate";
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
        case e_none_found:
            return "Nothing found";
        case e_incorrect_move:
            return "Incorrect move";
        case e_no_openings:
            return "No Chess Opening Data";
        case e_pgn_parse:
            return "PGN Parsing Error";
        case e_invalid_extension:
            return "Invalid Extension, must be CHS or PGN";
        case e_play_not_paused:
            return "Play not paused";
        default:
            return "Unknown Error";
        }
    }

    const char *c_open_board = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    chessclock_s::chessclock_s()
    {
        ctype = cc_none;
        for (int i = 0; i < 2; i++)
        {
            allowedms[i] = 0;
            remainms[i] = 0;
            addms[i] = 0;
        }
    }

    chessclock_s::chessclock_s(const chessclock_s &c)
    {
        ctype = c.ctype;
        for (int i = 0; i < 2; i++)
        {
            allowedms[i] = c.allowedms[i];
            remainms[i] = c.remainms[i];
            addms[i] = c.addms[i];
        }
    }

    void chessclock_s::operator=(const chessclock_s &c)
    {
        ctype = c.ctype;
        for (int i = 0; i < 2; i++)
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
        for (int i = 0; i < 2; i++)
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

    std::string clock_type_str(chessclock_e ct)
    {
        switch (ct)
        {
        case cc_suddendeath:
            return "Sudden Death";
        case cc_increment:
            return "Increment";
        case cc_bronstein_delay:
            return "Bronstein Delay";
        case cc_simple_delay:
            return "Simple Delay";
        default:
            return "None";
        }
    }

    chessclock_e str_clock_type(std::string ct)
    {
        chessclock_e ret = cc_none;
        std::string ctu = uppercase(ct);
        if (ctu == "SUDDEN DEATH")
            ret = cc_suddendeath;
        else if (ctu == "INCREMENT")
            ret = cc_increment;
        else if (ctu == "BRONSTEIN DELAY")
            ret = cc_bronstein_delay;
        else if (ctu == "SIMPLE DELAY")
            ret = cc_simple_delay;
        return ret;
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
        case puzzle_solution_e:
            return "Puzzle Solution";
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
        if (gu == "PUZZLE SOLUTION")
            return puzzle_solution_e;
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

    bool coord_int(std::string s, coord_s &c)
    {
        if (s.length() != 2)
            return false;
        s = lowercase(s);
        int8_t x = (int8_t)(s[0] - 'a');
        int8_t y = (int8_t)(s[1] - '1');
        if (in_range(y, x))
        {
            c.x = (int8_t)(s[0] - 'a');
            c.y = (int8_t)(s[1] - '1');
            return true;
        }
        return false;
    }

    bool is_coord(std::string s)
    {
        if (s.length() != 2)
            return false;
        int8_t x = (int8_t)(s[0] - 'a');
        int8_t y = (int8_t)(s[1] - '1');
        return in_range(y, x);
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

    std::string coord_str(coord_s s)
    {
        char pos[3];
        pos[0] = 'a' + s.x;
        pos[1] = '1' + s.y;
        pos[2] = 0;
        return std::string(pos);
    }

    std::string time_str(int32_t t)
    {
        char tbuf[32];
        int32_t tenths = (t % 100) / 10;
        int32_t seconds = (t / 1000) % 60;
        int32_t minutes = (t / 60000) % 60;
        int32_t hours = (t / 3600000);
        sprintf_s(tbuf, 32, "%d:%2.2d:%2.2d.%d", hours, minutes, seconds, tenths);
        std::string ret(tbuf);
        return ret;
    }

    bool is_color(unsigned char cell, color_e color)
    {
        return (color_e)(cell & color_mask) == color;
    }

    color_e other(color_e t)
    {
        return t == c_white ? c_black : c_white;
    }

    static volatile bool _sinit = false;
    float get_rand()
    {
        if (!_sinit)
        {
            using namespace std::chrono;
            milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
            srand((unsigned int)ms.count());
            _sinit = true;
        }
        return (float)rand() / (float)RAND_MAX;
    }

    int get_rand_int(int min, int max)
    {
        float r = get_rand();
        r = r * (float)(max - min);
        return min + (int)r;
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

    bool starts_with(std::string a, std::string b)
    {
        if (a.length() < b.length())
            return false;
        return (a.substr(0, b.length()) == b);
    }

    bool ends_with(std::string a, std::string b)
    {
        if (a.length() < b.length())
            return false;
        return (a.substr(a.length() - b.length(), b.length()) == b);
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
    std::string trim(std::string s)
    {
        return ltrim(rtrim(s));
    }

    std::vector<std::string> split_string(std::string cmd, char div)
    {
        std::vector<std::string> ret;
        std::string rem = cmd;
        size_t pos = rem.find(div);
        bool last_div = false;
        while (pos != std::string::npos)
        {
            last_div = (pos == rem.length() - 1);
            std::string trimmed = trim(rem.substr(0, pos));
            if ((div != ' ') || (trimmed != ""))
                ret.push_back(trim(rem.substr(0, pos)));
            rem = rem.substr(pos + 1);
            pos = rem.find(div);
        }
        if ((rem != "") || (last_div))
            ret.push_back(trim(rem));
        return ret;
    }

    uintmax_t get_file_size(std::string filename)
    {
        std::string fixedfile = fix_path(filename);
        struct stat stat_buf;
        uintmax_t rc = stat(fixedfile.c_str(), &stat_buf);
        return rc == 0 ? stat_buf.st_size : -1;
    }

    bool get_dir_exists(std::string dirname)
    {
        std::string fixeddir = fix_path(dirname);
        struct stat st;
        return (stat(fixeddir.c_str(), &st) == 0);
    }

    std::string data_folder = "..\\ChessData\\";

    std::string get_data_folder()
    {
        return data_folder;
    }

    bool set_data_folder(std::string f)
    {
        if (!get_dir_exists(f))
            return false;
        data_folder = f;
        return true;
    }

    std::string string_replace(std::string s, char o, char n)
    {
        std::string ret = s;
        std::replace(ret.begin(), ret.end(), o, n);
        return ret;
    }

    inline char separator()
    {
#ifdef _WIN32
        return '\\';
#else
        return '/';
#endif
    }

    std::string fix_path(std::string f)
    {
#ifdef _WIN32
        return string_replace(f, '/', '\\');
#else
        return string_replace(f, '\\', '/');
#endif
    }

    std::string data_file(std::string f)
    {
        if (data_folder == "")
            return f;
        char lc = data_folder[data_folder.length() - 1];
        if ((lc == '\\') || (lc == '/'))
            return fix_path(data_folder + f);
        return fix_path(data_folder + "\\" + f);
    }

    uint32_t hash(unsigned char *b, size_t l)
    {
        uint32_t ret = 0;
        uint32_t accv = 0;
        int n = 0;
        for (size_t i = 0; i < l; i++)
        {
            accv = (accv << 8) + b[i];
            if (++n >= 4)
            {
                ret += (ret << 1) + (ret << 4) + (ret << 7) + (ret << 8) + (ret << 24);
                ret ^= accv;
                accv = 0;
                n = 0;
            }
        }
        if (n)
        {
            ret += (ret << 1) + (ret << 4) + (ret << 7) + (ret << 8) + (ret << 24);
            ret ^= accv;
        }
        return ret;
    }

    uint32_t hash(std::vector<chessmove> moves)
    {
        // for moves we only want to hash the from/to elements of the move.
        unsigned char buf[256];
        if (moves.size() > 64)
            return 0;
        int p = 0;
        for (size_t i = 0; i < moves.size(); i++)
        {
            buf[p++] = moves[i].p0.y;
            buf[p++] = moves[i].p0.x;
            buf[p++] = moves[i].p1.y;
            buf[p++] = moves[i].p1.x;
        }
        return hash(buf, moves.size() * 4);
    }

    bool little_endian()
    {
        int n = 1;
        return (*(char *)&n == 1);
    }

    void save_nl(std::ofstream &of, char *c, size_t l)
    {
        if ((little_endian()) && (l <= 8))
        {
            char d[8];
            size_t i = l;
            size_t j = 0;
            while (i > 0)
                d[--i] = c[j++];
            of.write(d, l);
        }
        else
        {
            of.write(c, l);
        }
    }

    void load_nl(std::ifstream &inf, char *c, size_t l)
    {
        if ((little_endian()) && (l <= 8))
        {
            char d[8];
            inf.read(d, l);
            size_t i = l;
            size_t j = 0;
            while (i > 0)
                c[--i] = d[j++];
        }
        else
        {
            inf.read(c, l);
        }
    }

    bool save_binary(std::ofstream &of, size_t v)
    {
        int32_t i = (int32_t)v;
        save_nl(of, (char *)&i, sizeof(i));
        return true;
    }

    bool load_binary(std::ifstream &inf, size_t &v)
    {
        int32_t i = 0;
        load_nl(inf, (char *)&i, sizeof(i));
        v = (size_t)i;
        return true;
    }

    bool save_binary(std::ofstream &of, uint32_t &v)
    {
        save_nl(of, (char *)&v, sizeof(v));
        return true;
    }

    bool load_binary_u(std::ifstream &inf, uint32_t &v)
    {
        load_nl(inf, (char *)&v, sizeof(v));
        return true;
    }

    bool save_binary(std::ofstream &of, std::string &v)
    {
        save_binary(of, v.length());
        of.write(v.c_str(), v.length());
        return true;
    }

    bool load_binary(std::ifstream &inf, std::string &v)
    {
        size_t l = 0;
        load_binary(inf, l);
        char buf[256];
        if ((l < 0) || (l > 255))
            return false;
        inf.read(buf, l);
        buf[l] = 0;
        v = std::string(buf);
        return true;
    }

    bool save_binary(std::ofstream &of, chessmove v)
    {
        int8_t buf[5];
        buf[0] = v.p0.y;
        buf[1] = v.p0.x;
        buf[2] = v.p1.y;
        buf[3] = v.p1.x;
        of.write((char *)buf, 4);
        return true;
    }

    bool load_binary(std::ifstream &inf, chessmove &v)
    {
        int8_t buf[5];
        inf.read((char *)&buf, 4);
        v = new_move(buf[0], buf[1], buf[2], buf[3]);
        return true;
    }

    bool save_binary(std::ofstream &of, std::vector<chessmove> &v)
    {
        save_binary(of, v.size());
        for (size_t i = 0; i < v.size(); i++)
            save_binary(of, v[i]);
        return true;
    }

    bool load_binary(std::ifstream &inf, std::vector<chessmove> &v)
    {
        size_t l = 0;
        load_binary(inf, l);
        if ((l < 0) || (l > 64))
            return false;
        for (size_t i = 0; i < l; i++)
        {
            chessmove m;
            if (!load_binary(inf, m))
                return false;
            v.push_back(m);
        }
        return true;
    }

}