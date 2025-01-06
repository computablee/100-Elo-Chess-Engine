# Build

```sh
# Clone repo and submodules
git clone https://github.com/computablee/100-Elo-Chess-Engine.git
cd 100-Elo-Chess-Engine
git submodule init
git submodule update

# Build
mkdir build
cd build
cmake ..
make -j8
```

# Current Elo Estimates

Currently, the 100 Elo Chess Engine is using the [Stash engine](https://gitlab.com/mhouppin/stash-bot) and [cutechess](https://cutechess.com/) to get a rough idea of Elo.
We have compared to:

- v9 (1287 Elo)
- v11 (1698 Elo)
- v12 (1891 Elo)
- v13 (1977 Elo)
- v14 (2068 Elo)

Latest run was done against `v14` with a time control of `120+1` (in seconds) with `1000` total runs.

Results may be outdated.
I run these occasionally; the engine may be stronger than the results shown.

```
Score of 100 Elo Chess Engine vs Stash v14.0.1: 326 - 203 - 471  [0.561] 1000
...      100 Elo Chess Engine playing White: 172 - 83 - 245  [0.589] 500
...      100 Elo Chess Engine playing Black: 154 - 120 - 226  [0.534] 500
...      White vs Black: 292 - 237 - 471  [0.527] 1000
Elo difference: 43.0 +/- 15.7, LOS: 100.0 %, DrawRatio: 47.1 %
```

This results in an estimated Elo of: `2111`.

# Current Features

The 100 Elo Chess Engine uses the following architecture:

- Basic/buggy UCI support
- PeSTO evaluation
- Iterative deepening
- Transposition table
- Null move pruning
- Move ordering (best move -> MVV-LVA -> killer moves -> quiet moves)
- Quiescent search
- Principal variation search
- Late move reduction
- Check extensions