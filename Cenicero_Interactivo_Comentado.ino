// Bibliotecas para Wi-Fi, servidor web, comunicación I2C y pantalla LCD.
#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// =====================================================

// CENICERO INTERACTIVO
// ESP32 crea su propia red Wi-Fi (no necesita internet)
// Dos sensores FC-51 + una pantalla LCD I2C

// =====================================================

// ---------- RED WIFI CREADA POR EL ESP32 ----------

// Credenciales de la red Wi-Fi que crea el propio ESP32.
const char* AP_SSID = "CeniceroInteractivo";
const char* AP_PASSWORD = "Cenicero26";


// ---------- SENSORES ----------

// Pines GPIO donde se conectan las salidas digitales de los sensores FC-51.
const int SENSOR_1 = 18;
const int SENSOR_2 = 19;

// ---------- LCD I2C ----------

// Pines I2C utilizados por la pantalla LCD.
const int SDA_PIN = 21;
const int SCL_PIN = 22;
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Muchos FC-51 activan en LOW.

// Si tus sensores funcionan al revés, cambia a false.

// Configuración de detección: true significa que el sensor detecta cuando entrega LOW.
const bool ACTIVO_EN_LOW = true;
const unsigned long ANTIRREBOTE_MS = 50;

// ---------- TIEMPOS DE PANTALLA ----------

const unsigned long TIEMPO_RED_MS = 3000; // 3 segundos
const unsigned long TIEMPO_PREGUNTA_MS = 4000; // 4 segundos

// ---------- CONTADORES ----------

// Contadores independientes; se limitan a 100 colillas por contenedor.
unsigned long contador1 = 0;
unsigned long contador2 = 0;

// Guarda el estado de cada sensor para filtrar rebotes y evitar conteos duplicados.
struct EstadoSensor {
int lecturaAnterior;

int estadoEstable;

unsigned long ultimoCambio;

bool armado;

};



EstadoSensor sensor1;

EstadoSensor sensor2;



// Servidor HTTP local que atiende la página principal y la ruta /api.
WebServer server(80);



// ---------- PAGINA WEB ----------

// Página del dashboard almacenada en memoria de programa del ESP32.
// Incluye estilos, gráficos y JavaScript para consultar y mostrar los conteos.
const char PAGINA_HTML[] PROGMEM = R"rawliteral(

<!doctype html>

<html lang="es">

<head>

<meta charset="utf-8">

<meta name="viewport" content="width=device-width, initial-scale=1">

<meta name="theme-color" content="#174734">

<title>Cenicero interactivo</title>

<style>

