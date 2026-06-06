# SmartBib - IoT (ESP32)

Sistema embarcado com ESP32 para controle físico das salas de estudo da biblioteca SENAC. O dispositivo gerencia LEDs indicativos de status e se comunica via MQTT com o backend para sincronização em tempo real.

## Hardware

| Componente | Pino | Funcao |
|---|---|---|
| ESP32 DevKit | -- | Microcontrolador com WiFi |
| LED Verde | GPIO 2 | Sala LIVRE |
| LED Amarelo | GPIO 21 | Sala RESERVADA (aguardando confirmacao) |
| LED Vermelho | GPIO 5 | Sala OCUPADA |
| Botao (Push Button) | GPIO 4 (`INPUT_PULLUP`) | Alterna estado de ocupacao |
| Resistores 330ohm | -- | Protecao dos LEDs |

## Estados da Sala

| Estado | LED aceso | Significado |
|---|---|---|
| `LIVRE` | Verde | Sala disponivel para uso |
| `RESERVADA` | Amarelo | Reserva iniciada, aguardando presenca (timer de 5 min no backend) |
| `OCUPADA` | Vermelho | Usuario presente na sala |

## Transicoes de Estado

- **LIVRE** -- botao pressionado --> **OCUPADA** (aluno ocupou sem reserva)
- **LIVRE** -- comando MQTT `"reservada"` --> **RESERVADA** (backend notificou reserva)
- **RESERVADA** -- botao pressionado --> **OCUPADA** (aluno confirmou presenca)
- **RESERVADA** -- comando MQTT `"liberar"` --> **LIVRE** (timeout de confirmacao ou fim da aula)
- **OCUPADA** -- botao pressionado --> **LIVRE** (aluno liberou a sala)

O botao possui debounce de 50ms via software.

## Topicos MQTT

| Topico | Direcao | Payload | Finalidade |
|---|---|---|---|
| `senac/biblioteca/sala1/status` | Publica (ESP -> broker) | `"livre"`, `"reservada"`, `"ocupada"` | Reporta estado atual da sala |
| `senac/biblioteca/sala1/reserva` | Subscreve (broker -> ESP) | `"reservada"` | Comando para reservar a sala |
| `senac/biblioteca/sala1/liberar` | Subscreve (broker -> ESP) | `"liberar"` | Comando para liberar a sala |

Broker: `broker.hivemq.com:1883` (publico, sem autenticacao)
Client ID: `ESP32_SmartBib_Sala1`

## Estrutura do Repositorio

- `SmartBib_Sala.ino` -- Codigo-fonte Arduino para o ESP32
- `docs/` -- Documentacao tecnica e diagramas de arquitetura

## Configuracao

1. Edite as credenciais WiFi no codigo:
   ```cpp
   const char* ssid = "Nome_Wifi";
   const char* password = "senha";
   ```
2. Altere os topicos MQTT conforme o ID da sala (ex: `sala2`, `sala3`...)
3. Compile e grave no ESP32 via Arduino IDE

## Dependencias

- [WiFi.h](https://www.arduino.cc/reference/en/libraries/wifi/) (built-in ESP32)
- [PubSubClient.h](https://www.arduino.cc/reference/en/libraries/pubsubclient/) (MQTT)

---

*Faculdade Senac Pernambuco - Analise e Desenvolvimento de Sistemas (ADS)*
