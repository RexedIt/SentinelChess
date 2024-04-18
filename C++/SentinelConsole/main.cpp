#include <stdio.h>
#include <string>
#include <windows.h>
#include <conio.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <set>

#include "chessengine.h"
#include "chesslobby.h"
#include "console.h"
#include "chessplayer.h"
#include "chesspiece.h"

#include "chessopenings.h"
#include "chessplayerhub.h"

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

std::string get_arg_rem(std::string cmdu)
{
    size_t l = cmdu.find(' ');
    if (l == std::string::npos)
        return "";
    return trim(cmdu.substr(l));
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
int32_t time_rem = 0;
bool is_idle = false;

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

chessplayerdata get_hub_player(int32_t elo = 0)
{
    // is going to return a player object
    chessplayerdata pd;
    while (true)
    {
        std::cout << "\r\nHuman Players:\r\n--------------" << std::endl;
        std::vector<chessplayerdata> vec;
        error_e err = chessengine::hub_players(vec, t_human, elo);
        if (err != e_none)
        {
            print_error(err);
            return pd;
        }
        int i = 0;
        for (chessplayerdata pd : vec)
            std::cout << ++i << ". " << pd.username << "\t" << pd.elo << std::endl;
        std::cout << "--------------" << std::endl;
        std::cout << "Type Index, (D)EL Index, or (Q)UIT, (A)DD username, skill [,perm]" << std::endl;
        std::cout << " where skill=1-2500, perm=1 is optional and means permanent" << std::endl;
        std::cout << "Player: ";
        std::string cmd;
        std::getline(std::cin, cmd);
        std::vector<std::string> args = get_args(cmd, ',');
        if (cmd == "")
            continue;
        int index = atoi(cmd.c_str());
        if ((index < 0) || (index > (int)vec.size() + 1))
        {
            std::cout << " *** Invalid Index" << std::endl;
            continue;
        }
        else if (index > 0)
        {
            return vec[index - 1];
        }
        std::string cmdu = uppercase(cmd);
        std::string cmdl = cmdu.substr(0, 1);
        if (cmdl == "Q")
        {
            return pd;
        }
        if (cmdl == "A")
        {
            if ((args.size() < 2) || (args.size() > 3))
            {
                std::cout << " *** Invalid arguments, specify username (no spaces) and elo" << std::endl;
                continue;
            }
            std::vector<std::string> au = get_args(args[0], ' ');
            if (au.size() != 2)
            {
                std::cout << " *** Invalid arguments, specify username (no spaces) and elo" << std::endl;
                continue;
            }
            std::string name = au[1];
            int32_t skill = atoi(args[1].c_str());
            if ((skill < 0) || (skill > 2500))
            {
                print_error("Skill/elo must be 0-2500");
                continue;
            }
            bool perm = false;
            if (args.size() == 3)
                perm = atoi(args[2].c_str()) != 0;
            chessplayerdata pd = new_human_player(name, skill);
            pd.persistent = perm;
            err = chessengine::hub_register_player(pd);
            if (err != e_none)
            {
                print_error(err);
                continue;
            }
        }
        if (cmdl == "D")
        {
            if (args.size() != 2)
            {
                std::cout << " *** Invalid arguments, specify index of user to delete" << std::endl;
                continue;
            }
            int index = atoi(args[1].c_str());
            if ((index < 0) || (index > (int)vec.size() + 1))
            {
                std::cout << " *** Invalid Index" << std::endl;
                continue;
            }
            else if (index > 0)
            {
                err = chessengine::hub_unregister(vec[index - 1].guid);
                if (err != e_none)
                {
                    print_error(err);
                    continue;
                }
            }
        }
    }
    return pd;
}

chessplayerdata get_hub_computer(int32_t elo = 0)
{
    // is going to return a player data
    chessplayerdata pd;
    while (true)
    {
        std::cout << "\r\nComputer Players:\r\n-----------------" << std::endl;
        std::vector<chessplayerdata> vec;
        error_e err = chessengine::hub_players(vec, t_computer, elo);
        if (err != e_none)
        {
            print_error(err);
            return pd;
        }
        int i = 0;
        for (chessplayerdata pd : vec)
            std::cout << ++i << ". " << pd.username << "\t" << pd.elo << std::endl;
        std::cout << "-----------------" << std::endl;
        std::cout << "Type Number, or (Q)UIT" << std::endl;
        std::cout << "Computer: ";
        std::string cmd;
        std::getline(std::cin, cmd);
        std::vector<std::string> args = get_args(cmd, ',');
        if (cmd == "")
            continue;
        int index = atoi(cmd.c_str());
        if ((index < 0) || (index > (int)vec.size() + 1))
        {
            std::cout << " *** Invalid Index" << std::endl;
            continue;
        }
        else if (index > 0)
        {
            return vec[index - 1];
        }
        std::string cmdu = uppercase(cmd);
        std::string cmdl = cmdu.substr(0, 1);
        if (cmdl == "Q")
            break;
    }
    return pd;
}

bool load_puzzle(std::string userfile, chesslobby &lobby)
{
    std::string filename = userfile;
    if (filename == "")
        filename = chessengine::data_file("db_puzzles.csv");
    std::string name = "Human";
    int skill = 600;
    int rating = 700;
    std::cout << "\r\nEnter Player Options: name, skill, puzzle rating, keywords" << std::endl;
    std::cout << "  *** use HUB for name along with type to select.\r\n"
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

    chessplayerdata player;
    error_e err = e_none;
    if (uppercase(name) == "HUB")
    {
        player = get_hub_player(skill);
        if (player.guid == "")
        {
            std::cout << "Try Again.\r\n\r\n";
            return false;
        }
    }
    else
    {
        err = chessengine::hub_get_or_register_player(player, name, skill, t_human);
    }

    if (err != e_none)
        return print_error(err);

    err = lobby.load_puzzle(player, filename, keywords, rating);
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
        std::cout << "\r\n"
                  << color_str(color) << " Player: ";
        std::string cmd;
        std::getline(std::cin, cmd);
        std::vector<std::string> args = get_args(cmd, ',');
        if (args.size() > 3)
            continue;
        std::string name = "Computer";
        int skill = 500;
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
            if ((skill < 0) || (skill > 2500))
            {
                print_error("Skill/elo must be 0-2500");
                continue;
            }
        }

        chessplayerdata player;
        error_e err = e_none;
        if (uppercase(name) == "HUB")
        {
            if (ptype == t_human)
                player = get_hub_player(skill);
            else
                player = get_hub_computer(skill);
            if (player.guid == "")
            {
                std::cout << "Try Again.\r\n\r\n";
                continue;
            }
        }
        else
        {
            err = chessengine::hub_get_or_register_player(player, name, skill, ptype);
        }

        if (err != e_none)
        {
            print_error(err);
            return false;
        }

        if (ptype == t_computer)
        {
            std::cout << ">>>> FOUND: " << player.username << " ELO: " << player.elo << std::endl;
        }
        err = lobby.add_player(color, player);
        if (err != e_none)
        {
            print_error(err);
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
    std::cout << "  where type [optional] can be either Human or Computer" << std::endl;
    std::cout << "  and skill [optional] can be 0-2000" << std::endl;
    std::cout << "  *** use HUB for name along with type to select.\r\n\r\n";
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
    is_idle = false;
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
    board_to_console(t + 1, b, bottom);
}

void refresh_comment(std::string comment)
{
    if (comment == "")
        return;
    std::string stripped = comment;
    if (book_end(stripped, '{', '}'))
        stripped = trim(stripped.substr(1, stripped.length() - 2));
    std::cout << stripped << std::endl;
}

void on_consider(chessmove m, color_e c, int8_t p)
{
    std::cout << ".";
}

void on_turn(int16_t t, chessmove m, bool ch, chessboard &b, color_e tc, game_state_e g, color_e wc, int32_t wt, int32_t bt, std::string cmt)
{
    time_to_console(wt, bt);
    time_rem = (tc == c_black) ? bt : wt;
    refresh_board(t, b);
    if (m.is_valid())
    {
        color_e c = other(tc);
        if ((!locals.count(c)) || is_idle)
            move_to_console(m, p_game->board(t - 1), color_str(c));
        if (b.check_state(c_black))
            std::cout << "Black in Check." << std::endl;
        else if (b.check_state(c_white))
            std::cout << "White in Check." << std::endl;
    }
    refresh_comment(cmt);
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
        return "\r\nCommand (Idle) > ";
    color_e c = p_game->turn_color();
    bool ch = p_game->check_state(c);
    std::string s = "\r\n";
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
            on_turn(e.turn_no, e.move, e.check, e.board, e.color, e.game_state, e.win_color, e.wt, e.bt, e.cmt);
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

int main(int argc, char *argv[])
{
    // Default = EXE folder for chess data
    std::string datapath = "./ChessData";
    bool usage = false;

    for (int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];
        bool has_next = i + 1 < argc;
        if (arg == "--chessdata")
        {
            if (has_next)
                datapath = fix_path(argv[(i++) + 1]);
            else
                usage = true;
        }
    }

    if (usage)
    {
        std::cout << "USAGE: SentinelConsole [--data folder_with_chessdata]" << std::endl;
        return -1;
    }

    if (!ends_with(uppercase(datapath), "\\CHESSDATA"))
        datapath += "\\ChessData";

    error_e err = chessengine::initialize(datapath);
    if (err != e_none)
    {
        print_error(err);
        return -1;
    }

    /*
    std::shared_ptr<chessgamelistener_direct> p_listener(
        new chessgamelistener_direct(cl_user,
                                     &refresh_board,
                                     &on_consider,
                                     &on_turn,
                                     &on_state,
                                     &on_chat)); */

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
                std::cout << "[, <, >, ], T Turn PIECE Coord Piece, - [Coord], (X)FEN [String] (C)OMMENT [String]" << std::endl;
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
            else if (cmdl == "[")
            {
                error_e err = p_game->goto_turn(0);
                if (err != e_none)
                {
                    print_error(err);
                    continue;
                }
            }
            else if (cmdl == "<")
            {
                error_e err = p_game->rewind_game();
                if (err != e_none)
                {
                    print_error(err);
                    continue;
                }
            }
            else if (cmdl == ">")
            {
                error_e err = p_game->advance_game();
                if (err != e_none)
                {
                    print_error(err);
                    continue;
                }
            }
            else if (cmdl == "]")
            {
                error_e err = p_game->goto_turn(p_game->playmax());
                if (err != e_none)
                {
                    print_error(err);
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
                error_e err = p_game->goto_turn(turnno);
                if (err != e_none)
                {
                    print_error(err);
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
            if (cmdl == "C")
            {
                cmdu = get_arg_rem(cmd);
                int t = p_game->playno();
                p_game->comment(t + 1, cmdu);
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
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
}
