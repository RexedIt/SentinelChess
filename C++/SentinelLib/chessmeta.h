#pragma once

#include <map>
#include "chesscommon.h"

namespace chess
{
    class chessmeta
    {
    public:
        chessmeta();
        std::string operator[](const char *);
        bool remove_tag(std::string key);
        bool write_tag(std::string key, std::string value);
        bool write_tag(std::string key, int value);
        bool copy_tags_from(chessmeta &m);

    protected:
        virtual void reset_tags();
        std::string tag(std::string key);
        std::vector<std::pair<std::string, std::string>> m_tags;
    };
}