#pragma once

namespace chess
{
    const size_t cx_pgn_count = 50;
    const char *cx_pgn_final_xfen[] = {
        "r2kQb1r/pbpp3p/1pn1p3/7B/3PP2q/P1N5/1PP2PPP/R3K2R b KQ - 0 26",          // 00000001.pgn
        "7B/pppk1p2/3bp1p1/8/3nN3/3B4/PPPK1PP1/R7 b - - 0 36",                    // 00000002.pgn
        "r1b1k2r/ppN2ppp/4q3/4p3/2BnP1n1/P2P1N1P/1PP3P1/R1BQ1K1R b kq - 0 22",    // 00000003.pgn
        "8/p5p1/6P1/6bP/K1pk4/8/8/8 w - - 0 95",                                  // 00000004.pgn
        "r4b1r/pp3kp1/1q6/2ppP2p/5P2/2PP3b/PP1N4/R1B2K2 w - - 0 47",              // 00000005.pgn
        "5rk1/p1p2p2/Q2p1n2/2b1p1N1/4P1qp/2PP3b/P4PPB/R4RK1 w - - 0 39",          // 00000006.pgn
        "3r2nQ/1p2kp2/pNp4q/2P3pB/1P6/P2P3P/5PP1/4R1K1 b - - 0 56",               // 00000007.pgn
        "8/5pk1/6b1/8/8/Q4P2/1pq3PK/8 w - - 0 91",                                // 00000008.pgn
        "8/p6k/5R2/6p1/1PpPb1Pp/2P2P1P/P1r3K1/4r3 w - - 0 75",                    // 00000009.pgn
        "8/4K3/r3p3/P1kp3P/2p3p1/R7/7r/8 b - - 0 94",                             // 00000010.pgn
        "4Q1k1/bp4p1/p3p2p/3q1r2/1PNp4/P2P4/2P2PPP/R3R1K1 b - - 0 50",            // 00000011.pgn
        "3b2Q1/8/6Qk/p6p/1p1B4/1B1P4/PP5P/5K2 b - - 0 70",                        // 00000012.pgn
        "1K6/8/8/2k5/8/8/8/8 b - - 0 118",                                        // 00000013.pgn
        "rnb1k2r/ppp2ppp/8/3p4/4nN1q/1P3PP1/PBP1Pb1P/RN1QKB1R w KQkq - 0 17",     // 00000014.pgn
        "4Q3/6k1/1p4p1/5p1p/pP2p2P/P1r5/R4K2/8 w - - 0 75",                       // 00000015.pgn
        "b7/P3BP2/1r6/5R2/8/2K5/4k3/8 b - - 0 128",                               // 00000016.pgn
        "3rr3/1k2bppp/2p5/Pp1p4/3P1B2/4RK2/5PPP/4R3 b - - 0 60",                  // 00000017.pgn
        "3R4/2q2kr1/p4Q2/4P3/8/8/P5PP/5K2 b - - 0 72",                            // 00000018.pgn
        "r2q1rk1/pppb2pQ/3pP2p/8/8/3B4/PPP3PP/2KRb3 b - - 0 38",                  // 00000019.pgn
        "2r1kb1r/3np1p1/pp3p1p/8/1npP2b1/5NB1/PPP1BPPP/2NR1RK1 b k - 0 34",       // 00000020.pgn
        "7k/5Q2/4P2p/p7/Pp6/1P5P/6P1/2n4K b - - 0 94",                            // 00000021.pgn
        "2n5/8/1k6/8/pBPP1PP1/1p3K2/1P6/8 b - - 0 86",                            // 00000022.pgn
        "q2rn2k/p2p1p1P/1p1b4/2p5/2P5/3B4/PPn4P/R1B2bK1 w - - 0 43",              // 00000023.pgn
        "8/2k5/1r4p1/1PK1Rp1p/5P2/6P1/7P/8 b - - 0 98",                           // 00000024.pgn
        "2r2k1r/pp1N1p2/1q5p/3bP3/1P1P2pB/P2Q4/3N3P/R4RK1 b - - 0 56",            // 00000025.pgn
        "8/8/p7/4kppp/3b1P2/6PP/3B2K1/8 b - - 0 82",                              // 00000026.pgn
        "2R5/5k2/6p1/p5P1/P5K1/8/2Bb4/8 w - - 0 99",                              // 00000027.pgn
        "8/8/8/8/7k/1P4R1/P2K3Q/8 b - - 0 92",                                    // 00000028.pgn
        "1r2q1k1/p2b2r1/1pp1p2p/2PB1p2/3P4/7P/PP3PP1/RN1Q1RK1 w - - 0 49",        // 00000029.pgn
        "8/5p2/8/8/7p/6k1/P1K5/8 w - - 0 87",                                     // 00000030.pgn
        "2kr3r/p1Q2Rpp/8/8/1P2b3/7P/PP2N1q1/1RB1K3 b - - 0 50",                   // 00000031.pgn
        "r1b1kb1r/ppp1p1pp/2n2n2/3q4/Q7/N1Pp1N2/PP1P1PPP/R1B1KB1R w KQkq - 0 15", // 00000032.pgn
        "3R4/p2R3p/1pk3p1/1Bp2r2/P3pp2/2P2P1P/5P2/5K2 b - - 0 78",                // 00000033.pgn
        "7r/2q2pk1/r1Pb1pp1/4p3/1P6/2P1B3/P4PPP/2KR3R w - - 0 49",                // 00000034.pgn
        "r2qk1nr/ppp2ppp/1bnp4/8/2BPP1b1/B1N2N2/P4PPP/R2Q1RK1 b kq - 0 20",       // 00000035.pgn
        "3Q2k1/2Q5/8/8/1K6/8/8/8 b - - 0 172",                                    // 00000036.pgn
        "8/3k4/3p4/1p6/pp5b/8/6r1/1K6 w - - 0 107",                               // 00000037.pgn
        "2r2rk1/p5p1/4p2p/1p6/3QP3/qPp5/P1P3P1/1K2R2R w - - 0 55",                // 00000038.pgn
        "8/p3k3/4p1p1/2p1K1P1/2P2P2/3p4/P3r3/8 w - - 0 83",                       // 00000039.pgn
        "2k3nr/pp3p1p/2p1p3/6p1/1Q2P3/2N2P2/PP1r1qPP/5K1R w - - 0 55",            // 00000040.pgn
        "4Q3/2qk3p/1p1p3b/1bpP2p1/3p1p2/5B1P/3B1PP1/4R1K1 b - - 0 64",            // 00000041.pgn
        "1r3k2/5Q2/2pq3N/8/1n1p4/4P3/PP5P/2Kb1R2 b - - 0 52",                     // 00000042.pgn
        "1Q6/4ppkp/3p2p1/3P4/1P2P3/P1bn3P/2qBKPP1/8 w - - 0 63",                  // 00000043.pgn
        "8/5k2/1Q3pp1/pp1p4/3P4/P1Pb4/6R1/4q1K1 w - - 0 109",                     // 00000044.pgn
        "r4rk1/3b1pp1/p6p/1p2Q3/7q/1P6/PP3PPP/5RK1 w - - 0 43",                   // 00000045.pgn
        "1r2k2r/5pp1/p2p3p/2pPp3/P7/3bb1P1/3qB1PP/4RK1R w k - 0 49",              // 00000046.pgn
        "r2q2k1/pp3rP1/3p4/2pP1b1Q/2B5/2N3P1/PPP2pP1/R4K2 b - - 0 44",            // 00000047.pgn
        "5rk1/pp3ppp/2n5/8/8/P3P3/1P3P1P/2Kq4 w - - 0 43",                        // 00000048.pgn
        "2kr1bnr/ppp1p1p1/n5pp/3NP3/3P2B1/8/PP3PPP/R1BQK2R b KQ - 0 24",          // 00000049.pgn
        "r4rk1/3Q1pbp/P3p1p1/3pP3/2nP4/4PN2/1P2N1PP/R4RK1 b - - 0 44",            // 00000050.pgn
    };

    std::string pgn_final_xfen(int fileno)
    {
        if ((fileno >= 1) && (fileno <= cx_pgn_count))
            return cx_pgn_final_xfen[fileno - 1];
        return "";
    }

}