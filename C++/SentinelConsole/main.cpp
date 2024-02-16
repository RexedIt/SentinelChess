#include <stdio.h>
#include <iostream>
#include <string>
#include <windows.h>
#include <chrono>
#include <thread>
#include <set>

#include "chesslobby.h"
#include "console.h"
#include "chessplayer.h"

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

bool get_move(std::string cmdu, coord_s &p0, coord_s &p1)
{
    size_t pos = cmdu.find('-');
    if (pos == std::string::npos)
        return print_error(e_missing_move);
    if ((!coord_int(cmdu.substr(0, pos), p0)) ||
        (!coord_int(cmdu.substr(pos + 1), p1)))
        return print_error(e_invalid_move);
    return true;
}

// *** Callbacks from Player Object(s)
std::shared_ptr<chessgame> p_game = NULL;
std::set<color_e> humans;
std::set<color_e> computers;
color_e last_went = c_none;

void refresh_data(chesslobby &lobby)
{
    bool set_cb = false;
    humans.clear();
    computers.clear();
    last_went = c_none;

    p_game = lobby.game();

    std::map<color_e, std::shared_ptr<chessplayer>> p_players = lobby.players();
    for (const auto &kv : p_players)
    {
        if (kv.second->playertype() == t_human)
        {
            humans.insert(kv.second->playercolor());
        }
        else
        {
            computers.insert(kv.second->playercolor());
        }
    }
}

bool load_game(std::string cmd, chesslobby &lobby)
{
    std::string filename = get_arg(cmd);
    if (filename == "")
        return print_error(e_missing_filename);
    error_e err = lobby.load_game(filename);
    if (err != e_none)
        return print_error(e_loading);
    refresh_data(lobby);
    return true;
}

