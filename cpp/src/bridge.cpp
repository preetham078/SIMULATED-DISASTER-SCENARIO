#include "Connectivity.h"
#include "Dijkstra.h"
#include "Graph.h"
#include "Kruskal.h"
#include "Scheduler.h"

#include <cmath>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>

using namespace std;

namespace {
Graph graph;
string output;

string quote(const string& value) {
    ostringstream result;
    result << '"';
    for (unsigned char character : value) {
        switch (character) {
            case '"': result << "\\\""; break;
            case '\\': result << "\\\\"; break;
            case '\n': result << "\\n"; break;
            case '\r': result << "\\r"; break;
            case '\t': result << "\\t"; break;
            default:
                if (character < 0x20) result << "\\u" << hex << setw(4) << setfill('0') << static_cast<int>(character);
                else result << character;
        }
    }
    result << '"';
    return result.str();
}

const char* nodeTypeName(NodeType type) {
    switch (type) {
        case NodeType::Substation: return "Substation";
        case NodeType::Hospital: return "Hospital";
        case NodeType::Facility: return "Facility";
        case NodeType::Telecom: return "Telecom";
        case NodeType::Consumer: return "Consumer";
    }
    return "Consumer";
}

string nodeJson(size_t index) {
    const Node& node = graph.nodes()[index];
    ostringstream json;
    json << "{\"id\":" << quote(node.id)
         << ",\"name\":" << quote(node.name)
         << ",\"type\":" << quote(nodeTypeName(node.type))
         << ",\"lat\":" << node.latitude << ",\"lng\":" << node.longitude
         << ",\"source\":" << (node.source ? "true" : "false")
         << ",\"voltage\":\"11 kV\",\"load\":" << node.load
         << ",\"customers\":" << node.customers << ",\"priority\":" << node.priority << '}';
    return json.str();
}

string edgeJson(size_t index) {
    const Edge& edge = graph.edges()[index];
    const Node& from = graph.nodes()[edge.from];
    const Node& to = graph.nodes()[edge.to];
    ostringstream json;
    json << "{\"id\":" << quote(edge.id) << ",\"a\":" << quote(from.id)
         << ",\"b\":" << quote(to.id) << ",\"weight\":" << edge.effort
         << ",\"repaired\":" << (edge.repaired ? "true" : "false")
         << ",\"damage\":" << quote(edge.damage)
         << ",\"repairTime\":" << edge.repairHours << ",\"lengthKm\":" << edge.lengthKm
         << ",\"status\":" << quote(edge.repaired ? "Repaired" : "Damaged") << '}';
    return json.str();
}

string stringArray(const vector<size_t>& values, bool useEdges = false) {
    ostringstream json;
    json << '[';
    for (size_t i = 0; i < values.size(); ++i) {
        if (i) json << ',';
        json << quote(useEdges ? graph.edges()[values[i]].id : graph.nodes()[values[i]].id);
    }
    json << ']';
    return json.str();
}

const char* errorResult(const exception& error) {
    output = string("ERROR: ") + error.what();
    return output.c_str();
}
}

extern "C" {
const char* grid_nodes_json() {
    ostringstream json;
    json << '[';
    for (size_t i = 0; i < graph.nodes().size(); ++i) {
        if (i) json << ',';
        json << nodeJson(i);
    }
    json << ']';
    output = json.str();
    return output.c_str();
}

const char* grid_edges_json() {
    ostringstream json;
    json << '[';
    for (size_t i = 0; i < graph.edges().size(); ++i) {
        if (i) json << ',';
        json << edgeJson(i);
    }
    json << ']';
    output = json.str();
    return output.c_str();
}

int grid_total_repaired_effort() { return graph.totalRepairedEffort(); }

int grid_set_edge_repaired(const char* id, int repaired) {
    const size_t index = graph.edgeIndex(id ? id : "");
    if (index == graph.edges().size()) {
        output = "ERROR: The selected power line no longer exists.";
        return 0;
    }
    graph.edges()[index].repaired = repaired != 0;
    return 1;
}

const char* grid_add_node(const char* name, int type, double latitude, double longitude) {
    try {
        const NodeType nodeType = type >= 0 && type <= 4 ? static_cast<NodeType>(type) : NodeType::Consumer;
        const size_t index = graph.addNode(name ? name : "", nodeType, latitude, longitude);
        output = graph.nodes()[index].id;
        return output.c_str();
    } catch (const exception& error) {
        return errorResult(error);
    }
}

const char* grid_add_edge(const char* from, const char* to, int effort) {
    try {
        const size_t index = graph.addEdge(from ? from : "", to ? to : "", effort);
        output = graph.edges()[index].id;
        return output.c_str();
    } catch (const exception& error) {
        return errorResult(error);
    }
}

void grid_reset() { graph.resetLines(); }

const char* grid_bfs_json() {
    const Connectivity connectivity(graph);
    const auto powered = connectivity.poweredNodes();
    const auto order = connectivity.breadthFirstOrder();
    ostringstream json;
    json << "{\"powered\":" << stringArray(powered) << ",\"order\":" << stringArray(order) << '}';
    output = json.str();
    return output.c_str();
}

const char* grid_groups_json() {
    const auto groups = Connectivity(graph).unpoweredGroups();
    ostringstream json;
    json << '[';
    for (size_t i = 0; i < groups.size(); ++i) {
        if (i) json << ',';
        json << stringArray(groups[i]);
    }
    json << ']';
    output = json.str();
    return output.c_str();
}

int grid_restoration_value(const char* id) {
    const size_t index = graph.nodeIndex(id ? id : "");
    return index == graph.nodes().size() ? 0 : Connectivity(graph).restorationValue(index);
}

const char* grid_dijkstra_json(const char* targetId) {
    const Dijkstra algorithm(graph);
    const DijkstraResult result = algorithm.shortestRepairs();
    const size_t target = graph.nodeIndex(targetId ? targetId : "");
    const auto path = target == graph.nodes().size() ? vector<size_t>{} : algorithm.pathTo(result, target);
    ostringstream json;
    json << "{\"distance\":{";
    for (size_t i = 0; i < graph.nodes().size(); ++i) {
        if (i) json << ',';
        json << quote(graph.nodes()[i].id) << ':';
        if (isfinite(result.distance[i])) json << result.distance[i];
        else json << "null";
    }
    json << "},\"path\":[";
    for (size_t i = 0; i < path.size(); ++i) {
        if (i) json << ',';
        json << edgeJson(path[i]);
    }
    json << "]}";
    output = json.str();
    return output.c_str();
}

const char* grid_kruskal_json() {
    const KruskalResult result = Kruskal(graph).minimumRepairNetwork();
    ostringstream json;
    json << "{\"total\":" << result.totalEffort << ",\"connectable\":"
         << (result.connectable ? "true" : "false") << ",\"chosen\":[";
    for (size_t i = 0; i < result.chosenEdges.size(); ++i) {
        if (i) json << ',';
        json << edgeJson(result.chosenEdges[i]);
    }
    json << "]}";
    output = json.str();
    return output.c_str();
}

const char* grid_schedule_json() {
    const ScheduleResult result = Scheduler(graph).build();
    ostringstream json;
    json << "{\"totalEffort\":" << result.totalEffort << ",\"schedule\":[";
    for (size_t i = 0; i < result.steps.size(); ++i) {
        if (i) json << ',';
        const RepairStep& step = result.steps[i];
        json << "{\"edge\":" << edgeJson(step.edgeIndex) << ",\"gain\":" << step.restorationGain << '}';
    }
    json << "]}";
    output = json.str();
    return output.c_str();
}
}
