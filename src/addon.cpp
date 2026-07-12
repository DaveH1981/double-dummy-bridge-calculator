#include <napi.h>
#include <string>
#include <cstring>
#include "dds/dds_clean.h"

Napi::Value SolveBridgeBoard(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    // FIXED: Using info[0] array-like indexing for verification and extraction
    if (info.Length() < 1 || !info[0].IsString()) {
        Napi::TypeError::New(env, "String expected for PBN format").ThrowAsJavaScriptException();
        return env.Null();
    }

    // FIXED: Using info[0] array-like indexing to correctly fetch the value
    std::string pbnHandString = info[0].As<Napi::String>().Utf8Value();

    // 1. Initialize the correct single hand tracking block 
    dealPBN dlPBN;
    std::memset(&dlPBN, 0, sizeof(dealPBN));
    
    dlPBN.trump = 4; // 4 = No Trump
    dlPBN.first = 0; // 0 = North leads first

    // Clear out tracking for cards played mid-trick
    for (int i = 0; i < 3; i++) {
        dlPBN.currentTrickSuit[i] = 0;
        dlPBN.currentTrickRank[i] = 0;
    }

    // Safely copy the PBN string data into the strict 80-byte buffer allocation space
    std::strncpy(dlPBN.remainCards, pbnHandString.c_str(), sizeof(dlPBN.remainCards) - 1);

    // 2. Prepare the empty analytics payload capture bucket
    futureTricks solvedResults;
    std::memset(&solvedResults, 0, sizeof(futureTricks));

    // 3. Invoke the actual Bo Haglund library binding sequence
    int statusCode = SolveBoardPBN(dlPBN, -1, 1, 1, &solvedResults, 0);

    if (statusCode != 1) {
        std::string errorMsg = "DDS Solver failed with status error code: " + std::to_string(statusCode);
        Napi::Error::New(env, errorMsg).ThrowAsJavaScriptException();
        return env.Null();
    }

    // Index 0 contains the highest-scoring card path available
    int calculatedTricks = 13 - solvedResults.score[0];

    Napi::Object resultObj = Napi::Object::New(env);
    resultObj.Set("pbnEvaluated", pbnHandString);
    resultObj.Set("calculatedTricks", Napi::Number::New(env, calculatedTricks));
    
    return resultObj;
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    SetMaxThreads(0); 
    exports.Set(Napi::String::New(env, "solveBoard"), Napi::Function::New(env, SolveBridgeBoard));
    return exports;
}

NODE_API_MODULE(dds_bridge, Init)
