#pragma once

#include "Graph.h"

#include <vector>

using namespace std;

struct RepairedComponents {
    vector<int> componentByNode;
    vector<int> componentValues;
    vector<bool> componentPowered;
};

class Connectivity {
public:
    explicit Connectivity(const Graph& graph);
    vector<size_t> poweredNodes(const vector<bool>& simulated = {}) const;
    vector<size_t> breadthFirstOrder() const;
    vector<vector<size_t>> unpoweredGroups() const;
    int restorationValue(size_t nodeIndex) const;
    RepairedComponents repairedComponents(const vector<bool>& simulated) const;

private:
    const Graph& graph_;
    bool edgeAvailable(size_t edgeIndex, const vector<bool>& simulated) const;
};
