EMERGENCY POWER GRID RESTORATION CONTROL CENTER
==============================================

live Here : https://preetham078.github.io/SIMULATED-DISASTER-SCENARIO/

Open index.html in any modern browser.

This project has been transformed from a small abstract 7-node teaching graph into a realistic emergency power-grid restoration simulator for a simulated flood-response scenario in Bengaluru.

What is included:
- Leaflet + OpenStreetMap map base layer with pan/zoom controls
- Flood-risk overlays for low/medium/severe/extreme flood states
- Separate power-grid layer with electrical substations and distribution links
- Simulated critical facilities: hospital, water treatment plant, telecom relay, residential and industrial areas
- Repair actions that preserve the original graph algorithms:
  - BFS for connectivity and powered-node discovery
  - Dijkstra for minimum-effort hospital route
  - Kruskal for minimum spanning tree
  - Greedy scheduling for priority-first repair recommendation
  - Union-Find logic embedded in the MST process
- Interactive control-room dashboard with severity switching and repair/damage controls
- Offline fallback behavior when map tiles are unavailable

Important note:
The infrastructure shown is a simulated disaster-response scenario for educational purposes only.
The labels intentionally use terms such as "SIMULATED HOSPITAL" and "Simulated Substation" rather than claiming real-world infrastructure is destroyed.

Quick usage:
1. Open the page in a browser.
2. Toggle the flood layer on/off or change the flood severity.
3. Use the action buttons to repair the next recommended line, follow the hospital route, or highlight the Kruskal MST.
4. Click an individual damaged line on the map to repair it directly.
5. Use Reset to restore the initial flood-damaged scenario.

The original algorithmic logic remains in place while the visualization and scenario presentation are upgraded to a professional control-room style.
