    YakuResult winds() const {
        YakuResult res;
        if (isChiitoitsu){
            return res;
        }
        for (int i = 0; i < 4; i++){
            if (groupTypes[i] == 0){
                if (tileIds[i] == gameInfo.prevailingWind){
                    res.han += 1;
                    res.yaku += "场风 ";
                }
                if (tileIds[i] == gameInfo.seatWind){
                    res.han += 1;
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
            bool twoSided = false;
            if (waitMeldIndex >= 0 && waitMeldIndex < 4 && groupTypes[waitMeldIndex]){
                int base = tileIds[waitMeldIndex];
                if (waitTileId == base || waitTileId == base + 2){
                    int r1, r2;
                    if (waitTileId == base){
                        r1 = (base + 1) % 9 + 1;
                        r2 = (base + 2) % 9 + 1;
                    } else {
                        r1 = base % 9 + 1;
                        r2 = (base + 1) % 9 + 1;
                    }
                    if (r1 > 1 && r2 < 9){
                        twoSided = true;
                    }
                }
            }
            if (!twoSided){
                isPinfu = false;
            }
            if (pairTileId == z5 || pairTileId == z6 || pairTileId == z7 ||
                pairTileId == gameInfo.prevailingWind || pairTileId == gameInfo.seatWind){
                isPinfu = false;
            }
        }
        if (isPinfu){
            res.han += 1;
            res.yaku += "平和 ";
        }
        return res;
    }

    YakuResult peikou() const {
        YakuResult res;
        if (isChiitoitsu){
            return res;
        }
        bool isRyanpeikou = (groupTypes[0] == 1 && groupTypes[1] == 1 && groupTypes[2] == 1 && groupTypes[3] == 1 &&
            tileIds[0] == tileIds[1] && tileIds[2] == tileIds[3]);
        if (isRyanpeikou){
            res.han += 3;
            res.yaku += "两杯口 ";
            return res;
        }
        bool isIipeikou = false;
        if (groupTypes[0] == 1 && groupTypes[1] == 1 && tileIds[0] == tileIds[1]){
            isIipeikou = true;
        }
        if (groupTypes[1] == 1 && groupTypes[2] == 1 && tileIds[1] == tileIds[2]){
            isIipeikou = true;
        }
        if (groupTypes[2] == 1 && groupTypes[3] == 1 && tileIds[2] == tileIds[3]){
            isIipeikou = true;
        }
        if (isIipeikou){
            res.han += 1;
            res.yaku += "一杯口 ";
        }
        return res;
    }

    YakuResult chitoitsu() const {
        YakuResult res;
        if (isChiitoitsu && validChiitoi()){
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
        auto check_combo = [&](int a, int b, int c){
            if (groupTypes[a] == groupTypes[b] && groupTypes[b] == groupTypes[c] &&
                tileIds[a] % 9 == tileIds[b] % 9 && tileIds[b] % 9 == tileIds[c] % 9){
                int s1 = Tiles::suitIndex[tileIds[a]];
                int s2 = Tiles::suitIndex[tileIds[b]];
                int s3 = Tiles::suitIndex[tileIds[c]];
                if (s1 < 3 && s2 < 3 && s3 < 3 && s1 != s2 && s1 != s3 && s2 != s3){
                    sameGroupType = groupTypes[a];
                    isSanshoku = true;
                }
            }
        };
        check_combo(0,1,2);
        check_combo(1,2,3);
        check_combo(0,1,3);
        check_combo(0,2,3);
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
        std::array<int, 4> counts{};
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
        std::array<int, 4> counts{};
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
        if (isChiitoitsu){
            return res;
        }
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

