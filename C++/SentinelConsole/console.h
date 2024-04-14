#pragma once

#include <stdio.h>
#include <string>
#include <windows.h>
#include <chrono>

#include "chesscommon.h"

namespace chess
{
    class chessboard;
    class chessmove;
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
    ansi_light_cyan = 96,
    ansi_white = 97
} ansicolor;

class stopwatch
{
public:
    stopwatch();
    long elapsed_ms();
    long elapsed_sec();
    std::string elapsed_str();

private:
    std::chrono::steady_clock::time_point start;
};

void board_to_console(int n, chess::chessboard &b, chess::color_e bottom);
void move_to_console(chess::chessmove &m, std::string s);
void move_to_console(chess::chessmove &m, chess::chessboard b, std::string s);
void time_to_console(int32_t wt, int32_t bt);