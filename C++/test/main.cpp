#include "gtest/gtest.h"
#include "chesscommon.h"
#include "chessengine.h"

using namespace chess;

int main(int argc, char **argv)
{
    // Default = EXE folder for chess data
    chessengine::initialize(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}