EMERGENCY POWER GRID RESTORATION CONTROL CENTER
==============================================

live here: https://preetham078.github.io/SIMULATED-DISASTER-SCENARIO/


This is an educational Bengaluru flood-response simulation. It combines a
browser dashboard with C++ graph algorithms compiled to WebAssembly. The
JavaScript files only load the WebAssembly module, translate data for the UI,
and handle browser events; graph state and algorithm results come from C++.

Features:
- Leaflet/OpenStreetMap map, simulated flood overlays, and offline map fallback
- Simulated substations, hospitals, water treatment, telecom, residential, and
  industrial locations
- C++ BFS powered-node discovery and DFS grouping of offline nodes
- C++ Dijkstra minimum-effort repair paths
- C++ Union-Find with path compression and union by rank
- C++ Kruskal minimum additional repair network
- C++ binary max-heap repair schedule ranked by service gain per effort
- Browser controls for repairs, storm damage, and editing nodes and lines

C++ source guide:

| File | What to explain |
| --- | --- |
| `cpp/include/Graph.h`, `cpp/src/Graph.cpp` | Graph data structures, initial grid, validation, and editing |
| `cpp/include/Connectivity.h`, `cpp/src/Connectivity.cpp` | Multi-source BFS, DFS offline groups, and service value |
| `cpp/include/Dijkstra.h`, `cpp/src/Dijkstra.cpp` | Minimum additional repair effort and path reconstruction |
| `cpp/include/UnionFind.h`, `cpp/src/UnionFind.cpp` | Disjoint sets, path compression, and union by rank |
| `cpp/include/Kruskal.h`, `cpp/src/Kruskal.cpp` | Minimum spanning repair network using Union-Find |
| `cpp/include/Scheduler.h`, `cpp/src/Scheduler.cpp` | Binary max-heap prioritizing newly restored service per effort |
| `cpp/src/bridge.cpp` | C interface called by the browser to access the C++ graph |
| `cpp/tests.cpp` | Native checks for graph, algorithms, and editing |
| `js/wasm.js` | Loads the WebAssembly module and adapts its interface for the page |
| `js/main.js` | Map, dashboard, forms, buttons, and result rendering |
| `index.html` | Page layout and script loading order |

Suggested explanation order: Graph, BFS/DFS, Dijkstra, Union-Find, Kruskal,
the repair scheduler, the C bridge, and finally the browser adapter and UI.

Build the C++ WebAssembly module:
1. Install the Emscripten SDK (em++, emcc).
2. From the project directory, run `sh cpp/build-wasm.sh`.
   It generates `cpp/power-grid.js` and `cpp/power-grid.wasm`. Keep both
   generated files with the site when publishing it.
3. Serve the project over HTTP (for example, `python3 -m http.server 8000`)
   and open `http://localhost:8000`. Do not open `index.html` using `file://`;
   browsers block WebAssembly fetching from file URLs.

Run the native C++ tests:
`clang++ -std=c++17 -Wall -Wextra -pedantic -Icpp/include cpp/src/Graph.cpp cpp/src/Connectivity.cpp cpp/src/Dijkstra.cpp cpp/src/UnionFind.cpp cpp/src/Kruskal.cpp cpp/src/Scheduler.cpp cpp/tests.cpp -o /tmp/power-grid-tests`
`/tmp/power-grid-tests`

Usage:
1. Build the WebAssembly module and serve the project over HTTP.
2. Toggle the flood layer or change its severity.
3. Use the Overview, BFS, Dijkstra, Kruskal, Schedule, and Grid tabs.
4. Add grid nodes and power lines or repair/report damage on individual lines.
5. Use the action buttons to repair the next recommendation, follow the
   hospital route, highlight the Kruskal network, or inject storm damage.
6. Reset restores all lines to damaged; added nodes and lines remain in the
   current browser session.

The simulated infrastructure is for educational purposes only. It does not
represent real-world outages or damage.

Algorithm/workflow reference:
https://github.com/chetan-1747/data-structures-micro-project/tree/main/Power%20Grid%20v1
