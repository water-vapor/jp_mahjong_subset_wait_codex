#include <iostream>
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <cctype>
#include <cstdint>

class Tiles{
public:
    Tiles() = default;

    explicit Tiles(std::string_view s) {
        size_t i = 0;
        while (i < s.size()) {
            size_t j = i;
            while (j < s.size() && std::isdigit(s[j])) ++j;
            if (j == i || j >= s.size())
                throw std::invalid_argument{"Invalid tile string format"};

            char suit = s[j];
            int  base = [&](){
                switch(suit){
                    case 'm': return 0;
                    case 'p': return 9;
                    case 's': return 18;
                    case 'z': return 27;
                    default:  throw std::invalid_argument{"Unknown suit"};
                }
            }();

            for (size_t k = i; k < j; ++k) {
                int rank   = s[k] - '0';
                int tileId = base + (rank - 1);
                // bounds-check rank <…> and add(tileId), etc.
                if (!add(tileId))
                    throw std::out_of_range{"Too many copies of a tile"};
            }
            i = j + 1;
        }
    }

    // tileId: 0=man,1=pin,2=sou,3=honor
    static constexpr uint8_t fieldIndex[34] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3};

    // tileId: 3*rankIndex
    static constexpr uint8_t shiftAmount[34] = {
        0, 3, 6, 9, 12, 15, 18, 21, 24,
        0, 3, 6, 9, 12, 15, 18, 21, 24,
        0, 3, 6, 9, 12, 15, 18, 21, 24,
        0, 3, 6, 9, 12, 15, 18};


    /// Map a (1–9,'m'|'p'|'s') or (1–7,'z') pair to a tileId in [0..33].
    /// Throws std::invalid_argument or std::out_of_range on bad input.
    static inline int tileIndex(int rank, char suit) {
        int base;
        switch (suit) {
            case 'm': base =  0; if (rank < 1 || rank > 9)  break; return base + (rank - 1);
            case 'p': base =  9; if (rank < 1 || rank > 9)  break; return base + (rank - 1);
            case 's': base = 18; if (rank < 1 || rank > 9)  break; return base + (rank - 1);
            case 'z': base = 27; if (rank < 1 || rank > 7)  break; return base + (rank - 1);
            default:  throw std::invalid_argument("Invalid suit");
        }
        throw std::out_of_range("Tile rank out of range");
    }

    /// Map a two‐char string like "4s" or "1z" to its tileId.
    static inline int tileIndex(std::string_view tile) {
        if (tile.size() != 2)
            throw std::invalid_argument("Single tile must be two characters");
        int  rank = tile[0] - '0';
        char suit = tile[1];
        return tileIndex(rank, suit);
    }

    static inline bool isYaochu(int tileId){
        return tileId == z1 || tileId == z2 || tileId == z3 || tileId == z4 || tileId == z5 || tileId == z6 || tileId == z7 || tileId == m1 || tileId == m9 || tileId == p1 || tileId == p9 || tileId == s1 || tileId == s9;
    }

    bool add(int tileId, int count = 1)
    {
        auto idx = fieldIndex[tileId];
        auto s = shiftAmount[tileId];
        auto &f = this->*fieldPtr[idx];

        uint32_t cur = (f >> s) & 0x7u;
        if (cur + count > 4u)
        {
            std::cerr << "Too many tiles to add" << std::endl;
            return false;
        }
        f += uint32_t(count) << s;
        return true;
    }

    bool add(std::string_view tile, int count = 1)
    {
        return add(tileIndex(tile), count);
    }

    bool remove(int tileId, int count = 1)
    {
        auto idx = fieldIndex[tileId];
        auto s = shiftAmount[tileId];
        auto &f = this->*fieldPtr[idx];

        uint32_t cur = (f >> s) & 0x7u;
        if (cur < uint32_t(count))
        {
            std::cerr << "Not enough tiles to remove" << std::endl;
            return false;
        }
        f -= uint32_t(count) << s;
        return true;
    }

    bool remove(std::string_view tile, int count = 1)
    {
        return remove(tileIndex(tile), count);
    }

    int getCount(int tileId) const
    {
        auto idx = fieldIndex[tileId];
        auto s = shiftAmount[tileId];
        auto f = (this->*fieldPtr[idx]);
        return int((f >> s) & 0x7u);
    }

    int totalTiles() const
    {
        int sum = 0;
        uint32_t flds[4] = {man, pin, sou, honor};
        int lens[4] = {9, 9, 9, 7};
        for (int i = 0; i < 4; ++i)
        {
            uint32_t v = flds[i];
            for (int j = 0; j < lens[i]; ++j)
            {
                sum += int(v & 0x7u);
                v >>= 3;
            }
        }
        return sum;
    }

    std::string toString() const {
        std::string s;
        // man (0–8) → 'm'
        {
            bool any = false;
            for (int r = 0; r < 9; ++r) {
                int c = getCount(r);
                for (int i = 0; i < c; ++i) s.push_back(char('1' + r));
                any = any || (c > 0);
            }
            if (any) s.push_back('m');
        }
        // pin (9–17) → 'p'
        {
            bool any = false;
            for (int r = 0; r < 9; ++r) {
                int c = getCount(r + 9);
                for (int i = 0; i < c; ++i) s.push_back(char('1' + r));
                any = any || (c > 0);
            }
            if (any) s.push_back('p');
        }
        // sou (18–26) → 's'
        {
            bool any = false;
            for (int r = 0; r < 9; ++r) {
                int c = getCount(r + 18);
                for (int i = 0; i < c; ++i) s.push_back(char('1' + r));
                any = any || (c > 0);
            }
            if (any) s.push_back('s');
        }
        // honors (27–33) → 'z'
        {
            bool any = false;
            for (int r = 0; r < 7; ++r) {
                int c = getCount(r + 27);
                for (int i = 0; i < c; ++i) s.push_back(char('1' + r));
                any = any || (c > 0);
            }
            if (any) s.push_back('z');
        }
        return s;
    }



    bool operator==(Tiles const &o) const
    {
        return man == o.man && pin == o.pin && sou == o.sou && honor == o.honor;
    }

    struct Hash
    {
        size_t operator()(Tiles const &t) const
        {
            uint64_t lo = (uint64_t(t.man) << 32) | t.pin;
            uint64_t hi = (uint64_t(t.sou) << 32) | t.honor;
            return size_t(lo ^ (hi << 1));
        }
    };

