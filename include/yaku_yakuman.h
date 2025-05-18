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
        std::array<int, 9> counts{};
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
        int r1 = tileId1 % 9;
        int r2 = tileId2 % 9;
        int r3 = tileId3 % 9;
        int s1 = Tiles::suitIndex[tileId1];
        int s2 = Tiles::suitIndex[tileId2];
        int s3 = Tiles::suitIndex[tileId3];
        std::array<int,3> ranks{r1,r2,r3};
        std::sort(ranks.begin(), ranks.end());
        bool correctRanks = ranks[0] == 0 && ranks[1] == 3 && ranks[2] == 6;
        bool diffSuit     = (s1 != s2) && (s1 != s3) && (s2 != s3);
        return correctRanks && diffSuit;
    }

    YakuResult sanshoku_tsuukan() const {
        YakuResult res;
        if (isChiitoitsu){
            return res;
        }
        bool isSanshokuTsuukan = false;
        for (int i = 0; i < 4 && !isSanshokuTsuukan; ++i){
            for (int j = i + 1; j < 4 && !isSanshokuTsuukan; ++j){
                for (int k = j + 1; k < 4 && !isSanshokuTsuukan; ++k){
                    if (groupTypes[i] == 1 && groupTypes[j] == 1 && groupTypes[k] == 1 &&
                        _sanshouku_tsuukan_helper(tileIds[i], tileIds[j], tileIds[k])){
                        isSanshokuTsuukan = true;
                    }
                }
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
        for (int i = 0; i < 4 && !isSanrenkou; ++i){
            for (int j = i + 1; j < 4 && !isSanrenkou; ++j){
                for (int k = j + 1; k < 4 && !isSanrenkou; ++k){
                    if (groupTypes[i] == 0 && groupTypes[j] == 0 && groupTypes[k] == 0 &&
                        _sanrenkou_helper(tileIds[i], tileIds[j], tileIds[k])){
                        isSanrenkou = true;
                    }
                }
            }
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
        for (int i = 0; i < 4 && !isIsshokuSanjun; ++i){
            for (int j = i + 1; j < 4 && !isIsshokuSanjun; ++j){
                for (int k = j + 1; k < 4 && !isIsshokuSanjun; ++k){
                    if (groupTypes[i] == 1 && groupTypes[j] == 1 && groupTypes[k] == 1 &&
                        tileIds[i] == tileIds[j] && tileIds[j] == tileIds[k]){
                        isIsshokuSanjun = true;
                    }
                }
            }
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
        bool allSeq = true;
        for (int i = 0; i < 4; ++i){
            if (groupTypes[i] != 1){
                allSeq = false;
                break;
            }
        }
        if (allSeq){
            bool same = true;
            for (int i = 1; i < 4; ++i){
                if (tileIds[i] != tileIds[0]){
                    same = false;
                    break;
                }
            }
            if (same){
                res.han += 10000;
                res.yaku += "一色四顺 ";
            }
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
