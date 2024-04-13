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

    void chessmeta::clear_comments()
    {
        m_comments.clear();
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

    bool chessmeta::copy_from(chessmeta &m)
    {
        std::vector<std::pair<std::string, std::string>> oth = m.m_tags;
        for (size_t i = 0; i < oth.size(); i++)
        {
            std::pair<std::string, std::string> pair = oth[i];
            write_tag(pair.first, pair.second);
        }
        m_comments = m.m_comments;
        return true;
    }

    std::map<int, std::string> chessmeta::comments()
    {
        return m_comments;
    }

    std::string chessmeta::comment(int index)
    {
        if (m_comments.count(index))
            return m_comments[index];
        return "";
    }

    void chessmeta::comment(int index, std::string val)
    {
        if (book_end(val, "${(", ".})"))
            m_comments[index] = val;
        else
            m_comments[index] = "{" + val + "}";
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