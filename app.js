const API_BASE = "http://localhost:1337";
const USE_MOCK_DEFAULT = false;

const els = {
  q: document.getElementById('q'),
  month: document.getElementById('month'),
  searchBtn: document.getElementById('searchBtn'),
  suggestions: document.getElementById('suggestions'),
  results: document.getElementById('results'),
  ranking: document.getElementById('ranking'),
  mockToggle: document.getElementById('mockToggle'),
  status: document.getElementById('status')
};

function debounce(fn, delay = 250) {
  let t;
  return (...args) => {
    clearTimeout(t);
    t = setTimeout(() => fn(...args), delay);
  };
}

function setStatus(type, text = "") {
  els.status.className = "status " + (type || "");
  els.status.textContent = text;
}

let MOCK = null;

fetch('mock-data.json')
  .then(r => r.json())
  .then(d => { MOCK = d; init(); })
  .catch(() => { MOCK = null; init(); });

function mockAutocomplete(term) {
  if (!MOCK) return [];
  const s = term.toLowerCase();
  return (MOCK.autocomplete || []).filter(x => x.label.toLowerCase().includes(s));
}

function mockSearch(place, month) {
  const data = structuredClone(MOCK?.search?.[place] || { results: [], ranking: [] });
  if (month && data.results?.length) {
    data.results = data.results.filter(r => parseMonth(r.month) === Number(month));
  }
  return data;
}

function parseMonth(label) {
  const m = {Jan:1,Feb:2,Mar:3,Apr:4,May:5,Jun:6,Jul:7,Aug:8,Sep:9,Oct:10,Nov:11,Dec:12};
  return m[label?.slice(0,3)] || null;
}

async function apiAutocomplete(q) {
  const url = `${API_BASE}/api/autocomplete?q=${encodeURIComponent(q)}`;
  const r = await fetch(url);
  if (!r.ok) throw new Error("HTTP " + r.status);
  return await r.json();
}

async function apiSearch(place, month) {
  const url = `${API_BASE}/api/search?place=${encodeURIComponent(place)}&month=${encodeURIComponent(month)}`;
  const r = await fetch(url);
  if (!r.ok) throw new Error("HTTP " + r.status);
  return await r.json();
}

let acItems = [];
let acIndex = -1;

function renderSuggestions(list) {
  acItems = list;
  acIndex = -1;
  els.suggestions.innerHTML = '';
  if (!list.length) { els.suggestions.style.display = 'none'; return; }
  list.forEach((it, i) => {
    const li = document.createElement('li');
    li.textContent = it.label;
    li.dataset.index = i;
    li.addEventListener('mouseenter', () => highlight(i));
    li.addEventListener('mouseleave', () => highlight(-1));
    li.addEventListener('click', () => selectItem(i));
    els.suggestions.appendChild(li);
  });
  els.suggestions.style.display = 'block';
}

function highlight(i) {
  acIndex = i;
  [...els.suggestions.children].forEach((li, idx) => {
    li.classList.toggle('active', idx === i);
  });
}

function selectItem(i) {
  if (i < 0 || i >= acItems.length) return;
  els.q.value = acItems[i].label;
  els.suggestions.style.display = 'none';
  doSearch();
}

/* ---- Hilfsfunktion: verwaiste Platzhalter entfernen ---- */
function removeOrphanPlaceholders() {
  document.querySelectorAll('.chart-placeholder').forEach(node => {
    if (!node.closest('.tile')) node.remove();
  });
}

function renderResults(data) {
  els.results.innerHTML = '';

  if (!data.length) {
    const div = document.createElement('div');
    div.className = 'tile';
    div.innerHTML = '<p>No results. Try another place.</p>';
    els.results.appendChild(div);
    removeOrphanPlaceholders();
    return;
  }

  // Nur erstes Ergebnis mit History zeigen (verhindert leere zweite Box)
  data = data.filter(it => it.history && it.history.length > 0).slice(0, 1);

  if (!data.length) {
    const div = document.createElement('div');
    div.className = 'tile';
    div.innerHTML = '<p>No results with chart data.</p>';
    els.results.appendChild(div);
    removeOrphanPlaceholders();
    return;
  }

  data.forEach(it => {
    const card = document.createElement('div');
    card.className = 'tile';
    card.innerHTML = `
      <h3>${it.place}</span></h3>
      <div class="kv">
        <div><strong>7 Day Avg.</strong><br>${it.avg_temp} °F</div>
        <div><strong>Current Temp.</strong><br>${it.current_temp} °F</div>
        <div><strong>Storm Events in Month</strong><br>${it.storm_events}</div>
        <div><strong>Temp. Abnormality</strong><br>${it.deviation ?? "-"}</div>
      </div>
      <div class="chart-placeholder" id="chartArea"></div>
    `;
    els.results.appendChild(card);
    renderChart(it.history);
  });
  removeOrphanPlaceholders();
}

