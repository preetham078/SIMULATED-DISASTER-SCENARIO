#pragma once

#include <string>
#include <vector>

using namespace std;

enum class NodeType {
    Substation,
    Hospital,
    Facility,
    Telecom,
    Consumer
};

struct Node {
    string id;
    string name;
    NodeType type;
    double latitude;
    double longitude;
    bool source;
    int load;
    int customers;
    int priority;
};

struct Edge {
    string id;
    size_t from;
    size_t to;
    int effort;
    bool repaired;
    string damage;
    int repairHours;
    double lengthKm;
};

class Graph {
public:
    Graph();

    const vector<Node>& nodes() const;
    const vector<Edge>& edges() const;
    vector<Node>& nodes();
    vector<Edge>& edges();
    size_t nodeIndex(const string& id) const;
    size_t edgeIndex(const string& id) const;
    size_t addNode(const string& name, NodeType type, double latitude, double longitude);
    size_t addEdge(const string& from, const string& to, int effort);
    void resetLines();
    int totalRepairedEffort() const;

private:
    vector<Node> nodes_;
    vector<Edge> edges_;

    void seed();
};
