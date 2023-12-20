#pragma once

#include <stdio.h>
#include <iostream>
#include <string>
#include <windows.h>
#include "console.h"
#include "chessboard.h"
#include "chesspiece.h"

using namespace chess;

bool _initconsole = false;

void initconsole()
{
#if defined WIN32 || defined _WIN32 || defined WIN64 || defined _WIN64
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
#endif
    _initconsole = true;
}

std::string colorchar(int bkgnd, int fgnd, std::string s)
{
    if (!_initconsole)
        initconsole();
    std::string res = "\x1b[" + std::to_string(bkgnd + 10) + "m";
    res += "\x1b[" + std::to_string(fgnd) + "m";
    return res + s + "\x1b[0m";
}

void board_to_console(chess::chessboard &b)
{
    std::cout << "\r\n\t-----------Board-----------\r\n"
              << std::endl;
    std::cout << colorchar(ansi_black, ansi_light_blue, "\t    A  B  C  D  E  F  G  H\r\n") << std::endl;
    for (int i = 7; i >= 0; i--)
    {
        std::cout << colorchar(ansi_black, ansi_light_blue, "\t" + std::to_string(i + 1) + "  ");
        for (int j = 0; j < 8; j++)
        {
            int cell = i * 8 + j + i % 2;
            int bkgnd = (cell % 2 == 0) ? ansi_dark_gray : ansi_light_gray;
            chess::chesspiece piece(b.get(i, j));
            int fgnd = piece.color == c_black ? ansi_black : ansi_white;
            std::cout << colorchar(bkgnd, fgnd, " " + piece.abbr + " ");
        }
        std::cout << std::endl;
    }
    std::string s_check = b.check_state();
    if (s_check != "")
        std::cout << "\r\n\t    " << s_check << std::endl;
    std::cout << "\r\n\t    White: " << b.weight(c_white) << " Black: " << b.weight(c_black) << std::endl;
    std::cout << "\r\n\t---------------------------\r\n"
              << std::endl;
}

void move_to_console(chess::move_s &m, std::string s)
{
    std::cout << "\t" << s << " move " << m.to_string() << std::endl;
}
