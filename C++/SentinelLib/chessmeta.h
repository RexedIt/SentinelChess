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
        bool copy_from(chessmeta &m);
        std::map<int, std::string> comments();
        std::string comment(int index);
        void comment(int index, std::string val);

    protected:
        virtual void reset_tags();
        std::string tag(std::string key);
        std::vector<std::pair<std::string, std::string>> m_tags;
        std::map<int, std::string> m_comments;
    };
}