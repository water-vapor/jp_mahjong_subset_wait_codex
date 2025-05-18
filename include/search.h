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

// ----- Targeted generation based searching -----

struct GroupSpec {
    int tile;
    bool sequence; // true if sequence starting at tile, false if triplet of tile
};

inline const std::vector<GroupSpec> &orderedGroups() {
    static std::vector<GroupSpec> groups;
    if (!groups.empty()) return groups;
    for (int suit = 0; suit < 3; ++suit) {
        for (int r = 0; r < 9; ++r) {
            int base = suit * 9 + r;
            groups.push_back({base, false});
            if (r <= 6) groups.push_back({base, true});
        }
    }
    for (int id = 27; id < 34; ++id) groups.push_back({id, false});
    return groups;
}

inline bool canUse(const std::array<int,34> &cnt, const GroupSpec &g) {
    if (g.sequence) {
        if (Tiles::suitIndex[g.tile] >= 3 || g.tile % 9 > 6) return false;
        return cnt[g.tile] > 0 && cnt[g.tile+1] > 0 && cnt[g.tile+2] > 0;
    } else {
        return cnt[g.tile] >= 3;
    }
}

inline void applyGroup(std::array<int,34> &cnt, const GroupSpec &g, int delta) {
    if (g.sequence) {
        cnt[g.tile] += delta;
        cnt[g.tile+1] += delta;
        cnt[g.tile+2] += delta;
    } else {
        cnt[g.tile] += 3 * delta;
    }
}

inline Tiles buildTiles(const std::array<int,34> &cnt) {
    Tiles t;
    for (int i = 0; i < 34; ++i) if (cnt[i]) t.add(i, cnt[i]);
    return t;
}

inline void dfsGroups(int start, int depth, std::array<int,34> &cnt,
                      std::array<int,34> &used, WinningHand &cur,
                      int winTile, bool usedWin, ResultMap &res) {
    if (depth == 4) {
        if (!usedWin) return;
        Tiles full = buildTiles(used);
        WinningHand out = cur;
        out.waitTileId = winTile;
        out.calculateHan();
        WaitEntry we{winTile, out.totalHan, out.yakuString};
        Tiles partial = full;
        partial.remove(winTile);
        auto &mp = res[partial];
        if (mp.find(winTile) == mp.end()) mp[winTile] = we;
        return;
    }
    const auto &groups = orderedGroups();
    for (size_t i = start; i < groups.size(); ++i) {
        const auto &g = groups[i];
        if (!canUse(cnt, g)) continue;
        applyGroup(cnt, g, -1);
        if (g.sequence) {
            used[g.tile]++;
            used[g.tile+1]++;
            used[g.tile+2]++;
        } else {
            used[g.tile] += 3;
        }
        int old = cur.waitMeldIndex;
        bool use = usedWin;
        if (!usedWin) {
            if (!g.sequence && g.tile == winTile) { use = true; cur.waitMeldIndex = depth; }
            if (g.sequence && (winTile == g.tile || winTile == g.tile+1 || winTile == g.tile+2)) {
                use = true; cur.waitMeldIndex = depth; }
        }
        cur.tileIds[depth] = g.tile;
        cur.groupTypes[depth] = g.sequence;
        dfsGroups(i, depth + 1, cnt, used, cur, winTile, use, res);
        cur.waitMeldIndex = old;
        if (g.sequence) {
            used[g.tile]--;
            used[g.tile+1]--;
            used[g.tile+2]--;
        } else {
            used[g.tile] -= 3;
        }
        applyGroup(cnt, g, 1);
    }
}

inline void searchStandardForTile(const Tiles &superset, int winTile, ResultMap &res) {
    std::array<int,34> cnt{};
    for (int i = 0; i < 34; ++i) cnt[i] = superset.getCount(i);
    if (cnt[winTile] >= 4) return;
    cnt[winTile]++;
    std::array<int,34> used{};
    for (int pair = 0; pair < 34; ++pair) {
        if (cnt[pair] < 2) continue;
        cnt[pair] -= 2;
        used[pair] += 2;
        WinningHand cur{};
        cur.isChiitoitsu = false;
        cur.pairTileId = pair;
        cur.waitMeldIndex = (pair == winTile) ? 4 : -1;
        bool usedWin = pair == winTile;
        dfsGroups(0, 0, cnt, used, cur, winTile, usedWin, res);
        used[pair] -= 2;
        cnt[pair] += 2;
    }
}

inline void dfsSevenPairs(int start, int depth, std::array<int,34> &cnt,
                          std::array<int,34> &used, WinningHand &cur,
                          int winTile, ResultMap &res) {
    if (depth == 7) {
        if (used[winTile] == 0) return;
        Tiles full = buildTiles(used);
        WinningHand out = cur;
        out.waitTileId = winTile;
        out.isChiitoitsu = true;
        out.waitMeldIndex = -1;
        out.calculateHan();
        WaitEntry we{winTile, out.totalHan, out.yakuString};
        Tiles partial = full;
        partial.remove(winTile);
        auto &mp = res[partial];
        if (mp.find(winTile) == mp.end()) mp[winTile] = we;
        return;
    }
    for (int i = start; i < 34; ++i) {
        if (cnt[i] < 2) continue;
        cnt[i] -= 2;
        used[i] += 2;
        cur.tileIds[depth] = i;
        dfsSevenPairs(i + 1, depth + 1, cnt, used, cur, winTile, res);
        used[i] -= 2;
        cnt[i] += 2;
    }
}

