#pragma once
#include "tiles.h"
#include <array>
#include <string>
#include <iostream>

struct YakuResult {
    int han{};
    std::string yaku;
};
class WinningHand{
public:
    std::array<int, 7> tileIds;
    std::array<bool, 4> groupTypes; // 0 -> triplets, 1 -> sequences
    int pairTileId = 0;
    bool isChiitoitsu = false;
    int waitMeldIndex = -1;
    int waitTileId = -1;

    int totalHan = 0;
    std::string yakuString = "";

    WinningHand() = default;

    void addResult(const YakuResult& res) {
        if (res.han > 0) {
            totalHan += res.han;
            yakuString += res.yaku;
        }
    }

    Tiles toTiles() const {
        Tiles t;
        if (isChiitoitsu){
            for (int i = 0; i < 7; i++){
                t.add(tileIds[i], 2);
            }
        }
        else{
            for (int i = 0; i < 4; i++){
                if (groupTypes[i] == 0){
                    t.add(tileIds[i], 3);
                } else {
                    int currentTileId = tileIds[i];
                    t.add(currentTileId);
                    t.add(currentTileId+1);
                    t.add(currentTileId+2);
                }
            }
        }
        return t;
    }

    // iterate over all tiles in the hand
    std::array<int, 14> fullTiles() const {
        std::array<int, 14> res;
        if (isChiitoitsu){
            for (int i = 0; i < 7; i++){
                res[i*2] = tileIds[i];
                res[i*2+1] = tileIds[i];
            }
        }
        else{
            for (int i = 0; i < 4; i++){    
                if (groupTypes[i] == 0){
                    res[i*3] = tileIds[i];
                    res[i*3+1] = tileIds[i];
                    res[i*3+2] = tileIds[i];
                }
                else{
                    res[i*3] = tileIds[i];
                    res[i*3+1] = tileIds[i]+1;
                    res[i*3+2] = tileIds[i]+2;
                }
            }
            res[12] = pairTileId;
            res[13] = pairTileId;
        }
        return res;
    }

    void calculateHan() {
        if (totalHan != 0){
            std::cerr << "totalHan is already calculated" << std::endl;
            return;
        }
        calculateYakuman();
        if (totalHan == 0)
            calculateYaku();
    }

    void calculateYaku() {
        addResult(winds());
        addResult(dora());
        addResult(tanyao());
        addResult(pinfu());
        addResult(iipeikou());
        addResult(chitoitsu());
        addResult(toitoi());
        addResult(sanshoku());
        addResult(ittsuu());
        addResult(hon_chinitsu());
        addResult(sanankou());
        addResult(shousangen());
        addResult(ryanpeikou());
        addResult(hunroutou());
        addResult(taiyao());
        // local yakus
        addResult(reversibleTiles());
        addResult(tripleRedTiles());
        addResult(uumensai());
        addResult(sanshoku_tsuukan());
        addResult(sanrenkou());
        addResult(isshokuSanjun());
    }

    

    void calculateYakuman() {
        addResult(daisangen());
        addResult(suushi());
        addResult(tsuuiisou());
        addResult(ryuuiisou());
        addResult(chinroutou());
        addResult(chuuren());
        addResult(suuankouTanki());
        // local yakuman
        addResult(suurenkou());
        addResult(isshokuSanjun());
        addResult(isshokuYonjun());
        addResult(benikujaku());
        addResult(kouitten());
        addResult(kokuiisou());
        addResult(daiXrin());
    }

    YakuResult winds() const {
        YakuResult res;
        if (isChiitoitsu){
            return res;
        }
        for (int i = 0; i < 4; i++){
            if (groupTypes[i] == 0 && (tileIds[i] == gameInfo.prevailingWind || tileIds[i] == gameInfo.seatWind)){
                res.han += 1;
                if (tileIds[i] == gameInfo.prevailingWind){
                    res.yaku += "场风 ";
                }
                else if (tileIds[i] == gameInfo.seatWind){
                    res.yaku += "自风 ";
                }
            }
        }
        return res;
    }
    