function renderRanking(items) {
  els.ranking.innerHTML = '';
  if (!items.length) return;
  items.forEach(it => {
    const li = document.createElement('li');
    li.textContent = `${it.place} — score ${it.score}`;
    els.ranking.appendChild(li);
  });
}

function renderChart(history = []) {
  const chart = document.getElementById('chartArea');
  if (!chart) return;
  chart.innerHTML = '';
  if (!history.length) return;

  const W = chart.clientWidth || 640;
  const H = 220;
  const leftPad = 30, rightPad = 30, topPad = 18, bottomPad = 34;

  const min = Math.min(...history);
  const max = Math.max(...history);
  const posX = i => leftPad + i * ((W - leftPad - rightPad) / (history.length - 1));
  const posY = v => topPad + (H - topPad - bottomPad) * (1 - (v - min) / (max - min || 1));
  const months = ["-7","-6","-5","-4","-3","-2","-1","Today"];

  const pts = history.map((v, i) => `${posX(i)},${posY(v)}`).join(' ');
  const svg = `
    <svg width="${W}" height="${H}" viewBox="0 0 ${W} ${H}">
      <rect x="0" y="0" width="${W}" height="${H}" rx="12" fill="#fafbff" stroke="#e3e6ef"/>
      <polyline fill="none" stroke="#111" stroke-width="2" points="${pts}"/>
      ${history.map((v, i) => `<circle cx="${posX(i)}" cy="${posY(v)}" r="2.5" fill="#111"/>`).join('')}
      ${history.map((_, i) => `<text x="${posX(i)}" y="${H - 10}" text-anchor="middle" font-size="11" fill="#555">${months[i % 12]}</text>`).join('')}
    </svg>
  `;
  chart.innerHTML = svg;
}

function displayData(data) {
  renderResults(data.results || []);
  renderRanking(data.ranking || []);
  removeOrphanPlaceholders();
}

function init() {
  els.mockToggle.checked = false;

  els.q.addEventListener('input', debounce(onType, 200));
  els.q.addEventListener('focus', () => { if (els.q.value.trim()) onType(); });
  els.searchBtn.addEventListener('click', doSearch);

  els.q.addEventListener('keydown', e => {
    if (els.suggestions.style.display !== 'block') return;
    if (e.key === 'ArrowDown') highlight(Math.min(acIndex + 1, acItems.length - 1));
    else if (e.key === 'ArrowUp') highlight(Math.max(acIndex - 1, 0));
    else if (e.key === 'Enter' && acIndex >= 0) selectItem(acIndex);
    else if (e.key === 'Escape') els.suggestions.style.display = 'none';
  });

  document.addEventListener('click', e => {
    const inside = e.target.closest('.pill');
    if (!inside) els.suggestions.style.display = 'none';
  });

  removeOrphanPlaceholders();
  setStatus("", "");
}

async function onType() {
  const term = els.q.value.trim();
  if (!term) { els.suggestions.style.display = 'none'; return; }
  const useMock = els.mockToggle.checked || !API_BASE;
  try {
    setStatus("", "");
    const items = useMock ? mockAutocomplete(term) : await apiAutocomplete(term);
    renderSuggestions(items.slice(0, 8));
  } catch {
    setStatus("error", "Autocomplete failed.");
  }
}

async function doSearch() {
  const place = els.q.value.trim();
  if (!place) return;
  const month = els.month.value;
  const useMock = els.mockToggle.checked || !API_BASE;

  setStatus("loading", "");
  try {
    const data = useMock ? mockSearch(place, month) : await apiSearch(place, month);
    displayData(data);
    setStatus("", "");
  } catch {
    setStatus("error", "Request failed. Check API_BASE or CORS.");
    renderResults([]);
    renderRanking([]);
    removeOrphanPlaceholders();
  }
}
