#include "gtest/gtest.h"
#include "chesscommon.h"
#include "chessengine.h"

using namespace chess;

int main(int argc, char **argv)
{
    chessengine::initialize("..\\..\\..\\..\\ChessData\\");

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}