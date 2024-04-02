#include "gtest/gtest.h"
#include "chesscommon.h"

int main(int argc, char **argv)
{
    chess::set_data_folder("..\\..\\..\\..\\ChessData\\");
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}