#pragma once

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <windows.h>
#include "console.h"
#include "chessboard.h"
#include "chesspiece.h"

using namespace chess;

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x4
#endif

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

void board_to_console(int n, chess::chessboard &b, bool r)
{
    std::cout << "\r\n\r\n--- #" << n << " --------------------\r\n"
              << std::endl;

    if ((b.turn_color() == c_white) || (r == false))
    {
        std::cout << colorchar(ansi_black, ansi_light_blue, "    A  B  C  D  E  F  G  H\r\n") << std::endl;
        for (int i = 7; i >= 0; i--)
        {
            std::cout << colorchar(ansi_black, ansi_light_blue, std::to_string(i + 1) + "  ");
            for (int j = 0; j <= 7; j++)
            {
                int cell = i * 8 + j + i % 2;
                int bkgnd = (cell % 2 == 0) ? ansi_dark_gray : ansi_light_gray;
                chess::chesspiece piece(b.get(coord_s(i, j)));
                int fgnd = piece.color == c_black ? ansi_black : ansi_white;
                std::string piecestr(1, piece.abbr);
                std::cout << colorchar(bkgnd, fgnd, " " + piecestr + " ");
            }
            std::cout << std::endl;
        }
    }
    else
    {
        std::cout << colorchar(ansi_black, ansi_light_blue, "    H  G  F  E  D  C  B  A\r\n") << std::endl;
        for (int i = 0; i <= 7; i++)
        {
            std::cout << colorchar(ansi_black, ansi_light_blue, std::to_string(i + 1) + "  ");
            for (int j = 7; j >= 0; j--)
            {
                int cell = i * 8 + j + i % 2;
                int bkgnd = (cell % 2 == 0) ? ansi_dark_gray : ansi_light_gray;
                chess::chesspiece piece(b.get(coord_s(i, j)));
                int fgnd = piece.color == c_black ? ansi_black : ansi_white;
                std::string piecestr(1, piece.abbr);
                std::cout << colorchar(bkgnd, fgnd, " " + piecestr + " ");
            }
            std::cout << std::endl;
        }
    }
    std::string s_check = b.check_state();
    if (s_check != "")
        std::cout << "\r\n    " << s_check << std::endl;
    std::cout << "\r\n---------------------------" << std::endl;
}

void move_to_console(chess::move_s &m, std::string s)
{
    std::cout << s << " move " << m.to_string() << std::endl;
}

void time_to_console(int32_t wt, int32_t bt)
{
    if ((wt <= 0) && (bt <= 0))
        return;
    std::cout << "White:" << time_str(wt) << " Black: " << time_str(wt) << std::endl;
}

stopwatch::stopwatch()
{
    start = std::chrono::steady_clock::now();
}

long stopwatch::elapsed_ms()
{
    std::chrono::steady_clock::time_point finish = std::chrono::steady_clock::now();
    std::chrono::microseconds ms = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
    return (long)ms.count();
}

long stopwatch::elapsed_sec()
{
    std::chrono::steady_clock::time_point finish = std::chrono::steady_clock::now();
    std::chrono::seconds secs = std::chrono::duration_cast<std::chrono::seconds>(finish - start);
    return (long)secs.count();
}

std::string stopwatch::elapsed_str()
{
    long ms = elapsed_ms();
    std::stringstream s;
    long sec = ms / 1000000;
    long min = 0;
    long hr = 0;
    long mic = (ms / 1000) % 1000;

    if (sec > 60)
    {
        min = sec / 60;
        sec = sec % 60;
    }
    if (min > 0)
    {
        hr = min / 60;
        min = min % 60;
    }

    s << hr << ":";
    s << std::setfill('0') << std::setw(2) << min << ":";
    s << std::setfill('0') << std::setw(2) << sec << ":";
    s << std::setfill('0') << std::setw(3) << mic;
    return s.str();
}
