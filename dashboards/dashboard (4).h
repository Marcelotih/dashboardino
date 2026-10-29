// Página do dashboard servida pelo ESP32 (fica gravada na flash).
// Não usa nenhuma biblioteca externa/CDN: funciona mesmo sem internet
// (por exemplo, no modo Access Point "VagaInteligente").
#pragma once

const char DASHBOARD_HTML[] PROGMEM = R"rawliteral(
<!doctype html>
<html lang="pt-BR">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Vaga Inteligente</title>
<style>
  :root{
    --bg:#0f1216; --card:#181d23; --borda:#262d36; --txt:#e8edf2; --sub:#8b96a3;
    --verde:#22c55e; --amarelo:#facc15; --vermelho:#ef4444; --cinza:#4b5563; --azul:#3b82f6;
  }
  *{box-sizing:border-box}
  body{margin:0;background:var(--bg);color:var(--txt);font-family:system-ui,-apple-system,Segoe UI,Roboto,sans-serif}
  .wrap{max-width:960px;margin:0 auto;padding:16px}
  header{display:flex;justify-content:space-between;align-items:center;gap:12px;margin-bottom:16px;flex-wrap:wrap}
  h1{font-size:20px;margin:0}
  .con{font-size:13px;color:var(--sub);display:flex;align-items:center;gap:6px}
  .dot{width:9px;height:9px;border-radius:50%;background:var(--cinza)}
  .dot.on{background:var(--verde)}
  .grid{display:grid;gap:12px;grid-template-columns:repeat(12,1fr)}
  .card{background:var(--card);border:1px solid var(--borda);border-radius:14px;padding:16px}
  .s12{grid-column:span 12}.s8{grid-column:span 8}.s4{grid-column:span 4}.s3{grid-column:span 3}.s6{grid-column:span 6}
  @media(max-width:720px){.s8,.s4,.s6{grid-column:span 12}.s3{grid-column:span 6}}
  .lbl{font-size:12px;color:var(--sub);text-transform:uppercase;letter-spacing:.06em;margin-bottom:6px}
  .hero{display:flex;align-items:flex-end;justify-content:space-between;gap:16px;flex-wrap:wrap}
  .dist{font-size:64px;font-weight:700;line-height:1;font-variant-numeric:tabular-nums}
  .dist small{font-size:22px;color:var(--sub);font-weight:500}
  .zona{font-size:15px;font-weight:600;padding:8px 14px;border-radius:999px;background:var(--cinza);color:#111}
  .barra{position:relative;height:46px;margin-top:22px;border-radius:10px;overflow:hidden;display:flex;border-left:6px solid #9ca3af}
  .barra div{height:100%}
  .carro{position:absolute;top:50%;transform:translate(-50%,-50%);font-size:28px;transition:left .25s linear;filter:drop-shadow(0 2px 3px #000)}
  .escala{display:flex;justify-content:space-between;font-size:11px;color:var(--sub);margin-top:4px}
  .sonar{width:100%;max-width:230px;display:block;margin:0 auto}
  .sonar path{fill:none;stroke:#2a313a;stroke-width:7;stroke-linecap:round;transition:stroke .15s,opacity .08s}
  .sonar path.on{opacity:.55}
  .sonar.flash path.on,.sonar.cont path.on{opacity:1;filter:drop-shadow(0 0 5px currentColor)}
  .sonar.cont path.on{animation:pulsa .5s ease-in-out infinite}
  @keyframes pulsa{50%{opacity:.6}}
  .estado{text-align:center;font-size:14px;font-weight:600;margin:6px 0 12px;min-height:20px}
  .som{font-weight:600}
  .som.on{background:var(--verde);border-color:var(--verde);color:#062b14}
  input[type=range]{padding:0;accent-color:var(--azul)}
  .leds{display:flex;justify-content:space-around;align-items:center}
  .led{width:34px;height:34px;border-radius:50%;opacity:.15;transition:opacity .1s,box-shadow .1s}
  .led.on{opacity:1}
  .led.v{background:var(--verde)}.led.v.on{box-shadow:0 0 24px var(--verde)}
  .led.a{background:var(--amarelo)}.led.a.on{box-shadow:0 0 24px var(--amarelo)}
  .led.r{background:var(--vermelho)}.led.r.on{box-shadow:0 0 24px var(--vermelho)}
  .stat{font-size:26px;font-weight:700;font-variant-numeric:tabular-nums}
  canvas{width:100%;height:200px;display:block}
  .btns{display:flex;gap:8px;flex-wrap:wrap}
  button{background:#232a33;color:var(--txt);border:1px solid var(--borda);border-radius:10px;padding:10px 14px;font-size:14px;cursor:pointer}
  button:hover{border-color:var(--sub)}
  button.ativo{background:var(--azul);border-color:var(--azul)}
  button.perigo{color:var(--vermelho)}
  form{display:grid;grid-template-columns:repeat(auto-fit,minmax(150px,1fr));gap:12px;align-items:end}
  label{font-size:13px;color:var(--sub);display:flex;flex-direction:column;gap:6px}
  input{background:#0f1216;color:var(--txt);border:1px solid var(--borda);border-radius:8px;padding:9px;font-size:15px;width:100%}
  .msg{font-size:13px;margin-top:10px;min-height:18px}
  footer{font-size:12px;color:var(--sub);text-align:center;margin-top:16px}
</style>
</head>
<body>
<div class="wrap">
  <header>
    <h1>🚗 Vaga Inteligente</h1>
    <div class="con">
      <button class="som" id="btnSom">🔇 Ativar som</button>
      <span class="dot" id="dot"></span><span id="conTxt">Conectando…</span>
    </div>
  </header>

  <div class="grid">
    <div class="card s12" id="simCard" hidden>
      <div id="simOff">
        <div class="lbl">ESP32 não encontrado</div>
        <p style="margin:0 0 12px;color:var(--sub);font-size:14px">
          Não consegui falar com <b id="espUrl"></b>. Conecte o celular/PC na rede
          <b>VagaInteligente</b> — ou teste o dashboard sem o ESP32:
        </p>
        <button id="btnSim">▶ Simular sem ESP32</button>
      </div>
      <div id="simOn" hidden>
        <div class="lbl">Modo simulação — arraste para "aproximar o carro"</div>
        <input type="range" id="simDist" min="0" max="260" value="230">
        <div class="btns" style="margin-top:10px"><button id="btnSimParar">■ Parar simulação</button></div>
      </div>
    </div>

    <div class="card s8">
      <div class="lbl">Distância até a parede</div>
      <div class="hero">
        <div class="dist" id="dist">--<small> cm</small></div>
        <div class="zona" id="zona">—</div>
      </div>
      <div class="barra" id="barra">
        <div id="bR" style="background:var(--vermelho)"></div>
        <div id="bA" style="background:var(--amarelo)"></div>
        <div id="bV" style="background:var(--verde)"></div>
        <div id="bC" style="background:var(--cinza);flex:1"></div>
        <span class="carro" id="carro" style="left:100%">🚗</span>
      </div>
      <div class="escala"><span>Parede</span><span id="escMax">--</span></div>
    </div>

    <div class="card s4">
      <div class="lbl">Sensor de ré</div>
      <svg class="sonar" id="sonar" viewBox="0 0 200 190">
        <rect x="10" y="2" width="180" height="9" rx="2" fill="#4b5563"/>
        <g id="arcos"></g>
        <!-- carro visto de cima, de ré para a parede -->
        <rect x="58" y="128" width="10" height="20" rx="3" fill="#111"/>
        <rect x="132" y="128" width="10" height="20" rx="3" fill="#111"/>
        <rect x="64" y="116" width="72" height="90" rx="18" fill="#f97316"/>
        <rect x="66" y="117" width="12" height="5" rx="2" fill="#ef4444"/>
        <rect x="122" y="117" width="12" height="5" rx="2" fill="#ef4444"/>
        <rect x="74" y="128" width="52" height="16" rx="5" fill="#1f2937"/>
        <rect x="74" y="150" width="52" height="40" rx="7" fill="#ea580c"/>
      </svg>
      <div class="estado" id="estado">—</div>
      <div class="leds">
        <div class="led v" id="lv"></div>
        <div class="led a" id="la"></div>
        <div class="led r" id="lr"></div>
      </div>
    </div>

    <div class="card s3"><div class="lbl">Estacionamentos</div><div class="stat" id="sTotal">--</div></div>
    <div class="card s3"><div class="lbl">Menor distância</div><div class="stat" id="sMenor">--</div></div>
    <div class="card s3"><div class="lbl">Ligado há</div><div class="stat" id="sUp">--</div></div>
    <div class="card s3"><div class="lbl">Aparelhos conectados</div><div class="stat" id="sRssi">--</div></div>

    <div class="card s12">
      <div class="lbl">Histórico (últimos 60 s)</div>
      <canvas id="graf"></canvas>
    </div>

    <div class="card s6">
      <div class="lbl">Modo dos LEDs</div>
      <div class="btns">
        <button data-m="auto">Automático</button>
        <button data-m="teste">Teste de LEDs</button>
        <button data-m="desligado">Desligado</button>
      </div>
      <div class="lbl" style="margin-top:16px">Volume do apito</div>
      <input type="range" id="vol" min="0.05" max="1" step="0.05" value="0.6">
      <div class="lbl" style="margin-top:16px">Estatísticas</div>
      <button class="perigo" id="btnReset">Zerar contagem</button>
    </div>

    <div class="card s6">
      <div class="lbl">Zonas (cm) — salvas no ESP32</div>
      <form id="form">
        <label>🔴 Parada (abaixo de)<input type="number" name="parada" min="2" max="400"></label>
        <label>🟡 Atenção (até)<input type="number" name="livre" min="3" max="400"></label>
        <label>🟢 Livre (até)<input type="number" name="max" min="4" max="400"></label>
        <label>Pisca (ms)<input type="number" name="pisca" min="50" max="2000"></label>
        <button type="submit">Salvar</button>
      </form>
      <div class="msg" id="msg"></div>
    </div>
  </div>
  <footer id="rodape"></footer>
</div>

<script>
const $ = id => document.getElementById(id);
const ZONAS = {
  sem_leitura:{t:'Sem leitura',      c:'var(--cinza)'},
  longe:      {t:'Longe demais',     c:'var(--cinza)'},
  livre:      {t:'Livre — pode vir', c:'var(--verde)'},
  atencao:    {t:'Atenção — devagar',c:'var(--amarelo)'},
  parada:     {t:'PARE!',            c:'var(--vermelho)'}
};
const historico = [];          // {t, d}
let cfg = null, formPreenchido = false, falhas = 0;

// Endereço do ESP32. Quando a página é servida pelo próprio ESP32 fica vazio
// (mesmo endereço). Aberta pelo VS Code / arquivo local, usa http://192.168.4.1
// (ou o que vier em ?esp=http://IP na URL).
const ESP = new URLSearchParams(location.search).get('esp')
  || (location.hostname === '192.168.4.1' || location.hostname.endsWith('.local') ? '' : 'http://192.168.4.1');
const api = p => fetch(ESP + p, {cache:'no-store'});
$('espUrl').textContent = ESP || location.origin;

// ---------- Simulação (testar o dashboard sem ESP32) ----------
let sim = null;
function iniciarSim(){
  sim = {modo:'auto', config:{max:200, livre:100, parada:40, pisca:150},
         total:0, menor:-1, armado:true, inicio:Date.now()};
  formPreenchido = false;
  $('simOff').hidden = true; $('simOn').hidden = false; $('simCard').hidden = false;
}
function pararSim(){
  sim = null; falhas = 0; formPreenchido = false;
  $('simOff').hidden = false; $('simOn').hidden = true; $('simCard').hidden = true;
}
function statusSimulado(){
  const c = sim.config;
  $('simDist').max = Math.round(c.max * 1.3);   // régua acompanha as zonas
  const x = +$('simDist').value;
  const zona = x > c.max ? 'longe' : x > c.livre ? 'livre' : x > c.parada ? 'atencao' : 'parada';
  if (zona === 'livre' || zona === 'longe') sim.armado = true;
  else if (zona === 'parada' && sim.armado){ sim.armado = false; sim.total++; }
  if (sim.menor < 0 || x < sim.menor) sim.menor = x;
  const pisca = Math.floor(Date.now() / c.pisca) % 2 === 0, passo = Math.floor(Date.now()/500) % 3;
  let leds = {verde: zona==='livre', amarelo: zona==='atencao', vermelho: zona==='longe' || (zona==='parada' && pisca)};
  if (sim.modo === 'desligado') leds = {verde:false, amarelo:false, vermelho:false};
  if (sim.modo === 'teste') leds = {verde:passo===0, amarelo:passo===1, vermelho:passo===2};
  return {distancia:x, zona, modo:sim.modo, leds, config:c,
          stats:{total:sim.total, menor:sim.menor}, uptime:Math.floor((Date.now()-sim.inicio)/1000),
          clientes:1, ip:'simulação'};
}
$('btnSim').onclick = iniciarSim;
$('btnSimParar').onclick = pararSim;

function fmtTempo(s){
  const h = Math.floor(s/3600), m = Math.floor(s%3600/60);
  return h ? h+'h '+m+'min' : m ? m+'min '+(s%60)+'s' : s+'s';
}

function atualizar(d){
  cfg = d.config;
  const temLeitura = d.distancia >= 0;
  $('dist').innerHTML = (temLeitura ? d.distancia : '--') + '<small> cm</small>';
  const z = ZONAS[d.zona] || ZONAS.sem_leitura;
  $('zona').textContent = z.t;
  $('zona').style.background = z.c;
  $('dist').style.color = d.zona === 'parada' ? 'var(--vermelho)' : '';

  // Barra proporcional: 0 .. max*1.25
  const total = cfg.max * 1.25;
  $('bR').style.width = (cfg.parada/total*100)+'%';
  $('bA').style.width = ((cfg.livre-cfg.parada)/total*100)+'%';
  $('bV').style.width = ((cfg.max-cfg.livre)/total*100)+'%';
  $('escMax').textContent = Math.round(total)+' cm';
  const pos = temLeitura ? Math.min(d.distancia, total)/total*100 : 100;
  $('carro').style.left = Math.max(4, Math.min(96, pos))+'%';

  $('lv').classList.toggle('on', d.leds.verde);
  $('la').classList.toggle('on', d.leds.amarelo);
  $('lr').classList.toggle('on', d.leds.vermelho);

  $('sTotal').textContent = d.stats.total;
  $('sMenor').textContent = d.stats.menor >= 0 ? d.stats.menor+' cm' : '--';
  $('sUp').textContent = fmtTempo(d.uptime);
  $('sRssi').textContent = d.clientes;
  $('rodape').textContent = 'IP: '+d.ip;

  document.querySelectorAll('[data-m]').forEach(b => b.classList.toggle('ativo', b.dataset.m === d.modo));

  atualizarApito(d);

  if (!formPreenchido){
    const f = $('form');
    f.parada.value = cfg.parada; f.livre.value = cfg.livre; f.max.value = cfg.max; f.pisca.value = cfg.pisca;
    formPreenchido = true;
  }

  const agora = Date.now();
  historico.push({t:agora, d:d.distancia});
  while (historico.length && agora - historico[0].t > 60000) historico.shift();
  desenhar();
}

function desenhar(){
  const cv = $('graf'), dpr = window.devicePixelRatio || 1;
  const W = cv.clientWidth, H = cv.clientHeight;
  cv.width = W*dpr; cv.height = H*dpr;
  const g = cv.getContext('2d'); g.scale(dpr,dpr);
  g.clearRect(0,0,W,H);
  if (!cfg) return;
  const pad = {l:40,r:8,t:8,b:20}, w = W-pad.l-pad.r, h = H-pad.t-pad.b;
  const yMax = cfg.max * 1.25;
  const y = v => pad.t + h - Math.min(v,yMax)/yMax*h;
  const agora = Date.now(), x = t => pad.l + w - (agora-t)/60000*w;

  // faixas das zonas
  const faixa = (a,b,cor) => { g.fillStyle = cor; g.fillRect(pad.l, y(b), w, y(a)-y(b)); };
  faixa(0, cfg.parada, 'rgba(239,68,68,.12)');
  faixa(cfg.parada, cfg.livre, 'rgba(250,204,21,.10)');
  faixa(cfg.livre, cfg.max, 'rgba(34,197,94,.10)');

  // eixo y
  g.fillStyle = '#8b96a3'; g.font = '11px system-ui'; g.textAlign = 'right'; g.textBaseline = 'middle';
  [0, cfg.parada, cfg.livre, cfg.max].forEach(v => {
    g.fillText(v, pad.l-6, y(v));
    g.strokeStyle = 'rgba(255,255,255,.06)'; g.beginPath(); g.moveTo(pad.l,y(v)); g.lineTo(pad.l+w,y(v)); g.stroke();
  });
  g.textBaseline = 'top';
  ['-60s','-30s','agora'].forEach((s,i) => {
    g.textAlign = ['left','center','right'][i];
    g.fillText(s, pad.l + w*i/2, H-pad.b+5);
  });

  // linha da distância (quebra onde não houve leitura)
  g.strokeStyle = '#3b82f6'; g.lineWidth = 2; g.lineJoin = 'round'; g.beginPath();
  let desenhando = false;
  historico.forEach(p => {
    if (p.d < 0){ desenhando = false; return; }
    desenhando ? g.lineTo(x(p.t), y(p.d)) : g.moveTo(x(p.t), y(p.d));
    desenhando = true;
  });
  g.stroke();
}

async function buscar(){
  if (sim){
    atualizar(statusSimulado());
    $('dot').classList.add('on'); $('conTxt').textContent = 'Simulação';
    setTimeout(buscar, 100);
    return;
  }
  try{
    const r = await api('/api/status');
    atualizar(await r.json());
    falhas = 0;
    $('simCard').hidden = true;
    $('dot').classList.add('on'); $('conTxt').textContent = 'Ao vivo';
  }catch(e){
    if (++falhas > 2){
      $('dot').classList.remove('on'); $('conTxt').textContent = 'Sem conexão com o ESP32';
      $('simCard').hidden = false;
      aplicarApito({n:0, iv:0, cont:false}, 'Sem conexão — som pausado', '');   // nunca apitar com dado velho
    }
  }
  setTimeout(buscar, 250);
}

document.querySelectorAll('[data-m]').forEach(b => b.onclick = () => {
  if (sim) sim.modo = b.dataset.m;
  else api('/api/modo?m='+b.dataset.m).catch(() => {});
});

$('btnReset').onclick = async () => {
  if (!confirm('Zerar contagem de estacionamentos e menor distância?')) return;
  if (sim){ sim.total = 0; sim.menor = -1; }
  else await api('/api/reset').catch(() => {});
};

$('form').onsubmit = async e => {
  e.preventDefault();
  const f = e.target, n = k => +f[k].value;
  let j;
  if (sim){
    const ok = n('parada') >= 2 && n('parada') < n('livre') && n('livre') < n('max') && n('max') <= 400;
    if (ok) sim.config = {max:n('max'), livre:n('livre'), parada:n('parada'), pisca:n('pisca')};
    j = ok ? {ok:true} : {erro:'Use 2 <= parada < livre < max <= 400'};
  } else {
    try {
      const r = await api('/api/config?' + new URLSearchParams(new FormData(f)));
      j = await r.json();
    } catch(err){ j = {erro:'Sem conexão com o ESP32'}; }
  }
  $('msg').style.color = j.ok ? 'var(--verde)' : 'var(--vermelho)';
  $('msg').textContent = j.ok ? (sim ? 'Aplicado na simulação ✔' : 'Salvo no ESP32 ✔') : j.erro;
};

// ---------- Apito de sensor de ré (Web Audio + vibração) ----------
// O navegador só libera áudio depois de um toque, por isso o botão "Ativar som".
let ac = null, ganho = null, somOn = false, volume = 0.6;
let intervalo = 0, continuo = false, ultimoBip = 0, ultimaVib = 0, inicioParada = 0;
try { volume = +(localStorage.getItem('vol') || 0.6); } catch(e){}
$('vol').value = volume;

const CENTRO = {x:100, y:116}, RAIOS = [24, 42, 60, 78];   // arco 0 = mais perto do carro
RAIOS.forEach(r => {
  const p = a => [CENTRO.x + r*Math.cos(a*Math.PI/180), CENTRO.y + r*Math.sin(a*Math.PI/180)];
  const [x1,y1] = p(-140), [x2,y2] = p(-40);
  const el = document.createElementNS('http://www.w3.org/2000/svg', 'path');
  el.setAttribute('d', `M${x1} ${y1} A${r} ${r} 0 0 1 ${x2} ${y2}`);
  $('arcos').appendChild(el);
});

function ligarAudio(){
  if (!ac){
    ac = new (window.AudioContext || window.webkitAudioContext)();
    const osc = ac.createOscillator();
    ganho = ac.createGain();
    osc.type = 'sine'; osc.frequency.value = 1400; ganho.gain.value = 0;
    osc.connect(ganho).connect(ac.destination); osc.start();
  }
  ac.resume();
}

function bip(){
  const t = ac.currentTime, g = ganho.gain;
  g.cancelScheduledValues(t); g.setValueAtTime(0, t);
  g.linearRampToValueAtTime(volume, t + 0.005);
  g.setValueAtTime(volume, t + 0.07);
  g.linearRampToValueAtTime(0, t + 0.09);
}

function atualizarTom(){
  if (!ac) return;
  ganho.gain.cancelScheduledValues(ac.currentTime);
  ganho.gain.setTargetAtTime(somOn && continuo ? volume : 0, ac.currentTime, 0.015);
}

function vibrar(ms){ if (somOn && navigator.vibrate) navigator.vibrate(ms); }

// Decide o ritmo do apito pela distância: n = quantos arcos acendem, iv = ms entre bipes
function atualizarApito(d){
  const c = d.config, x = d.distancia, cor = (ZONAS[d.zona] || ZONAS.sem_leitura).c;

  // Depois de 4 s parado no vermelho o carro está estacionado: silencia até ele sair
  if (d.zona === 'parada'){ if (!inicioParada) inicioParada = Date.now(); }
  else if (x > c.parada + 10) inicioParada = 0;
  const estacionado = inicioParada && Date.now() - inicioParada > 4000;

  if (d.modo === 'desligado')
    return aplicarApito({n:0, iv:0, cont:false}, 'LEDs desligados', cor);
  if (estacionado)
    return aplicarApito({n:4, iv:0, cont:false}, '✔ Carro estacionado — som pausado', cor);
  if (d.zona === 'parada')
    return aplicarApito({n:4, iv:0, cont:true}, '🔴 Apito contínuo — PARE!', cor);
  if (d.zona === 'atencao'){
    const f = (x - c.parada) / (c.livre - c.parada);        // 1 = início do amarelo, 0 = quase vermelho
    return aplicarApito({n: f > .5 ? 2 : 3, iv: Math.round(120 + f*380)}, '🟡 Apito rápido — devagar', cor);
  }
  if (d.zona === 'livre'){
    const f = (x - c.livre) / (c.max - c.livre);
    return aplicarApito({n:1, iv: Math.round(600 + f*400)}, '🟢 Apito lento — pode vir', cor);
  }
  aplicarApito({n:0, iv:0, cont:false}, 'Nenhum carro se aproximando', cor);
}

function aplicarApito(a, texto, cor){
  intervalo = a.iv;
  if (!!a.cont !== continuo){ continuo = !!a.cont; atualizarTom(); }
  $('sonar').classList.toggle('cont', continuo);
  $('arcos').querySelectorAll('path').forEach((p, i) => {
    const on = i >= RAIOS.length - a.n;     // acende de fora para dentro conforme aproxima
    p.classList.toggle('on', on);
    p.style.stroke = on ? cor : '';
    p.style.color = on ? cor : '';
  });
  $('estado').textContent = texto + (somOn || !a.n ? '' : ' (toque em 🔇 para ouvir)');
}

function tick(){
  const agora = Date.now();
  if (continuo){
    if (agora - ultimaVib > 600){ vibrar(400); ultimaVib = agora; }
  } else if (intervalo && agora - ultimoBip >= intervalo){
    ultimoBip = agora;
    if (somOn && ac) bip();
    vibrar(50);
    $('sonar').classList.add('flash');
    setTimeout(() => $('sonar').classList.remove('flash'), 90);
  }
  setTimeout(tick, 15);
}

$('btnSom').onclick = () => {
  ligarAudio();
  somOn = !somOn;
  $('btnSom').textContent = somOn ? '🔊 Som ligado' : '🔇 Ativar som';
  $('btnSom').classList.toggle('on', somOn);
  atualizarTom();
  if (somOn && !continuo) bip();   // bipe de confirmação
};

$('vol').oninput = e => {
  volume = +e.target.value;
  try { localStorage.setItem('vol', volume); } catch(err){}
  atualizarTom();
};

window.onresize = desenhar;
buscar();
tick();
</script>
</body>
</html>
)rawliteral";
