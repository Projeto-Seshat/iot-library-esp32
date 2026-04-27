#include <WiFi.h>
#include <PubSubClient.h>

// ================= WIFI =================
const char* ssid = "Nome_Wifi";
const char* password = "senha";

// ================= MQTT =================
const char* mqtt_server = "broker.hivemq.com";
const char* topico_status = "senac/biblioteca/sala1/status";
const char* topico_reserva = "senac/biblioteca/sala1/reserva";
const char* topico_liberar = "senac/biblioteca/sala1/liberar";

WiFiClient espClient;
PubSubClient client(espClient);

// ================= PINOS =================
#define BOTAO 4
#define LED_VERDE 2
#define LED_VERMELHO 5
#define LED_AMARELO 21

// ================= ESTADOS =================
enum EstadoSala { LIVRE, RESERVADA, OCUPADA };
EstadoSala estadoAtual = LIVRE;
bool ultimoEstadoBotao = HIGH;
unsigned long tempoUltimoDebounce = 0;
unsigned long tempoDebounce = 50;

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

// ================= MQTT CALLBACK =================
void callback(char* topic, byte* payload, unsigned int length) {
  String mensagem = "";
  for (int i = 0; i < length; i++) {
    mensagem += (char)payload[i];
  }

  if (String(topic) == topico_reserva) {
    if (mensagem == "reservada") {
      estadoAtual = RESERVADA;
      atualizarSala();
      Serial.println("Reserva recebida - Sala reservada");
    }
  } else if (String(topic) == topico_liberar) {
    if (mensagem == "liberar") {
      estadoAtual = LIVRE;
      atualizarSala();
      Serial.println("Tempo esgotado - Sala liberada");
    }
  }
}

// ================= MQTT RECONNECT =================
void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando ao MQTT...");

    if (client.connect("ESP32_SmartBib_Sala1")) {
      Serial.println("conectado!");
      client.subscribe(topico_reserva);
      client.subscribe(topico_liberar);
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
  switch (estadoAtual) {
    case LIVRE:
      digitalWrite(LED_VERDE, HIGH);
      digitalWrite(LED_AMARELO, LOW);
      digitalWrite(LED_VERMELHO, LOW);
      client.publish(topico_status, "livre");
      Serial.println("Status: Sala Livre");
      break;
    case RESERVADA:
      digitalWrite(LED_VERDE, LOW);
      digitalWrite(LED_AMARELO, HIGH);
      digitalWrite(LED_VERMELHO, LOW);
      client.publish(topico_status, "reservada");
      Serial.println("Status: Sala Reservada");
      break;
    case OCUPADA:
      digitalWrite(LED_VERDE, LOW);
      digitalWrite(LED_AMARELO, LOW);
      digitalWrite(LED_VERMELHO, HIGH);
      client.publish(topico_status, "ocupada");
      Serial.println("Status: Sala Ocupada");
      break;
  }
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);

  pinMode(BOTAO, INPUT_PULLUP);
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);
  pinMode(LED_AMARELO, OUTPUT);

  digitalWrite(LED_VERDE, HIGH);
  digitalWrite(LED_VERMELHO, LOW);
  digitalWrite(LED_AMARELO, LOW);

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

  int leituraAtual = digitalRead(BOTAO);

  if (leituraAtual != ultimoEstadoBotao) {
    tempoUltimoDebounce = millis();
  }

  if ((millis() - tempoUltimoDebounce) > tempoDebounce) {
    if (leituraAtual == LOW && ultimoEstadoBotao == HIGH) {
      if (estadoAtual == LIVRE || estadoAtual == RESERVADA) {
        estadoAtual = OCUPADA;
      } else {
        estadoAtual = LIVRE;
      }
      atualizarSala();
    }
  }

  ultimoEstadoBotao = leituraAtual;
}