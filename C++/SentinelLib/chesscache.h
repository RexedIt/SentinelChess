#pragma once

#include <string>
#include "chesscommon.h"

namespace chess
{
    bool load_cache();
    bool save_cache();
    void clear_cache();
    move_s cached_move(color_e col, uint32_t hash);
    void cache_move(color_e col, uint32_t hash, move_s &m);
    std::string cache_stats();
}