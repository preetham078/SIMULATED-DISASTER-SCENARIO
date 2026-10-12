#include "Graph.h"

#include <algorithm>
#include <stdexcept>

using namespace std;

namespace {
constexpr size_t kMaximumNodes = 100;
constexpr size_t kMaximumEdges = 500;

int priorityFor(NodeType type) {
    switch (type) {
        case NodeType::Hospital: return 10;
        case NodeType::Facility: return 8;
        case NodeType::Telecom: return 6;
        case NodeType::Consumer: return 1;
        case NodeType::Substation: return 0;
    }
    return 0;
}
}

Graph::Graph() {
    seed();
}

const vector<Node>& Graph::nodes() const { return nodes_; }
const vector<Edge>& Graph::edges() const { return edges_; }
vector<Node>& Graph::nodes() { return nodes_; }
vector<Edge>& Graph::edges() { return edges_; }

size_t Graph::nodeIndex(const string& id) const {
    const auto found = find_if(nodes_.begin(), nodes_.end(), [&id](const Node& node) {
        return node.id == id;
    });
    return found == nodes_.end() ? nodes_.size() : static_cast<size_t>(found - nodes_.begin());
}

size_t Graph::edgeIndex(const string& id) const {
    const auto found = find_if(edges_.begin(), edges_.end(), [&id](const Edge& edge) {
        return edge.id == id;
    });
    return found == edges_.end() ? edges_.size() : static_cast<size_t>(found - edges_.begin());
}

size_t Graph::addNode(const string& name, NodeType type, double latitude, double longitude) {
    if (nodes_.size() >= kMaximumNodes) throw invalid_argument("The grid has reached its 100-node limit.");
    if (name.empty() || latitude < -90 || latitude > 90 || longitude < -180 || longitude > 180) {
        throw invalid_argument("Enter a name and valid latitude/longitude.");
    }
    size_t number = 1;
    string id;
    do {
        id = "N" + to_string(number++);
    } while (nodeIndex(id) != nodes_.size());
    nodes_.push_back({id, name, type, latitude, longitude, type == NodeType::Substation, 0, 0, priorityFor(type)});
    return nodes_.size() - 1;
}

size_t Graph::addEdge(const string& from, const string& to, int effort) {
    if (edges_.size() >= kMaximumEdges) throw invalid_argument("The grid has reached its 500-line limit.");
    const size_t left = nodeIndex(from);
    const size_t right = nodeIndex(to);
    if (left == nodes_.size() || right == nodes_.size() || left == right || effort < 0) {
        throw invalid_argument("Choose two different existing nodes and enter a non-negative whole-number effort.");
    }
    for (const Edge& edge : edges_) {
        if ((edge.from == left && edge.to == right) || (edge.from == right && edge.to == left)) {
            throw invalid_argument("A power line already connects those two nodes.");
        }
    }
    size_t number = 1;
    string id;
    do {
        id = "L" + to_string(number++);
    } while (edgeIndex(id) != edges_.size());
    edges_.push_back({id, left, right, effort, false, "None", max(1, effort), 0});
    return edges_.size() - 1;
}

void Graph::resetLines() {
    for (Edge& edge : edges_) edge.repaired = false;
}

int Graph::totalRepairedEffort() const {
    int total = 0;
    for (const Edge& edge : edges_) if (edge.repaired) total += edge.effort;
    return total;
}

void Graph::seed() {
    nodes_ = {
        {"S1", "Simulated Substation S1", NodeType::Substation, 12.9691, 77.5604, true, 72, 4820, 0},
        {"S2", "Simulated Substation S2", NodeType::Substation, 12.9165, 77.6107, true, 68, 4310, 0},
        {"H1", "SIMULATED HOSPITAL", NodeType::Hospital, 12.9727, 77.5944, false, 84, 940, 10},
        {"W1", "Water Treatment Plant", NodeType::Facility, 12.9348, 77.6655, false, 77, 1380, 8},
        {"T1", "Telecom Relay", NodeType::Telecom, 12.9264, 77.5648, false, 64, 1160, 6},
        {"R1", "Residential Cluster", NodeType::Consumer, 12.9965, 77.6210, false, 56, 2800, 4},
        {"R2", "Industrial Zone", NodeType::Consumer, 12.8797, 77.6762, false, 61, 2100, 5}
    };
    edges_ = {
        {"L1", 0, 1, 5, false, "Severe", 6, 4.8},
        {"L2", 0, 2, 3, false, "Moderate", 3, 2.1},
        {"L3", 1, 3, 4, false, "High", 4, 3.6},
        {"L4", 1, 4, 2, false, "Moderate", 2, 1.6},
        {"L5", 2, 5, 6, false, "Severe", 7, 5.2},
        {"L6", 3, 6, 7, false, "Critical", 9, 6.1},
        {"L7", 5, 6, 8, false, "High", 8, 6.8}
    };
}