bool save_game(std::string cmd, chesslobby &lobby)
{
    std::string filename = get_arg(cmd);
    if (filename == "")
        return print_error(e_missing_filename);
    if (lobby.save_game(filename) != e_none)
        return print_error(e_saving);
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

bool new_game(chesslobby &lobby)
{
    std::cout << "\r\nEnter Player Options: name [,skill] [,type]" << std::endl;
    std::cout << "where type [optional] can be either Human or Computer" << std::endl;
    std::cout << "and skill [optional] can be 0-2000\r\n"
              << std::endl;
    if (!add_player(lobby, c_white))
        return false;
    if (!add_player(lobby, c_black))
        return false;
    error_e err = lobby.new_game();
    if (err != e_none)
        return print_error(err);
    refresh_data(lobby);
    return true;
}

void refresh_board(int16_t t, chessboard &b)
{
    board_to_console(t, b);
}

void on_consider(move_s &m, color_e c, int8_t p)
{
}

void on_move(int16_t t, move_s &m, color_e c)
{
}

void on_turn(int16_t, bool ch, chessboard &b, color_e c)
{
}

void on_end(game_state_e g, color_e c)
{
}

void chat(std::string msg, color_e c)
{
}

int main(void)
{

    std::shared_ptr<chessgamelistener_direct> p_listener(
        new chessgamelistener_direct(cl_user,
                                     &refresh_board,
                                     &on_consider,
                                     &on_move,
                                     &on_turn,
                                     &on_end,
                                     &chat));

    chesslobby lobby(p_listener);

    while (true)
    {
        std::cout << "\r\nWelcome to Sentinel Console Chess!!!\r\n";
        std::cout << "\r\nNEW, LOAD FileName, or QUIT?\r\n";
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
        else if (cmdl == "Q")
        {
            exit(EXIT_SUCCESS);
        }
    }

    /*
    chessgame game;

    game.set_callbacks(
        &draw_board,
        &game_over,
        &computer_moved,
        &draw_move,
        &request_promote_piece,
        &thinking,
        &traces);

    // chessboard board;

    std::string cmd;
    std::cout << "Hello There! This is Chess\r\n\r\n";
    std::cout << "NEW Game, LOAD FileName, or QUIT?\r\n";

    color_e my_col = c_white;
    int level = 4;
    std::string coll = "White";

    while (true)
    {
        std::cout << "> ";
        std::getline(std::cin, cmd);
        std::string cmdu = uppercase(cmd);
        std::string cmdl = cmdu.substr(0, 1);

        if (cmdl == "N")
        {
            if (get_new_game_options(my_col, coll, level))
            {
                game.new_game(my_col, level);
                break;
            }
        }
        else if (cmdl == "L")
        {
            if (load_game(cmd, game))
            {
                my_col = game.user_color();
                coll = color_str(my_col);
                break;
            }
        }
        else if (cmdl == "Q")
        {
            exit(EXIT_SUCCESS);
        }
    }

    game_state_e game_state = game.state();
    color_e turn_col = game.turn_color();
    color_e comp_col = other(turn_col);

    while (game_state == play_e)
    {
        if (turn_col != my_col)
        {
            if (computer_move(turn_col, game) != e_none)
            {
                print_error(e_failed_move);
                exit(EXIT_FAILURE);
            }
            turn_col = my_col;
        }
        // Is the game over?
        game_state = game.state();
        if (game_state != play_e)
            break;
        std::cout << coll << " Move > ";
        std::getline(std::cin, cmd);
        std::string cmdu = uppercase(cmd);
        std::string cmdl = cmdu.substr(0, 1);
        if ((cmdu == "?") || (cmdu == "HELP") || (cmdu == "H"))
        {
            std::cout << "Commands: NEW LOAD or L [FileName] SAVE or S [FileName] QUIT or Q MOVE or M [XX-XX]" << std::endl;
            std::cout << "Also < [Turn], REMOVE or R [XX], and TRACE or T [RecLvl]" << std::endl;
            continue;
        }
        if (cmdl == "N")
        {
            if (get_new_game_options(my_col, coll, level))
            {
                game.new_game(my_col, level);
                turn_col = c_white;
                comp_col = other(turn_col);
                continue;
            }
        }
        else if (cmdl == "L")
        {
            if (load_game(cmd, game))
            {
                my_col = game.user_color();
                coll = color_str(my_col);
                turn_col = game.turn_color();
                comp_col = other(turn_col);
                continue;
            }
        }
        else if (cmdl == "S")
        {
            save_game(cmd, game);
            continue;
        }
        else if (cmdl == "Q")
        {
            exit(EXIT_SUCCESS);
        }
        else if (cmdl == "<")
        {
            cmdu = get_arg(cmdu);
            if (cmdu == "")
            {
                print_error(e_rewind_missing);
                continue;
            }
            int moveno = atoi(cmdu.c_str());
            if (game.rewind_game(moveno) != e_none)
            {
                print_error(e_rewind_failed);
                continue;
            }
            turn_col = game.turn_color();
        }
        else if (cmdl == "R")
        {
            cmdu = get_arg(cmdu);
            if (cmdu == "")
            {
                print_error(e_missing_coord);
                continue;
            }
            coord_s p0;
            if (coord_int(cmdu, p0) != e_none)
            {
                print_error(e_invalid_coord);
                continue;
            }
            game.remove_piece(p0);
        }
        else if (cmdl == "P")
        {
            std::vector<std::string> args = get_args(cmd);
            if (args.size() != 2)
            {
                print_error(e_missing_coord_piece);
                continue;
            }
            coord_s p0;
            if (coord_int(args[0], p0) != e_none)
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
            game.add_piece(p0, p1);
        }
        else if (cmdl == "T")
        {
            // Poke a move for the CPU's next move search
            cmdu = get_arg(cmd);
            coord_s p0;
            coord_s p1;
            if (!get_move(cmdu, p0, p1))
                continue;
            game.suggest_move(p0, p1);
        }
        if (cmdl == "X")
        {
            // XFEN string
            cmdu = get_arg(cmd);
            if (cmdu == "")
                std::cout << game.save_xfen() << std::endl;
            else if (game.load_xfen(cmdu) != e_none)
            {
                print_error(e_xfen_read);
                continue;
            }
        }
        else
        {
            if (cmdl == "M")
            {
                cmdu = get_arg(cmdu);
                if (cmdu == "")
                {
                    print_error(e_missing_move);
                    continue;
                }
            }
            coord_s p0, p1;
            if (!get_move(cmdu, p0, p1))
                continue;
            if (game.move(my_col, p0, p1) != e_none)
            {
                print_error(e_invalid_move);
                continue;
            }
            if ((game.check_state(my_col)) && (game.state() == play_e))
            {
                print_error(e_check);
                continue;
            }
            std::cout << std::endl;
            turn_col = comp_col;
        }
    }
    */
}
