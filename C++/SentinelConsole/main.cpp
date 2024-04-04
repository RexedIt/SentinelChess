#include <stdio.h>
#include <string>
#include <windows.h>
#include <conio.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <set>

#include "chesslobby.h"
#include "console.h"
#include "chessplayer.h"
#include "chesspiece.h"
#include "chessopenings.h"

using namespace chess;

bool print_error(error_e num)
{
    std::cout << "ERROR: " << errorstr(num) << std::endl;
    return false;
}

bool print_error(std::string msg)
{
    std::cout << "ERROR: " << msg << std::endl;
    return false;
}

std::string get_arg(std::string cmdu)
{
    size_t pos = cmdu.find(' ');
    if (pos == std::string::npos)
        return "";
    return cmdu.substr(pos + 1);
}

std::vector<std::string> get_args(std::string cmdu)
{
    return split_string(get_arg(cmdu), ' ');
}

std::vector<std::string> get_args(std::string cmdu, char sep)
{
    return split_string(cmdu, sep);
}

bool get_move(std::string cmdu, color_e c, chessboard b, chessmove &m)
{
    error_e err = str_move(cmdu, c, b, m);
    if (err != e_none)
        return print_error(err);
    return true;
}

bool get_promotion(chessmove &m)
{
    std::string cmd;
    std::cout << "Promote pawn to [Q,R,N,B] : ";
    std::getline(std::cin, cmd);
    if (cmd.length() > 0)
        m.promote = char_abbr(cmd[0]);
    if (m.promote == p_none)
        return false;
    return true;
}

// *** Callbacks from Player Object(s)
std::shared_ptr<chessgame> p_game = NULL;
std::set<color_e> locals;

void refresh_data(chesslobby &lobby)
{
    p_game = lobby.game();
    locals = lobby.local_players();
}

bool load_game(std::string cmd, chesslobby &lobby)
{
    std::string filename = get_arg(cmd);
    if (filename == "")
        return print_error(e_missing_filename);
    std::string errextra;
    error_e err = lobby.load_game(filename, errextra);
    if (err != e_none)
    {
        if (errextra != "")
            print_error(errextra);
        return print_error(e_loading);
    }
    refresh_data(lobby);
    return true;
}

bool load_puzzle(std::string userfile, chesslobby &lobby)
{
    std::string filename = userfile;
    if (filename == "")
        filename = data_file("db_puzzles.csv");
    std::string name = "Human";
    int skill = 600;
    int rating = 700;
    std::cout << "\r\nEnter Player Options: name, skill, puzzle rating, keywords\r\n"
              << std::endl;
    std::cout << "Player: ";
    std::string cmd;
    std::getline(std::cin, cmd);
    std::vector<std::string> args = get_args(cmd, ',');
    if (args.size() > 0)
        name = args[0];
    if (args.size() > 1)
    {
        skill = atoi(args[1].c_str());
        rating = skill + 100;
    }
    if (args.size() > 2)
        rating = atoi(args[2].c_str());
    std::string keywords = "";
    for (size_t i = 3; i < args.size(); i++)
    {
        if (keywords != "")
            keywords += ",";
        keywords += args[i];
    }
    error_e err = lobby.load_puzzle(name, skill, filename, keywords, rating);
    if (err != e_none)
        return print_error(err);
    refresh_data(lobby);
    std::cout << p_game->title() << std::endl;
    int p = p_game->points();
    std::cout << "Worth up to " << p << " points. ";
    int n = p_game->hints();
    if (n > 0)
        std::cout << "You have " << n << " hints.";
    std::cout << std::endl;
    return true;
}

bool save_game(std::string cmd, chesslobby &lobby)
{
    std::string filename = get_arg(cmd);
    if (filename == "")
        return print_error(e_missing_filename);
    if (lobby.save_game(filename) != e_none)
        return print_error(e_saving);
    std::cout << "Saved." << std::endl;
    return true;
}

