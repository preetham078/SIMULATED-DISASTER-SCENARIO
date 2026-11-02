🚨 Emergency Power Grid Restoration Control Center

An educational disaster-response and power-grid restoration simulation that models how emergency teams can analyze damaged infrastructure, identify critical connected locations, and prioritize repair operations during a simulated flood.

The project combines a browser-based interactive dashboard with C++ graph algorithms compiled to WebAssembly (WASM). The C++ layer handles the graph state and algorithms, while JavaScript connects the algorithms to the web interface.

🌐 Live Demo :https://preetham078.github.io/SIMULATED-DISASTER-SCENARIO/

Launch the Emergency Power Grid Simulation

⸻

🎯 Project Objective

The goal of this project is to demonstrate how Data Structures and Algorithms can be applied to real-world disaster-management scenarios.

The simulation represents critical infrastructure such as:

* ⚡ Electrical substations
* 🏥 Hospitals
* 💧 Water treatment facilities
* 📡 Telecommunication facilities
* 🏠 Residential areas
* 🏭 Industrial locations

When infrastructure is affected by simulated flooding or storm damage, graph algorithms are used to determine connectivity, repair paths, and the most effective restoration strategy.

⸻

🧠 Algorithms Implemented

🔵 Breadth-First Search (BFS)

Used for multi-source powered-node discovery to determine which infrastructure nodes can still receive power from operational sources.

🟢 Depth-First Search (DFS)

Used to identify and group offline/disconnected infrastructure components.

🟡 Dijkstra’s Algorithm

Finds the minimum-effort repair path between infrastructure nodes and supports path reconstruction.

🔴 Union-Find / Disjoint Set

Maintains connectivity between infrastructure components using:

* Path compression
* Union by rank

🟣 Kruskal’s Algorithm

Generates a minimum spanning repair network, helping determine an efficient set of additional connections required to restore infrastructure.

🟠 Binary Max-Heap

The repair scheduler ranks possible repair operations based on service restored per unit of repair effort, allowing the system to prioritize high-impact repairs.

⸻

🏗️ System Architecture

                ┌─────────────────────┐
                │   Browser Dashboard │
                │    HTML / CSS / JS  │
                └──────────┬──────────┘
                           │
                           ▼
                ┌─────────────────────┐
                │    JavaScript       │
                │    WASM Adapter     │
                └──────────┬──────────┘
                           │
                    WebAssembly
                           │
                           ▼
                ┌─────────────────────┐
                │     C++ Engine      │
                │                     │
                │ BFS / DFS           │
                │ Dijkstra            │
                │ Union-Find          │
                │ Kruskal             │
                │ Max-Heap Scheduler  │
                └─────────────────────┘

⸻

✨ Features

* 🌊 Simulated flood and storm-damage scenarios
* 🗺️ Interactive map using Leaflet/OpenStreetMap
* ⚡ Simulated power-grid infrastructure
* 🏥 Hospital and critical-service locations
* 🔍 BFS and DFS connectivity analysis
* 🛣️ Dijkstra minimum-effort repair paths
* 🌐 Kruskal-based repair network optimization
* 🔗 Union-Find connectivity management
* 📊 Priority-based repair scheduling
* 🛠️ Add and edit infrastructure nodes and power lines
* 💥 Simulate storm damage
* 🔧 Repair individual damaged lines
* 📈 Dashboard for algorithm results and system status
* 💻 C++ algorithms running in the browser through WebAssembly

⸻

📁 Project Structure

SIMULATED-DISASTER-SCENARIO/
│
├── cpp/
│   ├── include/
│   │   ├── Graph.h
│   │   ├── Connectivity.h
│   │   ├── Dijkstra.h
│   │   ├── UnionFind.h
│   │   ├── Kruskal.h
│   │   └── Scheduler.h
│   │
│   ├── src/
│   │   ├── Graph.cpp
│   │   ├── Connectivity.cpp
│   │   ├── Dijkstra.cpp
│   │   ├── UnionFind.cpp
│   │   ├── Kruskal.cpp
│   │   ├── Scheduler.cpp
│   │   └── bridge.cpp
│   │
│   └── tests.cpp
│
├── js/
│   ├── main.js
│   └── wasm.js
│
├── index.html
├── serve.js
└── README.txt

⸻

🛠️ Technologies Used

Technology	Purpose
C++17	Graph algorithms and simulation engine
WebAssembly	Run C++ algorithms inside the browser
JavaScript	UI interaction and WASM communication
HTML/CSS	Dashboard interface
Leaflet	Interactive mapping
OpenStreetMap	Map data
GitHub Pages	Web deployment

⸻

⚙️ How to Run

1. Install Emscripten

Install the Emscripten SDK and make sure emcc and em++ are available.

2. Build the WebAssembly module

From the project directory:

sh cpp/build-wasm.sh

This generates:

cpp/power-grid.js
cpp/power-grid.wasm

Keep both generated files with the project when deploying.

3. Start a local server

python3 -m http.server 8000

Then open:

http://localhost:8000

Important: Do not open index.html directly using file://, because browsers can block WebAssembly resources loaded from local file URLs.

⸻

🧪 Running C++ Tests

Compile the native test suite using:

clang++ -std=c++17 -Wall -Wextra -pedantic \
-Icpp/include \
cpp/src/Graph.cpp \
cpp/src/Connectivity.cpp \
cpp/src/Dijkstra.cpp \
cpp/src/UnionFind.cpp \
cpp/src/Kruskal.cpp \
cpp/src/Scheduler.cpp \
cpp/tests.cpp \
-o /tmp/power-grid-tests

Run:

/tmp/power-grid-tests

⸻

🎮 How to Use the Simulation

1. Launch the web application.
2. Toggle the flood layer.
3. Adjust flood severity.
4. Explore the Overview dashboard.
5. Use the BFS tab to analyze powered infrastructure.
6. Use Dijkstra to find efficient repair paths.
7. Use Kruskal to visualize the minimum repair network.
8. Use Schedule to identify prioritized repairs.
9. Add or modify infrastructure nodes and power lines.
10. Inject simulated storm damage.
11. Repair damaged connections.
12. Use the hospital-route feature to identify an appropriate restoration path.
13. Reset the simulation to return infrastructure lines to their damaged state.

⸻

📚 Learning Outcomes

This project demonstrates practical applications of:

* Graph data structures
* BFS and DFS traversal
* Shortest-path algorithms
* Minimum spanning trees
* Disjoint-set data structures
* Priority queues / heaps
* WebAssembly
* C++ and JavaScript interoperability
* Interactive web-based simulations
* Algorithmic decision-making

⸻

⚠️ Disclaimer

This project is created for educational and demonstration purposes only.

The infrastructure, flood conditions, outages, repair costs, and damage scenarios are simulated and do not represent real-world infrastructure, outages, or emergency-response decisions.

⸻

👨‍💻 Author

Preetham M

Computer Science Engineering Student

GitHub: @preetham078

⸻

⭐ Project

If you find this project useful for learning Data Structures, Algorithms, C++, or WebAssembly, consider giving the repository a ⭐.

Built to demonstrate how algorithms can support decision-making during simulated disaster-response scenarios.
