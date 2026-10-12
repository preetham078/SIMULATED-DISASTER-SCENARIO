#pragma once

#include "Graph.h"

#include <vector>

struct DijkstraResult {
    std::vector<double> distance;
    std::vector<int> previousEdge;
};

class Dijkstra {
public:
    explicit Dijkstra(const Graph& graph);
    DijkstraResult shortestRepairs() const;
    std::vector<std::size_t> pathTo(const DijkstraResult& result, std::size_t target) const;

private:
    const Graph& graph_;
};
