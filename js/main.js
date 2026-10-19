window.PowerGridReady.then(() => {
  const { nodes, edges, sourceNodes, getNodeById, getEdgeById, totalRepairedEffort } = window.GridGraph;

    const floodZones = [
      { id: 'low', severity: 'Low', fillColor: '#34d399', opacity: 0.20, coords: [[12.992, 77.530], [13.004, 77.548], [12.998, 77.667], [12.977, 77.648], [12.965, 77.596], [12.970, 77.548]] },
      { id: 'medium', severity: 'Medium', fillColor: '#facc15', opacity: 0.24, coords: [[12.965, 77.515], [12.995, 77.520], [13.010, 77.690], [12.980, 77.706], [12.944, 77.650], [12.938, 77.560]] },
      { id: 'severe', severity: 'Severe', fillColor: '#f97316', opacity: 0.30, coords: [[12.955, 77.551], [12.979, 77.545], [12.980, 77.635], [12.962, 77.675], [12.935, 77.643], [12.926, 77.580]] },
      { id: 'extreme', severity: 'Extreme', fillColor: '#1d4ed8', opacity: 0.34, coords: [[12.943, 77.572], [12.963, 77.560], [12.963, 77.646], [12.941, 77.670], [12.916, 77.642], [12.914, 77.585]] }
    ];

    const map = L.map('map', { zoomControl: true, scrollWheelZoom: false, preferCanvas: true });
    map.fitBounds(nodes.map((node) => [node.lat, node.lng]), { padding: [35, 35], maxZoom: 12 });
    const baseLayer = L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
      maxZoom: 19,
      attribution: '&copy; OpenStreetMap contributors'
    });
    baseLayer.addTo(map);

    const floodLayer = L.layerGroup().addTo(map);
    const lineLayer = L.layerGroup().addTo(map);
    const markerLayer = L.layerGroup().addTo(map);
    const flowRenderer = L.svg({ padding: 0.5 });

    const state = {
      activeTab: 'overview',
      floodEnabled: true,
      floodSeverity: 'Medium',
      highlightIds: [],
      dijkstraTarget: 'H1',
      dispatchMessage: 'All critical power lines are currently damaged; dispatch your restoration team.'
    };

    function formatEdge(edge) {
      return `${edge.a}–${edge.b} (${edge.weight})`;
    }

    function escapeHTML(value) {
      return String(value).replace(/[&<>"']/g, (character) => ({
        '&': '&amp;',
        '<': '&lt;',
        '>': '&gt;',
        '"': '&quot;',
        "'": '&#39;'
      })[character]);
    }

    function toast(text) {
      const banner = document.createElement('div');
      banner.style.position = 'fixed';
      banner.style.bottom = '20px';
      banner.style.left = '50%';
      banner.style.transform = 'translateX(-50%)';
      banner.style.background = '#ffffff';
      banner.style.border = '1px solid #d1d5db';
      banner.style.boxShadow = '0 8px 24px rgba(15, 23, 42, 0.14)';
      banner.style.color = '#1f2937';
      banner.style.borderRadius = '8px';
      banner.style.padding = '10px 16px';
      banner.style.zIndex = '1200';
      banner.style.fontSize = '0.8rem';
      banner.textContent = text;
      document.body.appendChild(banner);
      setTimeout(() => banner.remove(), 2200);
    }

    function updateSummary() {
      const connected = GridConnectivity.poweredSet();
      const criticalDown = nodes.filter((node) => {
        const priority = node.priority ?? 0;
        return priority >= 6 && !connected.has(node.id);
      }).length;

      document.getElementById('servedText').textContent = `${connected.size}/${nodes.length}`;
      document.getElementById('criticalText').textContent = String(criticalDown);
      document.getElementById('repairText').textContent = `${edges.filter((edge) => edge.repaired).length}/${edges.length}`;
      document.getElementById('effortText').textContent = String(totalRepairedEffort());
      document.getElementById('scenarioBadge').textContent = `Flood severity: ${state.floodSeverity.toLowerCase()}`;
    }

    function updateFaultState() {
      const floodState = {
        Low: 'moderate disruption',
        Medium: 'critical disruption',
        Severe: 'severe disruption',
        Extreme: 'extreme flood emergency'
      };
      document.getElementById('scenarioBadge').className = `tag ${state.floodSeverity === 'Extreme' ? 'danger' : 'warning'}`;
      document.getElementById('scenarioBadge').textContent = `Flood severity: ${state.floodSeverity.toLowerCase()} • ${floodState[state.floodSeverity]}`;
    }

    function setFloodSeverity(value) {
      state.floodSeverity = value;
      updateFloodLayer();
      updateFaultState();
      updateSummary();
    }

    function updateFloodLayer() {
      floodLayer.clearLayers();
      if (!state.floodEnabled) return;

      const levels = ['Low', 'Medium', 'Severe', 'Extreme'];
      const index = levels.indexOf(state.floodSeverity);
      const visibleZones = levels.slice(0, index + 1);

      for (const zone of floodZones) {
        if (visibleZones.includes(zone.severity)) {
          L.polygon(zone.coords, {
            color: zone.fillColor,
            fillColor: zone.fillColor,
            fillOpacity: zone.opacity,
            weight: 1.2,
            opacity: 0.7,
            interactive: false
          }).addTo(floodLayer);
        }
      }
    }

    function renderMapLines() {
      lineLayer.clearLayers();
      const highlightSet = new Set(state.highlightIds);
      const connected = GridConnectivity.poweredSet();

      for (const edge of edges) {
        const a = getNodeById(edge.a);
        const b = getNodeById(edge.b);
        const latlngs = [
          [a.lat, a.lng],
          [b.lat, b.lng]
        ];

        const lineColor = edge.repaired ? '#15803d' : highlightSet.has(edge.id) ? '#d97706' : '#dc2626';
        const lineWeight = highlightSet.has(edge.id) ? 7 : edge.repaired ? 4 : 3.5;

        const polyline = L.polyline(latlngs, {
          color: lineColor,
          weight: lineWeight,
          opacity: edge.repaired ? 0.9 : 0.85,
          dashArray: edge.repaired ? null : '8 10',
          lineCap: 'round',
          lineJoin: 'round'
        }).bindPopup(`
          <strong>${escapeHTML(edge.id)}</strong>
          <div class="meta">${escapeHTML(a.name)} → ${escapeHTML(b.name)}</div>
          <div class="meta">Repair effort: ${edge.weight}</div>
          <div class="meta">Damage: ${edge.damage}</div>
          <div class="meta">Status: ${edge.repaired ? 'Repaired' : 'Damaged'}</div>
        `);

        if (edge.repaired && connected.has(a.id) && connected.has(b.id)) {
          L.polyline(latlngs, {
            renderer: flowRenderer,
            color: '#bbf7d0',
            weight: 4,
            opacity: 0.95,
            dashArray: '2 18',
            lineCap: 'round',
            className: 'power-flow',
            interactive: false
          }).addTo(lineLayer);
        }

        polyline.on('click', () => {
          if (!edge.repaired) repairEdge(edge.id);
        });
        polyline.addTo(lineLayer);

        if (!edge.repaired && state.floodSeverity === 'Extreme' && edge.damage === 'Critical') {
          const mid = L.latLng((a.lat + b.lat) / 2, (a.lng + b.lng) / 2);
          L.circle(mid, {
            radius: 350,
            color: '#1d4ed8',
            fillColor: '#1d4ed8',
            fillOpacity: 0.22,
            weight: 1,
            interactive: false
          }).addTo(lineLayer);
        }
      }

      markerLayer.clearLayers();
      for (const node of nodes) {
        const isPowered = connected.has(node.id);
        const color = isPowered ? '#15803d' : '#dc2626';
        const marker = L.circleMarker([node.lat, node.lng], {
          radius: 13,
          color: '#ffffff',
          weight: 2,
          fillColor: color,
          fillOpacity: 0.9
        }).bindPopup(`
          <strong>${escapeHTML(node.name)}</strong>
          <div class="meta">Type: ${escapeHTML(node.type)}</div>
          <div class="meta">Status: ${isPowered ? 'Online' : 'Offline'}</div>
          <div class="meta">Voltage: ${node.voltage || '11 kV'}</div>
          <div class="meta">Load: ${node.load || 0}%</div>
          <div class="meta">Affected customers: ${node.customers || 0}</div>
        `);
        marker.addTo(markerLayer);

        const label = L.marker([node.lat + 0.0027, node.lng + 0.0024], {
          icon: L.divIcon({
            className: 'node-label',
            html: `<div style="background: rgba(255,255,255,0.94); border: 1px solid #d1d5db; color: #1f2937; padding: 4px 7px; border-radius: 999px; font-size: 10px; font-weight: 700; white-space: nowrap;">${node.id}</div>`,
            iconSize: [36, 20],
            iconAnchor: [18, 10]
          })
        }).addTo(markerLayer);
        label.bindPopup(`
          <strong>${escapeHTML(node.name)}</strong>
          <div class="meta">Priority: ${node.priority || 'N/A'}</div>
        `);
      }
    }

    function renderPanel() {
      const panel = document.getElementById('panelContent');
      const connected = GridConnectivity.poweredSet();
      const dijkstraTarget = getNodeById(state.dijkstraTarget) ? state.dijkstraTarget : nodes.find((node) => !node.source)?.id;
      state.dijkstraTarget = dijkstraTarget || sourceNodes[0];

      if (state.activeTab === 'overview') {
        const repairPlan = GridScheduler.buildRepairSchedule().schedule;
        const bestPath = GridDijkstra.dijkstraTo('H1');
        panel.innerHTML = `
          <div class="panel-group">
            <h3>Operational view</h3>
            <div class="info-block">
              <p>${escapeHTML(state.dispatchMessage)}</p>
            </div>
            <div class="row"><span>Repair strategy</span><b>Priority-first restoration</b></div>
            <div class="row"><span>Current power nodes</span><b>${Array.from(connected).join(', ')}</b></div>
            <div class="row"><span>Hospital path</span><b>${bestPath.path.length ? bestPath.path.map((edge) => edge.id).join(' → ') : 'Already restored'}</b></div>
            <div class="row"><span>Best effort to hospital</span><b>${bestPath.distance.H1 === Infinity ? '∞' : bestPath.distance.H1}</b></div>
          </div>
          <div class="panel-group">
            <h3>Recommended immediate action</h3>
            <div class="info-block">
              <p>${repairPlan.length ? `${escapeHTML(repairPlan[0].edge.id)} is the highest-impact next repair because it restores value ${repairPlan[0].gain} for ${repairPlan[0].edge.weight} effort.` : 'No immediate gain is available under the current topology.'}</p>
            </div>
            <div class="row"><span>Next line</span><b>${repairPlan.length ? escapeHTML(repairPlan[0].edge.id) : 'None'}</b></div>
            <div class="row"><span>Newly powered value</span><b>${repairPlan.length ? repairPlan[0].gain : 0}</b></div>
            <div class="row"><span>Repair effort</span><b>${repairPlan.length ? repairPlan[0].edge.weight : 0}</b></div>
            <div class="row"><span>Estimated time</span><b>${repairPlan.length ? `${repairPlan[0].edge.repairTime} hours` : 'N/A'}</b></div>
          </div>
        `;
      }

      if (state.activeTab === 'bfs') {
        const bfs = GridConnectivity.bfsState();
        const groups = GridConnectivity.unpoweredGroups();
        panel.innerHTML = `
          <div class="panel-group">
            <h3>BFS power graph</h3>
            <div class="info-block"><p>BFS expands simultaneously from the two energized substations and only follows repaired lines.</p></div>
            <div class="row"><span>Visit order</span><b>${bfs.order.join(' → ')}</b></div>
            <div class="row"><span>Powered nodes</span><b>${Array.from(bfs.seen).join(', ')}</b></div>
            <div class="row"><span>Critical facilities offline</span><b>${nodes.filter((node) => GridConnectivity.restorationValue(node.id) >= 6 && !connected.has(node.id)).length}</b></div>
          </div>
          <div class="panel-group">
            <h3>DFS unpowered groups</h3>
            <div class="info-block"><p>Depth-first search groups nodes that are still disconnected from every substation through repaired lines.</p></div>
            ${groups.map((group, index) => `<div class="row"><span>Group ${index + 1}</span><b>${group.join(', ')}</b></div>`).join('') || '<div class="row"><span>Unpowered groups</span><b>None</b></div>'}
          </div>
        `;
      }

      if (state.activeTab === 'dijkstra') {
        const results = GridDijkstra.dijkstraTo(dijkstraTarget);
        panel.innerHTML = `
          <div class="panel-group">
            <h3>Dijkstra — cheapest repair path</h3>
            <div class="info-block"><p>Finds the lowest-effort route from any substation. Repaired lines have zero additional repair cost.</p></div>
            <label class="editor-form">Destination
              <select id="dijkstraTarget">
                ${nodes.filter((node) => !node.source).map((node) => `<option value="${escapeHTML(node.id)}" ${node.id === dijkstraTarget ? 'selected' : ''}>${escapeHTML(node.name)} (${escapeHTML(node.id)})</option>`).join('')}
              </select>
            </label>
            <div class="row"><span>Best total effort</span><b>${results.distance[dijkstraTarget] === Infinity ? '∞' : results.distance[dijkstraTarget]}</b></div>
            <div class="row"><span>Lines to repair</span><b>${results.path.length ? results.path.map((edge) => edge.id).join(' → ') : 'No incomplete route is necessary'}</b></div>
            <table>
              <thead><tr><th>Node</th><th>Distance</th></tr></thead>
              <tbody>
                ${nodes.map((node) => `<tr><td>${escapeHTML(node.name)}</td><td>${results.distance[node.id] === Infinity ? '∞' : results.distance[node.id]}</td></tr>`).join('')}
              </tbody>
            </table>
          </div>
        `;
      }

      if (state.activeTab === 'kruskal') {
        const mst = GridKruskal.minimumSpanningTree();
        panel.innerHTML = `
          <div class="panel-group">
            <h3>Kruskal minimum spanning tree</h3>
            <div class="info-block"><p>Union-Find with path compression and rank skips cycles. Existing repaired links are included at no additional effort.</p></div>
            <div class="row"><span>Grid connected</span><b>${mst.connectable ? 'Yes' : 'No — disconnected components remain'}</b></div>
            <div class="row"><span>Additional repair effort</span><b>${mst.total}</b></div>
            ${mst.chosen.map((edge, index) => `<div class="row"><span>${index + 1}. ${formatEdge(edge)}</span><span class="tag">accept</span></div>`).join('') || '<div class="row"><span>No accepted links</span><b>—</b></div>'}
          </div>
        `;
      }

      if (state.activeTab === 'schedule') {
        const schedule = GridScheduler.buildRepairSchedule();
        panel.innerHTML = `
          <div class="panel-group">
            <h3>Priority repair schedule</h3>
            <div class="info-block"><p>Each step uses a max-heap ranked by newly powered value per unit of effort. The network is re-evaluated after every simulated repair.</p></div>
            <div class="row"><span>Repairs that restore service</span><b>${schedule.schedule.length}</b></div>
            <div class="row"><span>Total repair effort</span><b>${schedule.totalEffort}</b></div>
            ${schedule.schedule.map((step, index) => `<div class="row"><span>${index + 1}. ${escapeHTML(step.edge.id)} — ${escapeHTML(step.edge.a)} to ${escapeHTML(step.edge.b)}<br><small>Gain ${step.gain} / effort ${step.edge.weight}</small></span><span class="tag">${(step.gain / Math.max(1, step.edge.weight)).toFixed(2)}</span></div>`).join('') || '<div class="row"><span>No remaining repair restores new service.</span></div>'}
            <button id="scheduledRepairBtn" class="primary" ${schedule.schedule.length ? '' : 'disabled'}>Carry out next scheduled repair</button>
          </div>
        `;
      }

      if (state.activeTab === 'grid') {
        const nodeOptions = nodes.map((node) => `<option value="${escapeHTML(node.id)}">${escapeHTML(node.id)} — ${escapeHTML(node.name)}</option>`).join('');
        panel.innerHTML = `
          <div class="panel-group">
            <h3>Grid nodes (${nodes.length})</h3>
            ${nodes.map((node) => `<div class="row"><span>${escapeHTML(node.id)} — ${escapeHTML(node.name)}<br><small>${escapeHTML(node.type)}</small></span><span class="tag ${connected.has(node.id) ? '' : 'danger'}">${connected.has(node.id) ? 'powered' : 'offline'}</span></div>`).join('')}
          </div>
          <div class="panel-group">
            <h3>Add a grid node</h3>
            <form id="addNodeForm" class="editor-form">
              <label>Name<input name="name" maxlength="60" required placeholder="e.g. North clinic"></label>
              <label>Facility type
                <select name="type">
                  <option value="Substation">Substation</option>
                  <option value="Hospital">Hospital</option>
                  <option value="Facility">Water treatment</option>
                  <option value="Telecom">Telecom</option>
                  <option value="Consumer" selected>Consumer</option>
                </select>
              </label>
              <div class="form-row">
                <label>Latitude<input name="lat" type="number" min="-90" max="90" step="any" value="12.95" required></label>
                <label>Longitude<input name="lng" type="number" min="-180" max="180" step="any" value="77.60" required></label>
              </div>
              <button class="primary" type="submit">Add node</button>
            </form>
          </div>
          <div class="panel-group">
            <h3>Add a damaged power line</h3>
            <form id="addEdgeForm" class="editor-form">
              <div class="form-row">
                <label>From<select name="a">${nodeOptions}</select></label>
                <label>To<select name="b">${nodeOptions}</select></label>
              </div>
              <label>Repair effort<input name="weight" type="number" min="0" step="1" value="3" required></label>
              <button class="primary" type="submit">Add power line</button>
            </form>
          </div>
          <div class="panel-group">
            <h3>Grid lines (${edges.length})</h3>
            <div class="edge-list">
              ${edges.map((edge) => `<div class="info-block edge-item"><span><b>${escapeHTML(edge.id)}</b> ${escapeHTML(edge.a)}–${escapeHTML(edge.b)} · effort ${edge.weight}<br><small>${edge.repaired ? 'Repaired' : 'Damaged'}</small></span><button class="${edge.repaired ? 'danger' : 'ghost'}" data-edge-action="${edge.repaired ? 'damage' : 'repair'}" data-edge-id="${escapeHTML(edge.id)}">${edge.repaired ? 'Report damage' : 'Repair'}</button></div>`).join('')}
            </div>
          </div>
        `;
      }
    }

    function repairEdge(edgeId) {
      const edge = getEdgeById(edgeId);
      if (!edge) {
        toast('The selected power line no longer exists.');
        return;
      }
      if (edge.repaired) {
        toast('This line is already repaired.');
        return;
      }

      GridGraph.setEdgeRepaired(edgeId, true);
      state.dispatchMessage = `${edge.id} repaired. Control center re-evaluated the restoration plan for the affected service area.`;
      state.highlightIds = [edgeId];
      toast(`${edge.id} repaired`);
      renderPanel();
      renderMapLines();
      updateSummary();
      window.setTimeout(() => {
        state.highlightIds = [];
        renderMapLines();
      }, 1200);
    }

    function repairRecommendedLine() {
      const plan = GridScheduler.buildRepairSchedule().schedule;
      if (!plan.length) {
        toast('No further repair yields immediate benefit in the current scenario.');
        return;
      }
      repairEdge(plan[0].edge.id);
    }

    function repairHospitalPath() {
      const path = GridDijkstra.dijkstraTo('H1').path;
      const edge = path.find((candidate) => !candidate.repaired);
      if (!edge) {
        toast('Hospital route is already fully restored.');
        return;
      }
      repairEdge(edge.id);
    }

    function highlightMST() {
      const chosen = GridKruskal.minimumSpanningTree().chosen.map((edge) => edge.id);
      state.highlightIds = chosen;
      state.activeTab = 'kruskal';
      document.querySelectorAll('.tab').forEach((btn) => btn.classList.toggle('active', btn.dataset.tab === 'kruskal'));
      renderPanel();
      renderMapLines();
    }

    function randomDamage() {
      const damaged = edges.filter((edge) => edge.repaired);
      if (!damaged.length) {
        toast('No repaired line is available to damage.');
        return;
      }
      const target = damaged[Math.floor(Math.random() * damaged.length)];
      damageEdge(target.id);
    }

    function damageEdge(edgeId) {
      const edge = getEdgeById(edgeId);
      if (!edge) {
        toast('The selected power line no longer exists.');
        return;
      }
      if (!edge.repaired) {
        toast('This line is already damaged.');
        return;
      }
      GridGraph.setEdgeRepaired(edgeId, false);
      state.dispatchMessage = `Storm damage reported on ${edge.id}; field crews are rerouting emergency restoration work.`;
      toast(`${edge.id} marked damaged`);
      renderPanel();
      renderMapLines();
      updateSummary();
    }

    function addGridNode(form) {
      const formData = new FormData(form);
      const type = String(formData.get('type') || 'Consumer');
      const lat = Number(formData.get('lat'));
      const lng = Number(formData.get('lng'));
      let node;
      try {
        node = GridGraph.addNode({
          name: String(formData.get('name') || ''),
          type,
          lat,
          lng
        });
      } catch (error) {
        toast(error.message);
        return;
      }
      state.dijkstraTarget = node.id;
      state.dispatchMessage = `${node.name} added to the grid. Add power lines to connect it to the network.`;
      const bounds = L.latLngBounds(nodes.map((item) => [item.lat, item.lng]));
      map.fitBounds(bounds, { padding: [35, 35], maxZoom: 12 });
      renderPanel();
      renderMapLines();
      updateSummary();
      toast(`${node.id} added to the grid`);
    }

    function addGridEdge(form) {
      const formData = new FormData(form);
      const a = String(formData.get('a') || '');
      const b = String(formData.get('b') || '');
      const weight = Number(formData.get('weight'));
      let edge;
      try {
        edge = GridGraph.addEdge({ a, b, weight });
      } catch (error) {
        toast(error.message);
        return;
      }
      state.dispatchMessage = `${edge.id} added as a damaged line with effort ${weight}.`;
      renderPanel();
      renderMapLines();
      updateSummary();
      toast(`${edge.id} added to the grid`);
    }

    function resetScenario() {
      GridGraph.reset();
      state.highlightIds = [];
      state.dispatchMessage = 'All critical power lines are currently damaged; dispatch your restoration team.';
      setFloodSeverity('Medium');
      renderPanel();
      renderMapLines();
      updateSummary();
      toast('Scenario reset');
    }

    function bindTabs() {
      document.querySelectorAll('.tab').forEach((button) => {
        button.addEventListener('click', () => {
          state.activeTab = button.dataset.tab;
          document.querySelectorAll('.tab').forEach((el) => el.classList.toggle('active', el === button));
          renderPanel();
        });
      });
      const panel = document.getElementById('panelContent');
      panel.addEventListener('change', (event) => {
        if (event.target.id === 'dijkstraTarget') {
          state.dijkstraTarget = event.target.value;
          renderPanel();
        }
      });
      panel.addEventListener('click', (event) => {
        if (!(event.target instanceof Element)) return;
        const edgeButton = event.target.closest('[data-edge-action]');
        if (edgeButton) {
          if (edgeButton.dataset.edgeAction === 'repair') repairEdge(edgeButton.dataset.edgeId);
          else damageEdge(edgeButton.dataset.edgeId);
          return;
        }
        if (event.target.closest('#scheduledRepairBtn')) repairRecommendedLine();
      });
      panel.addEventListener('submit', (event) => {
        if (!(event.target instanceof HTMLFormElement)) return;
        event.preventDefault();
        if (event.target.id === 'addNodeForm') addGridNode(event.target);
        if (event.target.id === 'addEdgeForm') addGridEdge(event.target);
      });
    }

    function initControls() {
      document.getElementById('toggleFloodBtn').addEventListener('click', () => {
        state.floodEnabled = !state.floodEnabled;
        document.getElementById('toggleFloodBtn').textContent = `Flood Layer: ${state.floodEnabled ? 'ON' : 'OFF'}`;
        updateFloodLayer();
      });

      document.getElementById('severitySelect').addEventListener('change', (event) => {
        setFloodSeverity(event.target.value);
      });

      document.getElementById('nextRepairBtn').addEventListener('click', repairRecommendedLine);
      document.getElementById('hospitalPathBtn').addEventListener('click', repairHospitalPath);
      document.getElementById('mstBtn').addEventListener('click', highlightMST);
      document.getElementById('damageBtn').addEventListener('click', randomDamage);
      document.getElementById('resetBtn').addEventListener('click', resetScenario);
    }

    function validateMapAvailability() {
      const overlay = document.getElementById('fallbackOverlay');
      if (!navigator.onLine) {
        overlay.classList.add('visible');
      }
      window.addEventListener('offline', () => overlay.classList.add('visible'));
      window.addEventListener('online', () => overlay.classList.remove('visible'));
    }

      const mapResizeObserver = new ResizeObserver(() => {
        map.invalidateSize({ pan: false });
      });
      mapResizeObserver.observe(document.querySelector('.map-frame'));
      window.addEventListener('resize', () => map.invalidateSize({ pan: false }));

      bindTabs();
      initControls();
      setFloodSeverity('Medium');
      renderPanel();
      renderMapLines();
      updateSummary();
      validateMapAvailability();
      requestAnimationFrame(() => map.invalidateSize({ pan: false }));
}).catch((error) => {
  console.error('Unable to initialize the C++ WebAssembly grid:', error);
  const overlay = document.getElementById('fallbackOverlay');
  overlay.classList.add('visible');
  overlay.innerHTML = '<div><div style="font-size:1.1rem; margin-bottom:8px; font-weight:700; color: var(--text);">Grid engine unavailable</div><div>Serve this page over HTTP and build the C++ WebAssembly module using <code>cpp/build-wasm.sh</code>.</div></div>';
});