    YakuResult dora() const {
        YakuResult res;
        // dora
        int doraCount = 0;
        if (isChiitoitsu){
            for (int i = 0; i < 7; i++){
                if (tileIds[i] == gameInfo.doraTileId){
                    doraCount += 2;
                }
            }
        }
        else{
            for (int i = 0; i < 4; i++){
                if (groupTypes[i] == 0 && tileIds[i] == gameInfo.doraTileId){
                    doraCount += 3;
                }
                else if (groupTypes[i] == 1 && (tileIds[i] == gameInfo.doraTileId || tileIds[i] + 1 == gameInfo.doraTileId || tileIds[i] + 2 == gameInfo.doraTileId)){
                    doraCount += 1;
                }
            }
        }
        if (doraCount > 0){
            res.han += doraCount;
            res.yaku += "宝牌" + std::to_string(doraCount) + " ";
        }
        // akadora
        int hasRed5m = 0, hasRed5p = 0, hasRed5s = 0;
        for (auto tileId : fullTiles()){
            if (tileId == m5 && gameInfo.red5m){
                hasRed5m = 1;
            }
            else if (tileId == p5 && gameInfo.red5p){
                hasRed5p = 1;
            }
            else if (tileId == s5 && gameInfo.red5s){
                hasRed5s = 1;
            }
        }
        int akaDoraCount = hasRed5m + hasRed5p + hasRed5s;
        if (akaDoraCount > 0){
            res.han += akaDoraCount;
            res.yaku += "赤宝牌" + std::to_string(akaDoraCount) + " ";
        }
        return res;
    }

    YakuResult tanyao() const {
        YakuResult res;
        bool isTanyao = true;
        for (auto tileId : fullTiles()){
            if (Tiles::isYaochu(tileId)){
                isTanyao = false;
                break;
            }
        }
        if (isTanyao){
            res.han += 1;
            res.yaku += "断幺 ";
        }
        return res;
    }

    YakuResult pinfu() const {
        YakuResult res;
        if (isChiitoitsu){
            return res;
        }
        bool isPinfu = true;
        for (int i = 0; i < 4; i++){
            if (groupTypes[i] == 0){
                isPinfu = false;
                break;
            }
        }
        if (isPinfu){
            bool leftWait = waitTileId == tileIds[waitMeldIndex] && (tileIds[waitMeldIndex] % 9 != 0);
            bool rightWait = waitTileId == tileIds[waitMeldIndex] + 2 && ((tileIds[waitMeldIndex] + 2) % 9 != 8);
            if (!leftWait && !rightWait){
                isPinfu = false;
            }
        }
        if (isPinfu){
            res.han += 1;
            res.yaku += "平和 ";
        }
        return res;
    }

    YakuResult iipeikou() const {
        YakuResult res;
        if (isChiitoitsu){
            return res;
        }
        bool isLipeikou = false;
        if (groupTypes[0] == 1 && groupTypes[1] == 1 && tileIds[0] == tileIds[1]){
            isLipeikou = true;
        }
        if (groupTypes[1] == 1 && groupTypes[2] == 1 && tileIds[1] == tileIds[2]){
            isLipeikou = true;
        }
        if (groupTypes[2] == 1 && groupTypes[3] == 1 && tileIds[2] == tileIds[3]){
            isLipeikou = true;
        }
        if (isLipeikou){
            res.han += 1;
            res.yaku += "一杯口 ";
        }
        return res;
    }

    YakuResult chitoitsu() const {
        YakuResult res;
        if (isChiitoitsu){
            res.han += 2;
            res.yaku += "七对子 ";
        }
        return res;
    }

