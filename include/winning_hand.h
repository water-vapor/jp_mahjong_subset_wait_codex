#pragma once
#include "tiles.h"
#include <array>
#include <string>
#include <iostream>
#include <cassert>
#include <algorithm>

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

    bool isOrdered() const {
        size_t limit = isChiitoitsu ? tileIds.size() : 4;
        for (size_t i = 1; i < limit; ++i) {
            if (tileIds[i] < tileIds[i-1]) return false;
        }
        return true;
    }

    bool validChiitoi() const {
        if (!isChiitoitsu) return false;
        for (size_t i = 1; i < tileIds.size(); ++i) {
            if (tileIds[i] <= tileIds[i-1]) return false;
        }
        return true;
    }

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
        assert(isOrdered());
        if (isChiitoitsu) assert(validChiitoi());
        calculateYakuman();
        if (totalHan == 0)
            calculateYaku();
    }

    void calculateYaku() {
        addResult(winds());
        addResult(dora());
        addResult(tanyao());
        addResult(pinfu());
        addResult(peikou());
        addResult(chitoitsu());
        addResult(toitoi());
        addResult(sanshoku());
        addResult(ittsuu());
        addResult(hon_chinitsu());
        addResult(sanankou());
        addResult(shousangen());
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
        addResult(hyakumannGoku());
        addResult(goldenGateBridge());
        addResult(touhokuShinkansen());
        addResult(daichisei());
    }

#include "yaku_basic.h"
#include "yaku_yakuman.h"
};





