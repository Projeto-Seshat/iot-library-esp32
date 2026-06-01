#include <WiFi.h>
#include <PubSubClient.h>

// ================= WIFI =================
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// ================= MQTT =================
const char* mqtt_server = "broker.hivemq.com";
const char* topico_status = "senac/biblioteca/sala1/status";
const char* topico_comando = "senac/biblioteca/sala1/comando";

WiFiClient espClient;
PubSubClient client(espClient);

// ================= PINOS =================
#define LED_VERDE 2
#define LED_AMARELO 21
#define LED_VERMELHO 5

// ================= ESTADOS =================
enum EstadoSala { LIVRE, RESERVADA, OCUPADA };
EstadoSala estadoAtual = LIVRE;

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
}

// ================= MQTT CALLBACK =================
void callback(char* topic, byte* payload, unsigned int length) {
  String mensagem = "";
  for (int i = 0; i < length; i++) {
    mensagem += (char)payload[i];
  }

  Serial.print("Mensagem recebida no topico [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(mensagem);

  if (String(topic) == topico_comando) {
    if (mensagem == "reservar") {
      estadoAtual = RESERVADA;
      atualizarSala();
    } else if (mensagem == "ocupar") {
      estadoAtual = OCUPADA;
      atualizarSala();
    } else if (mensagem == "liberar") {
      estadoAtual = LIVRE;
      atualizarSala();
    }
  }
}

// ================= MQTT RECONNECT =================
void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando ao MQTT...");

    if (client.connect("ESP32_SmartBib_Sala1_Wokwi")) {
      Serial.println("conectado!");
      client.subscribe(topico_comando);
      atualizarSala();
    } else {
      Serial.print("falha, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 2s...");
      delay(2000);
    }
  }
}

// ================= LOGICA DA SALA =================
void atualizarSala() {
  switch (estadoAtual) {
    case LIVRE:
      digitalWrite(LED_VERDE, HIGH);
      digitalWrite(LED_AMARELO, LOW);
      digitalWrite(LED_VERMELHO, LOW);
      client.publish(topico_status, "livre");
      Serial.println("Status Atualizado: Sala Livre");
      break;
    case RESERVADA:
      digitalWrite(LED_VERDE, LOW);
      digitalWrite(LED_AMARELO, HIGH);
      digitalWrite(LED_VERMELHO, LOW);
      client.publish(topico_status, "reservada");
      Serial.println("Status Atualizado: Sala Reservada");
      break;
    case OCUPADA:
      digitalWrite(LED_VERDE, LOW);
      digitalWrite(LED_AMARELO, LOW);
      digitalWrite(LED_VERMELHO, HIGH);
      client.publish(topico_status, "ocupada");
      Serial.println("Status Atualizado: Sala Ocupada");
      break;
  }
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);

  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_AMARELO, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);

  // Estado inicial: Livre (verde aceso)
  digitalWrite(LED_VERDE, HIGH);
  digitalWrite(LED_AMARELO, LOW);
  digitalWrite(LED_VERMELHO, LOW);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

// ================= LOOP =================
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