    YakuResult toitoi() const {
        YakuResult res;
        if (isChiitoitsu){
            return res;
        }
        if (groupTypes[0] == 0 && groupTypes[1] == 0 && groupTypes[2] == 0 && groupTypes[3] == 0){
            res.han += 2;
            res.yaku += "对对和 ";
        }
        return res;
    }

    YakuResult sanshoku() const {
        YakuResult res;
        if (isChiitoitsu){
            return res;
        }
        bool isSanshoku = false;
        int sameGroupType = -1;
        if (groupTypes[0] == groupTypes[1] && groupTypes[1] == groupTypes[2] && tileIds[0] % 9 == tileIds[1] % 9 && tileIds[1] % 9 == tileIds[2] % 9){
            sameGroupType = groupTypes[0];
            isSanshoku = true;
        }
        if (groupTypes[1] == groupTypes[2] && groupTypes[2] == groupTypes[3] && tileIds[1] % 9 == tileIds[2] % 9 && tileIds[2] % 9 == tileIds[3] % 9){
            sameGroupType = groupTypes[1];
            isSanshoku = true;
        }
        if (groupTypes[0] == groupTypes[1] && groupTypes[1] == groupTypes[3] && tileIds[0] % 9 == tileIds[1] % 9 && tileIds[1] % 9 == tileIds[3] % 9){
            sameGroupType = groupTypes[0];
            isSanshoku = true;
        }
        if (groupTypes[0] == groupTypes[2] && groupTypes[2] == groupTypes[3] && tileIds[0] % 9 == tileIds[2] % 9 && tileIds[2] % 9 == tileIds[3] % 9){
            sameGroupType = groupTypes[0];
            isSanshoku = true;
        }
        if (isSanshoku){
            res.han += 2;
            if (sameGroupType == 0){
                res.yaku += "三色同刻 ";
            }
            else{
                res.yaku += "三色同顺 ";
            }
        }
        return res;
    }

    bool _ittsuu_helper() const {
        if (isChiitoitsu){
            return false;
        }
        std::array<int, 4> counts;
        for (int i = 0; i < 4; i++){
            counts[Tiles::suitIndex[tileIds[i]]]++;
        }
        int suit = -1;
        if (counts[0] >= 3){
            suit = 0;
        }
        else if (counts[1] >= 3){
            suit = 1;
        }
        else if (counts[2] >= 3){
            suit = 2;
        }
        else{
            return false;
        }
        bool seq1 = false, seq2 = false, seq3 = false;
        for (int i = 0; i < 4; i++){
            if (Tiles::suitIndex[tileIds[i]] == suit){
                if (groupTypes[i] == 1 && tileIds[i] % 9 == 0){
                    seq1 = true;
                }
                else if (groupTypes[i] == 1 && tileIds[i] % 9 == 3){
                    seq2 = true;
                }
                else if (groupTypes[i] == 1 && tileIds[i] % 9 == 6){
                    seq3 = true;
                }
            }
        }
        if (seq1 && seq2 && seq3){
            return true;
        }
        return false;
    }

    YakuResult ittsuu() const {
        YakuResult res;
        if (_ittsuu_helper()){
            res.han += 2;
            res.yaku += "一气通贯 ";
        }
        return res;
    }

    YakuResult hon_chinitsu() const {
        YakuResult res;
        std::array<int, 4> counts;
        int han = 0;
        for (auto tileId : fullTiles()){
            counts[Tiles::suitIndex[tileId]]++;
        }
        if (counts[0] == 0 && counts[1] == 0){
            if (counts[3] == 0){
                han = 6;
            }else{
                han = 3;
            }
        }
        if (counts[0] == 0 && counts[2] == 0){
            if (counts[3] == 0){
                han = 6;
            }else{
                han = 3;
            }
        }
        if (counts[1] == 0 && counts[2] == 0){
            if (counts[3] == 0){
                han = 6;
            }else{
                han = 3;
            }
        }
        if (han == 6){
            res.yaku += "清一色 ";
        }else if (han == 3){
            res.yaku += "混一色 ";
        }
        res.han += han;
        return res;
    }