bool add_player(chesslobby &lobby, color_e color)
{
    while (true)
    {
        std::cout << color_str(color) << " Player: ";
        std::string cmd;
        std::getline(std::cin, cmd);
        std::vector<std::string> args = get_args(cmd, ',');
        if (args.size() > 3)
            continue;
        std::string name = "Computer";
        int skill = 600;
        chessplayertype_e ptype = t_computer;

        if (args.size() >= 1)
            name = args[0];

        if (args.size() >= 2)
        {
            std::string ptypes = uppercase(args[1]);
            ptype = playertypefromstring(ptypes);
            if (ptype == p_none)
            {
                print_error("Invalid Player Type");
                continue;
            }
        }

        if (args.size() >= 3)
        {
            skill = atoi(args[2].c_str());
            if ((skill < 0) || (skill > 2000))
            {
                print_error("Skill must be 0-2000");
                continue;
            }
        }

        if (lobby.add_player(color, name, skill, ptype) != e_none)
        {
            print_error("Error adding player");
            return false;
        }
        return true;
    }
}

int32_t get_ms_value(std::string label, std::string units, double multvalue, double defvalue)
{
    while (true)
    {
        std::cout << label << " [" << defvalue << " " << units << "]: ";
        std::string cmd;
        std::getline(std::cin, cmd);
        double temp = defvalue;
        if (cmd != "")
            temp = ::atof(cmd.c_str());
        if (temp >= 0.0)
            return (int32_t)(temp * multvalue);
    }
}

chessclock_s get_clock_options()
{
    chessclock_s ret;
    while (true)
    {
        std::cout << "\r\nFor clock, Enter/N=None, D=Sudden Death," << std::endl;
        std::cout << "I=Increment, B=Bronstein, S=Simple Delay: ";
        std::string cmd;
        std::getline(std::cin, cmd);
        if (cmd == "")
            return ret;
        std::string cmdu = uppercase(cmd);
        std::string cmdl = cmdu.substr(0, 1);
        if (cmdl == "N")
            return ret;
        if (cmdl == "D")
        {
            ret.ctype = cc_suddendeath;
            break;
        }
        if (cmdl == "I")
        {
            ret.ctype = cc_increment;
            break;
        }
        if (cmdl == "B")
        {
            ret.ctype = cc_bronstein_delay;
            break;
        }
        if (cmdl == "S")
        {
            ret.ctype = cc_simple_delay;
            break;
        }
        print_error("Choose one of the options for clock");
    }
    // get delay range
    ret.allowedms[0] = get_ms_value("White time", "Minutes", 60000.0, 10.0);
    ret.allowedms[1] = get_ms_value("Black time", "Minutes", 60000.0, 10.0);
    if (ret.ctype > cc_suddendeath)
    {
        ret.addms[0] = get_ms_value("White bonus/delay", "Seconds", 1000.0, 10.0);
        ret.addms[1] = get_ms_value("Black bonus/delay", "Seconds", 1000.0, 10.0);
    }
    ret.remainms[0] = ret.allowedms[0];
    ret.remainms[1] = ret.allowedms[1];
    return ret;
}

bool new_game(chesslobby &lobby)
{
    std::cout << "\r\nEnter Player Options: name [,type] [,skill]" << std::endl;
    std::cout << "where type [optional] can be either Human or Computer" << std::endl;
    std::cout << "and skill [optional] can be 0-2000\r\n"
              << std::endl;
    lobby.clear_players();
    if (!add_player(lobby, c_white))
        return false;
    if (!add_player(lobby, c_black))
        return false;
    // Now, for clock options
    chessclock_s clock = get_clock_options();
    error_e err = lobby.new_game("New Game - Console", clock);
    if (err != e_none)
        return print_error(err);
    refresh_data(lobby);
    return true;
}

void refresh_board(int16_t t, chessboard &b)
{
    color_e bottom = c_white;
    if (locals.count(c_white) == 0)
        if (locals.count(c_black))
            bottom = c_black;
    color_e turn_color = b.turn_color();
    if (locals.count(turn_color) > 0)
        bottom = turn_color;
    board_to_console(t, b, bottom);
}

void on_consider(chessmove m, color_e c, int8_t p)
{
    std::cout << ".";
}

int32_t time_rem = 0;
bool is_idle = false;

void on_turn(int16_t t, chessmove m, bool ch, chessboard &b, color_e tc, game_state_e g, color_e wc, int32_t wt, int32_t bt)
{
    time_to_console(wt, bt);
    time_rem = (tc == c_black) ? bt : wt;
    refresh_board(t, b);
    if (m.is_valid())
    {
        color_e c = other(tc);
        if (!locals.count(c))
            move_to_console(m, color_str(c));
        if (b.check_state(c_black))
            std::cout << "Black in Check." << std::endl;
        else if (b.check_state(c_white))
            std::cout << "White in Check." << std::endl;
    }
    if (g > play_e)
    {
        std::cout << game_state_str(g) << std::endl;
        if (wc != c_none)
            std::cout << color_str(wc) + " Wins! ";
        is_idle = true;
    }
}

