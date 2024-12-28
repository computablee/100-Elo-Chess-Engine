cutechess-cli -engine cmd=100Elo -engine cmd=stash-11.0-linux-x86_64-modern \
    -each tc=10+0.1 proto=uci restart=on -rounds 128000 -concurrency 14 \
    -sprt elo0=0 elo1=10 alpha=0.05 beta=0.05 \
    -openings file=8moves_v3.pgn plies=8