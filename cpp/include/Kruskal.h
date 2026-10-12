#pragma once

#include "Graph.h"

#include <cstddef>
#include <vector>

struct KruskalResult {
    std::vector<std::size_t> chosenEdges;
    int totalEffort;
    bool connectable;
};

class Kruskal {
public:
    explicit Kruskal(const Graph& graph);
    KruskalResult minimumRepairNetwork() const;

private:
    const Graph& graph_;
};
