#pragma once

namespace chess
{
    const size_t cx_eco_count = 6;

    const int cx_eco_typical_1 = 0;
    const int cx_eco_typical_2 = 1;
    const int cx_eco_seqerr_1 = 2;
    const int cx_eco_seqerr_2 = 3;
    const int cx_eco_seqerr_3 = 4;
    const int cx_eco_malformed = 5;

    const char *cx_eco[] = {
        "A00b \"Barnes Opening\"  1.f3 *",
        "C63r \"Spanish: Schliemann, 4.Nc3 fxe4 5.Nxe4 d5\" 1.e4 e5 2.Nf3 Nc6 3.Bb5 f5 4.Nc3 fxe4 5.Nxe4 d5 *",
        "A05 \"Reti: KIA, Reti-Smyslov Variation\"  1.Nf3 Nf6 2.g3 g6 3.b4 Bg7 5.Bb2 *",
        "C40m \"Latvian Gambit: 3.d4 fxe4 5.Nxe5 Nf6\" 1.e4 e5 2.Nf3 f5 3.d4 fxe4 5.Nxe5 Nf6 *",
        "D20f \"QGA: 3.e3 e5\" 1.d4 d5 2.c4 dxc4 3.e3 e5 4.Bxc4 exd4 5.exd4 Bb4+ 5.Nc3 Nf6 *",
        "E09a \"\"Catalan: Closed, Main Line\" 1.de4 Nf6 2.c4 e6 3.g3 d5 4.Kg2 Be7 5.Nf32 O-O 6.O-O Nbd7 7.Qc2 c6 8.Nbd2 *"};
}