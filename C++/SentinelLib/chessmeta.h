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
        bool write_tag(std::string key, std::string value);
        bool write_tag(std::string key, int value);
        std::map<int, std::string> comments();
        std::string comment(int index);
        void comment(int index, std::string val);
        bool copy_from(chessmeta &m);

    protected:
        virtual void reset_tags();
        void clear_comments();
        bool remove_tag(std::string key);
        bool has_tag(std::string key);
        std::string tag(std::string key);
        std::vector<std::pair<std::string, std::string>> m_tags;
        std::map<int, std::string> m_comments;
    };
}