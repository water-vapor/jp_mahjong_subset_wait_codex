#pragma once
#include <iostream>
#include <string_view>
#include <array>
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <cctype>
#include <cstdint>

enum Tile{
    m1, m2, m3, m4, m5, m6, m7, m8, m9,
    p1, p2, p3, p4, p5, p6, p7, p8, p9,
    s1, s2, s3, s4, s5, s6, s7, s8, s9,
    z1, z2, z3, z4, z5, z6, z7,
};

struct GameInfo{
    int doraTileId;
    bool red5m;
    bool red5p;
    bool red5s;
    int prevailingWind;
    int seatWind;
} gameInfo;


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
