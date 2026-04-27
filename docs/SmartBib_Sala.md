# Documentação do Código SmartBib_Sala

Este código foi desenvolvido para um ESP32 e implementa um sistema de gerenciamento de occupancy de sala de biblioteca via botões, LEDs e comunicação MQTT.

## Visão Geral

O sistema monitora o estado de ocupação de uma sala (livre/reservada/ocupada) através de um botão e exibe o status via LEDs, além de publicar mensagens ao servidor MQTT e responder a comandos externos.

## Configurações de Rede

### WiFi
| Parâmetro | Descrição |
|----------|-----------|
| `ssid` | Nome da rede WiFi |
| `password` | Senha da rede WiFi |

### MQTT
| Parâmetro | Descrição |
|----------|-----------|
| `mqtt_server` | Broker MQTT (broker.hivemq.com) |
| `topico_status` | Tópico para publicação do status (`senac/biblioteca/sala1/status`) |
| `topico_reserva` | Tópico para receber comando de reserva (`senac/biblioteca/sala1/reserva`) |
| `topico_liberar` | Tópico para receber comando de liberação (`senac/biblioteca/sala1/liberar`) |

## Hardware

### Pinos Utilizados
| Pino | Função | Descrição |
|-----|-------|-----------|
| `4` | INPUT_PULLUP | Botão para togglear estado da sala |
| `2` | OUTPUT | LED Verde (indica sala livre) |
| `5` | OUTPUT | LED Vermelho (indica sala ocupada) |
| `21` | OUTPUT | LED Amarelo (indica sala reservada) |

## Estados do Sistema

O sistema possui 3 estados possíveis:

| Estado | LED | Descrição |
|--------|-----|-----------|
| `LIVRE` | Verde | Sala disponível para uso |
| `RESERVADA` | Amarelo | Sala reservada, aguardando usuário pressionar botão |
| `OCUPADA` | Vermelho | Sala em uso |

## Variáveis de Estado

| Variável | Tipo | Descrição |
|----------|------|-----------|
| `estadoAtual` | enum EstadoSala | Estado atual da sala (LIVRE, RESERVADA, OCUPADA) |
| `ultimoEstadoBotao` | bool | Estado anterior do botão (para debounce) |
| `tempoUltimoDebounce` | unsigned long | Timestamp da última leitura válida |
| `tempoDebounce` | unsigned long | Tempo de debounce (50ms) |

## Funções

### `setup_wifi()`
Conecta o ESP32 à rede WiFi configurada. Aguarda até que a conexão seja estabelecida e exibe o IP obtido no Serial Monitor.

### `callback(char* topic, byte* payload, unsigned int length)`
Função chamada quando uma mensagem MQTT é recebida. Processa mensagens nos tópicos:
- `topico_reserva`: Recebe comando "reservada" e muda estado para RESERVADA
- `topico_liberar`: Recebe comando "liberar" e muda estado para LIVRE

### `reconnect()`
Mantém a conexão MQTT ativa. Se a conexão for perdida, tenta reconectar a cada 2 segundos. Ao reconectar, subscreve nos tópicos de comando e atualiza LEDs.

### `atualizarSala()`
Atualiza o estado dos LEDs conforme o estado atual e publica o status no tópico MQTT:
- **LIVRE**: LED Verde = HIGH, LED Amarelo = LOW, LED Vermelho = LOW, publish "livre"
- **RESERVADA**: LED Verde = LOW, LED Amarelo = HIGH, LED Vermelho = LOW, publish "reservada"
- **OCUPADA**: LED Verde = LOW, LED Amarelo = LOW, LED Vermelho = HIGH, publish "ocupada"

### `setup()`
- Inicializa a comunicação serial (115200 baud)
- Configura os pinos dos LEDs como OUTPUT
- Configura o pino do botão como INPUT_PULLUP
- Define LEDs iniciais (verde acesso, demais apagados)
- Inicia conexão WiFi
- Configura o servidor MQTT
- Define a função de callback para mensagens MQTT

### `loop()`
1. Verifica conexão MQTT e reconnect se necessário
2. Processa mensagens MQTT recebidas
3. Realiza leitura do botão com **debounce** (50ms)
4. Lógica do botão:
   - Se estado for LIVRE ou RESERVADA → muda para OCUPADA
   - Se estado for OCUPADA → muda para LIVRE
5. Atualiza LEDs e publica no MQTT

## Fluxo de Funcionamento

```
┌─────────────────────────────────────────────────────────────┐
│                    SALA LIVRE (LED Verde)                    │
│                         Estado: LIVRE                        │
└─────────────────────────┬───────────────────────────────────┘
                          │
          ┌───────────────┴───────────────┐
          │                               │
          ▼                               ▼
┌─────────────────┐             ┌─────────────────────────┐
│ Aluno chega     │             │ Aluno faz reserva       │
│ (sem reserva)   │             │ via app (horário X-Y)   │
└────────┬────────┘             └────────────┬─────────────┘
         │                                 │
         │      ┌──────────────────────────┤
         ▼      ▼                          ▼
┌─────────────────┐             ┌─────────────────────────┐
│ Pressiona      │             │ LED AMARELO             │
│ botão          │             │ Estado: RESERVADA      │
└────────┬────────┘             │ Timer 10 min inicia    │
         │                      └───────────┬─────────────┘
         ▼                                    │
┌─────────────────┐              ┌────────────┴────────────┐
│ LED VERMELHO    │              │                          │
│ Estado: OCUPADA │              ▼                          ▼
└─────────────────┘    ┌─────────────────┐    ┌─────────────────┐
                      │ Pressiona botão │    │ Não pressiona   │
                      │ (em até 10 min) │    │ (após 10 min)   │
                      └────────┬────────┘    └────────┬────────┘
                               │                      │
                               ▼                      ▼
                      ┌─────────────────┐   ┌─────────────────┐
                      │ LED VERMELHO    │   │ LED VERDE       │
                      │ Estado: OCUPADA │   │ Estado: LIVRE   │
                      └─────────────────┘   │ Reserva cancelada│
                                         └─────────────────┘
```

## Mensagens MQTT

### Publicação (ESP32 → Broker)
| Tópico | Mensagem | Descrição |
|--------|----------|-----------|
| `senac/biblioteca/sala1/status` | `livre` | Sala disponível |
| `senac/biblioteca/sala1/status` | `reservada` | Sala reservada |
| `senac/biblioteca/sala1/status` | `ocupada` | Sala em uso |

### Assinatura (ESP32 ← Broker)
| Tópico | Mensagem | Ação |
|--------|----------|------|
| `senac/biblioteca/sala1/reserva` | `reservada` | muda estado para RESERVADA |
| `senac/biblioteca/sala1/liberar` | `liberar` | muda estado para LIVRE |

## Credenciais

> **Atenção**: Altere as credenciais WiFi antes de fazer upload para o ESP32:
> - `ssid` = "Nome_Wifi"
> - `password` = "senha"

## Componentes Necessários

- 1x ESP32 DevKit
- 1x Botão push-button
- 3x LEDs (verde, amarelo, vermelho)
- 3x Resistores 330Ω
- Fios jumper
- Protoboard (opcional)