#pragma once

#include "Graph.h"

#include <vector>

using namespace std;

struct DijkstraResult {
    vector<double> distance;
    vector<int> previousEdge;
};

class Dijkstra {
public:
    explicit Dijkstra(const Graph& graph);
    DijkstraResult shortestRepairs() const;
    vector<size_t> pathTo(const DijkstraResult& result, size_t target) const;

private:
    const Graph& graph_;
};
