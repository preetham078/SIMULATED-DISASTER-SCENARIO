#pragma once

#include "Graph.h"

#include <cstddef>
#include <vector>

struct RepairStep {
    std::size_t edgeIndex;
    int restorationGain;
};

struct ScheduleResult {
    std::vector<RepairStep> steps;
    int totalEffort;
};

class Scheduler {
public:
    explicit Scheduler(const Graph& graph);
    ScheduleResult build() const;

private:
    const Graph& graph_;
};
