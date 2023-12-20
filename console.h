#pragma once

#include <stdio.h>
#include <iostream>
#include <string>
#include <windows.h>

namespace chess
{
    class chessboard;
    struct move_s;
}

std::string colorchar(int bkgnd, int fgnd, std::string s);

typedef enum ansicolor
{
    ansi_black = 30,
    ansi_red = 31,
    ansi_green = 32,
    ansi_yellow = 33,
    ansi_blue = 34,
    ansi_magenta = 35,
    ansi_cyan = 36,
    ansi_light_gray = 37,
    ansi_dark_gray = 90,
    ansi_light_red = 91,
    ansi_light_green = 92,
    ansi_light_yellow = 93,
    ansi_light_blue = 94,
    ansi_light_magenta = 95,
    ansi_light_cyan = 97,
    ansi_white = 97
} ansicolor;

void board_to_console(chess::chessboard &b);
void move_to_console(chess::move_s &m, std::string s);
