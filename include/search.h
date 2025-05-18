#pragma once
#include "tiles.h"
#include "winning_hand.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <algorithm>

struct WaitEntry {
    int waitTile = -1;    // tile needed to win
    int han = 0;          // total han
    std::string yaku;     // yaku description
};

using WaitMap = std::unordered_map<int, WaitEntry>;
using ResultMap = std::unordered_map<Tiles, WaitMap, Tiles::Hash>;

// Convert tile id to simple string like "1m" or "5z".
inline std::string tileToString(int tileId) {
    static const char suits[] = {'m','p','s','z'};
    int suit = Tiles::suitIndex[tileId];
    int rank = tileId - suit * 9 + 1;
    return std::to_string(rank) + suits[suit];
}

// Check if tiles form seven pairs hand.
inline bool isSevenPairs(const Tiles &t) {
    for (int i=0;i<34;++i) {
        int c = t.getCount(i);
        if (c==0) continue;
        if (c!=2) return false;
    }
    return t.totalTiles()==14;
}

// Helper to check if remaining tiles can be formed into melds.
inline bool dfsMelds(std::array<int,34> &cnt, bool pairUsed) {
    int first = -1;
    for (int i=0;i<34;++i) if (cnt[i]>0) { first=i; break; }
    if (first==-1) return pairUsed; // all tiles used
    if (!pairUsed && cnt[first]>=2) {
        cnt[first]-=2;
        if (dfsMelds(cnt,true)) { cnt[first]+=2; return true; }
        cnt[first]+=2;
    }
    if (cnt[first]>=3) {
        cnt[first]-=3;
        if (dfsMelds(cnt,pairUsed)) { cnt[first]+=3; return true; }
        cnt[first]+=3;
    }
    if (Tiles::suitIndex[first]<3 && first%9<=6 && cnt[first+1]>0 && cnt[first+2]>0) {
        cnt[first]--; cnt[first+1]--; cnt[first+2]--;
        if (dfsMelds(cnt,pairUsed)) { cnt[first]++; cnt[first+1]++; cnt[first+2]++; return true; }
        cnt[first]++; cnt[first+1]++; cnt[first+2]++;
    }
    return false;
}

// Check if tiles form standard hand (4 melds + pair)
inline bool isStandardHand(const Tiles &t) {
    if (t.totalTiles()!=14) return false;
    std::array<int,34> cnt{};
    for(int i=0;i<34;++i) cnt[i]=t.getCount(i);
    return dfsMelds(cnt,false);
}

// Build WinningHand representation from tiles. Returns false if no hand found.
inline bool buildStandardRec(std::array<int,34> &cnt, int depth, bool pairUsed,
                             bool usedWin, int winTile, WinningHand &out) {
    int first=-1;
    for(int i=0;i<34;++i) if(cnt[i]>0){ first=i; break; }
    if(first==-1)
        return pairUsed && usedWin && depth==4;

    if(!pairUsed && cnt[first]>=2) {
        cnt[first]-=2;
        int old=out.waitMeldIndex;
        out.pairTileId=first;
        bool use=false;
        if(!usedWin && first==winTile){ use=true; out.waitMeldIndex=4; }
        if(buildStandardRec(cnt, depth, true, usedWin||use, winTile, out)) return true;
        cnt[first]+=2;
        if(use) out.waitMeldIndex=old;
    }

    if(cnt[first]>=3) {
        cnt[first]-=3;
        out.tileIds[depth]=first;
        out.groupTypes[depth]=0;
        int old=out.waitMeldIndex;
        bool use=false;
        if(!usedWin && first==winTile){ use=true; out.waitMeldIndex=depth; }
        if(buildStandardRec(cnt, depth+1, pairUsed, usedWin||use, winTile, out)) return true;
        cnt[first]+=3;
        if(use) out.waitMeldIndex=old;
    }

    if(Tiles::suitIndex[first]<3 && first%9<=6 && cnt[first+1]>0 && cnt[first+2]>0) {
        cnt[first]--; cnt[first+1]--; cnt[first+2]--;
        out.tileIds[depth]=first;
        out.groupTypes[depth]=1;
        int old=out.waitMeldIndex;
        bool use=false;
        if(!usedWin && (first==winTile || first+1==winTile || first+2==winTile))
            { use=true; out.waitMeldIndex=depth; }
        if(buildStandardRec(cnt, depth+1, pairUsed, usedWin||use, winTile, out)) return true;
        cnt[first]++; cnt[first+1]++; cnt[first+2]++;
        if(use) out.waitMeldIndex=old;
    }
    return false;
}