inline void searchSevenPairsForTile(const Tiles &superset, int winTile, ResultMap &res) {
    std::array<int,34> cnt{};
    for (int i = 0; i < 34; ++i) cnt[i] = superset.getCount(i);
    if (cnt[winTile] >= 4) return;
    cnt[winTile]++;
    std::array<int,34> used{};
    WinningHand cur{};
    dfsSevenPairs(0, 0, cnt, used, cur, winTile, res);
}

inline void searchKokushi(const Tiles &superset, ResultMap &res) {
    std::array<int,34> cnt{};
    for (int i = 0; i < 34; ++i) cnt[i] = superset.getCount(i);
    constexpr int yaochu[13] = {m1, m9, p1, p9, s1, s9, z1, z2, z3, z4, z5, z6, z7};

    int present = 0;
    for (int t : yaochu) if (cnt[t] > 0) ++present;
    bool allPresent = present == 13;
    if (present < 12) return;

    if (allPresent) {
        std::array<int,34> need{};
        for (int t : yaochu) need[t] = 1;
        Tiles partial = buildTiles(need);
        for (int w : yaochu) {
            if (cnt[w] >= 4) continue; // cannot draw a 5th copy
            WaitEntry we{w, 20000, "国士无双十三面"};
            auto &mp = res[partial];
            if (mp.find(w) == mp.end()) mp[w] = we;
        }
    }

    for (int p : yaochu) {
        if (cnt[p] < 2) continue;
        for (int w : yaochu) {
            if (allPresent && w == p) continue; // handled by 13-sided case
            if (cnt[w] >= 4) continue;
            std::array<int,34> need{};
            for (int t : yaochu) need[t] = 1;
            need[p]++;
            need[w]--;
            bool ok = true;
            for (int t : yaochu) {
                if (need[t] < 0 || need[t] > cnt[t]) { ok = false; break; }
            }
            if (!ok) continue;
            Tiles partial = buildTiles(need);
            auto &mp = res[partial];
            if (mp.find(w) == mp.end()) mp[w] = WaitEntry{w, 10000, "国士无双"};
        }
    }
}

inline void searchHands(const Tiles &superset, ResultMap &res) {
    searchKokushi(superset, res);
    for (int tile = 0; tile < 34; ++tile) {
        searchSevenPairsForTile(superset, tile, res);
        searchStandardForTile(superset, tile, res);
    }
}

// Print results sorted by han descending. If limit > 0, only the first
// `limit` results in each section are printed.
inline void printResults(const ResultMap &res, size_t limit = 0) {
    struct Item { Tiles hand; WaitEntry entry; };
    std::vector<Item> all;
    for (auto const &p : res) {
        for (auto const &kv : p.second) all.push_back({p.first, kv.second});
    }
    std::sort(all.begin(), all.end(), [](const Item &a, const Item &b) {
        return a.entry.han > b.entry.han;
    });
    size_t printed = 0;
    for (auto const &it : all) {
        if (it.entry.han == 0) continue; // Skip zero-han results
        std::cout << it.hand.toString() << " waiting "
                  << tileToString(it.entry.waitTile) << ' ' << it.entry.han
                  << "番 " << it.entry.yaku << "\n";
        if (limit && ++printed >= limit) break;
    }

    std::cout << "-----\n"; // separator between sorting outputs

    struct CountItem { Tiles hand; const WaitMap *waits; size_t count; };
    std::vector<CountItem> counts;
    for (auto const &p : res) {
        counts.push_back({p.first, &p.second, p.second.size()});
    }
    std::sort(counts.begin(), counts.end(), [](const CountItem &a, const CountItem &b) {
        return a.count > b.count;
    });
    printed = 0;
    for (auto const &c : counts) {
        std::vector<int> tiles;
        tiles.reserve(c.waits->size());
        const WaitEntry *best = nullptr;
        for (auto const &kv : *c.waits) {
            tiles.push_back(kv.first);
            if (!best || kv.second.han > best->han) best = &kv.second;
        }
        if (!best || best->han == 0) continue; // skip configurations with zero-han waits only
        std::sort(tiles.begin(), tiles.end());
        std::cout << c.hand.toString() << ' ';
        for (size_t i = 0; i < tiles.size(); ++i) {
            if (i) std::cout << ' ';
            std::cout << tileToString(tiles[i]);
        }
        std::cout << ' ' << c.count << "面待ち";
        std::cout << ' ' << tileToString(best->waitTile) << ' '
                  << best->han << "番 " << best->yaku;
        std::cout << "\n";
        if (limit && ++printed >= limit) break;
    }
}

