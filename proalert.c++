#include <WiFi.h>
#include <HTTPClient.h>

// ================================
// Pinos do sensor e do LED
// ================================
#define TRIG_PIN 4
#define ECHO_PIN 3
#define LED_PIN 10

// ================================
// Wi-Fi
// ================================
const char* ssid = "THIAGO_CleonteNet";
const char* password = "29051973";
String serverUrl = "https://backend-pi-o2zm.onrender.com/sensor";

// ================================
// Níveis (em CM)
// ================================
#define NIVEL_NORMAL 20
#define NIVEL_MEDIO 10
#define NIVEL_ENCHENTE 5

long duracao;
float distancia;

// Piscar LED
unsigned long ultimoBlink = 0;
bool ledEstado = false;
int tempoOn = 200;
int tempoOff = 200;

// ================================
// SETUP
// ================================
void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  WiFi.begin(ssid, password);
  Serial.print("Conectando ao WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi conectado!");
}

// ================================
// LOOP PRINCIPAL
// ================================
void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    reconectarWiFi();
  }

  medirDistancia();
  controlarPiscaLED();
  enviarParaBackend(distancia);

  delay(200);
}

// ================================
// Medir distância do sensor HC-SR04
// ================================
void medirDistancia() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);

  duracao = pulseIn(ECHO_PIN, HIGH, 30000); // timeout para evitar travamento

  distancia = (duracao * 0.0343) / 2;

  if (duracao == 0) {
    Serial.println("⚠ Sensor sem resposta.");
    return;
  }

  Serial.print("Distância: ");
  Serial.println(distancia);

  // ===================
  // LÓGICA DE NÍVEIS
  // ===================
  
  if (distancia > NIVEL_NORMAL) {
    Serial.println("Nível NORMAL");
    tempoOn = 100;
    tempoOff = 900;
  }
  else if (distancia > NIVEL_MEDIO && distancia <= NIVEL_NORMAL) {
    Serial.println("Nível MÉDIO");
    tempoOn = 500;
    tempoOff = 500;
  }
  else {
    Serial.println("⚠ ALERTA MÁXIMO! RISCO DE ENCHENTE");
    tempoOn = 100;
    tempoOff = 100;
  }
}

// ================================
// Controle do Pisca sem travar o loop
// ================================
void controlarPiscaLED() {
  unsigned long agora = millis();

  if (ledEstado && (agora - ultimoBlink >= tempoOn)) {
    digitalWrite(LED_PIN, LOW);
    ledEstado = false;
    ultimoBlink = agora;
  }
  else if (!ledEstado && (agora - ultimoBlink >= tempoOff)) {
    digitalWrite(LED_PIN, HIGH);
    ledEstado = true;
    ultimoBlink = agora;
  }
}

// ================================
// Envio periódico para o backend
// ================================
void enviarParaBackend(float distancia) {
  static unsigned long ultimoEnvio = 0;

  // Envia a cada 12s
  if (millis() - ultimoEnvio < 12000) return;
  ultimoEnvio = millis();

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");

    String json = "{\"distancia\": " + String(distancia, 1) + "}";

    int resposta = http.POST(json);

    Serial.print("Enviado → Status HTTP: ");
    Serial.println(resposta);

    http.end();
  }
  else {
    Serial.println("❌ Sem Wi-Fi, não foi possível enviar.");
  }
}

// ================================
// Reconectar caso o WiFi caia
// ================================
void reconectarWiFi() {
  Serial.println("⚠ Wi-Fi desconectado! Tentando reconectar...");
  WiFi.disconnect();
  WiFi.begin(ssid, password);

  int tentativas = 0;

  while (WiFi.status() != WL_CONNECTED && tentativas < 10) {
    delay(500);
    Serial.print(".");
    tentativas++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✔ Reconectado!");
  } else {
    Serial.println("\n❌ Não reconectou. Tentará novamente no próximo loop.");
  }
}
