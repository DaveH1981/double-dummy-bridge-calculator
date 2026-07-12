#include <napi.h>
#include <string>
#include <cstring>
#include "dds/dds_clean.h"

int getDdsRank(char rankChar) {
    if (rankChar == 'A') return 14;
    if (rankChar == 'K') return 13;
    if (rankChar == 'Q') return 12;
    if (rankChar == 'J') return 11;
    if (rankChar == 'T') return 10;
    if (rankChar >= '2' && rankChar <= '9') return rankChar - '0';
    return 0;
}

Napi::Value SolveBridgeBoard(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    // FIXED: Using info[0] to check if the first argument passed from JS is an Object
    if (info.Length() < 1 || !info[0].IsObject()) {
        Napi::TypeError::New(env, "Object expected for Game Configuration constraints").ThrowAsJavaScriptException();
        return env.Null();
    }

    // FIXED: Using info[0] to extract the argument as a Napi Object instance
    Napi::Object jsObject = info[0].As<Napi::Object>();

    std::string pbnHandString = jsObject.Get("pbn").As<Napi::String>().Utf8Value();
    int chosenTrump = jsObject.Get("trump").As<Napi::Number>().Int32Value();
    int leaderSeat = jsObject.Get("first").As<Napi::Number>().Int32Value();

    dealPBN dlPBN;
    std::memset(&dlPBN, 0, sizeof(dealPBN));
    
    dlPBN.trump = chosenTrump; 
    dlPBN.first = leaderSeat; 

    int cardCount = 0;
    Napi::Array playedCardsArray;

    // Safely check if playedCards exists and is an array type before extracting
    if (jsObject.Has("playedCards") && jsObject.Get("playedCards").IsArray()) {
        playedCardsArray = jsObject.Get("playedCards").As<Napi::Array>();
        cardCount = playedCardsArray.Length();
    }

    for (int i = 0; i < 3; i++) {
        if (i < cardCount && playedCardsArray.Has(i) && !playedCardsArray.Get(i).IsUndefined() && !playedCardsArray.Get(i).IsNull()) {
            Napi::Object cardObj = playedCardsArray.Get(i).As<Napi::Object>();
            
            int suit = cardObj.Has("suit") ? cardObj.Get("suit").As<Napi::Number>().Int32Value() : 0;
            std::string rankStr = cardObj.Has("rank") ? cardObj.Get("rank").As<Napi::String>().Utf8Value() : "";
            
            dlPBN.currentTrickSuit[i] = suit;
            char rankChar = rankStr.empty() ? '0' : rankStr[0];
            dlPBN.currentTrickRank[i] = getDdsRank(rankChar);
        } else {
            dlPBN.currentTrickSuit[i] = 0;
            dlPBN.currentTrickRank[i] = 0;
        }
    }

    std::strncpy(dlPBN.remainCards, pbnHandString.c_str(), sizeof(dlPBN.remainCards) - 1);

    futureTricks solvedResults;
    std::memset(&solvedResults, 0, sizeof(futureTricks));

    int statusCode = SolveBoardPBN(dlPBN, -1, 1, 1, &solvedResults, 0);

    if (statusCode != 1) {
        std::string errorMsg = "DDS Solver failed with status error code: " + std::to_string(statusCode);
        Napi::Error::New(env, errorMsg).ThrowAsJavaScriptException();
        return env.Null();
    }

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
