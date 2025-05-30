#pragma once
#include "tiles.h"
#include "winning_hand.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_set>
#include <iomanip>

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

inline size_t digits(size_t v){
    size_t d = 1;
    while (v >= 10){
        v /= 10;
        ++d;
    }
    return d;
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
    struct HandInfo {
        Tiles hand;
        std::vector<WaitEntry> waits;  // sorted by han desc
        std::vector<int> tiles;        // sorted ascending
    };
    std::vector<HandInfo> infos;
    for (auto const &p : res) {
        HandInfo hi;
        hi.hand = p.first;
        for (auto const &kv : p.second) {
            if (kv.second.han == 0) continue; // ignore zero-han waits entirely
            hi.waits.push_back(kv.second);
            hi.tiles.push_back(kv.first);
        }
        if (hi.waits.empty()) continue;
        std::sort(hi.waits.begin(), hi.waits.end(), [](const WaitEntry &a, const WaitEntry &b){
            return a.han > b.han;
        });
        std::sort(hi.tiles.begin(), hi.tiles.end());
        infos.push_back(std::move(hi));
    }
    std::sort(infos.begin(), infos.end(), [](const HandInfo &a, const HandInfo &b){
        return a.waits[0].han > b.waits[0].han;
    });

    // Pre-compute column widths for aligned output
    size_t handWidth = 0, waitWidth = 0, countWidth = 0, hanWidth = 0;
    std::vector<std::string> waitStrs;
    waitStrs.reserve(infos.size());
    for (auto const &hi : infos) {
        std::string hs = hi.hand.toString();
        handWidth = std::max(handWidth, hs.size());
        std::string ws;
        for (size_t i = 0; i < hi.tiles.size(); ++i) {
            if (i) ws.push_back(' ');
            ws += tileToString(hi.tiles[i]);
        }
        waitWidth = std::max(waitWidth, ws.size());
        countWidth = std::max(countWidth, digits(hi.tiles.size()));
        hanWidth = std::max(hanWidth, digits(size_t(hi.waits[0].han)));
        waitStrs.push_back(std::move(ws));
    }

    size_t printed = 0;
    std::unordered_set<Tiles, Tiles::Hash> printedHands;
    for (size_t idx = 0; idx < infos.size(); ++idx) {
        const auto &hi = infos[idx];
        if (limit && printed >= limit) break;
        std::string handStr = hi.hand.toString();
        const std::string &waitStr = waitStrs[idx];
        std::cout << std::left << std::setw(handWidth) << handStr << ' '
                  << std::left << std::setw(waitWidth) << waitStr << ' '
                  << std::right << std::setw(countWidth) << hi.tiles.size()
                  << "面待ち ";
        const WaitEntry &best = hi.waits[0];
        std::cout << std::left << std::setw(2) << tileToString(best.waitTile) << ' '
                  << std::right << std::setw(hanWidth) << best.han
                  << "番 " << best.yaku << "\n";
        ++printed;
        printedHands.insert(hi.hand);
        // "\xE9\x9D\xA2\xE5\xBE\x85\xE3\x81\xA1 " is three full-width characters
        // ("面待ち") plus a space. These CJK characters typically occupy two
        // columns each when printed, so adjust the indent accordingly.
        std::string indent(handWidth + 1 + waitWidth + 1 + countWidth + 7, ' ');
        for (size_t i = 1; i < hi.waits.size(); ++i) {
            const WaitEntry &we = hi.waits[i];
            std::cout << indent << std::left << std::setw(2)
                      << tileToString(we.waitTile) << ' '
                      << std::right << std::setw(hanWidth) << we.han
                      << "番 " << we.yaku << "\n";
        }
    }

    std::cout << "-----\n"; // separator between sorting outputs

    struct CountItem { Tiles hand; const WaitMap *waits; size_t count; };
    std::vector<CountItem> counts;
    for (auto const &p : res) {
        if (printedHands.count(p.first)) continue;
        counts.push_back({p.first, &p.second, p.second.size()});
    }
    std::sort(counts.begin(), counts.end(), [](const CountItem &a, const CountItem &b) {
        return a.count > b.count;
    });
    // Column widths for the second section
    size_t handWidth2 = 0, waitWidth2 = 0, countWidth2 = 0;
    struct CInfo { const CountItem *item; std::string handStr; std::string waitStr; const WaitEntry *best; std::vector<int> tiles; };
    std::vector<CInfo> cinfo;
    for (auto const &c : counts) {
        CInfo ci; ci.item = &c; ci.handStr = c.hand.toString();
        ci.tiles.reserve(c.waits->size());
        ci.best = nullptr;
        for (auto const &kv : *c.waits) {
            ci.tiles.push_back(kv.first);
            if (!ci.best || kv.second.han > ci.best->han) ci.best = &kv.second;
        }
        if (!ci.best || ci.best->han == 0) continue;
        std::sort(ci.tiles.begin(), ci.tiles.end());
        ci.waitStr.clear();
        for (size_t i = 0; i < ci.tiles.size(); ++i) {
            if (i) ci.waitStr.push_back(' ');
            ci.waitStr += tileToString(ci.tiles[i]);
        }
        handWidth2 = std::max(handWidth2, ci.handStr.size());
        waitWidth2 = std::max(waitWidth2, ci.waitStr.size());
        countWidth2 = std::max(countWidth2, digits(ci.item->count));
        hanWidth = std::max(hanWidth, digits(size_t(ci.best->han)));
        cinfo.push_back(std::move(ci));
    }

    printed = 0;
    for (auto const &ci : cinfo) {
        if (limit && printed >= limit) break;
        std::cout << std::left << std::setw(handWidth2) << ci.handStr << ' '
                  << std::left << std::setw(waitWidth2) << ci.waitStr << ' '
                  << std::right << std::setw(countWidth2) << ci.item->count
                  << "面待ち ";
        std::cout << std::left << std::setw(2) << tileToString(ci.best->waitTile) << ' '
                  << std::right << std::setw(hanWidth) << ci.best->han
                  << "番 " << ci.best->yaku << "\n";
        ++printed;
    }
}

