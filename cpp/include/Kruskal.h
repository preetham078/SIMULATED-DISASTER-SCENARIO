#pragma once

#include "Graph.h"

#include <cstddef>
#include <vector>

using namespace std;

struct KruskalResult {
    vector<size_t> chosenEdges;
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
