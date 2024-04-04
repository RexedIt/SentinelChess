#include "gtest/gtest.h"
#include "chesscommon.h"
#include "chessopenings.h"

using namespace chess;

int main(int argc, char **argv)
{
    set_data_folder("..\\..\\..\\..\\ChessData\\");
    chessecodb co;
    co.load_binary(data_file("scid.bin"));
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}