    YakuResult sanankou() const {
        YakuResult res;
        int ankou_count = 0;
        for (int i = 0; i < 4; i++){
            if (groupTypes[i] == 0 && waitMeldIndex != i){
                ankou_count++;
            }
        }
        if (ankou_count >= 3){
            res.han += 2;
            res.yaku += "三暗刻 ";
        }
        return res;
    }

    YakuResult shousangen() const {
        YakuResult res;
        if (isChiitoitsu){
            return res;
        }
        int count = 0;
        for (int i = 0; i < 4; i++){
            if (tileIds[i] == z5 || tileIds[i] == z6 || tileIds[i] == z7){
                count++;
            }
        }
        if (pairTileId == z5 || pairTileId == z6 || pairTileId == z7){
            count++;
        }
        if (count == 3){
            res.han += 2;
            res.yaku += "小三元 ";
        }
        return res;
    }

    YakuResult ryanpeikou() const {
        YakuResult res;
        if (isChiitoitsu){
            return res;
        }
        if (groupTypes[0] == 1 && groupTypes[1] == 1 && groupTypes[2] == 1 && groupTypes[3] == 1 && tileIds[0] == tileIds[1] && tileIds[2] == tileIds[3]){
            res.han += 2;
            res.yaku += "两杯口 ";
        }
        return res;
    }

    YakuResult hunroutou() const {
        YakuResult res;
        bool isHunroutou = true;
        for (auto tileId : fullTiles()){
            if (!Tiles::isYaochu(tileId)){
                isHunroutou = false;
                break;
            }
        }
        if (isHunroutou){
            res.han += 2;
            res.yaku += "混老头 ";
        }
        return res;
    }

    YakuResult taiyao() const {
        YakuResult res;
        if (isChiitoitsu){
            return res;
        }
        bool isChanta = true;
        bool isJunchan = true;
        for (int i = 0; i < 4; i++){
            if (groupTypes[i] == 0){
                if (!Tiles::isYaochu(tileIds[i])){
                    isChanta = false;
                    isJunchan = false;
                }else{
                    if (Tiles::suitIndex[tileIds[i]] == 3){
                        isJunchan = false;
                    }
                }
            }else{
                if (!(tileIds[i] % 9 == 0 || tileIds[i] % 9 == 6)){
                    isChanta = false;
                    isJunchan = false;
                }
            }
            
        }
        if (!Tiles::isYaochu(pairTileId)){
            isChanta = false;
            isJunchan = false;
        }else{
            if (Tiles::suitIndex[pairTileId] == 3){
                isJunchan = false;
            }
        }
        if (isJunchan){
            res.han += 3;
            res.yaku += "纯全 ";
            return res;
        }
        if (isChanta){
            res.han += 2;
            res.yaku += "全带 ";
        }
        return res;
    }

    // yakuman
    YakuResult daisangen() const {
        YakuResult res;
        if (isChiitoitsu){
            return res;
        }
        int count = 0;
        for (int i = 0; i < 4; i++){
            if (tileIds[i] == z5 || tileIds[i] == z6 || tileIds[i] == z7){
                count++;
            }
        }
        if (count == 3){
            res.han += 10000;
            res.yaku += "大三元 ";
        }
        return res;
    }

    YakuResult suushi() const {
        YakuResult res;
        if (isChiitoitsu){
            return res;
        }
        int suushiCount = 0;
        for (auto tileId : fullTiles()){
            if (tileId == z1 || tileId == z2 || tileId == z3 || tileId == z4){
                suushiCount++;
            }
        }
        if (suushiCount == 12){ // dai suushi
            res.han += 20000;
            res.yaku += "大四喜 ";
        }
        if (suushiCount == 11){ // shou suushi
            res.han += 10000;
            res.yaku += "小四喜 ";
        }
        return res;
    }

