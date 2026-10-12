#include "Connectivity.h"
#include "Dijkstra.h"
#include "Graph.h"
#include "Kruskal.h"
#include "Scheduler.h"
#include "UnionFind.h"

#include <cassert>
#include <cmath>
#include <iostream>
#include <stdexcept>

using namespace std;

int main() {
    Graph graph;
    assert(graph.nodes().size() == 7);
    assert(graph.edges().size() == 7);

    const Connectivity connectivity(graph);
    assert(connectivity.poweredNodes().size() == 2);
    assert(connectivity.unpoweredGroups().size() == 5);
    assert(connectivity.unpoweredGroups()[0].size() == 1);
    assert(connectivity.restorationValue(graph.nodeIndex("H1")) == 10);
    assert(connectivity.restorationValue(graph.nodeIndex("R1")) == 1);

    const Dijkstra dijkstra(graph);
    const DijkstraResult distances = dijkstra.shortestRepairs();
    const size_t hospital = graph.nodeIndex("H1");
    assert(distances.distance[hospital] == 3);
    assert(dijkstra.pathTo(distances, hospital).size() == 1);
    assert(graph.edges()[dijkstra.pathTo(distances, hospital)[0]].id == "L2");

    const KruskalResult mst = Kruskal(graph).minimumRepairNetwork();
    assert(mst.connectable);
    assert(mst.totalEffort == 27);
    assert(mst.chosenEdges.size() == 6);

    const ScheduleResult schedule = Scheduler(graph).build();
    assert(!schedule.steps.empty());
    assert(graph.edges()[schedule.steps.front().edgeIndex].id == "L2");
    assert(graph.edges()[schedule.steps.front().edgeIndex].repaired == false);

    graph.edges()[1].repaired = true;
    assert(Connectivity(graph).poweredNodes().size() == 3);
    assert(Dijkstra(graph).shortestRepairs().distance[hospital] == 0);
    graph.resetLines();
    assert(graph.totalRepairedEffort() == 0);

    UnionFind sets(4);
    assert(sets.unite(0, 1));
    assert(!sets.unite(0, 1));
    assert(sets.unite(2, 3));
    assert(sets.unite(1, 2));
    assert(sets.components() == 1);
    assert(sets.find(0) == sets.find(3));

    const size_t addedNode = graph.addNode("North clinic", NodeType::Hospital, 13.0, 77.6);
    assert(graph.nodes()[addedNode].id == "N1");
    const size_t addedEdge = graph.addEdge("S1", "N1", 0);
    assert(graph.edges()[addedEdge].id == "L8");

    bool rejectedDuplicate = false;
    try {
        graph.addEdge("N1", "S1", 2);
    } catch (const invalid_argument&) {
        rejectedDuplicate = true;
    }
    assert(rejectedDuplicate);

    cout << "All C++ graph and algorithm tests passed.\n";
}
