#pragma once
#include "tiles.h"
#include "winning_hand.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <algorithm>

struct WaitEntry {
    int waitTile = -1;    // tile needed to win
    int han = 0;          // total han (currently not calculated)
    int fu = 0;           // fu value (placeholder)
    std::string yaku;     // yaku description (placeholder)
};

using ResultMap = std::unordered_map<Tiles, std::vector<WaitEntry>, Tiles::Hash>;

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

// Evaluate a 13-tile partial hand by checking each possible winning tile.
inline void evaluateHand(const Tiles &hand, ResultMap &res) {
    for(int tile=0; tile<34; ++tile) {
        Tiles tmp = hand;
        tmp.add(tile);
        if (isSevenPairs(tmp) || isStandardHand(tmp)) {
            WaitEntry we{tile,0,0,""};
            auto &vec = res[hand];
            bool exists=false;
            for(auto &v:vec) if(v.waitTile==tile){ exists=true; break; }
            if(!exists) vec.push_back(we);
        }
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

// Print results sorted by han (desc) then fu.
inline void printResults(const ResultMap &res) {
    struct Item { Tiles hand; WaitEntry entry; };
    std::vector<Item> all;
    for(auto const &p:res) {
        for(auto const &w:p.second) all.push_back({p.first,w});
    }
    std::sort(all.begin(), all.end(), [](const Item&a,const Item&b){
        if(a.entry.han!=b.entry.han) return a.entry.han>b.entry.han;
        return a.entry.fu>b.entry.fu;
    });
    for(auto const &it:all) {
        std::cout << it.hand.toString() << " waiting " << tileToString(it.entry.waitTile)
                  << " han " << it.entry.han << " fu " << it.entry.fu << " " << it.entry.yaku
                  << "\n";
    }
}