    YakuResult tsuuiisou() const {
        YakuResult res;
        bool isTsuuiisou = true;
        for (auto tileId : fullTiles()){
            if (!(tileId == z1 || tileId == z2 || tileId == z3 || tileId == z4 || tileId == z5 || tileId == z6 || tileId == z7)){
                isTsuuiisou = false;
            }
        }
        if (isTsuuiisou){
            res.han += 10000;
            res.yaku += "字一色 ";
        }
        return res;
    }

    YakuResult ryuuiisou() const {
        YakuResult res;
        bool isRyuuiisou = true;
        bool isChinryuusou = true;
        for (auto tileId : fullTiles()){
            if (!(tileId == s2 || tileId == s3 || tileId == s4 || tileId == s6 || tileId == s8 || tileId == z6)){
                isRyuuiisou = false;
            }
            if (!(tileId == s2 || tileId == s3 || tileId == s4 || tileId == s6 || tileId == s8)){
                isChinryuusou = false;
            }
        }
        if (isChinryuusou){
            res.han += 20000;
            res.yaku += "无发绿一色 ";
        }
        if (isRyuuiisou){
            res.han += 10000;
            res.yaku += "绿一色 ";
        }
        return res;
    }

    YakuResult chinroutou() const {
        YakuResult res;
        bool isChinroutou = true;
        for (auto tileId : fullTiles()){
            if (!(tileId == m1 || tileId == m9 || tileId == p1 || tileId == p9 || tileId == s1 || tileId == s9)){
                isChinroutou = false;
            }
        }
        if (isChinroutou){
            res.han += 10000;
            res.yaku += "清老头 ";
        }
        return res;
    }

    YakuResult chuuren() const {
        YakuResult res;
        if (isChiitoitsu){
            return res;
        }
        bool isChuuren = false;
        bool isChuuren9wait = false;
        int inferredSuit = Tiles::suitIndex[tileIds[0]];
        std::array<int, 9> counts;
        for (auto tileId : fullTiles()){
            if (Tiles::suitIndex[tileId] != inferredSuit){
                return res;
            }
            counts[tileId % 9]++;
        }
        if (counts[0] >= 3 && counts[1] >= 1 && counts[2] >= 1 && counts[3] >= 1 && counts[4] >= 1 && counts[5] >= 1 && counts[6] >= 1 && counts[7] >= 1 && counts[8] >= 3){
            int waitTile = waitTileId % 9;
            if ((waitTile == 0 || waitTile == 8) && counts[waitTile] == 4){
                isChuuren9wait = true;
            }else if (counts[waitTile] == 2){
                isChuuren9wait = true;
            }
            isChuuren = true;
        }
        if (isChuuren9wait){
            res.han += 20000;
            res.yaku += "纯正九莲宝灯 ";
            return res;
        }
        if (isChuuren){
            res.han += 10000;
            res.yaku += "九莲宝灯 ";
        }
        return res;
    }

    YakuResult suuankouTanki() const {
        YakuResult res;
        if (isChiitoitsu){
            return res;
        }
        if (groupTypes[0] == 0 && groupTypes[1] == 0 && groupTypes[2] == 0 && groupTypes[3] == 0 && waitMeldIndex == 4){
            res.han += 20000;
            res.yaku += "四暗刻单骑 ";
            return res;
        }
        return res;
    }

    // local yakus
    YakuResult reversibleTiles() const {
        YakuResult res;
        bool isReversible = true;
        for (auto tileId : fullTiles()){
            if (tileId == p1 || tileId == p2 || tileId == p3 || tileId == p4 || tileId == p5 || tileId == p8 || tileId == p9 || tileId == s2 || tileId == s4 || tileId == s6 || tileId == s8 || tileId == s9 || tileId == z5){
                isReversible = false;
            }
        }
        if (isReversible){
            res.han += 1;
            res.yaku += "推不倒 ";
        }
        return res;
    }

