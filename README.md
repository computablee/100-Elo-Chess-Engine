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

- v9 (1275 Elo)
- v11 (1690 Elo)
- v12 (1886 Elo)
- v13 (1972 Elo)

Latest run was done against `v12` with a time control of `120+1` (in seconds) with `1000` total runs.

Results may be outdated.
I run these occasionally; the engine may be stronger than the results shown.

```
Score of 100 Elo Chess Engine vs Stash v12.0: 417 - 234 - 349  [0.592] 1000
Elo difference: 64.3 +/- 17.5, LOS: 100.0 %, DrawRatio: 34.9 %
```

This results in an estimated Elo of: `1950`.

# Current Features

The 100 Elo Chess Engine uses the following architecture:

- Basic/buggy UCI support
- PeSTO evaluation
- Iterative deepening
- Transposition table
- Null move pruning
- Move ordering (best move, then MVV-LVA for captures)
- Quiescent search
- Killer moves