# Setup
 - This is a C++ project with no external dependencies.
 - Build the main binary with `make`.
 - Run unit tests with `make test`.

# Problem Statement
 - You are playing japanese mahjong in a special setup.
 - You are given a set of 34 (or similar number) mahjong tiles, and you wish to choose a hand of 13 tiles that forms a Wait.
 - Preferably, you want the Han you can obtain to as high as possible, so you need to search for all possibilities, rank them, and then sort by the Han.

# What has been done
 - The basic design of mahjong representation is from Tenhou. The `Tile` class is a general map that counts each tile, while the winning hand stores either 7-pair, or 4 groups and 1 pair in an efficient way.
 - The testing or yaku, yakuman, local yakus are implemented as methods of winning hands.
 - The CLI reading is implemented. It takes the superset (of tiles), dora indicator, aka dora presence in the tiles, and the prevailing and seat winds.
 - Yaku checking functions now return a `YakuResult` struct and are aggregated through `addResult`.
 - Unit tests cover tiles and yaku evaluation. See `tests/test_tiles.cpp`, `tests/test_yaku.cpp` and `tests/test_compound.cpp`.

# What has not been done
 - The result storing map. It should contain the 13-hand as key, and for each value, you need to have a nested map (since there can be multiple waits) containing the tile to wait (perhaps as the key), score and yaku string (as the value). Note that if there are multiple interpretations of the same 14-tile set, they will result in different `WinningHand` instances, but should not duplicate in this map since the keys are identical.
 - The DFS searching logic.
     - In my design, first we should check for Kokushi (13 yao) or Kokushi Jusanmen (13 different + any tile), since it does not form a valid WinningHand in the design, and are easy to check
     - Then check for 7-pairs, we can filter all tiles and only consider these with pairs. Do a DFS on the 6-pair combination, then combine the selected 6-pair with any (different) tile available.
     - Then check for regular 4 group + 1 pair combination. In this case you should consider (a) waiting one tile in the pair, so you just do DFS selection for four groups; (b) waiting for one tile in a group. More considerations are needed here. Generally you can select unique groups by increasing the beginning tile and group type.

 - The final sorting and pretty printing system

# Contributor Guide
1. The project is now split across headers and sources. When adding new files, match the existing naming style and keep code well commented.
2. Unit tests already cover tiles, yaku and compound scenarios. Extend the test suite whenever new logic is introduced.
3. Yaku methods already return a `YakuResult`. Ensure new yaku related code follows this pattern instead of mutating members directly.
4. You should check for duplicated or inelegant code. If there are better and cleaner ways to write it, you can change it.
5. A `.gitignore` file already excludes build artifacts (`main` and test binaries`). Keep it up to date when adding new tools.

# Next Steps
1. **Result storage map** – create a dedicated type (e.g. `ResultMap`) that maps a normalized 13‑tile set to a list of waits. Each wait entry should record the winning tile, total Han, fu and a formatted yaku list. This avoids duplicate keys when different `WinningHand` objects share the same tiles.
2. **Search algorithm** – implement the DFS in three phases. First, check for Kokushi and its 13‑way wait. Second, look for seven pairs by enumerating all 6‑pair combinations. Finally, enumerate regular hands by recursively selecting groups and a pair. Factor the logic into helper functions so that `main.cpp` only orchestrates the search.
3. **Sorting & output** – after collecting all results, sort them by Han (descending) and then by fu. Provide a helper that converts a tile set into an easily readable string so that printing each result becomes trivial.
4. **Refactor headers** – `winning_hand.h` mixes data structures with yaku logic. Split the yaku checks into their own headers (e.g. `yaku_basic.h`, `yaku_yakuman.h`) and keep the `WinningHand` class focused on representation.
5. **Documentation** – write a short README covering build/test commands and a sample invocation. As features grow, keep the documentation up to date.
