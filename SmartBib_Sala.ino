#include <WiFi.h>
#include <PubSubClient.h>

// ================= WIFI =================
const char* ssid = "Nome_Wifi";
const char* password = "senha";

// ================= MQTT =================
const char* mqtt_server = "broker.hivemq.com";
const char* topico_status = "senac/biblioteca/sala1/status";

WiFiClient espClient;
PubSubClient client(espClient);

// ================= PINOS =================
#define BOTAO 4
#define LED_VERDE 2
#define LED_VERMELHO 5

// ================= VARIÁVEIS DE ESTADO =================
bool salaOcupada = false;
bool ultimoEstadoBotao = HIGH;
unsigned long tempoUltimoDebounce = 0;
unsigned long tempoDebounce = 50; // 50ms para evitar duplo clique

// ================= WIFI SETUP =================
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando ao WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

// ================= MQTT RECONNECT =================
void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando ao MQTT...");

    // ID do cliente deve ser único
    if (client.connect("ESP32_SmartBib_Sala1")) { 
      Serial.println("conectado!");
      // Publica o estado inicial ao conectar
      atualizarSala(); 
    } else {
      Serial.print("falha, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 2s...");
      delay(2000);
    }
  }
}

// ================= LÓGICA DA SALA =================
void atualizarSala() {
  if (salaOcupada) {
    digitalWrite(LED_VERDE, LOW);
    digitalWrite(LED_VERMELHO, HIGH);
    client.publish(topico_status, "ocupada");
    Serial.println("Status: Sala Ocupada");
  } else {
    digitalWrite(LED_VERMELHO, LOW);
    digitalWrite(LED_VERDE, HIGH);
    client.publish(topico_status, "livre");
    Serial.println("Status: Sala Livre");
  }
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);

  pinMode(BOTAO, INPUT_PULLUP);
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);

  // Estado inicial padrão
  digitalWrite(LED_VERDE, HIGH);
  digitalWrite(LED_VERMELHO, LOW);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

// ================= LOOP =================
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Leitura do botão com Debounce
  int leituraAtual = digitalRead(BOTAO);

  if (leituraAtual != ultimoEstadoBotao) {
    tempoUltimoDebounce = millis();
  }

  if ((millis() - tempoUltimoDebounce) > tempoDebounce) {
    // Se o estado mudou para pressionado (LOW)
    if (leituraAtual == LOW && ultimoEstadoBotao == HIGH) {
      salaOcupada = !salaOcupada; // Inverte o estado da sala
      atualizarSala();            // Atualiza LEDs e publica no MQTT
    }
  }
  
  ultimoEstadoBotao = leituraAtual;
}