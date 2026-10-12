#include "Connectivity.h"

#include <algorithm>
#include <queue>

using namespace std;

Connectivity::Connectivity(const Graph& graph) : graph_(graph) {}

bool Connectivity::edgeAvailable(size_t edgeIndex, const vector<bool>& simulated) const {
    const auto& edges = graph_.edges();
    return edges[edgeIndex].repaired || (edgeIndex < simulated.size() && simulated[edgeIndex]);
}

vector<size_t> Connectivity::poweredNodes(const vector<bool>& simulated) const {
    const auto& nodes = graph_.nodes();
    vector<bool> seen(nodes.size(), false);
    queue<size_t> pending;
    vector<size_t> order;
    for (size_t i = 0; i < nodes.size(); ++i) {
        if (!nodes[i].source) continue;
        seen[i] = true;
        pending.push(i);
        order.push_back(i);
    }
    while (!pending.empty()) {
        const size_t current = pending.front();
        pending.pop();
        for (size_t e = 0; e < graph_.edges().size(); ++e) {
            const Edge& edge = graph_.edges()[e];
            if (!edgeAvailable(e, simulated)) continue;
            const size_t next = edge.from == current ? edge.to :
                edge.to == current ? edge.from : nodes.size();
            if (next == nodes.size() || seen[next]) continue;
            seen[next] = true;
            order.push_back(next);
            pending.push(next);
        }
    }
    return order;
}

vector<size_t> Connectivity::breadthFirstOrder() const {
    return poweredNodes();
}

vector<vector<size_t>> Connectivity::unpoweredGroups() const {
    const auto& nodes = graph_.nodes();
    const auto& edges = graph_.edges();
    vector<bool> seen(nodes.size(), false);
    for (size_t index : poweredNodes()) seen[index] = true;
    vector<vector<size_t>> groups;
    for (size_t start = 0; start < nodes.size(); ++start) {
        if (seen[start]) continue;
        groups.emplace_back();
        vector<size_t> stack{start};
        seen[start] = true;
        while (!stack.empty()) {
            const size_t current = stack.back();
            stack.pop_back();
            groups.back().push_back(current);
            for (const Edge& edge : edges) {
                if (!edge.repaired) continue;
                const size_t next = edge.from == current ? edge.to :
                    edge.to == current ? edge.from : nodes.size();
                if (next == nodes.size() || seen[next]) continue;
                seen[next] = true;
                stack.push_back(next);
            }
        }
    }
    return groups;
}

int Connectivity::restorationValue(size_t nodeIndex) const {
    if (nodeIndex >= graph_.nodes().size() || graph_.nodes()[nodeIndex].source) return 0;
    switch (graph_.nodes()[nodeIndex].type) {
        case NodeType::Hospital: return 10;
        case NodeType::Facility: return 8;
        case NodeType::Telecom: return 6;
        case NodeType::Substation: return 0;
        case NodeType::Consumer: return 1;
    }
    return 0;
}

RepairedComponents Connectivity::repairedComponents(const vector<bool>& simulated) const {
    const auto& nodes = graph_.nodes();
    RepairedComponents result{
        vector<int>(nodes.size(), -1), {}, {}
    };
    for (size_t start = 0; start < nodes.size(); ++start) {
        if (result.componentByNode[start] >= 0) continue;
        const int component = static_cast<int>(result.componentValues.size());
        result.componentValues.push_back(0);
        result.componentPowered.push_back(false);
        queue<size_t> pending;
        pending.push(start);
        result.componentByNode[start] = component;
        while (!pending.empty()) {
            const size_t current = pending.front();
            pending.pop();
            result.componentValues[component] += restorationValue(current);
            if (nodes[current].source) result.componentPowered[component] = true;
            for (size_t e = 0; e < graph_.edges().size(); ++e) {
                if (!edgeAvailable(e, simulated)) continue;
                const Edge& edge = graph_.edges()[e];
                const size_t next = edge.from == current ? edge.to :
                    edge.to == current ? edge.from : nodes.size();
                if (next == nodes.size() || result.componentByNode[next] >= 0) continue;
                result.componentByNode[next] = component;
                pending.push(next);
            }
        }
    }
    return result;
}
