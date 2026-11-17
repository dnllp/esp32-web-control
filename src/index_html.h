// ARCHIVO GENERADO por tools/embed_web.py a partir de web/index.html. No editar.
#pragma once
static const char INDEX_HTML[] = R"rawliteral(<!DOCTYPE html>
<html lang="es">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1,user-scalable=no">
<title>ROmBA</title>
<style>
:root{--bg:#10141a;--card:#1a2029;--line:#2b3440;--txt:#e6ebf1;--mut:#8b97a6;--acc:#3fb6ff;--ok:#3ecf8e;--warn:#f5b748;--err:#ff6161}
*{box-sizing:border-box}
body{margin:0;font:14px/1.4 system-ui,-apple-system,Segoe UI,Roboto,sans-serif;background:var(--bg);color:var(--txt);-webkit-user-select:none;user-select:none}
header{display:flex;align-items:center;gap:12px;padding:10px 16px;background:var(--card);border-bottom:1px solid var(--line);position:sticky;top:0;z-index:5}
header h1{font-size:18px;margin:0}
#dot{width:10px;height:10px;border-radius:50%;background:var(--err)}
#dot.on{background:var(--ok)}
#board{color:var(--mut);font-size:12px}
nav{margin-left:auto;display:flex;gap:4px}
nav button{background:none;border:1px solid var(--line);color:var(--mut);padding:6px 12px;border-radius:6px;cursor:pointer}
nav button.act{background:var(--acc);border-color:var(--acc);color:#001018}
main{padding:16px;display:grid;gap:16px;grid-template-columns:repeat(auto-fit,minmax(320px,1fr));max-width:1300px;margin:auto}
.card{background:var(--card);border:1px solid var(--line);border-radius:10px;padding:14px}
.card h2{font-size:13px;text-transform:uppercase;letter-spacing:.06em;color:var(--mut);margin:0 0 10px}
.hidden{display:none!important}
#banner{grid-column:1/-1;background:#3a2d0c;border:1px solid var(--warn);color:var(--warn);padding:10px 14px;border-radius:8px}
.drive{display:flex;gap:16px;flex-wrap:wrap;justify-content:center;align-items:center}
#joy{width:200px;height:200px;touch-action:none;border-radius:50%;background:radial-gradient(circle,#232b36 0,#161b22 70%);border:1px solid var(--line)}
.pad{display:grid;grid-template-columns:repeat(3,56px);gap:6px}
.pad button{height:56px;font-size:22px;background:#232b36;border:1px solid var(--line);color:var(--txt);border-radius:8px;touch-action:none;cursor:pointer}
.pad button:active,.pad button.on{background:var(--acc);color:#001018}
.pad .stop{background:#4a1e1e;color:var(--err)}
.row{display:flex;align-items:center;gap:10px;margin-top:12px}
.row input[type=range]{flex:1}
.btn{background:#232b36;border:1px solid var(--line);color:var(--txt);padding:7px 12px;border-radius:6px;cursor:pointer}
.btn.pri{background:var(--acc);border-color:var(--acc);color:#001018}
.btn.dan{border-color:var(--err);color:var(--err)}
#traj{width:100%;height:320px;display:block;background:#0c1015;border-radius:8px}
table{width:100%;border-collapse:collapse;font-variant-numeric:tabular-nums}
td,th{padding:4px 6px;text-align:right;border-bottom:1px solid var(--line)}
td:first-child,th:first-child{text-align:left;color:var(--mut)}
.kv{display:grid;grid-template-columns:repeat(3,1fr);gap:8px;margin-bottom:10px}
.kv div{background:#141920;border-radius:6px;padding:6px 8px}
.kv b{display:block;font-size:18px;font-variant-numeric:tabular-nums}
.kv span{font-size:11px;color:var(--mut)}
.pidg{display:grid;grid-template-columns:repeat(3,1fr);gap:8px}
label{display:block;font-size:12px;color:var(--mut)}
input[type=number],input[type=text],input[type=password],select{width:100%;background:#0f1318;border:1px solid var(--line);color:var(--txt);padding:6px 8px;border-radius:6px;font:inherit}
.cfg fieldset{border:1px solid var(--line);border-radius:8px;margin:0 0 12px;padding:10px 12px}
.cfg legend{color:var(--acc);padding:0 6px}
.cfg .f{display:grid;grid-template-columns:1fr 150px;gap:8px;align-items:center;padding:4px 0}
.cfg .f small{display:block;color:var(--mut);font-size:11px}
.der{font-size:12px;color:var(--mut)}
.der b{color:var(--txt)}
#toast{position:fixed;bottom:16px;left:50%;transform:translateX(-50%);padding:10px 16px;border-radius:8px;background:var(--card);border:1px solid var(--line);opacity:0;transition:opacity .3s;pointer-events:none;max-width:90vw}
#toast.show{opacity:1}
#toast.ok{border-color:var(--ok)}#toast.warn{border-color:var(--warn)}#toast.err{border-color:var(--err)}
.pill{display:inline-block;padding:1px 8px;border-radius:10px;font-size:11px;background:#232b36}
.pill.ok{color:var(--ok)}.pill.err{color:var(--err)}.pill.warn{color:var(--warn)}
@media(max-width:520px){main{padding:10px}.cfg .f{grid-template-columns:1fr 120px}}
</style>
</head>
<body>
<header>
  <div id="dot"></div><h1>ROmBA</h1><span id="board">desconectado</span>
  <nav><button id="tabCtl" class="act">Control</button><button id="tabCfg">Configuración</button></nav>
</header>

<main id="pCtl">
  <div id="banner" class="hidden">Primera vez: revisa la pestaña <b>Configuración</b> (encoders, IMU y medidas del robot) y guárdala.</div>

  <section class="card">
    <h2>Manejo</h2>
    <div class="drive">
      <canvas id="joy" width="400" height="400"></canvas>
      <div class="pad" id="pad">
        <button data-v="1" data-w="1">↖</button><button data-v="1" data-w="0">↑</button><button data-v="1" data-w="-1">↗</button>
        <button data-v="0" data-w="1">↺</button><button class="stop" data-stop="1">■</button><button data-v="0" data-w="-1">↻</button>
        <button data-v="-1" data-w="-1">↙</button><button data-v="-1" data-w="0">↓</button><button data-v="-1" data-w="1">↘</button>
      </div>
    </div>
    <div class="row"><label style="width:70px">Velocidad</label><input type="range" id="spd" min="10" max="100" value="50"><b id="spdV" style="width:44px;text-align:right">50%</b></div>
    <div class="row der">Teclado: flechas o WASD, espacio = paro. El robot se detiene solo si no recibe comandos en <b id="tmo">500</b> ms.</div>
  </section>

  <section class="card">
    <h2>Odometría</h2>
    <div class="kv">
      <div><span>x (m)</span><b id="ox">0.00</b></div>
      <div><span>y (m)</span><b id="oy">0.00</b></div>
      <div><span>θ (°)</span><b id="oth">0.0</b></div>
      <div><span>v (m/s)</span><b id="ov">0.00</b></div>
      <div><span>ω (rad/s)</span><b id="ow">0.00</b></div>
      <div><span>recorrido (m)</span><b id="od">0.00</b></div>
    </div>
    <canvas id="traj"></canvas>
    <div class="row"><button class="btn" id="bReset">Reiniciar odometría</button><button class="btn" id="bClear">Borrar trazo</button><span class="der" id="scale"></span></div>
  </section>

  <section class="card">
    <h2>Ruedas</h2>
    <table>
      <tr><th></th><th>Izquierda</th><th>Derecha</th></tr>
      <tr><td>Objetivo (m/s)</td><td id="lt">-</td><td id="rt">-</td></tr>
      <tr><td>Medida (m/s)</td><td id="lm">-</td><td id="rm">-</td></tr>
      <tr><td>PWM (%)</td><td id="lp">-</td><td id="rp">-</td></tr>
      <tr><td>Ticks</td><td id="lk">-</td><td id="rk">-</td></tr>
    </table>
    <h2 style="margin-top:14px">IMU <span class="pill" id="imuSt">-</span></h2>
    <table>
      <tr><td>Yaw / Roll / Pitch (°)</td><td id="iy">-</td></tr>
      <tr><td>Giro Z (°/s)</td><td id="igz">-</td></tr>
      <tr><td>Acel. x / y / z (m/s²)</td><td id="iacc">-</td></tr>
      <tr><td>Calibración BNO055 (sis/gir/ace/mag)</td><td id="ical">-</td></tr>
      <tr><td>Fusión en odometría</td><td id="ifu">-</td></tr>
    </table>
    <div class="row"><button class="btn" id="bImuCal">Calibrar giroscopio</button><span class="der" id="sysInfo"></span></div>
  </section>

  <section class="card">
    <h2>PID de velocidad (ajuste en vivo)</h2>
    <div class="row" style="margin-top:0"><label><input type="checkbox" id="pidEn"> PID activo (si no, PWM directo proporcional a la velocidad)</label></div>
    <div class="pidg" style="margin-top:8px">
      <div><label>Kp</label><input type="number" id="kp" step="1"></div>
      <div><label>Ki</label><input type="number" id="ki" step="1"></div>
      <div><label>Kd</label><input type="number" id="kd" step="0.1"></div>
    </div>
    <div class="row"><button class="btn pri" id="bPid">Aplicar</button><button class="btn" id="bPidSave">Aplicar y guardar</button></div>
    <h2 style="margin-top:16px">Robot</h2>
    <div class="der" id="robotInfo">-</div>
  </section>
</main>

<main id="pCfg" class="hidden">
  <section class="card cfg" style="grid-column:1/-1">
    <h2>Configuración (se guarda en la ESP32 y reinicia)</h2>
    <div id="cfgForm"></div>
    <div class="der" id="cfgDer" style="margin:6px 0 12px"></div>
    <div class="row"><button class="btn pri" id="bSave">Guardar y reiniciar</button><button class="btn" id="bReload">Descartar cambios</button><button class="btn dan" id="bFactory" style="margin-left:auto">Valores de fábrica</button></div>
  </section>
</main>

<div id="toast"></div>

<script>
// ------------------------------------------------------------ esquema de configuración
// [clave, etiqueta, tipo, ayuda, extra]. Tipos: n=número, i=entero, b=sí/no, s=texto, p=clave, sel=lista
const SCHEMA=[
 ["Robot (odometría)",[
  ["wheelDiamMm","Diámetro de rueda (mm)","n","Rueda de 130 mm por defecto"],
  ["wheelBaseMm","Distancia entre ruedas (mm)","n","De centro a centro de las llantas"],
  ["robotDiamMm","Diámetro del robot (mm)","n","Solo para el dibujo de la trayectoria"],
  ["maxSpeed","Velocidad lineal máx. (m/s)","n","Joystick al 100 %"],
  ["maxAngular","Velocidad angular máx. (rad/s)","n","Joystick al 100 % hacia un lado"],
  ["accelMax","Aceleración máx. (m/s²)","n","Rampa por rueda. 0 = sin rampa"]]],
 ["Encoders",[
  ["encType","Tipo de encoder","sel","Simple: un canal, el sentido se toma del motor",[[0,"Simple (1 canal)"],[1,"Cuadratura (A/B)"]]],
  ["pprSimple","PPR encoder simple","i","Pulsos por vuelta de rueda"],
  ["pprQuad","PPR encoder cuadratura","i","Pulsos del canal A por vuelta de rueda (cuenta x1)"],
  ["gearRatio","Relación de engranes","n","Vueltas del encoder por vuelta de rueda. 1 si el encoder está en la rueda"],
  ["encFilterUs","Filtro de rebotes (µs)","i","Ignora pulsos más cercanos que esto"],
  ["invertEncL","Invertir encoder izquierdo","b","Solo cuadratura: si cuenta al revés"],
  ["invertEncR","Invertir encoder derecho","b","Solo cuadratura"]]],
 ["IMU",[
  ["imuType","Tipo de IMU","sel","",[[0,"Ninguna"],[1,"BNO055"],[2,"MPU6050"]]],
  ["imuAddrBno","Dirección I2C BNO055","i","40 (0x28) o 41 (0x29)"],
  ["imuAddrMpu","Dirección I2C MPU6050","i","104 (0x68) o 105 (0x69)"],
  ["imuFusion","Usar IMU en la odometría","b","Corrige el ángulo con el giroscopio"],
  ["imuAlpha","Peso de la IMU (0-1)","n","0 = solo encoders, 1 = solo IMU"],
  ["imuInvertYaw","Invertir yaw","b","Si la IMU va montada de cabeza"]]],
 ["Motores (puente H)",[
  ["pwmFreq","Frecuencia PWM (Hz)","i","L298N: 1000. TB6612: hasta 20000"],
  ["pwmBits","Resolución PWM (bits)","i","8 a 12"],
  ["pwmMinPct","PWM mínimo (%)","n","Zona muerta: PWM con el que la rueda empieza a girar"],
  ["invertMotorL","Invertir motor izquierdo","b",""],
  ["invertMotorR","Invertir motor derecho","b",""]]],
 ["PID",[
  ["pidEnabled","PID activo","b",""],
  ["kp","Kp","n","% de PWM por m/s de error"],["ki","Ki","n",""],["kd","Kd","n",""]]],
 ["Pines (GPIO)",[
  ["pinEna","ENA (PWM izq.)","i",""],["pinIn1","IN1","i",""],["pinIn2","IN2","i",""],
  ["pinEnb","ENB (PWM der.)","i",""],["pinIn3","IN3","i",""],["pinIn4","IN4","i",""],
  ["pinEncLA","Encoder izq. A","i",""],["pinEncLB","Encoder izq. B","i","Solo cuadratura"],
  ["pinEncRA","Encoder der. A","i",""],["pinEncRB","Encoder der. B","i","Solo cuadratura"],
  ["pinSda","I2C SDA","i",""],["pinScl","I2C SCL","i",""]]],
 ["WiFi (punto de acceso) y control",[
  ["apSsid","Nombre de la red","s",""],
  ["apPass","Clave","p","Mínimo 8 caracteres, vacía = abierta"],
  ["apChannel","Canal","i","1 a 13"],
  ["wsTimeoutMs","Paro sin comandos (ms)","i","Si se pierde la conexión"],
  ["telemetryHz","Telemetría (Hz)","i",""],
  ["controlHz","Lazo de control (Hz)","i",""]]]
];

const $=id=>document.getElementById(id);
let ws,conf=null,cfg={},speed=.5,joy={v:0,w:0,on:false},pad={v:0,w:0,on:false},keys={},trail=[],last=null;

// ------------------------------------------------------------ WebSocket
function connect(){
  ws=new WebSocket(`ws://${location.host||"192.168.4.1"}/ws`);
  ws.onopen=()=>{$("dot").classList.add("on")};
  ws.onclose=()=>{$("dot").classList.remove("on");$("board").textContent="desconectado";setTimeout(connect,1000)};
  ws.onmessage=e=>{let m;try{m=JSON.parse(e.data)}catch{return}
    if(m.t==="tel")onTel(m);else if(m.t==="config")onConfig(m);else if(m.t==="msg")toast(m.text,m.level)};
}
function send(o){if(ws&&ws.readyState===1)ws.send(JSON.stringify(o))}

// ------------------------------------------------------------ comandos (10 Hz)
function currentCmd(){
  let v=0,w=0;
  if(joy.on){v=joy.v;w=joy.w}else if(pad.on){v=pad.v;w=pad.w}else{
    if(keys.up)v+=1;if(keys.down)v-=1;if(keys.left)w+=1;if(keys.right)w-=1}
  return{v:v*speed,w:w*speed}
}
setInterval(()=>{if(document.hidden)return;const c=currentCmd();send({t:"cmd",v:+c.v.toFixed(3),w:+c.w.toFixed(3)})},100);

// joystick
const jc=$("joy"),jx=jc.getContext("2d");let knob={x:0,y:0};
function drawJoy(){const W=jc.width,R=W/2;jx.clearRect(0,0,W,W);
  jx.strokeStyle="#2b3440";jx.lineWidth=2;jx.beginPath();jx.arc(R,R,R*.8,0,7);jx.stroke();
  jx.beginPath();jx.moveTo(R,R*.2);jx.lineTo(R,R*1.8);jx.moveTo(R*.2,R);jx.lineTo(R*1.8,R);jx.stroke();
  jx.fillStyle=joy.on?"#3fb6ff":"#4b5868";jx.beginPath();jx.arc(R+knob.x*R*.8,R+knob.y*R*.8,R*.22,0,7);jx.fill()}
function joyMove(e){const r=jc.getBoundingClientRect();let x=(e.clientX-r.left)/r.width*2-1,y=(e.clientY-r.top)/r.height*2-1;
  x/=.8;y/=.8;const d=Math.hypot(x,y);if(d>1){x/=d;y/=d}knob={x,y};
  joy.v=Math.abs(y)<.08?0:-y;joy.w=Math.abs(x)<.08?0:-x;drawJoy()}
jc.addEventListener("pointerdown",e=>{jc.setPointerCapture(e.pointerId);joy.on=true;joyMove(e)});
jc.addEventListener("pointermove",e=>{if(joy.on)joyMove(e)});
const joyEnd=()=>{joy={v:0,w:0,on:false};knob={x:0,y:0};drawJoy();send({t:"cmd",v:0,w:0})};
jc.addEventListener("pointerup",joyEnd);jc.addEventListener("pointercancel",joyEnd);drawJoy();

// botones
$("pad").querySelectorAll("button").forEach(b=>{
  b.addEventListener("pointerdown",e=>{b.setPointerCapture(e.pointerId);
    if(b.dataset.stop){pad={v:0,w:0,on:false};send({t:"stop"});return}
    pad={v:+b.dataset.v,w:+b.dataset.w*(b.dataset.v==="0"?1:.5),on:true};b.classList.add("on")});
  const up=()=>{b.classList.remove("on");if(pad.on){pad={v:0,w:0,on:false};send({t:"cmd",v:0,w:0})}};
  b.addEventListener("pointerup",up);b.addEventListener("pointercancel",up)});

// teclado
const KM={ArrowUp:"up",KeyW:"up",ArrowDown:"down",KeyS:"down",ArrowLeft:"left",KeyA:"left",ArrowRight:"right",KeyD:"right"};
addEventListener("keydown",e=>{if(e.target.tagName==="INPUT")return;if(e.code==="Space"){send({t:"stop"});keys={};e.preventDefault();return}
  if(KM[e.code]){keys[KM[e.code]]=true;e.preventDefault()}});
addEventListener("keyup",e=>{if(KM[e.code])keys[KM[e.code]]=false});
addEventListener("blur",()=>{keys={};joyEnd()});

$("spd").oninput=e=>{speed=e.target.value/100;$("spdV").textContent=e.target.value+"%"};

// ------------------------------------------------------------ telemetría
const f=(x,n=2)=>(+x).toFixed(n);
function onTel(m){
  $("ox").textContent=f(m.x);$("oy").textContent=f(m.y);$("oth").textContent=f(m.th*57.2958,1);
  $("ov").textContent=f(m.v);$("ow").textContent=f(m.w);$("od").textContent=f(m.dist);
  $("lt").textContent=f(m.wl.tgt);$("rt").textContent=f(m.wr.tgt);
  $("lm").textContent=f(m.wl.meas);$("rm").textContent=f(m.wr.meas);
  $("lp").textContent=f(m.wl.pwm,0);$("rp").textContent=f(m.wr.pwm,0);
  $("lk").textContent=m.wl.ticks;$("rk").textContent=m.wr.ticks;
  const i=m.imu,st=$("imuSt");
  st.textContent=i.type==="ninguna"?"sin IMU":i.type+(i.ok?" OK":" sin respuesta");
  st.className="pill "+(i.type==="ninguna"?"":i.ok?"ok":"err");
  $("iy").textContent=`${f(i.yaw,1)} / ${f(i.roll,1)} / ${f(i.pitch,1)}`;
  $("igz").textContent=f(i.gz*57.2958,1);
  $("iacc").textContent=`${f(i.ax)} / ${f(i.ay)} / ${f(i.az)}`;
  $("ical").textContent=i.type==="BNO055"?[(i.cal>>6)&3,(i.cal>>4)&3,(i.cal>>2)&3,i.cal&3].join(" / ")+" (de 3)":"-";
  $("ifu").textContent=i.fusion?"sí":"no";
  $("sysInfo").innerHTML=(m.timeout?'<span class="pill warn">detenido: sin comandos</span> ':'<span class="pill ok">recibiendo comandos</span> ')+`memoria libre ${(m.heap/1024).toFixed(0)} KB`;
  if(!last||Math.hypot(m.x-last.x,m.y-last.y)>.01){trail.push([m.x,m.y]);if(trail.length>5000)trail.shift()}
  last=m;drawTraj();
}

// ------------------------------------------------------------ trayectoria
const tc=$("traj"),tx=tc.getContext("2d");
function drawTraj(){
  const dpr=devicePixelRatio||1,W=tc.clientWidth,H=tc.clientHeight;
  if(tc.width!==W*dpr){tc.width=W*dpr;tc.height=H*dpr}
  tx.setTransform(dpr,0,0,dpr,0,0);tx.clearRect(0,0,W,H);
  const rob=(cfg.robotDiamMm||300)/2000,p=last||{x:0,y:0,th:0};
  let minX=Math.min(-1,p.x-rob),maxX=Math.max(1,p.x+rob),minY=Math.min(-1,p.y-rob),maxY=Math.max(1,p.y+rob);
  for(const[x,y]of trail){if(x<minX)minX=x;if(x>maxX)maxX=x;if(y<minY)minY=y;if(y>maxY)maxY=y}
  const s=Math.min(W/(maxX-minX+.4),H/(maxY-minY+.4)),cx=(minX+maxX)/2,cy=(minY+maxY)/2;
  const X=x=>W/2+(x-cx)*s,Y=y=>H/2-(y-cy)*s;
  let step=[.1,.25,.5,1,2,5,10].find(g=>g*s>40)||20;
  tx.strokeStyle="#1d242d";tx.lineWidth=1;tx.beginPath();
  for(let g=Math.floor((cx-W/2/s)/step)*step;g<cx+W/2/s;g+=step){tx.moveTo(X(g),0);tx.lineTo(X(g),H)}
  for(let g=Math.floor((cy-H/2/s)/step)*step;g<cy+H/2/s;g+=step){tx.moveTo(0,Y(g));tx.lineTo(W,Y(g))}
  tx.stroke();$("scale").textContent=`cuadro = ${step} m`;
  tx.strokeStyle="#3b4757";tx.beginPath();tx.moveTo(X(0),Y(-.15));tx.lineTo(X(0),Y(.15));tx.moveTo(X(-.15),Y(0));tx.lineTo(X(.15),Y(0));tx.stroke();
  if(trail.length>1){tx.strokeStyle="#3fb6ff";tx.lineWidth=2;tx.beginPath();tx.moveTo(X(trail[0][0]),Y(trail[0][1]));
    for(const[x,y]of trail)tx.lineTo(X(x),Y(y));tx.stroke()}
  tx.fillStyle="rgba(62,207,142,.25)";tx.strokeStyle="#3ecf8e";tx.lineWidth=2;tx.beginPath();tx.arc(X(p.x),Y(p.y),rob*s,0,7);tx.fill();tx.stroke();
  tx.beginPath();tx.moveTo(X(p.x),Y(p.y));tx.lineTo(X(p.x+Math.cos(p.th)*rob),Y(p.y+Math.sin(p.th)*rob));tx.stroke();
}
addEventListener("resize",drawTraj);
$("bReset").onclick=()=>{trail=[];last=null;send({t:"resetOdom"})};
$("bClear").onclick=()=>{trail=[];drawTraj()};
$("bImuCal").onclick=()=>send({t:"imuCal"});

// ------------------------------------------------------------ configuración
function onConfig(m){
  conf=m;cfg=m.cfg;
  $("board").textContent=`${m.board} · fw ${m.fw}`;
  $("banner").classList.toggle("hidden",m.configured);
  $("tmo").textContent=cfg.wsTimeoutMs;
  $("pidEn").checked=cfg.pidEnabled;$("kp").value=cfg.kp;$("ki").value=cfg.ki;$("kd").value=cfg.kd;
  const d=m.derived;
  $("robotInfo").innerHTML=`Rueda <b>${cfg.wheelDiamMm} mm</b> (perímetro ${f(d.wheelCircM*1000,1)} mm) · entre ruedas <b>${cfg.wheelBaseMm} mm</b> · robot <b>${cfg.robotDiamMm} mm</b><br>`+
    `Encoder <b>${cfg.encType?"cuadratura":"simple"}</b>, ${d.ppr} PPR × ${cfg.gearRatio} = <b>${f(d.ticksPerRev,1)} ticks/vuelta</b> → <b>${f(d.mmPerTick)} mm/tick</b><br>`+
    `IMU <b>${["ninguna","BNO055","MPU6050"][cfg.imuType]}</b>${cfg.imuFusion&&cfg.imuType?` (fusión, peso ${cfg.imuAlpha})`:""} · v máx ${cfg.maxSpeed} m/s · ω máx ${cfg.maxAngular} rad/s`;
  buildForm();
  if(!m.configured&&!buildForm.shown){buildForm.shown=true;showTab("cfg")}
}
function buildForm(){
  const el=$("cfgForm");el.innerHTML="";
  for(const[title,fields]of SCHEMA){
    const fs=document.createElement("fieldset");fs.innerHTML=`<legend>${title}</legend>`;
    for(const[k,label,type,help,opts]of fields){
      const row=document.createElement("div");row.className="f";
      let inp;
      if(type==="sel"){inp=`<select id="c_${k}">${opts.map(([v,t])=>`<option value="${v}">${t}</option>`).join("")}</select>`}
      else if(type==="b"){inp=`<input type="checkbox" id="c_${k}" style="justify-self:start">`}
      else{inp=`<input id="c_${k}" type="${type==="s"?"text":type==="p"?"password":"number"}" ${type==="i"?'step="1"':'step="any"'}>`}
      row.innerHTML=`<div>${label}${help?`<small>${help}</small>`:""}</div>${inp}`;
      fs.appendChild(row);
      const i=row.querySelector("input,select");
      if(type==="b")i.checked=!!cfg[k];else i.value=cfg[k];
      i.addEventListener("input",updateDerived);
    }
    el.appendChild(fs);
  }
  updateDerived();
}
function readForm(){
  const o={};
  for(const[,fields]of SCHEMA)for(const[k,,type]of fields){const i=$("c_"+k);
    o[k]=type==="b"?i.checked:(type==="s"||type==="p")?i.value:+i.value}
  return o;
}
function updateDerived(){
  const c=readForm(),ppr=c.encType?c.pprQuad:c.pprSimple,tpr=ppr*c.gearRatio,circ=Math.PI*c.wheelDiamMm;
  $("cfgDer").innerHTML=`Con estos valores: <b>${f(tpr,1)} ticks/vuelta</b>, <b>${f(circ/tpr)} mm/tick</b>, perímetro de rueda ${f(circ,1)} mm, `+
    `giro de 360° en el lugar = ${f(Math.PI*c.wheelBaseMm/circ,2)} vueltas de rueda.`;
}
$("bSave").onclick=()=>{send({t:"setConfig",cfg:readForm()});toast("Guardando...","warn")};
$("bReload").onclick=()=>send({t:"getConfig"});
$("bFactory").onclick=()=>{if(confirm("¿Borrar la configuración guardada y volver a los valores de fábrica?"))send({t:"factoryReset"})};
const pidMsg=save=>({t:"pid",en:$("pidEn").checked,kp:+$("kp").value,ki:+$("ki").value,kd:+$("kd").value,save});
$("bPid").onclick=()=>{send(pidMsg(false));toast("PID aplicado (sin guardar)","ok")};
$("bPidSave").onclick=()=>send(pidMsg(true));

// ------------------------------------------------------------ pestañas y avisos
function showTab(t){$("pCtl").classList.toggle("hidden",t!=="ctl");$("pCfg").classList.toggle("hidden",t!=="cfg");
  $("tabCtl").classList.toggle("act",t==="ctl");$("tabCfg").classList.toggle("act",t==="cfg");if(t==="ctl")drawTraj()}
$("tabCtl").onclick=()=>showTab("ctl");$("tabCfg").onclick=()=>showTab("cfg");
let tt;function toast(t,l="ok"){const e=$("toast");e.textContent=t;e.className="show "+l;clearTimeout(tt);tt=setTimeout(()=>e.className="",3500)}

connect();drawTraj();
</script>
</body>
</html>
)rawliteral";
