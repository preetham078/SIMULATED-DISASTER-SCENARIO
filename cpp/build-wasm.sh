#!/bin/sh
set -eu

cd "$(dirname "$0")/.."
: "${EMXX:=em++}"

"$EMXX" \
  -std=c++17 -O2 -fexceptions \
  -Icpp/include \
  cpp/src/Graph.cpp \
  cpp/src/Connectivity.cpp \
  cpp/src/Dijkstra.cpp \
  cpp/src/UnionFind.cpp \
  cpp/src/Kruskal.cpp \
  cpp/src/Scheduler.cpp \
  cpp/src/bridge.cpp \
  -o cpp/power-grid.js \
  -sMODULARIZE=1 \
  -sEXPORT_NAME=createPowerGridModule \
  -sENVIRONMENT=web \
  -sALLOW_MEMORY_GROWTH=1 \
  -sFILESYSTEM=0 \
  -sDISABLE_EXCEPTION_CATCHING=0 \
  -sEXPORTED_FUNCTIONS='["_grid_nodes_json","_grid_edges_json","_grid_total_repaired_effort","_grid_set_edge_repaired","_grid_add_node","_grid_add_edge","_grid_reset","_grid_bfs_json","_grid_groups_json","_grid_restoration_value","_grid_dijkstra_json","_grid_kruskal_json","_grid_schedule_json"]' \
  -sEXPORTED_RUNTIME_METHODS='["cwrap"]'
