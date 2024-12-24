cutechess-cli -engine cmd=100elo-no-tt -engine cmd=100elo \
    -each tc=100+5 proto=uci restart=on -rounds 64 -concurrency 12 \
    -sprt elo0=100 elo1=0 alpha=0.05 beta=0.05 \
    -openings file=8moves_v3.pgn plies=8 -debug