inline bool buildWinningHand(const Tiles &t, int winTile, WinningHand &out) {
    if(isSevenPairs(t)) {
        out = WinningHand{};
        out.isChiitoitsu=true;
        int idx=0;
        for(int i=0;i<34;++i) if(t.getCount(i)==2) out.tileIds[idx++]=i;
        out.waitTileId=winTile;
        out.waitMeldIndex=-1;
        return true;
    }
    if(isStandardHand(t)) {
        out = WinningHand{};
        out.isChiitoitsu=false;
        std::array<int,34> cnt{};
        for(int i=0;i<34;++i) cnt[i]=t.getCount(i);
        out.waitTileId=winTile;
        out.waitMeldIndex=-1;
        if(buildStandardRec(cnt,0,false,false,winTile,out)) return true;
    }
    return false;
}

// Check Kokushi and Kokushi Jusanmen. Returns true if hand+winTile forms
// Kokushi; fills WaitEntry accordingly.
inline bool checkKokushi(const Tiles &hand, int winTile, WaitEntry &out) {
    static const int yaochu[13] = {m1,m9,p1,p9,s1,s9,z1,z2,z3,z4,z5,z6,z7};
    Tiles tmp = hand;
    if(!tmp.add(winTile)) return false;
    if(tmp.totalTiles()!=14) return false;
    int pairCount = 0;
    for(int i=0;i<34;++i) {
        int c = tmp.getCount(i);
        if(c==0) continue;
        if(!Tiles::isYaochu(i)) return false;
        if(c>2) return false;
        if(c==2) pairCount++;
    }
    if(pairCount!=1) return false;

    bool thirteenUnique=true;
    for(int id:yaochu){
        int c = hand.getCount(id);
        if(c!=1) { thirteenUnique=false; break; }
    }

    out.waitTile = winTile;
    if(thirteenUnique){
        out.han = 20000;
        out.yaku = "国士無双十三面待ち";
    } else {
        out.han = 10000;
        out.yaku = "国士無双";
    }
    return true;
}

// Evaluate a 13-tile partial hand by checking each possible winning tile.
inline void evaluateHand(const Tiles &hand, ResultMap &res) {
    for(int tile=0; tile<34; ++tile) {
        WaitEntry we{};
        bool kokushi = checkKokushi(hand, tile, we);
        Tiles tmp = hand;
        if(!tmp.add(tile)) continue;
        if(!kokushi) {
            WinningHand wh;
            if(!buildWinningHand(tmp, tile, wh)) continue;
            wh.calculateHan();
            we.waitTile = tile;
            we.han = wh.totalHan;
            we.yaku = wh.yakuString;
        }
        auto &mp = res[hand];
        if(mp.find(tile)==mp.end()) mp[tile]=we;
    }
}

// Recursively enumerate all 13-tile combinations from superset.
inline void searchRec(int tileId, int remaining, Tiles &cur, const Tiles &superset, ResultMap &res) {
    if (remaining==0) {
        evaluateHand(cur,res);
        return;
    }
    if (tileId>=34) return;
    int maxCnt = std::min(superset.getCount(tileId), remaining);
    for(int k=0;k<=maxCnt;++k) {
        if(k>0) cur.add(tileId,k);
        searchRec(tileId+1, remaining-k, cur, superset, res);
        if(k>0) cur.remove(tileId,k);
    }
}

// Entry point to search all waits from a superset.
inline void searchHands(const Tiles &superset, ResultMap &res) {
    Tiles cur;
    searchRec(0,13,cur,superset,res);
}

// Print results sorted by han descending.
inline void printResults(const ResultMap &res) {
    struct Item { Tiles hand; WaitEntry entry; };
    std::vector<Item> all;
    for(auto const &p:res) {
        for(auto const &kv:p.second) all.push_back({p.first, kv.second});
    }
    std::sort(all.begin(), all.end(), [](const Item&a,const Item&b){
        return a.entry.han>b.entry.han;
    });
    for(auto const &it:all) {
        std::cout << it.hand.toString() << " waiting " << tileToString(it.entry.waitTile)
                  << " han " << it.entry.han << " " << it.entry.yaku
                  << "\n";
    }
}

