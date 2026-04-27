# Documentação do Código SmartBib_Sala

Este código foi desenvolvido para um ESP32 e implementa um sistema de gerenciamento de occupancy de sala de biblioteca via botões e LEDs, com comunicação MQTT.

## Visão Geral

O sistema monitora o estado de occupancy de uma sala (livre/ocupada) através de um botão e exibe o status via LEDs, além de publishes这个消息 ao servidor MQTT.

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

## Hardware

### Pinos Utilizados
| Pino | Função | Descrição |
|-----|-------|-----------|
| `4` | INPUT_PULLUP | Botão para togglear estado da sala |
| `2` | OUTPUT | LED Verde (indica sala livre) |
| `5` | OUTPUT | LED Vermelho (indica sala ocupada) |

## Variáveis de Estado

| Variável | Tipo | Descrição |
|----------|------|-----------|
| `salaOcupada` | bool | Estado atual da sala (false = livre, true = ocupada) |
| `ultimoEstadoBotao` | bool | Estado anterior do botão (para debounce) |
| `tempoUltimoDebounce` | unsigned long | Timestamp da última leitura válida |
| `tempoDebounce` | unsigned long | Tempo de debounce (50ms) |

## Funções

### `setup_wifi()`
Conecta o ESP32 à rede WiFi configurada. Aguarda até que a conexão seja estabelecida e exibe o IP obtido no Serial Monitor.

### `reconnect()`
Mantém a conexão MQTT ativa. Se a conexão for perdida, tenta reconectar a cada 2 segundos. Ao reconectar, publica o estado atual da sala.

### `atualizarSala()`
Atualiza o estado dos LEDs e publica o status no tópico MQTT:
- **Sala Ocupada**: LED Verde = LOW, LED Vermelho = HIGH, publishes "ocupada"
- **Sala Livre**: LED Vermelho = LOW, LED Verde = HIGH, publishes "livre"

### `setup()`
- Inicializa a comunicação serial (115200 baud)
- Configura os pinos dos LEDs como OUTPUT
- Configura o pino do botão como INPUT_PULLUP
- Define LEDs iniciais (verde acesso, vermelho apagado)
- Inicia conexão WiFi
- Configura o servidor MQTT

### `loop()`
1. Verifica conexão MQTT e reconnect se necessário
2. Realiza leitura do botão com **debounce** (50ms)
3. Quando o botão é pressionado (transição LOW → HIGH), inverte o estado da sala
4. Atualiza LEDs e publica no MQTT

## Fluxo de Funcionamento

```
┌─────────────┐     ┌─────────────┐
│   Botão     │────>│  Debounce  │
│   (Pino 4)  │     │   (50ms)   │
└─────────────┘     └─────────────┘
                          │
                          ▼
                   ┌─────────────┐
                   │  Inverte    │
                   │  salaOcupada│
                   └─────────────┘
                          │
                          ▼
                   ┌─────────────┐     ┌─────────────┐
                   │ atualizar  │────>│   MQTT     │
                   │   Sala     │     │  publish   │
                   └─────────────┘     └─────────────┘
                          │
              ┌───────────┴───────────┐
              ▼                       ▼
       ┌─────────────┐          ┌────────��────┐
       │ LED Verde  │          │LED Vermelho │
       │  (livre)  │          │ (ocupada)  │
       └─────────────┘          └─────────────┘
```

## Mensagens MQTT

O código publica no tópico `senac/biblioteca/sala1/status`:
- `"livre"` - Quando a sala está disponível
- `"ocupada"` - Quando a sala está em uso

## Credenciais

> **Atenção**: Altere as credenciais WiFi antes de fazer upload para o ESP32:
> - `ssid` = "Nome_Wifi"
> - `password` = "senha"