    YakuResult tripleRedTiles() const {
        YakuResult res;
        bool isTripleRed = gameInfo.red5m && gameInfo.red5p && gameInfo.red5s;
        bool hand5m = false, hand5p = false, hand5s = false;
        for (auto tileId : fullTiles()){
            if (tileId == m5){
                hand5m = true;
            }
            if (tileId == p5){
                hand5p = true;
            }
            if (tileId == s5){
                hand5s = true;
            }
        }
        if (isTripleRed && hand5m && hand5p && hand5s){
            res.han += 2;
            res.yaku += "赤三色 ";
        }
        return res;
    }

    YakuResult uumensai() const {
        YakuResult res;
        bool m = false, p = false, s = false, w = false, d = false;
        for (auto tileId : fullTiles()){
            if (Tiles::suitIndex[tileId] == 0){
                m = true;
            }
            if (Tiles::suitIndex[tileId] == 1){
                p = true;
            }
            if (Tiles::suitIndex[tileId] == 2){
                s = true;
            }
            if (Tiles::suitIndex[tileId] == 3){
                if (tileId == z1 || tileId == z2 || tileId == z3 || tileId == z4){
                    w = true;
                }
                if (tileId == z5 || tileId == z6 || tileId == z7){
                    d = true;
                }
            }
        }
        if (m && p && s && w && d){
            res.han += 2;
            res.yaku += "五门齐 ";
        }
        return res;
    }

    bool _sanshouku_tsuukan_helper(int tileId1, int tileId2, int tileId3) const {
        std::array<int, 3> numCounts;
        std::array<int, 3> suitCounts;
        numCounts[(tileId1 % 9)/3]++;
        numCounts[(tileId2 % 9)/3]++;
        numCounts[(tileId3 % 9)/3]++;
        suitCounts[Tiles::suitIndex[tileId1]]++;
        suitCounts[Tiles::suitIndex[tileId2]]++;
        suitCounts[Tiles::suitIndex[tileId3]]++;
        if (numCounts[0] == 1 && numCounts[1] == 1 && numCounts[2] == 1){
            if (suitCounts[0] == 1 && suitCounts[1] == 1 && suitCounts[2] == 1){
                return true;
            }
        }
        return false;
    }

    YakuResult sanshoku_tsuukan() const {
        YakuResult res;
        if (isChiitoitsu){
            return res;
        }
        bool isSanshokuTsuukan = false;
        if (groupTypes[0] == 1 && groupTypes[1] == 1 && groupTypes[2] == 1){
            if (_sanshouku_tsuukan_helper(tileIds[0], tileIds[1], tileIds[2])){
                isSanshokuTsuukan = true;
            }
        }
        if (groupTypes[0] == 1 && groupTypes[1] == 1 && groupTypes[3] == 1){
            if (_sanshouku_tsuukan_helper(tileIds[0], tileIds[1], tileIds[3])){
                isSanshokuTsuukan = true;
            }
        }
        if (groupTypes[0] == 1 && groupTypes[2] == 1 && groupTypes[3] == 1){
            if (_sanshouku_tsuukan_helper(tileIds[0], tileIds[2], tileIds[3])){
                isSanshokuTsuukan = true;
            }
        }
        if (groupTypes[1] == 1 && groupTypes[2] == 1 && groupTypes[3] == 1){
            if (_sanshouku_tsuukan_helper(tileIds[1], tileIds[2], tileIds[3])){
                isSanshokuTsuukan = true;
            }
        }
        if (isSanshokuTsuukan){
            res.han += 2;
            res.yaku += "三色通贯 ";
        }
        return res;
    }

    bool _sanrenkou_helper(int tileId1, int tileId2, int tileId3) const {
        if (Tiles::suitIndex[tileId1] == Tiles::suitIndex[tileId2] && Tiles::suitIndex[tileId2] == Tiles::suitIndex[tileId3] && tileId1 + 1 == tileId2 && tileId2 + 1 == tileId3 && tileId3 < z1){
            return true;
        }
        return false;
    }

