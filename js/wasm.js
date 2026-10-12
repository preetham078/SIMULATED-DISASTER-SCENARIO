(function () {
  function parseJson(value) {
    return JSON.parse(typeof value === 'function' ? value() : value);
  }

  window.PowerGridReady = window.createPowerGridModule().then((module) => {
    const nodesJson = module.cwrap('grid_nodes_json', 'string', []);
    const edgesJson = module.cwrap('grid_edges_json', 'string', []);
    const addNodeWasm = module.cwrap('grid_add_node', 'string', ['string', 'number', 'number', 'number']);
    const addEdgeWasm = module.cwrap('grid_add_edge', 'string', ['string', 'string', 'number']);
    const setEdgeRepairedWasm = module.cwrap('grid_set_edge_repaired', 'number', ['string', 'number']);
    const restorationValueWasm = module.cwrap('grid_restoration_value', 'number', ['string']);
    const dijkstraWasm = module.cwrap('grid_dijkstra_json', 'string', ['string']);
    const edgeType = { Substation: 0, Hospital: 1, Facility: 2, Telecom: 3, Consumer: 4 };
    const nodes = [];
    const edges = [];
    const sourceNodes = [];

    function refresh() {
      nodes.splice(0, nodes.length, ...parseJson(nodesJson));
      edges.splice(0, edges.length, ...parseJson(edgesJson));
      sourceNodes.splice(0, sourceNodes.length, ...nodes.filter((node) => node.source).map((node) => node.id));
    }

    function checkedId(result) {
      if (result.startsWith('ERROR: ')) throw new Error(result.slice(7));
      return result;
    }

    function dijkstraTo(target) {
      const result = JSON.parse(dijkstraWasm(target));
      for (const id of Object.keys(result.distance)) {
        if (result.distance[id] === null) result.distance[id] = Infinity;
      }
      return result;
    }

    refresh();
    window.GridGraph = {
      nodes,
      edges,
      sourceNodes,
      getNodeById: (id) => nodes.find((node) => node.id === id),
      getEdgeById: (id) => edges.find((edge) => edge.id === id),
      totalRepairedEffort: () => module._grid_total_repaired_effort(),
      addNode({ name, type, lat, lng }) {
        const id = checkedId(addNodeWasm(String(name).trim(), edgeType[type] ?? edgeType.Consumer, lat, lng));
        refresh();
        return nodes.find((node) => node.id === id);
      },
      addEdge({ a, b, weight }) {
        if (!Number.isInteger(weight) || weight < 0) {
          throw new Error('Choose two different existing nodes and enter a non-negative whole-number effort.');
        }
        const id = checkedId(addEdgeWasm(a, b, weight));
        refresh();
        return edges.find((edge) => edge.id === id);
      },
      setEdgeRepaired(id, repaired) {
        if (!setEdgeRepairedWasm(id, repaired ? 1 : 0)) throw new Error('The selected power line no longer exists.');
        refresh();
      },
      reset() {
        module._grid_reset();
        refresh();
      }
    };
    window.GridConnectivity = {
      poweredSet: () => new Set(parseJson(module.cwrap('grid_bfs_json', 'string', [])()).powered),
      bfsState() {
        const result = parseJson(module.cwrap('grid_bfs_json', 'string', [])());
        return { seen: new Set(result.powered), order: result.order };
      },
      unpoweredGroups: () => parseJson(module.cwrap('grid_groups_json', 'string', [])()),
      restorationValue: (id) => restorationValueWasm(id)
    };
    window.GridDijkstra = { dijkstraTo };
    window.GridKruskal = {
      minimumSpanningTree: () => parseJson(module.cwrap('grid_kruskal_json', 'string', [])())
    };
    window.GridScheduler = {
      buildRepairSchedule: () => parseJson(module.cwrap('grid_schedule_json', 'string', [])())
    };
    return true;
  });
})();
