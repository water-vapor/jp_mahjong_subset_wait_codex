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

# The searching algorithm
 - The result storing map. It should contain the 13-hand as key, and for each value, you need to have a nested map (since there can be multiple waits) containing the tile to wait (perhaps as the key), score and yaku string (as the value). Note that if there are multiple interpretations of the same 14-tile set, they will result in different `WinningHand` instances, but should not duplicate in this map since the keys are identical.
 - The DFS searching logic.
     - In my design, first we should check for Kokushi (13 yao) or Kokushi Jusanmen (13 different + any tile), since it does not form a valid WinningHand in the design, and are easy to check. The checking logic should be as follows: 1. records the counts of all yaochu tiles. 2. if all counts are >=1, then add a "国士无双十三面" 20000 to the result. 3. if at least 12 of the counts are >=1, iterate through all counts > 1 to be the pair, and with the rest forming a "国士无双" 10000 (of course, you can wait any one of them if all 13 are present in this way). 
     - Then check for 7-pairs, we can filter all tiles and only consider these with pairs. Do a DFS on the 6-pair combination, then combine the selected 6-pair with any (different) tile available.
     - Then check for regular 4 group + 1 pair combination. In this case you should consider (a) waiting one tile in the pair, so you just do DFS selection for four groups; (b) waiting for one tile in a group. More considerations are needed here. Generally you can select unique groups (of 3 tiles) by increasing the beginning tile and group type. For example, you can iterate through (ordered group list: 1m1m1m, 1m2m3m, 2m2m2m, 2m3m4m, ..., 7m7m7m, 7m8m9m, 8m8m8m, 9m9m9m, 1p1p1p, ... ,9s9s9s, 1z1z1z, ..., 7z7z7z) on the same dfs level (of course, some of these will not be present in the current superset, in which case skipping is the action), and subtract the selected 3 tiles from the full hand and pass to the next level. There is no need of a starting index increase at each level so that multiple 1m2m3m can be selected at different level; we only need to guarentee the newly selected group is not smaller in that ordered group list. 

 - The final sorting and pretty printing system

# Contributor Guide
1. The project is now split across headers and sources. When adding new files, match the existing naming style and keep code well commented.
2. Unit tests already cover tiles, yaku and compound scenarios. Extend the test suite whenever new logic is introduced.
3. Yaku methods already return a `YakuResult`. Ensure new yaku related code follows this pattern instead of mutating members directly.
4. You should check for duplicated or inelegant code. If there are better and cleaner ways to write it, you can change it.
5. A `.gitignore` file already excludes build artifacts (`main` and test binaries`). Keep it up to date when adding new tools.
