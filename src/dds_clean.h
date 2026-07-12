#ifndef DDS_CLEAN_H
#define DDS_CLEAN_H

extern "C" {
    struct dealPBN {
        int trump;
        int first;
        int currentTrickSuit[3]; // Track cards already on table for mid-play solves
        int currentTrickRank[3]; // Track cards already on table for mid-play solves
        char remainCards[80];    // Strict 80 character buffer limit
    };

    struct futureTricks {
        int nodes;
        int cards;
        int suit[13];            // Card suit results array
        int rank[13];            // Card rank results array
        int equals[13];
        int score[13];           // Max trick capacity outcomes per card played
    };

    // The official, true signature exported by the precompiled 2.x/3.x dds.dll binaries
    __declspec(dllimport) void __stdcall SetMaxThreads(int userThreads);
    __declspec(dllimport) int __stdcall SolveBoardPBN(
        struct dealPBN dlPBN, 
        int target, 
        int solutions, 
        int mode, 
        struct futureTricks* futp, 
        int threadIndex
    );
}

#endif
