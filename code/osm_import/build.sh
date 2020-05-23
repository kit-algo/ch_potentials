#!/bin/sh
g++ src/bin/import_osm.cpp -Wall -Wextra -O3 -o import_osm -lroutingkit
g++ src/bin/make_bfs_node_seq.cpp -Wall -Wextra -O3 -DNDEBUG -march=native -ffast-math -o make_bfs_node_seq -lroutingkit
g++ src/bin/render_node_sequence.cpp -Wall -Wextra -O3 -DNDEBUG -fopenmp -march=native -ffast-math -o render_node_sequence -lroutingkit
g++ src/bin/latlon_box_around_node.cpp -Wall -Wextra -O3 -DNDEBUG -march=native -ffast-math -o latlon_box_around_node -lroutingkit