std::string prompt()
{
    if (is_idle)
        return "Command (Idle) > ";
    color_e c = p_game->turn_color();
    bool ch = p_game->check_state(c);
    std::string s;
    if (time_rem > 0)
        s += "[" + time_str(time_rem) + "] ";
    s += color_str(c) + " move ";
    if (ch)
        s += "(Check) ";
    s += "> ";
    return s;
}

void on_state(game_state_e g, color_e c)
{
    if (g > play_e)
    {
        std::cout << "\r\n**** Game Over! ";
        std::cout << game_state_str(g) + " ";
        if (c != c_none)
            std::cout << color_str(c) + " Wins! ";
        std::cout << "****" << std::endl;
    }
    if ((g == idle_e) || (g > play_e))
        is_idle = true;
}

void on_chat(std::string msg, color_e c)
{
}

void process_queue_listener(std::shared_ptr<chessgamelistener_queue> p_listener)
{
    while (p_listener->has_event())
    {
        chessevent e = p_listener->pop_event();
        switch (e.etype)
        {
        case ce_refresh_board:
            refresh_board(e.turn_no, e.board);
            break;
        case ce_consider:
            on_consider(e.move, e.color, e.percent);
            break;
        case ce_turn:
            on_turn(e.turn_no, e.move, e.check, e.board, e.color, e.game_state, e.win_color, e.wt, e.bt);
            break;
        case ce_state:
            on_state(e.game_state, e.win_color);
            break;
        case ce_chat:
            on_chat(e.msg, e.color);
            break;
        }
    }
}

