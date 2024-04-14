#include "gtest/gtest.h"
#include "chesscommon.h"
#include "chessengine.h"

using namespace chess;

std::string test_datapath = "test/TestData";

int main(int argc, char **argv)
{
    // Default = EXE folder for chess data
    std::string datapath = ".";

    for (int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];
        bool has_next = i + 1 < argc;
        if (arg == "--chessdata")
        {
            if (has_next)
                datapath = fix_path(argv[(i++) + 1]);
        }
        else if (arg == "--testdata")
        {
            if (has_next)
                test_datapath = fix_path(argv[(i++) + 1]);
        }
    }

    chessengine::initialize(datapath);

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}