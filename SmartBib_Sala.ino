#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";

const char* mqtt_server = "broker.hivemq.com";
const char* topico_status = "senac/biblioteca/sala1/status";
const char* topico_comando = "senac/biblioteca/sala1/comando";

WiFiClient espClient;
PubSubClient client(espClient);

#define LED_VERDE 2
#define LED_VERMELHO 3

enum EstadoSala { LIVRE, OCUPADA };
EstadoSala estadoAtual = LIVRE;

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

void callback(char* topic, byte* payload, unsigned int length) {
  String mensagem = "";
  for (int i = 0; i < length; i++) mensagem += (char)payload[i];

  Serial.print("Mensagem recebida em ");
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(mensagem);

  if (String(topic) == topico_comando) {
    if (mensagem == "ocupar" || mensagem == "reservar") {
      estadoAtual = OCUPADA;
      atualizarSala();
      Serial.println("--> Sala OCUPADA (LED vermelho)");
    } else if (mensagem == "liberar") {
      estadoAtual = LIVRE;
      atualizarSala();
      Serial.println("--> Sala LIVRE (LED verde)");
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando ao MQTT...");
    if (client.connect("ESP32_SmartBib_Sala1_Wokwi")) {
      Serial.println("conectado!");
      client.subscribe(topico_comando);
      atualizarSala(); // publica status atual
    } else {
      Serial.print("falha, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 2s...");
      delay(2000);
    }
  }
}

void atualizarSala() {
  switch (estadoAtual) {
    case LIVRE:
      digitalWrite(LED_VERDE, HIGH);
      digitalWrite(LED_VERMELHO, LOW);
      client.publish(topico_status, "livre");
      Serial.println("Status publicado: livre");
      break;
    case OCUPADA:
      digitalWrite(LED_VERDE, LOW);
      digitalWrite(LED_VERMELHO, HIGH);
      client.publish(topico_status, "ocupada");
      Serial.println("Status publicado: ocupada");
      break;
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);
  digitalWrite(LED_VERDE, HIGH);
  digitalWrite(LED_VERMELHO, LOW);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();
}