    YakuResult sanrenkou() const {
        YakuResult res;
        if (isChiitoitsu){
            return res;
        }
        bool isSanrenkou = false;
        if (groupTypes[0] == 0 && groupTypes[1] == 0 && groupTypes[2] == 0 && _sanrenkou_helper(tileIds[0], tileIds[1], tileIds[2])){
            isSanrenkou = true;
        }
        if (groupTypes[0] == 0 && groupTypes[1] == 0 && groupTypes[3] == 0 && _sanrenkou_helper(tileIds[0], tileIds[1], tileIds[3])){
            isSanrenkou = true;
        }
        if (groupTypes[0] == 0 && groupTypes[2] == 0 && groupTypes[3] == 0 && _sanrenkou_helper(tileIds[0], tileIds[2], tileIds[3])){
            isSanrenkou = true;
        }   
        if (groupTypes[1] == 0 && groupTypes[2] == 0 && groupTypes[3] == 0 && _sanrenkou_helper(tileIds[1], tileIds[2], tileIds[3])){
            isSanrenkou = true;
        }
        if (isSanrenkou){
            res.han += 2;
            res.yaku += "三连刻 ";
        }
        return res;
    }

    YakuResult isshokuSanjun() const {
        YakuResult res;
        if (isChiitoitsu){
            return res;
        }
        bool isIsshokuSanjun = false;
        if (groupTypes[0] == 1 && groupTypes[1] == 1 && groupTypes[2] == 1 && tileIds[0] == tileIds[1] && tileIds[1] == tileIds[2]){
            isIsshokuSanjun = true;
        }
        if (groupTypes[0] == 1 && groupTypes[1] == 1 && groupTypes[3] == 1 && tileIds[0] == tileIds[1] && tileIds[1] == tileIds[3]){
            isIsshokuSanjun = true;
        }
        if (groupTypes[0] == 1 && groupTypes[2] == 1 && groupTypes[3] == 1 && tileIds[0] == tileIds[2] && tileIds[2] == tileIds[3]){
            isIsshokuSanjun = true;
        }
        if (groupTypes[1] == 1 && groupTypes[2] == 1 && groupTypes[3] == 1 && tileIds[1] == tileIds[2] && tileIds[2] == tileIds[3]){
            isIsshokuSanjun = true;
        }
        if (isIsshokuSanjun){
            res.han += 2;
            res.yaku += "一色三同顺 ";
        }
        return res;
    }

    // local yakuman
    YakuResult suurenkou() const {
        YakuResult res;
        if (isChiitoitsu){
            return res;
        }
        if (groupTypes[0] == 0 && groupTypes[1] == 0 && groupTypes[2] == 0 && groupTypes[3] == 0 && tileIds[0] == tileIds[1] && tileIds[1] == tileIds[2] && tileIds[2] == tileIds[3]){
            res.han += 10000;
            res.yaku += "四连刻 ";
        }
        return res;
    }

    YakuResult isshokuYonjun() const {
        YakuResult res;
        if (isChiitoitsu){
            return res;
        }
        if (groupTypes[0] == 1 && groupTypes[1] == 1 && groupTypes[2] == 1 && groupTypes[3] == 1 && tileIds[0] == tileIds[1] && tileIds[1] == tileIds[2] && tileIds[2] == tileIds[3]){
            res.han += 10000;
            res.yaku += "一色四顺 ";
        }
        return res;
    }

    YakuResult benikujaku() const {
        YakuResult res;
        if (isChiitoitsu){
            return res;
        }
        if (groupTypes[0] == 0 && groupTypes[1] == 0 && groupTypes[2] == 0 && groupTypes[3] == 0 && tileIds[0] == s1 && tileIds[1] == s5 && tileIds[2] == s7 && tileIds[3] == s9 && pairTileId == z7){
            res.han += 10000;
            res.yaku += "红孔雀 ";
        }
        return res;
    }