int main(void)
{

    if (!set_data_folder("..\\..\\..\\..\\ChessData\\"))
        print_error("Data folder does not exist! Try SET command with path to ChessData");

    /*
    std::shared_ptr<chessgamelistener_direct> p_listener(
        new chessgamelistener_direct(cl_user,
                                     &refresh_board,
                                     &on_consider,
                                     &on_turn,
                                     &on_state,
                                     &on_chat)); */

    // Initialize the eco db
    chessecodb co;
    // co.load_scid_eco(data_file("..\\SourceData\\scid.eco"));
    // co.save_binary(data_file("scid.bin"));
    co.load_binary(data_file("scid.bin"));

    std::shared_ptr<chessgamelistener_queue> p_listener(
        new chessgamelistener_queue(cl_user));

    chesslobby lobby(p_listener);

    while (true)
    {
        process_queue_listener(p_listener);
        std::cout << "\r\nWelcome to Sentinel Console Chess!!!\r\n";
        std::cout << "\r\n(N)EW, (L)OAD FileName, PU(Z)ZLE or (Q)UIT?\r\n";
        std::string cmd;
        std::cout << "> ";
        std::getline(std::cin, cmd);
        std::string cmdu = uppercase(cmd);
        std::string cmdl = cmdu.substr(0, 1);
        if (cmdl == "N")
        {
            if (new_game(lobby))
                break;
        }
        else if (cmdl == "L")
        {
            if (load_game(cmd, lobby))
                break;
        }
        else if ((cmdl == "Z") || (cmdu == "PUZZLE"))
        {
            std::string filename = get_arg(cmd);
            if (load_puzzle(filename, lobby))
                break;
        }
        else if (cmdl == "Q")
        {
            exit(EXIT_SUCCESS);
        }
    }

    while (true)
    {
        process_queue_listener(p_listener);
        if (lobby.is_local_turn() || is_idle)
        {
            std::cout << prompt();
            std::string cmd;
            std::getline(std::cin, cmd);
            std::string cmdu = uppercase(cmd);
            std::string cmdl = cmdu.substr(0, 1);
            if ((cmdu == "?") || (cmdu == "HELP") || (cmdu == "H"))
            {
                std::cout << "\r\nCommands: (N)EW, (L)OAD Filename, (S)AVE FileName, PU(Z)ZLE FileName Difficulty, " << std::endl;
                std::cout << "\t(P)LAY, (I)DLE, (Q)UIT, ";
                if (!is_idle)
                    std::cout << "(M)OVE MoveStr, HINT, ";
                std::cout << "<, >, T Turn PIECE Coord Piece, - [Coord], (X)FEN [String] " << std::endl;
                continue;
            }
            if (cmdl == "N")
            {
                if (new_game(lobby))
                    continue;
            }
            else if (cmdl == "L")
            {
                if (load_game(cmd, lobby))
                    continue;
            }
            else if ((cmdl == "Z") || (cmdu == "PUZZLE"))
            {
                std::string filename = get_arg(cmd);
                if (load_puzzle(filename, lobby))
                    continue;
            }
            else if (cmdl == "S")
            {
                save_game(cmd, lobby);
                continue;
            }
            else if (cmdl == "Q")
            {
                exit(EXIT_SUCCESS);
            }
            else if (cmdl == "<")
            {
                if (p_game->rewind_game() != e_none)
                {
                    print_error(e_rewind_failed);
                    continue;
                }
            }
            else if (cmdl == ">")
            {
                if (p_game->advance_game() != e_none)
                {
                    print_error(e_advance_failed);
                    continue;
                }
            }
            else if (cmdl == "T")
            {
                cmdu = get_arg(cmdu);
                if (cmdu == "")
                {
                    print_error(e_missing_move);
                    continue;
                }
                int turnno = atoi(cmdu.c_str());
                if (p_game->goto_turn(turnno) != e_none)
                {
                    print_error(e_advance_failed);
                    continue;
                }
            }
            else if (cmdl == "P")
            {
                p_game->play_game();
                is_idle = false;
                continue;
            }
            else if (cmdl == "I")
            {
                p_game->pause_game();
                is_idle = true;
                continue;
            }
            else if (cmdl == "-")
            {
                cmdu = get_arg(cmdu);
                if (cmdu == "")
                {
                    print_error(e_missing_coord);
                    continue;
                }
                coord_s p0;
                if (!coord_int(cmdu, p0))
                {
                    print_error(e_invalid_coord);
                    continue;
                }
                p_game->remove_piece(p0);
            }
            else if (cmdl == "+")
            {
                std::vector<std::string> args = get_args(cmd);
                if (args.size() != 2)
                {
                    print_error(e_missing_coord_piece);
                    continue;
                }
                coord_s p0;
                if (!coord_int(args[0], p0))
                {
                    print_error(e_invalid_coord);
                    continue;
                }
                if (args[1].size() != 1)
                {
                    print_error(e_missing_piece);
                    continue;
                }
                chesspiece p1(args[1][0]);
                if (p1.ptype == p_none)
                {
                    print_error(e_invalid_piece);
                    continue;
                }
                p_game->add_piece(p0, p1);
            }
            if (cmdl == "X")
            {
                // XFEN string
                cmdu = get_arg(cmd);
                if (cmdu == "")
                    std::cout << p_game->save_xfen() << std::endl;
                else if (p_game->load_xfen(cmdu) != e_none)
                {
                    print_error(e_xfen_read);
                    continue;
                }
            }
            else
            {
                if (!is_idle)
                {
                    if (cmdu == "HINT")
                    {
                        int n = p_game->hints();
                        cmd = get_arg(cmd);
                        if ((cmd != "?") && (n > 0))
                            std::cout << "*** " << p_game->hintstr() << " *** ";
                        if (n == 0)
                            std::cout << "No hints remain.\r\n\r\n";
                        else
                            std::cout << n << "hints remain.\r\n\r\n";
                        continue;
                    }
                    if (cmdl == "M")
                    {
                        cmd = get_arg(cmd);
                        if (cmd == "")
                        {
                            print_error(e_missing_move);
                            continue;
                        }
                    }
                    chessmove m;
                    if (!get_move(cmd, p_game->turn_color(), p_game->board(), m))
                        continue;
                    color_e whose_turn = p_game->turn_color();
                    error_e err = p_game->move(whose_turn, m);
                    if (err == e_invalid_move_needs_promote)
                        if (!get_promotion(m))
                        {
                            print_error("You need to select a promotion piece for this move, try again.");
                            continue;
                        }
                        else
                        {
                            p_game->move(whose_turn, m);
                        }
                    if (err != e_none)
                    {
                        print_error(err);
                        continue;
                    }
                }
            }
        }
        else
        {
            if (_kbhit())
            {
                int c = _getch();
                if ((c == 'i') || (c == 'I'))
                {
                    p_game->pause_game();
                    is_idle = true;
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(25));
        }
    }
}
