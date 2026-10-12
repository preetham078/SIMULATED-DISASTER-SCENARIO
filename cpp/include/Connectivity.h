#pragma once

#include "Graph.h"

#include <vector>

struct RepairedComponents {
    std::vector<int> componentByNode;
    std::vector<int> componentValues;
    std::vector<bool> componentPowered;
};

class Connectivity {
public:
    explicit Connectivity(const Graph& graph);
    std::vector<std::size_t> poweredNodes(const std::vector<bool>& simulated = {}) const;
    std::vector<std::size_t> breadthFirstOrder() const;
    std::vector<std::vector<std::size_t>> unpoweredGroups() const;
    int restorationValue(std::size_t nodeIndex) const;
    RepairedComponents repairedComponents(const std::vector<bool>& simulated) const;

private:
    const Graph& graph_;
    bool edgeAvailable(std::size_t edgeIndex, const std::vector<bool>& simulated) const;
};