private:
    uint32_t man = 0;
    uint32_t pin = 0;
    uint32_t sou = 0;
    uint32_t honor = 0;

    // pointers to each of {man, pin, sou, honor}
    static constexpr uint32_t Tiles::*fieldPtr[4] = {
        &Tiles::man, &Tiles::pin, &Tiles::sou, &Tiles::honor};
};

enum Tile{
    m1, m2, m3, m4, m5, m6, m7, m8, m9,
    p1, p2, p3, p4, p5, p6, p7, p8, p9,
    s1, s2, s3, s4, s5, s6, s7, s8, s9,
    z1, z2, z3, z4, z5, z6, z7,
};

// game specific dora information
struct GameInfo{
    int doraTileId; // only one dora is available in this setting
    bool red5m;
    bool red5p;
    bool red5s;
    int prevailingWind;
    int seatWind;
} gameInfo;

void initGameStatus(std::string_view doraIndicator, Tiles &akadoraIndicators, std::string_view windIndicator, std::string_view seatWindIndicator){
    gameInfo.prevailingWind = Tiles::tileIndex(windIndicator);
    gameInfo.seatWind = Tiles::tileIndex(seatWindIndicator);
    gameInfo.doraTileId = 0;
    gameInfo.red5m = false;
    gameInfo.red5p = false;
    gameInfo.red5s = false; 
    if (akadoraIndicators.getCount(m5) == 1){
        gameInfo.red5m = true;
    }
    else if (akadoraIndicators.getCount(p5) == 1){
        gameInfo.red5p = true;
    }
    else if (akadoraIndicators.getCount(s5) == 1){
        gameInfo.red5s = true;
    }
    int doraIndicatorTileId = Tiles::tileIndex(doraIndicator);
    int suit = Tiles::fieldIndex[doraIndicatorTileId];
    int rank = doraIndicatorTileId - (suit * 9);
    if (suit < 3) {
        rank = (rank + 1) % 9;
        gameInfo.doraTileId = (suit * 9) + rank;
    } else {
        if (rank < 4) {
            rank = (rank + 1) % 4;
        } else {
            rank = 4 + ((rank - 4 + 1) % 3);
        }
        gameInfo.doraTileId = 27 + rank;
    }
}

