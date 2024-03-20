#pragma once

#include "chesscommon.h"
#include "chessmove.h"

#include <vector>
#include <map>
#include <memory>

namespace chess
{
    class chessopening
    {
    public:
        chessopening();
        chessopening(const chessopening &);
        chessopening(std::string eco, std::string title, std::vector<chessmove> moves);
        void copy(const chessopening &);
        void operator=(const chessopening &);

        std::string eco;
        std::string title;
        std::vector<chessmove> moves;
        uint32_t hash();

    private:
        uint32_t m_hash;
    };

    class chessopenings
    {
    public:
        chessopenings();
        ~chessopenings();

        error_e load_scid_eco(std::string filename);

    private:
        error_e load_scid_line(std::string line, std::string &errextra);

        std::map<uint32_t, std::shared_ptr<chessopening>> m_op_hash;
        std::map<std::string, std::shared_ptr<chessopening>> m_op_name;
    };
}