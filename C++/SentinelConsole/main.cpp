#include <stdio.h>
#include <iostream>
#include <string>
#include <windows.h>

#include "chessgame.h"
#include "chesspiece.h"
#include "chesscache.h"
#include "console.h"

using namespace chess;

bool print_error(error_e num)
{
    std::cout << "ERROR: " << errorstr(num) << std::endl;
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

bool load_game(std::string cmd, chessgame &game)
{
    std::string filename = get_arg(cmd);
    if (filename == "")
        return print_error(e_missing_filename);
    if (game.load_game(filename) != e_none)
        return print_error(e_loading);
    return true;
}

bool save_game(std::string cmd, chessgame &game)
{
    std::string filename = get_arg(cmd);
    if (filename == "")
        return print_error(e_missing_filename);
    if (game.save_game(filename) != e_none)
        return print_error(e_saving);
    return true;
}

bool get_new_game_options(color_e &my_col, std::string &coll, int &level)
{
    std::cout << "\r\nBlack or White (default)? ";
    std::string mycolor;
    std::getline(std::cin, mycolor);
    if (mycolor == "")
        return false;
    std::string colu = uppercase(mycolor);
    coll = colu.substr(0, 1);
    if (coll == "B")
        my_col = c_black;
    coll = color_str(my_col);
    std::cout << "Level (1-5)? ";
    std::string mylevel;
    std::getline(std::cin, mylevel);
    if (mylevel == "")
        return false;
    level = atoi(mylevel.c_str());
    if ((level < 1) || (level > 5))
        return false;
    return true;
}

// *** Callbacks ***
void draw_board(int n, chessboard &b)
{
    board_to_console(n, b);
}

void draw_move(int n, move_s &m, color_e c)
{
    move_to_console(m, color_str(c));
}

void game_over(game_state_e state, color_e win_color)
{
    std::cout << "\r\n**** Game Over! ";
    if (win_color == c_none)
        std::cout << "Stalemate! ";
    else
        std::cout << color_str(win_color) + " Wins! ";
    std::cout << "****" << std::endl;
}

piece_e request_promote_piece()
{
    piece_e promote = p_queen;
    std::cout << "Q = Queen, (default) R = Rook, B = Bishop, K = Knight? ";
    std::string pcu;
    std::getline(std::cin, pcu);
    pcu = uppercase(pcu).substr(0, 1);
    if (pcu == "R")
        promote = p_rook;
    if (pcu == "B")
        promote = p_bishop;
    if (pcu == "K")
        promote = p_knight;
    return promote;
}

void thinking(move_s m, int pct)
{
    std::cout << ".";
}

void traces(std::string msg)
{
    std::cout << msg;
}

// *** End Callbacks ***

int main(void)
{

    chessgame game;

    game.set_callbacks(
        &draw_board,
        &game_over,
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
            std::cout << "Computer Turn" << std::endl;
            stopwatch s;
            if (game.computer_move(turn_col) != e_none)
            {
                print_error(e_failed_move);
                exit(EXIT_FAILURE);
            }
            std::cout << s.elapsed_str() << " " << cache_stats() << std::endl;
            std::cout << std::endl;
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
            std::cout << "Also SAVE CACHE, < [Turn], REMOVE or R [XX], and TRACE or T [RecLvl]" << std::endl;
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
        else if (cmdu == "SAVE CACHE")
        {
            std::cout << "Saving Cache..." << std::endl;
            save_cache();
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
            if (game.user_move(my_col, p0, p1) != e_none)
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
}