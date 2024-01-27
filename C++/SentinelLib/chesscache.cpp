#include "chesscache.h"
#include "chesscommon.h"

#include <map>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <sstream>

namespace chess
{

    int m_move_hits = 0;
    int m_move_misses = 0;
    std::map<color_e, std::map<uint32_t, move_s>> m_move_cache;
    bool m_cache_init = false;

    void clear_cache()
    {
        m_move_cache.clear();
        m_move_hits = 0;
        m_move_misses = 0;
        m_cache_init = false;
    }

    bool load_cache()
    {
        try
        {
            std::ifstream is;
            is.open("CACHE.BIN", std::ios::binary | std::ios::in);
            int num_white = 0;
            int num_black = 0;
            is.read((char *)&num_white, 4);
            is.read((char *)&num_black, 4);
            for (int i = 0; i < num_white; i++)
            {
                uint32_t hash = 0;
                move_s m;
                is.read((char *)&hash, 4);
                is.read((char *)&m, sizeof(move_s));
                m_move_cache[c_white][hash] = m;
            }
            for (int i = 0; i < num_black; i++)
            {
                uint32_t hash = 0;
                move_s m;
                is.read((char *)&hash, 4);
                is.read((char *)&m, sizeof(move_s));
                m_move_cache[c_black][hash] = m;
            }
            is.close();
            return true;
        }
        catch (const std::exception &)
        {
            return false;
        }
    }

    bool save_cache()
    {
        try
        {
            std::ofstream os;
            os.open("CACHE.BIN", std::ios::binary | std::ios::out);
            int num_white = (int)m_move_cache[c_white].size();
            int num_black = (int)m_move_cache[c_black].size();
            os.write((char *)&num_white, 4);
            os.write((char *)&num_black, 4);
            for (std::map<uint32_t, move_s>::iterator it = m_move_cache[c_white].begin(); it != m_move_cache[c_white].end(); ++it)
            {
                os.write((char *)&it->first, 4);
                os.write((char *)&it->second, sizeof(move_s));
            }
            for (std::map<uint32_t, move_s>::iterator it = m_move_cache[c_black].begin(); it != m_move_cache[c_black].end(); ++it)
            {
                os.write((char *)&it->first, 4);
                os.write((char *)&it->second, sizeof(move_s));
            }
            os.close();
            return true;
        }
        catch (const std::exception &)
        {
            return false;
        }
    }

    void init_cache()
    {
        if (m_move_cache.size() == 0)
        {
            std::map<uint32_t, move_s> empty;
            m_move_cache[c_white] = empty;
            m_move_cache[c_black] = empty;
        }

        load_cache();

        m_cache_init = true;
    }

    move_s cached_move(color_e col, uint32_t hash)
    {
        if (!m_cache_init)
            init_cache();
        move_s best;
        std::map<uint32_t, move_s>::iterator it = m_move_cache[col].find(hash);
        if (it != m_move_cache[col].end())
        {
            m_move_hits++;
            return it->second;
        }
        m_move_misses++;
        return best;
    }

    void cache_move(color_e col, uint32_t hash, move_s &m)
    {
        if (!m_cache_init)
            init_cache();
        if (m.cache == false)
        {
            m.cache = true;
            m_move_cache[col][hash] = m;
        }
    }

    std::string cache_stats()
    {
        std::stringstream s;
        if (m_move_cache.size() == 0)
            return "No Cache";
        s << "Size: " << m_move_cache[c_white].size() + m_move_cache[c_black].size();
        s << " Hits: " << m_move_hits << " Misses: " << m_move_misses;
        double pct = 100.0;
        if (m_move_misses > 0)
            pct = (double)m_move_hits * 100.0 / (double)m_move_misses;
        s << " " << pct << "%";
        return s.str();
    }

}