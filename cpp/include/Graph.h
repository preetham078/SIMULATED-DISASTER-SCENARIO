#pragma once

#include <string>
#include <vector>

enum class NodeType {
    Substation,
    Hospital,
    Facility,
    Telecom,
    Consumer
};

struct Node {
    std::string id;
    std::string name;
    NodeType type;
    double latitude;
    double longitude;
    bool source;
    int load;
    int customers;
    int priority;
};

struct Edge {
    std::string id;
    std::size_t from;
    std::size_t to;
    int effort;
    bool repaired;
    std::string damage;
    int repairHours;
    double lengthKm;
};

class Graph {
public:
    Graph();

    const std::vector<Node>& nodes() const;
    const std::vector<Edge>& edges() const;
    std::vector<Node>& nodes();
    std::vector<Edge>& edges();
    std::size_t nodeIndex(const std::string& id) const;
    std::size_t edgeIndex(const std::string& id) const;
    std::size_t addNode(const std::string& name, NodeType type, double latitude, double longitude);
    std::size_t addEdge(const std::string& from, const std::string& to, int effort);
    void resetLines();
    int totalRepairedEffort() const;

private:
    std::vector<Node> nodes_;
    std::vector<Edge> edges_;

    void seed();
};
