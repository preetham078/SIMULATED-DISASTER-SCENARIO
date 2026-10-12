#include "Scheduler.h"
#include "Connectivity.h"

#include <algorithm>

using namespace std;

namespace {
struct Candidate {
    size_t edgeIndex;
    int gain;
};

bool higherPriority(const Candidate& left, const Candidate& right, const vector<Edge>& edges) {
    const long long leftScore = static_cast<long long>(left.gain) * max(1, edges[right.edgeIndex].effort);
    const long long rightScore = static_cast<long long>(right.gain) * max(1, edges[left.edgeIndex].effort);
    return leftScore == rightScore ? left.edgeIndex < right.edgeIndex : leftScore > rightScore;
}

void heapPush(vector<Candidate>& heap, Candidate candidate, const vector<Edge>& edges) {
    heap.push_back(candidate);
    size_t index = heap.size() - 1;
    while (index > 0) {
        const size_t parent = (index - 1) / 2;
        if (!higherPriority(heap[index], heap[parent], edges)) break;
        swap(heap[index], heap[parent]);
        index = parent;
    }
}

Candidate heapPop(vector<Candidate>& heap, const vector<Edge>& edges) {
    Candidate first = heap.front();
    heap.front() = heap.back();
    heap.pop_back();
    size_t index = 0;
    while (index < heap.size()) {
        size_t best = index;
        const size_t left = index * 2 + 1;
        const size_t right = left + 1;
        if (left < heap.size() && higherPriority(heap[left], heap[best], edges)) best = left;
        if (right < heap.size() && higherPriority(heap[right], heap[best], edges)) best = right;
        if (best == index) break;
        swap(heap[index], heap[best]);
        index = best;
    }
    return first;
}
}

Scheduler::Scheduler(const Graph& graph) : graph_(graph) {}

ScheduleResult Scheduler::build() const {
    const auto& edges = graph_.edges();
    vector<bool> simulated(edges.size(), false);
    for (size_t i = 0; i < edges.size(); ++i) simulated[i] = edges[i].repaired;
    Connectivity connectivity(graph_);
    ScheduleResult result{{}, 0};
    while (true) {
        const RepairedComponents components = connectivity.repairedComponents(simulated);
        vector<Candidate> heap;
        for (size_t i = 0; i < edges.size(); ++i) {
            if (simulated[i]) continue;
            const Edge& edge = edges[i];
            const int from = components.componentByNode[edge.from];
            const int to = components.componentByNode[edge.to];
            if (from == to || components.componentPowered[from] == components.componentPowered[to]) continue;
            const int unpowered = components.componentPowered[from] ? to : from;
            const int gain = components.componentValues[unpowered];
            if (gain > 0) heapPush(heap, {i, gain}, edges);
        }
        if (heap.empty()) break;
        const Candidate next = heapPop(heap, edges);
        simulated[next.edgeIndex] = true;
        result.steps.push_back({next.edgeIndex, next.gain});
        result.totalEffort += edges[next.edgeIndex].effort;
    }
    return result;
}
