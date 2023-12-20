#include <stdio.h>
#include <iostream>
#include <string>
#include <windows.h>
#include <algorithm>

#include "chessboard.h"
#include "console.h"

using namespace chess;

std::string uppercase(std::string l)
{
    std::string cmdu = l;
    transform(cmdu.begin(), cmdu.end(), cmdu.begin(), ::toupper);
    return cmdu;
}

bool load_game(std::string cmd, chessboard &board)
{
    size_t pos = cmd.find(' ');
    if (pos == std::string::npos)
    {
        std::cout << "ERROR - Must include filename" << std::endl;
        return false;
    }
    if (!board.load_game(cmd.substr(pos + 1)))
    {
        std::cout << "ERROR Loading" << std::endl;
        return false;
    }
    return true;
}

bool save_game(std::string cmd, chessboard &board)
{
    size_t pos = cmd.find(' ');
    if (pos == std::string::npos)
    {
        std::cout << "ERROR - Must include filename" << std::endl;
        return false;
    }
    if (!board.save_game(cmd.substr(pos + 1)))
    {
        std::cout << "ERROR Saving" << std::endl;
        return false;
    }
    return true;
}

bool is_game_over(color_e col, move_s &m)
{
    color_e winner_col = col == c_white ? c_black : c_white;
    if ((m.check == false) && (m.mate == false))
    {
        std::cout << "\t" + color_str(winner_col) << " wins, Stalemate!" << std::endl;
        return true;
    }
    else if (m.check && m.mate)
    {
        std::cout << "\t" + color_str(winner_col) << " wins, Stalemate!" << std::endl;
        return true;
    }
    return false;
}

int main(void)
{

    chessboard board;

    std::string cmd;
    std::cout << "Hello There! This is Chess\r\n\r\n";
    std::cout << "NEW Game, LOAD FileName, or QUIT?\r\n";

    while (true)
    {
        std::cout << "> ";
        std::getline(std::cin, cmd);
        std::string cmdu = uppercase(cmd);
        std::string cmdl = cmdu.substr(0, 1);
        if (cmdl == "N")
        {
            board.new_game();
            break;
        }
        else if (cmdl == "L")
        {
            if (load_game(cmd, board))
                break;
        }
        else if (cmdl == "Q")
        {
            exit(EXIT_SUCCESS);
        }
    }

    color_e my_col = c_white;

    board_to_console(board);
    std::cout << "\r\n\tBlack or White (default)? ";
    std::string mycolor;
    std::getline(std::cin, mycolor);
    std::string colu = uppercase(mycolor);
    std::string coll = colu.substr(0, 1);

    if (coll == "B")
        my_col = c_black;
    coll = color_str(my_col);

    color_e turn_col = board.turn_color();
    color_e comp_col = c_white;
    if (my_col == c_white)
        comp_col = c_black;
    bool game_over = false;

    while (!game_over)
    {
        if (turn_col != my_col)
        {
            std::cout << "\tComputer Turn" << std::endl;
            move_s m = board.computer_move(turn_col, 4);
            game_over = is_game_over(turn_col, m);
            if (game_over)
                break;
            board_to_console(board);
            move_to_console(m, color_str(turn_col));
            std::cout << std::endl;
            turn_col = my_col;
        }
        std::cout << "\t" << coll << " Move > ";
        std::getline(std::cin, cmd);
        std::string cmdu = uppercase(cmd);
        std::string cmdl = cmdu.substr(0, 1);
        if ((cmdu == "?") || (cmdu == "HELP") || (cmdu == "H"))
        {
            std::cout << "Commands: NEW LOAD or L filename SAVE or S filename QUIT or Q MOVE or M XX-XX" << std::endl;
            continue;
        }
        if (cmdl == "N")
        {
            board.new_game();
            board_to_console(board);
            turn_col = board.turn_color();
            continue;
        }
        else if (cmdl == "L")
        {
            if (load_game(cmd, board))
            {
                board_to_console(board);
                turn_col = board.turn_color();
                continue;
            }
        }
        else if (cmdl == "S")
        {
            save_game(cmd, board);
            continue;
        }
        else if (cmdl == "Q")
        {
            exit(EXIT_SUCCESS);
        }
        else
        {
            if (cmdl == "M")
            {
                size_t pos = cmdu.find(' ');
                if (pos == std::string::npos)
                {
                    std::cout << "ERROR - Invalid move use M or MOVE XX-XX" << std::endl;
                    continue;
                }
                cmdu = cmdu.substr(pos + 1);
            }
            size_t pos = cmdu.find('-');
            if (pos == std::string::npos)
            {
                std::cout << "ERROR - Move must be in the form XX-XX" << std::endl;
                continue;
            }
            short y0 = 0;
            short x0 = 0;
            short y1 = 0;
            short x1 = 0;
            if ((!coord_int(cmdu.substr(0, pos), y0, x0)) ||
                (!coord_int(cmdu.substr(pos + 1), y1, x1)))
            {
                std::cout << "ERROR - Move must be in the form XX-XX for example A1-A2" << std::endl;
                continue;
            }
            move_s m = board.user_move(my_col, y0, x0, y1, x1);
            if (!m.is_valid())
            {
                std::cout << "ERROR - Invalid Move" << std::endl;
                continue;
            }
            if ((m.check) || (!m.mate))
            {
                std::cout << "ERROR - You are in Check, try again" << std::endl;
                continue;
            }
            game_over = is_game_over(my_col, m);
            if (game_over)
                break;
            board_to_console(board);
            move_to_console(m, color_str(turn_col));
            std::cout << std::endl;
            turn_col = comp_col;
        }
    }
}