    YakuResult kouitten() const {
        YakuResult res;
        if (isChiitoitsu){
            return res;
        }
        if (groupTypes[0] == 0 && groupTypes[1] == 0 && groupTypes[2] == 0 && groupTypes[3] == 0 && tileIds[0] == s2 && tileIds[1] == s3 && tileIds[2] == s4 && tileIds[3] == s6 && pairTileId == z7){
            res.han += 10000;
            res.yaku += "红一点 ";
        }
        return res;
    }

    YakuResult kokuiisou() const {
        YakuResult res;
        bool isKokuiisou = true;
        for (auto tileId : fullTiles()){
            if (!(tileId == p2 || tileId == p4 || tileId == p8 || tileId == z1 || tileId == z2 || tileId == z3 || tileId == z4)){
                isKokuiisou = false;
            }
        }
        if (isKokuiisou){
            res.han += 10000;
            res.yaku += "黑一色 ";
        }
        return res;
    }

    YakuResult daiXrin() const {
        YakuResult res;
        if (isChiitoitsu){
            int inferredSuit = Tiles::suitIndex[tileIds[0]];
            for (auto tileId : fullTiles()){
                if (Tiles::suitIndex[tileId] != inferredSuit){
                    return res;
                }
            }
            if (tileIds[0] == 1 && tileIds[1] == 2 && tileIds[2] == 3 && tileIds[3] == 4 && tileIds[4] == 5 && tileIds[5] == 6 && tileIds[6] == 7){
                res.han += 10000;
                if (inferredSuit == 0){
                    res.yaku += "大数邻 ";
                }else if (inferredSuit == 1){
                    res.yaku += "大车轮 ";
                }else if (inferredSuit == 2){
                    res.yaku += "大竹林 ";
                }
            }
        }
        return res;
    }

    YakuResult hyakumannGoku() const {
        YakuResult res;
        int sum = 0;
        for (auto tileId : fullTiles()){
            if (Tiles::suitIndex[tileId] != 0){
                return res;
            }
            sum += (tileId+1);
        }
        if (sum >= 100){
            res.han += 10000;
            res.yaku += "百万石 ";
        }
        return res;
    }

    YakuResult goldenGateBridge() const {
        YakuResult res;
        if (isChiitoitsu){
            return res;
        }
        if (Tiles::suitIndex[tileIds[0]] == Tiles::suitIndex[tileIds[1]] && Tiles::suitIndex[tileIds[1]] == Tiles::suitIndex[tileIds[2]] && Tiles::suitIndex[tileIds[2]] == Tiles::suitIndex[tileIds[3]]){
            if (tileIds[0] % 9 == 0 && tileIds[1] % 9 == 2 && tileIds[2] % 9 == 4 && tileIds[3] % 9 == 6){
                res.han += 10000;
                res.yaku += "金门桥 ";
            }
        }
        return res;
    }

    YakuResult touhokuShinkansen() const {
        YakuResult res;
        if (isChiitoitsu){
            return res;
        }
        bool hasZ1 = false, hasZ4 = false;
        if (_ittsuu_helper()){
            for (auto tileId : fullTiles()){
                if (tileId == z1){
                    hasZ1 = true;
                }
                if (tileId == z4){
                    hasZ4 = true;
                }
            }
            if (hasZ1 && hasZ4){
                res.han += 10000;
                res.yaku += "东北新干线 ";
            }
        }
        return res;
    }

    YakuResult daichisei() const {
        YakuResult res;
        if (isChiitoitsu){
            for (auto tileId : fullTiles()){
                if (tileId < z1){
                    return res;
                }
            }
            res.han += 20000;
            res.yaku += "大七星 ";
        }
        return res;
    }
};