class WinningHand{
public:
    std::array<int, 7> tileIds;
    std::array<bool, 4> groupTypes; // 0 -> triplets, 1 -> sequences
    int pairTileId = 0;
    bool isChiitoitsu = false;
    int waitGroup = -1;
    int waitTileId = -1;

    int totalHan = 0;
    std::string yakuString = "";

    WinningHand() = default;

    Tiles toTiles() const {
        Tiles t;
        if (isChiitoitsu){
            for (int i = 0; i < 7; i++){
                t.add(tileIds[i], 2);
            }
        }
        else{
            for (int i = 0; i < 4; i++){
                if (groupTypes[i]){
                    t.add(tileIds[i], 3);
                }
                else{
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
        winds();
        dora();
        tanyao();
        pinfu();
        lipeikou();
        chitoitsu();
        toitoi();
        sanshoku();
        ittsuu();
        hon_chinitsu();
        sanankou();
        shousangen();
        ryanpeikou();
        hunroutou();
        taiyao();
        // local yakus
        reversibleTiles();
        tripleRedTiles();
        uumensai();
        sanshoku_tsuukan();
        sanrenkou();
        isshokuSanjun();
    }

    

    void calculateYakuman() {
        daisangen();
        suushi();
        tsuuiisou();
        ryuuiisou();
        chinroutou();
        chuuren();
        suuankouTanki();
        // local yakuman
        suurenkou();
        isshokuSanjun();
        isshokuYonjun();
        benikujaku();
        kouitten();
        kokuiisou();
        daiXrin();
    }

    void winds() {
        if (isChiitoitsu){
            return;
        }
        else{
            for (int i = 0; i < 4; i++){
                if (groupTypes[i] == 0 && (tileIds[i] == gameInfo.prevailingWind || tileIds[i] == gameInfo.seatWind)){
                    totalHan += 1;
                    if (tileIds[i] == gameInfo.prevailingWind){
                        yakuString += "场风 ";
                    }
                    else if (tileIds[i] == gameInfo.seatWind){
                        yakuString += "自风 ";
                    }
                }
            }
        }
    }
    
    void dora() {
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
            totalHan += doraCount;
            yakuString += "宝牌" + std::to_string(doraCount) + " ";
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
            totalHan += akaDoraCount;
            yakuString += "赤宝牌" + std::to_string(akaDoraCount) + " ";
        }
    }

    void tanyao() {
        bool isTanyao = true;
        for (auto tileId : fullTiles()){
            if (Tiles::isYaochu(tileId)){
                isTanyao = false;
                break;
            }
        }
        if (isTanyao){
            totalHan += 1;
            yakuString += "断幺 ";
        }
    }

    void pinfu() {
        if (isChiitoitsu){
            return;
        }
        bool isPinfu = true;
        for (int i = 0; i < 4; i++){
            if (groupTypes[i] == 0){
                isPinfu = false;
                break;
            }
        }
        if (isPinfu){
            bool leftWait = waitTileId == tileIds[waitGroup] && (Tiles::fieldIndex[waitTileId] == Tiles::fieldIndex[waitTileId + 3]);
            bool rightWait = waitTileId == tileIds[waitGroup] + 2 && (Tiles::fieldIndex[waitTileId] == Tiles::fieldIndex[waitTileId - 3]);
            if (!leftWait && !rightWait){
                isPinfu = false;
            }
        }
        if (isPinfu){
            totalHan += 1;
            yakuString += "平和 ";
        }
    }

    void lipeikou() {
        if (isChiitoitsu){
            return;
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
            totalHan += 1;
            yakuString += "一杯口 ";
        }
    }

    void chitoitsu() {
        if (isChiitoitsu){
            totalHan += 2;
            yakuString += "七对子 ";
        }
    }

    void toitoi() {
        if (isChiitoitsu){
            return;
        }
        if (groupTypes[0] == 0 && groupTypes[1] == 0 && groupTypes[2] == 0 && groupTypes[3] == 0){
            totalHan += 2;
            yakuString += "对对和 ";
        }
    }

    void sanshoku() {
        if (isChiitoitsu){
            return;
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
            totalHan += 2;
            if (sameGroupType == 0){
                yakuString += "三色同刻 ";
            }
            else{
                yakuString += "三色同顺 ";
            }
        }
    }

    bool _ittsuu_helper() const {
        if (isChiitoitsu){
            return false;
        }
        std::array<int, 4> counts;
        for (int i = 0; i < 4; i++){
            counts[Tiles::fieldIndex[tileIds[i]]]++;
        }
        int group = -1;
        if (counts[0] >= 3){
            group = 0;
        }
        else if (counts[1] >= 3){
            group = 1;
        }
        else if (counts[2] >= 3){
            group = 2;
        }
        else{
            return false;
        }
        bool seq1 = false, seq2 = false, seq3 = false;
        for (int i = 0; i < 4; i++){
            if (Tiles::fieldIndex[tileIds[i]] == group){
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

    void ittsuu() {
        if (_ittsuu_helper()){
            totalHan += 2;
            yakuString += "一气通贯 ";
        }
    }

    void hon_chinitsu() {
        std::array<int, 4> counts;
        int han = 0;
        for (auto tileId : fullTiles()){
            counts[Tiles::fieldIndex[tileId]]++;
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
            yakuString += "清一色 ";
        }else if (han == 3){
            yakuString += "混一色 ";
        }
        totalHan += han;
    }

    void sanankou() {
        int ankou_count = 0;
        for (int i = 0; i < 4; i++){
            if (groupTypes[i] == 0 && waitGroup != i){
                ankou_count++;
            }
        }
        if (ankou_count >= 3){
            totalHan += 2;
            yakuString += "三暗刻 ";
        }
    }

    void shousangen() {
        if (isChiitoitsu){
            return;
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
            totalHan += 2;
            yakuString += "小三元 ";
        }
    }

    void ryanpeikou() {
        if (isChiitoitsu){
            return;
        }
        if (groupTypes[0] == 1 && groupTypes[1] == 1 && groupTypes[2] == 1 && groupTypes[3] == 1 && tileIds[0] == tileIds[1] && tileIds[2] == tileIds[3]){
            totalHan += 2;
            yakuString += "两杯口 ";
        }
    }

    void hunroutou() {
        bool isHunroutou = true;
        for (auto tileId : fullTiles()){
            if (!Tiles::isYaochu(tileId)){
                isHunroutou = false;
                break;
            }
        }
        if (isHunroutou){
            totalHan += 2;
            yakuString += "混老头 ";
        }
    }

    void taiyao() {
        if (isChiitoitsu){
            return;
        }
        bool isChanta = true;
        bool isJunchan = true;
        for (int i = 0; i < 4; i++){
            if (groupTypes[i] == 0){
                if (!Tiles::isYaochu(tileIds[i])){
                    isChanta = false;
                    isJunchan = false;
                }else{
                    if (Tiles::fieldIndex[tileIds[i]] == 3){
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
            if (Tiles::fieldIndex[pairTileId] == 3){
                isJunchan = false;
            }
        }
        if (isJunchan){
            totalHan += 3;
            yakuString += "纯全 ";
            return;
        }
        if (isChanta){
            totalHan += 2;
            yakuString += "全带 ";
        }
    }

    // yakuman
    void daisangen() {
        if (isChiitoitsu){
            return;
        }
        int count = 0;  
        for (int i = 0; i < 4; i++){
            if (tileIds[i] == z5 || tileIds[i] == z6 || tileIds[i] == z7){
                count++;
            }
        }
        if (count == 3){
            totalHan += 10000;
            yakuString += "大三元 ";
        }
    }

    void suushi() {
        if (isChiitoitsu){
            return;
        }
        int suushiCount = 0;
        for (auto tileId : fullTiles()){
            if (tileId == z1 || tileId == z2 || tileId == z3 || tileId == z4){
                suushiCount++;
            }
        }
        if (suushiCount == 12){ // dai suushi
            totalHan += 20000;
            yakuString += "大四喜 ";
        }
        if (suushiCount == 11){ // shou suushi
            totalHan += 10000;
            yakuString += "小四喜 ";
        }
    }

    void tsuuiisou() {
        bool isTsuuiisou = true;
        for (auto tileId : fullTiles()){
            if (!(tileId == z1 || tileId == z2 || tileId == z3 || tileId == z4 || tileId == z5 || tileId == z6 || tileId == z7)){
                isTsuuiisou = false;
            }
        }
        if (isTsuuiisou){
            totalHan += 10000;
            yakuString += "字一色 ";
        }
    }

    void ryuuiisou() {
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
            totalHan += 20000;
            yakuString += "无发绿一色 ";
        }
        if (isRyuuiisou){
            totalHan += 10000;
            yakuString += "绿一色 ";
        }
    }

    void chinroutou() {
        bool isChinroutou = true;
        for (auto tileId : fullTiles()){
            if (!(tileId == m1 || tileId == m9 || tileId == p1 || tileId == p9 || tileId == s1 || tileId == s9)){
                isChinroutou = false;
            }
        }
        if (isChinroutou){
            totalHan += 10000;
            yakuString += "清老头 ";
        }
    }

    void chuuren() {
        if (isChiitoitsu){
            return;
        }
        bool isChuuren = false;
        bool isChuuren9wait = false;
        int inferredSuit = Tiles::fieldIndex[tileIds[0]];
        std::array<int, 9> counts;
        for (auto tileId : fullTiles()){
            if (Tiles::fieldIndex[tileId] != inferredSuit){
                return;
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
            totalHan += 20000;
            yakuString += "纯正九莲宝灯 ";
            return;
        }
        if (isChuuren){
            totalHan += 10000;
            yakuString += "九莲宝灯 ";
        }
    }

    void suuankouTanki() {
        if (isChiitoitsu){
            return;
        }
        if (groupTypes[0] == 0 && groupTypes[1] == 0 && groupTypes[2] == 0 && groupTypes[3] == 0 && waitGroup == 4){
            totalHan += 20000;
            yakuString += "四暗刻单骑 ";
            return;
        }
    }

    // local yakus
    void reversibleTiles() {
        bool isReversible = true;
        for (auto tileId : fullTiles()){
            if (tileId == p1 || tileId == p2 || tileId == p3 || tileId == p4 || tileId == p5 || tileId == p8 || tileId == p9 || tileId == s2 || tileId == s4 || tileId == s6 || tileId == s8 || tileId == s9 || tileId == 5z){
                isReversible = false;
            }
        }
        if (isReversible){
            totalHan += 1;
            yakuString += "推不倒 ";
        }
    }

    void tripleRedTiles() {
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
            totalHan += 2;
            yakuString += "赤三色 ";
        }
    }

    void uumensai() {
        bool m = false, p = false, s = false, w = false, d = false;
        for (auto tileId : fullTiles()){
            if (Tiles::fieldIndex[tileId] == 0){
                m = true;
            }
            if (Tiles::fieldIndex[tileId] == 1){
                p = true;
            }
            if (Tiles::fieldIndex[tileId] == 2){
                s = true;
            }
            if (Tiles::fieldIndex[tileId] == 3){
                if (tileId == z1 || tileId == z2 || tileId == z3 || tileId == z4){
                    w = true;
                }
                if (tileId == z5 || tileId == z6 || tileId == z7){
                    d = true;
                }
            }
        }
        if (m && p && s && w && d){
            totalHan += 2;
            yakuString += "五门齐 ";
        }
    }

    bool _sanshouku_tsuukan_helper(int tileId1, int tileId2, int tileId3) const {
        std::array<int, 3> numCounts;
        std::array<int, 3> suitCounts;
        numCounts[(tileId1 % 9)/3]++;
        numCounts[(tileId2 % 9)/3]++;
        numCounts[(tileId3 % 9)/3]++;
        suitCounts[Tiles::fieldIndex[tileId1]]++;
        suitCounts[Tiles::fieldIndex[tileId2]]++;
        suitCounts[Tiles::fieldIndex[tileId3]]++;
        if (numCounts[0] == 1 && numCounts[1] == 1 && numCounts[2] == 1){
            if (suitCounts[0] == 1 && suitCounts[1] == 1 && suitCounts[2] == 1){
                return true;
            }
        }
        return false;
    }

    void sanshoku_tsuukan() {
        if (isChiitoitsu){
            return;
        }
        std::array<int, 4> counts;
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
            totalHan += 2;
            yakuString += "三色通贯 ";
        }
    }

    bool _sanrenkou_helper(int tileId1, int tileId2, int tileId3) const {
        if (Tiles::fieldIndex[tileId1] == Tiles::fieldIndex[tileId2] && Tiles::fieldIndex[tileId2] == Tiles::fieldIndex[tileId3] && tileId1 + 1 == tileId2 && tileId2 + 1 == tileId3 && tileId3 < z1){
            return true;
        }
        return false;
    }

    void sanrenkou() {
        if (isChiitoitsu){
            return;
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
            totalHan += 2;
            yakuString += "三连刻 ";
        }
    }

    void isshokuSanjun() {
        if (isChiitoitsu){
            return;
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
            totalHan += 2;
            yakuString += "一色三同顺 ";
        }
    }

    // local yakuman
    void suurenkou() {
        if (isChiitoitsu){
            return;
        }
        if (groupTypes[0] == 0 && groupTypes[1] == 0 && groupTypes[2] == 0 && groupTypes[3] == 0 && tileIds[0] == tileIds[1] && tileIds[1] == tileIds[2] && tileIds[2] == tileIds[3]){  
            totalHan += 10000;
            yakuString += "四连刻 ";
        }
    }

    void isshokuYonjun() {
        if (isChiitoitsu){
            return;
        }
        if (groupTypes[0] == 1 && groupTypes[1] == 1 && groupTypes[2] == 1 && groupTypes[3] == 1 && tileIds[0] == tileIds[1] && tileIds[1] == tileIds[2] && tileIds[2] == tileIds[3]){  
            totalHan += 10000;
            yakuString += "一色四顺 ";
        }
    }

    void benikujaku() {
        if (isChiitoitsu){
            return;
        }
        if (groupTypes[0] == 0 && groupTypes[1] == 0 && groupTypes[2] == 0 && groupTypes[3] == 0 && tileIds[0] == s1 && tileIds[1] == s5 && tileIds[2] == s7 && tileIds[3] == s9 && pairTileId == z7){  
            totalHan += 10000;
            yakuString += "红孔雀 ";
        }
    }

    void kouitten() {
        if (isChiitoitsu){
            return;
        }
        if (groupTypes[0] == 0 && groupTypes[1] == 0 && groupTypes[2] == 0 && groupTypes[3] == 0 && tileIds[0] == s2 && tileIds[1] == s3 && tileIds[2] == s4 && tileIds[3] == s6 && pairTileId == z7){  
            totalHan += 10000;
            yakuString += "红一点 ";
        }
    }

    void kokuiisou() {
        bool isKokuiisou = true;
        for (auto tileId : fullTiles()){
            if (!(tileId == p2 || tileId == p4 || tileId == p8 || tileId == z1 || tileId == z2 || tileId == z3 || tileId == z4)){
                isKokuiisou = false;
            }
        }
        if (isKokuiisou){
            totalHan += 10000;
            yakuString += "黑一色 ";
        }
    }

    void daiXrin() {
        if (isChiitoitsu){
            int inferredSuit = Tiles::fieldIndex[tileIds[0]];
            for (auto tileId : fullTiles()){
                if (Tiles::fieldIndex[tileId] != inferredSuit){
                    return;
                }
            }
            if (tileIds[0] == 1 && tileIds[1] == 2 && tileIds[2] == 3 && tileIds[3] == 4 && tileIds[4] == 5 && tileIds[5] == 6 && tileIds[6] == 7){
                totalHan += 10000;
                if (inferredSuit == 0){
                    yakuString += "大数邻 ";
                }else if (inferredSuit == 1){
                    yakuString += "大车轮 ";
                }else if (inferredSuit == 2){
                    yakuString += "大竹林 ";
                }
            }
        }
    }

    void hyakumannGoku() {
        int sum = 0;
        for (auto tileId : fullTiles()){
            if (Tiles::fieldIndex[tileId] != 0){
                return;
            }
            sum += (tileId+1);
        }
        if (sum >= 100){
            totalHan += 10000;
            yakuString += "百万石 ";
        }
    }

    void goldenGateBridge() {
        if (isChiitoitsu){
            return;
        }
        if (Tiles::fieldIndex[tileIds[0]] == Tiles::fieldIndex[tileIds[1]] && Tiles::fieldIndex[tileIds[1]] == Tiles::fieldIndex[tileIds[2]] && Tiles::fieldIndex[tileIds[2]] == Tiles::fieldIndex[tileIds[3]]){  
            if (tileIds[0] % 9 == 0 && tileIds[1] % 9 == 2 && tileIds[2] % 9 == 4 && tileIds[3] % 9 == 6){
                totalHan += 10000;
                yakuString += "金门桥 ";
            }
        }
    }

    void touhokuShinkansen() {
        if (isChiitoitsu){
            return;
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
                totalHan += 10000;
                yakuString += "东北新干线 ";
            }
        }
    }

    void daichisei() {
        if (isChiitoitsu){
            for (auto tileId : fullTiles()){
                if (tileId < z1){
                    return;
                }
            }
            totalHan += 20000;
            yakuString += "大七星 ";
        }
    } 
};


void test_basic_tiles(){
    Tiles allHand;
    std::string doraIndicator;
    Tiles akadoraIndicators;
    allHand = Tiles("123456789m123456789p123456789s123456789z");
    doraIndicator = "4z";
    akadoraIndicators = Tiles("5m5s");
    Tiles customHand = Tiles();
    // customHand.add(33);
    // customHand.add(33, 2);
    // customHand.add("2m");
    // // customHand.add("2m", 3);
    // customHand.add("2m", 4);
    customHand.remove("2m", 1);
    customHand.add(10);
    customHand.add(0);

    std::unordered_map<Tiles, int, Tiles::Hash> M;
    M.reserve(16);
    M[allHand] = 1;
    M[customHand] = 3;
    M[akadoraIndicators] = 4;
    std::cout << "Full hand size:   " << allHand.totalTiles() << std::endl;
    std::cout << "All hand: " << allHand.toString() << std::endl;
    std::cout << "Dora indicator: " << doraIndicator << std::endl;
    std::cout << "Akadora indicators: " << akadoraIndicators.toString() << std::endl;
    for (auto const &[k, v] : M)
    {
        std::cout << "Key: " << k.toString() << ", Value: " << v << "\n";
    }
    
}


int main(int argc, char *argv[]){
    test_basic_tiles();
    if (argc != 6)
    {
        std::cerr << "Usage: " << argv[0]
                  << " <tile_collection> <dora_indicator> <akadora_indicators> <wind_indicator> <seat_wind_indicator>\n";
        return 1;
    }

    Tiles allHand;
    std::string doraIndicator;
    Tiles akadoraIndicators;
    std::string windIndicator;
    std::string seatWindIndicator;

    try
    {
        allHand = Tiles(argv[1]);
        doraIndicator = argv[2];
        akadoraIndicators = Tiles(argv[3]);
        windIndicator = argv[4];
        seatWindIndicator = argv[5];
        initGameStatus(doraIndicator, akadoraIndicators, windIndicator, seatWindIndicator);
    }
    catch (std::exception const &e)
    {
        std::cerr << "Parsing error: " << e.what() << "\n";
        return 1;
    }

    std::unordered_map<Tiles, int, Tiles::Hash> M;

    // core logic:
    // 
    return 0;
}
