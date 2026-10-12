#include "Dijkstra.h"

#include <cmath>
#include <limits>

using namespace std;

Dijkstra::Dijkstra(const Graph& graph) : graph_(graph) {}

DijkstraResult Dijkstra::shortestRepairs() const {
    const auto& nodes = graph_.nodes();
    const auto& edges = graph_.edges();
    DijkstraResult result{
        vector<double>(nodes.size(), numeric_limits<double>::infinity()),
        vector<int>(nodes.size(), -1)
    };
    vector<bool> visited(nodes.size(), false);
    for (size_t i = 0; i < nodes.size(); ++i) if (nodes[i].source) result.distance[i] = 0;

    for (size_t iteration = 0; iteration < nodes.size(); ++iteration) {
        size_t current = nodes.size();
        for (size_t i = 0; i < nodes.size(); ++i) {
            if (!visited[i] && (current == nodes.size() || result.distance[i] < result.distance[current])) current = i;
        }
        if (current == nodes.size() || !isfinite(result.distance[current])) break;
        visited[current] = true;
        for (size_t e = 0; e < edges.size(); ++e) {
            const Edge& edge = edges[e];
            const size_t next = edge.from == current ? edge.to :
                edge.to == current ? edge.from : nodes.size();
            if (next == nodes.size()) continue;
            const double candidate = result.distance[current] + (edge.repaired ? 0 : edge.effort);
            if (candidate >= result.distance[next]) continue;
            result.distance[next] = candidate;
            result.previousEdge[next] = static_cast<int>(e);
        }
    }
    return result;
}

vector<size_t> Dijkstra::pathTo(const DijkstraResult& result, size_t target) const {
    vector<size_t> path;
    if (target >= result.distance.size() || !isfinite(result.distance[target])) return path;
    size_t current = target;
    for (size_t steps = 0; steps < graph_.nodes().size(); ++steps) {
        const int edgeIndex = result.previousEdge[current];
        if (edgeIndex < 0) break;
        const Edge& edge = graph_.edges()[static_cast<size_t>(edgeIndex)];
        if (!edge.repaired) path.insert(path.begin(), static_cast<size_t>(edgeIndex));
        current = edge.from == current ? edge.to : edge.from;
    }
    return path;
}