:root{--verde:#174734;--verde2:#28704e;--fondo:#f3f6f2;--texto:#183126;--muted:#65766c}

*{box-sizing:border-box}

body{margin:0;font-family:Arial,Helvetica,sans-serif;background:var(--fondo);color:var(--texto)}

header{background:linear-gradient(135deg,var(--verde),var(--verde2));color:white;padding:28px 18px 34px}

.wrap{width:min(900px,calc(100% - 32px));margin:auto}

.kicker{font-size:.78rem;letter-spacing:.12em;text-transform:uppercase;opacity:.8}

h1{margin:8px 0;font-size:clamp(1.8rem,5vw,2.8rem)}

header p{margin:0;color:#e0eee5;line-height:1.5}

main{padding:24px 0 40px}

.status{display:flex;align-items:center;gap:8px;color:var(--muted);font-size:.9rem;margin:0 0 18px}

.dot{width:9px;height:9px;border-radius:50%;background:#20a464}

.cards{display:grid;grid-template-columns:repeat(2,minmax(0,1fr));gap:16px}

.card{background:white;border:1px solid #e0e8e1;border-radius:20px;padding:22px;box-shadow:0 8px 24px #1831260a}

.label{font-size:.78rem;color:var(--muted);margin:0 0 8px;letter-spacing:.04em}

.name{font-size:1.2rem;font-weight:700;margin:0}

.number{font-size:clamp(3.2rem,10vw,5rem);line-height:1.1;font-weight:800;color:var(--verde);margin:18px 0 8px}

.unit{color:var(--muted);font-size:.95rem}

.total-card{background:var(--verde);color:white;border-radius:20px;padding:22px;margin-top:16px;box-shadow:0 8px 24px #1831260a}

.total-card .label{color:#dcebe0}

.total-number{font-size:clamp(3rem,9vw,4.5rem);line-height:1.1;font-weight:800;color:white;margin:14px 0 8px}

.impact{background:#e8efe8;border:1px solid #d5e4d7;border-radius:18px;padding:20px;margin-top:16px;color:#174734}

.impact-title{font-size:.78rem;letter-spacing:.06em;text-transform:uppercase;color:#526c5b;margin:0 0 8px}

.impact-message{font-size:clamp(1.15rem,3vw,1.45rem);font-weight:700;line-height:1.4;margin:0}

.icon{width:42px;height:42px;display:grid;place-items:center;border-radius:13px;background:#e6f2e9;font-size:1.3rem;margin-bottom:15px}

.note{margin-top:18px;background:#e8efe8;padding:16px;border-radius:14px;color:#3e5948;font-size:.9rem;line-height:1.5}

footer{color:var(--muted);font-size:.8rem;text-align:center;margin-top:24px}


.stats{background:white;border:1px solid #e0e8e1;border-radius:20px;padding:22px;margin-top:16px;box-shadow:0 8px 24px #1831260a}
.stats h2{font-size:1.2rem;margin:0 0 6px;color:var(--verde)}
.stats p{color:var(--muted);font-size:.9rem;margin:0 0 14px}
.chart-wrap{height:260px;position:relative;width:100%}
#statsChart{width:100%;height:100%;display:block}
.alert-card{background:#fff;border:1px solid #e0e8e1;border-radius:20px;padding:22px;margin-top:16px}
.alert-card h2{font-size:1.2rem;margin:0 0 8px;color:var(--verde)}
.alert-card p{color:var(--muted);line-height:1.5;margin:0 0 14px}
.alert-button{border:0;border-radius:12px;padding:13px 18px;background:var(--verde);color:white;font-weight:700;font-size:.95rem;cursor:pointer}
.alert-button:disabled{opacity:.65;cursor:default}
.full-status{margin-top:14px;padding:13px;border-radius:12px;background:#e8efe8;color:#174734;font-weight:700;line-height:1.45}
.full-status.warning{background:#fff1df;color:#8a4b00}
.full-status.full{background:#fde8e7;color:#9d2924}
.stat-summary{display:grid;grid-template-columns:repeat(2,minmax(0,1fr));gap:12px;margin-top:14px}
.stat-item{background:#f3f6f2;border-radius:12px;padding:13px}
.stat-item strong{display:block;font-size:1.35rem;color:var(--verde);margin-top:4px}

@media(max-width:560px){.cards{grid-template-columns:1fr}.card{padding:20px}.number{font-size:4rem}.chart-wrap{height:220px}}

</style>

</head>

<body>

<header><div class="wrap">

<div class="kicker">Proyecto de sostenibilidad</div>

<h1>Cenicero interactivo</h1>

<p>Una colilla menos en el suelo, un entorno más limpio.</p>

</div></header>

<main class="wrap">

<div class="status"><span class="dot"></span><span id="estado">Conectando con el ESP32…</span></div>

<section class="cards">

<article class="card">

<div class="icon">🌿</div>

<p class="label">CONTENEDOR 1 · SENSOR GPIO 18</p>

<p class="name">CHAYANNE</p>

<div class="number" id="c1">—</div>

<div class="unit">colillas registradas</div>

</article>

<article class="card">

<div class="icon">♻️</div>

<p class="label">CONTENEDOR 2 · SENSOR GPIO 19</p>

<p class="name">LUIS MIGUEL</p>

<div class="number" id="c2">—</div>

<div class="unit">colillas registradas</div>

</article>

</section>

<section class="total-card">

<p class="label">TOTAL DE COLILLAS REGISTRADAS</p>

<div class="total-number" id="total">—</div>

<div class="unit" style="color:#e0eee5">colillas entre ambos contenedores</div>

</section>

<section class="stats">
  <h2>Impacto ambiental</h2>
  <p>Indicadores de recolección calculados con los registros de los dos contenedores.</p>
  <div class="stat-summary">
    <div class="stat-item">Colillas retiradas del suelo<strong id="colillasRetiradas">0</strong></div>
    <div class="stat-item">Capacidad total utilizada<strong id="capacidadTotal">0%</strong></div>
  </div>
  <div class="note">
    Cada colilla recolectada es una colilla que no queda abandonada en el entorno.
    No se muestra una cantidad de litros de agua “salvados” porque el impacto de una colilla
    depende de las condiciones ambientales y no existe una equivalencia universal fiable.
  </div>
  <p class="label" style="margin-top:14px">PERÍODO DEL REGISTRO</p>
  <p id="periodoRegistro" style="margin:0;color:var(--muted)">Desde que se encendió el cenicero.</p>
</section>

<section class="stats">
  <h2>Estadísticas de recolección</h2>
  <p>Comparación de colillas registradas por contenedor. Cada uno tiene una capacidad máxima de 100.</p>
  <div class="chart-wrap"><canvas id="statsChart" role="img" aria-label="Gráfico de colillas por contenedor"></canvas></div>
  <div class="stat-summary">
    <div class="stat-item">CHAYANNE · ocupación<strong id="porcentaje1">0%</strong></div>
    <div class="stat-item">LUIS MIGUEL · ocupación<strong id="porcentaje2">0%</strong></div>
  </div>
</section>

<section class="alert-card">
  <h2>Alerta de contenedor lleno</h2>
  <p>Activa las alertas para recibir un aviso cuando CHAYANNE o LUIS MIGUEL llegue a 100 colillas.</p>
  <button class="alert-button" id="activarAlertas" type="button" onclick="activarAlertas()">Activar alertas de llenado</button>
  <div class="full-status" id="estadoLlenado" aria-live="polite">Ambos contenedores tienen capacidad disponible.</div>
</section>

<section class="impact">

<p class="impact-title">Impacto ambiental</p>

<p class="impact-message" id="impacto">¡Ya recolectamos 0 colillas con este cenicero!</p>

</section>

<div class="note">La página se actualiza automáticamente cada segundo. El total corresponde a la suma de CHAYANNE y LUIS MIGUEL. Esta red funciona sin internet.</div>

<footer>Dirección local: <strong>http://192.168.4.1</strong> · Actualizado: <span id="actualizado">—</span></footer>

</main>

<script>

// Lógica del dashboard: capacidad por contenedor y control de notificaciones.
const CAPACIDAD_MAXIMA = 100;
let alertasActivadas = false;
let notificado1 = false;
let notificado2 = false;

// Dibuja en el canvas una comparación visual de los conteos de ambos contenedores.
function dibujarGrafico(valor1, valor2) {
  const canvas = document.getElementById('statsChart');
  if (!canvas) return;
  const ctx = canvas.getContext('2d');
  const dpr = window.devicePixelRatio || 1;
  const ancho = canvas.clientWidth || 600;
  const alto = canvas.clientHeight || 260;
  canvas.width = ancho * dpr;
  canvas.height = alto * dpr;
  ctx.setTransform(dpr, 0, 0, dpr, 0, 0);
  ctx.clearRect(0, 0, ancho, alto);

  const margen = {izq: 38, der: 14, arriba: 18, abajo: 42};
  const grafW = ancho - margen.izq - margen.der;
  const grafH = alto - margen.arriba - margen.abajo;

  ctx.font = '12px Arial';
  ctx.textAlign = 'right';
  ctx.textBaseline = 'middle';
  ctx.fillStyle = '#65766c';

  for (let valor = 0; valor <= 100; valor += 20) {
    const y = margen.arriba + grafH - (valor / 100) * grafH;
    ctx.strokeStyle = '#e5ece6';
    ctx.lineWidth = 1;
    ctx.beginPath();
    ctx.moveTo(margen.izq, y);
    ctx.lineTo(ancho - margen.der, y);
    ctx.stroke();
    ctx.fillText(String(valor), margen.izq - 8, y);
  }

  const barras = [
    {nombre: 'CHAYANNE', valor: Math.min(valor1, 100), color: '#28704e'},
    {nombre: 'LUIS MIGUEL', valor: Math.min(valor2, 100), color: '#78a98a'}
  ];
  const centro1 = margen.izq + grafW * 0.28;
  const centro2 = margen.izq + grafW * 0.72;
  const centros = [centro1, centro2];
  const barraW = Math.min(72, grafW * 0.22);

  barras.forEach((barra, i) => {
    const h = (barra.valor / 100) * grafH;
    const x = centros[i] - barraW / 2;
    const y = margen.arriba + grafH - h;
    ctx.fillStyle = barra.color;
    ctx.beginPath();
    ctx.roundRect(x, y, barraW, Math.max(h, 1), 7);
    ctx.fill();

    ctx.fillStyle = '#183126';
    ctx.font = 'bold 13px Arial';
    ctx.textAlign = 'center';
    ctx.fillText(String(barra.valor), centros[i], Math.max(margen.arriba + 10, y - 10));

    ctx.fillStyle = '#526c5b';
    ctx.font = '11px Arial';
    ctx.fillText(barra.nombre, centros[i], alto - 17);
  });
}

// Actualiza el mensaje de capacidad y genera una notificación si se alcanzan 100 colillas.
function actualizarEstadoLlenado(valor1, valor2) {
  const estado = document.getElementById('estadoLlenado');
  const lleno1 = valor1 >= CAPACIDAD_MAXIMA;
  const lleno2 = valor2 >= CAPACIDAD_MAXIMA;

  if (lleno1 || lleno2) {
    const nombres = [];
    if (lleno1) nombres.push('CHAYANNE');
    if (lleno2) nombres.push('LUIS MIGUEL');
    estado.className = 'full-status full';
    estado.textContent = '¡Contenedor lleno! ' + nombres.join(' y ') +
      ' alcanzó 100 de 100 colillas. Se debe vaciar.';
  } else if (valor1 >= 80 || valor2 >= 80) {
    estado.className = 'full-status warning';
    estado.textContent = 'Atención: un contenedor está cerca de llenarse.';
  } else {
    estado.className = 'full-status';
    estado.textContent = 'Ambos contenedores tienen capacidad disponible.';
  }

  if (alertasActivadas && 'Notification' in window &&
      Notification.permission === 'granted') {
    if (lleno1 && !notificado1) {
      new Notification('Cenicero interactivo: contenedor lleno', {
        body: 'CHAYANNE alcanzó 100 colillas. Es necesario vaciarlo.'
      });
      notificado1 = true;
    }
    if (lleno2 && !notificado2) {
      new Notification('Cenicero interactivo: contenedor lleno', {
        body: 'LUIS MIGUEL alcanzó 100 colillas. Es necesario vaciarlo.'
      });
      notificado2 = true;
    }
  }
}

// Solicita permiso al navegador para mostrar avisos de contenedor lleno.
async function activarAlertas() {
  const boton = document.getElementById('activarAlertas');
  if (!('Notification' in window)) {
    alert('Este navegador no permite notificaciones. Mantén esta página abierta para ver la alerta de contenedor lleno.');
    return;
  }
  try {
    const permiso = await Notification.requestPermission();
    if (permiso === 'granted') {
      alertasActivadas = true;
      boton.textContent = 'Alertas activadas';
      boton.disabled = true;
      alert('Alertas activadas. Recibirás un aviso cuando un contenedor llegue a 100 colillas, mientras esta página permanezca abierta.');
      actualizar();
    } else {
      alert('No se concedió permiso para las notificaciones. La alerta seguirá apareciendo dentro de esta página.');
    }
  } catch (error) {
    alert('No fue posible activar las notificaciones en este navegador.');
  }
}

// Consulta /api cada segundo y actualiza los números, porcentajes y gráfico del dashboard.
async function actualizar(){
  try{
    const respuesta=await fetch('/api',{cache:'no-store'});
    if(!respuesta.ok) throw new Error('Respuesta no válida');
    const datos=await respuesta.json();

    document.getElementById('c1').textContent=datos.chayanne;
    document.getElementById('c2').textContent=datos.luisMiguel;
    document.getElementById('total').textContent=datos.total;
    document.getElementById('impacto').textContent =
      '¡Ya recolectamos ' + datos.total + ' colillas con este cenicero!';
    document.getElementById('colillasRetiradas').textContent=datos.total;
    document.getElementById('capacidadTotal').textContent =
      Math.min(100, Math.round(datos.total / (CAPACIDAD_MAXIMA * 2) * 100)) + '%';
    document.getElementById('estado').textContent='Conectado directamente al cenicero';
    document.getElementById('actualizado').textContent=new Date().toLocaleTimeString();

    document.getElementById('porcentaje1').textContent =
      Math.min(100, Math.round(datos.chayanne / CAPACIDAD_MAXIMA * 100)) + '%';
    document.getElementById('porcentaje2').textContent =
      Math.min(100, Math.round(datos.luisMiguel / CAPACIDAD_MAXIMA * 100)) + '%';

    dibujarGrafico(datos.chayanne, datos.luisMiguel);
    actualizarEstadoLlenado(datos.chayanne, datos.luisMiguel);
  }catch(error){
    document.getElementById('estado').textContent='Reconectando… revisa que estés conectado a CeniceroInteractivo';
  }
}

actualizar();
setInterval(actualizar,1000);
window.addEventListener('resize', () => {
  const c1 = Number(document.getElementById('c1').textContent) || 0;
  const c2 = Number(document.getElementById('c2').textContent) || 0;
  dibujarGrafico(c1, c2);
});

</script>

</body>

</html>

)rawliteral";



// ---------- DETECCIÓN Y CONTEO DE COLILLAS ----------

// Interpreta la lectura digital según la configuración ACTIVO_EN_LOW.
bool detectaObjeto(int lectura) {

return ACTIVO_EN_LOW ? lectura == LOW : lectura == HIGH;

}

// Inicializa el estado del sensor para que una detección presente al encender
// no se contabilice como una colilla nueva.
void iniciarSensor(EstadoSensor &s, int pin) {
int lectura = digitalRead(pin);
s.lecturaAnterior = lectura;
s.estadoEstable = lectura;
s.ultimoCambio = millis();
s.armado = !detectaObjeto(lectura);

}

// Muestra en la pantalla LCD los conteos actuales de ambos contenedores.
void mostrarContadoresLCD() {
lcd.setCursor(0, 0);
lcd.print("CHAYANNE:");
lcd.print(contador1);
lcd.print(" ");


lcd.setCursor(0, 1);
lcd.print("LUIS MIGUEL:");
lcd.print(contador2);
lcd.print(" ");

}

// Lee un sensor, aplica el filtro antirrebote y suma una unidad por detección.
// El sensor debe volver a quedar libre antes de poder registrar otra colilla.
void actualizarSensor(EstadoSensor &s, int pin, int numeroSensor) {
unsigned long ahora = millis();
int lectura = digitalRead(pin);

// Registra el instante en que cambia la lectura para iniciar el filtro antirrebote.
if (lectura != s.lecturaAnterior) {
s.lecturaAnterior = lectura;
s.ultimoCambio = ahora;

}

// Solo acepta el cambio cuando se mantiene estable durante ANTIRREBOTE_MS.
if (lectura != s.estadoEstable &&
ahora - s.ultimoCambio >= ANTIRREBOTE_MS) {
s.estadoEstable = lectura;
bool detectado = detectaObjeto(s.estadoEstable);


// Cuenta una detección nueva únicamente si el sensor estaba armado.
if (detectado && s.armado) {
if (numeroSensor == 1) {
        if (contador1 < 100) contador1++;
      } else {
        if (contador2 < 100) contador2++;
      }



Serial.print("Sensor ");
Serial.print(numeroSensor);
Serial.print(" | CHAYANNE: ");
Serial.print(contador1);
Serial.print(" | LUIS MIGUEL: ");
Serial.println(contador2);

mostrarContadoresLCD();
s.armado = false;

}

if (!detectado) {
s.armado = true;
Serial.print("Sensor ");
Serial.print(numeroSensor);
Serial.println(" libre");

}

}

}


// ---------- RUTAS DE LA PÁGINA WEB Y ENTREGA DE DATOS ----------

// Envía al navegador el contenido HTML del dashboard.
void manejarInicio() {
server.send_P(200, "text/html; charset=utf-8", PAGINA_HTML);

}

// Construye una respuesta JSON con los dos conteos y el total acumulado.
void manejarAPI() {
String json = "{\"chayanne\":";
json += String(contador1);
json += ",\"luisMiguel\":";
json += String(contador2);
json += ",\"total\":";
json += String(contador1 + contador2);
json += "}";
server.send(200, "application/json", json);

}


// Se ejecuta una vez al encender: inicia comunicación serial, sensores, LCD,
// punto de acceso Wi-Fi y servidor web.
void setup() {
Serial.begin(115200);

pinMode(SENSOR_1, INPUT);
pinMode(SENSOR_2, INPUT);

Wire.begin(SDA_PIN, SCL_PIN);
lcd.init();
lcd.backlight();
lcd.clear();


iniciarSensor(sensor1, SENSOR_1);
iniciarSensor(sensor2, SENSOR_2);

// Crea el punto de acceso Wi-Fi del prototipo; no requiere router ni internet.

WiFi.mode(WIFI_AP);
bool creada = WiFi.softAP(AP_SSID, AP_PASSWORD);
IPAddress ip = WiFi.softAPIP();



if (creada) {

Serial.println();
Serial.println("Red Wi-Fi del cenicero creada.");
Serial.print("Nombre de red: ");
Serial.println(AP_SSID);
Serial.print("Contrasena: ");
Serial.println(AP_PASSWORD);
Serial.print("Abre en el navegador: http://");
Serial.println(ip);

// Pantalla 1: pregunta para invitar a participar.

lcd.clear();
lcd.setCursor(0, 0);
lcd.print("PREGUNTA CUAL ES");
lcd.setCursor(0, 1);
lcd.print("TU PREFERIDO...");
delay(TIEMPO_PREGUNTA_MS);

// Pantalla 2: contadores.

lcd.clear();
mostrarContadoresLCD();

} else {

Serial.println("No se pudo crear la red Wi-Fi.");
lcd.clear();
lcd.setCursor(0, 0);
lcd.print("ERROR AL CREAR");
lcd.setCursor(0, 1);
lcd.print("RED WIFI");

}

// Asocia la página principal y la API con sus respectivas direcciones web.
server.on("/", HTTP_GET, manejarInicio);
server.on("/api", HTTP_GET, manejarAPI);
server.begin();
Serial.println("Servidor web iniciado.");

}

// Se repite continuamente: revisa ambos sensores y atiende solicitudes del navegador.
void loop() {
actualizarSensor(sensor1, SENSOR_1, 1);
actualizarSensor(sensor2, SENSOR_2, 2);
server.handleClient();
}