#include "chessmeta.h"

namespace chess
{

    chessmeta::chessmeta()
    {
        reset_tags();
    }

    void chessmeta::reset_tags()
    {
        m_tags.clear();
    }

    std::string chessmeta::operator[](const char *key)
    {
        return tag(key);
    }

    bool chessmeta::remove_tag(std::string key)
    {
        for (std::vector<std::pair<std::string, std::string>>::iterator it = m_tags.begin(); it != m_tags.end(); ++it)
        {
            std::pair<std::string, std::string> pair = *it;
            if (pair.first == key)
            {
                m_tags.erase(it);
                return true;
            }
        }
        return false;
    }

    bool chessmeta::write_tag(std::string key, std::string value)
    {
        if (key == "")
            return false;
        if (value == "")
            return remove_tag(key);
        for (size_t i = 0; i < m_tags.size(); i++)
        {
            std::pair<std::string, std::string> pair = m_tags[i];
            if (pair.first == key)
            {
                pair.second = value;
                m_tags[i] = pair;
                return true;
            }
        }
        m_tags.push_back(std::pair<std::string, std::string>(key, value));
        return true;
    }

    bool chessmeta::write_tag(std::string key, int value)
    {
        return write_tag(key, std::to_string(value));
    }

    bool chessmeta::copy_tags_from(chessmeta &m)
    {
        std::vector<std::pair<std::string, std::string>> oth = m.m_tags;
        for (size_t i = 0; i < oth.size(); i++)
        {
            std::pair<std::string, std::string> pair = oth[i];
            write_tag(pair.first, pair.second);
        }
        return true;
    }

    std::string chessmeta::tag(std::string key)
    {
        for (size_t i = 0; i < m_tags.size(); i++)
        {
            std::pair<std::string, std::string> pair = m_tags[i];
            if (pair.first == key)
                return pair.second;
        }
        return "";
    }
}