#include "Kruskal.h"
#include "UnionFind.h"

#include <algorithm>

using namespace std;

Kruskal::Kruskal(const Graph& graph) : graph_(graph) {}

KruskalResult Kruskal::minimumRepairNetwork() const {
    const auto& nodes = graph_.nodes();
    const auto& edges = graph_.edges();
    UnionFind sets(nodes.size());
    for (const Edge& edge : edges) if (edge.repaired) sets.unite(edge.from, edge.to);
    vector<size_t> candidates;
    for (size_t i = 0; i < edges.size(); ++i) if (!edges[i].repaired) candidates.push_back(i);
    stable_sort(candidates.begin(), candidates.end(), [&edges](size_t left, size_t right) {
        return edges[left].effort < edges[right].effort;
    });
    KruskalResult result{{}, 0, nodes.empty()};
    for (size_t index : candidates) {
        const Edge& edge = edges[index];
        if (!sets.unite(edge.from, edge.to)) continue;
        result.chosenEdges.push_back(index);
        result.totalEffort += edge.effort;
    }
    result.connectable = nodes.empty() || sets.components() == 1;
    return result;
}
