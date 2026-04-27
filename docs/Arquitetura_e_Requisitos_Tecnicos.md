# 3. Arquitetura e Requisitos Técnicos

## 3.1 Diagrama de Arquitetura de Nuvem

### 3.1.1 Visão Geral da Arquitetura

```
┌─────────────────────────────────────────────────────────────────────────┐
│                           ARQUITETURA DO SISTEMA                        │
└─────────────────────────────────────────────────────────────────────────┘

                                    INTERNET
                                      │
                    ┌─────────────────┼─────────────────┐
                    │                 │                 │
                    ▼                 ▼                 ▼
            ┌───────────────┐ ┌───────────────┐ ┌───────────────┐
            │   App Expo    │ │   Web App     │ │  Recepcionista│
            │ (React Native)│ │   (React)     │ │   (Web App)  │
            └───────┬───────┘ └───────┬───────┘ └───────┬───────┘
                    │                 │                 │
                    └─────────────────┼─────────────────┘
                                      │
                                      ▼ HTTPS/REST API
                            ┌─────────────────────┐
                            │      BACKEND        │
                            │    (Node.js)        │
                            │                     │
                            │  ┌───────────────┐  │
                            │  │  API REST     │  │
                            │  │  - Reservas   │  │
                            │  │  - Status     │  │
                            │  └───────────────┘  │
                            │                     │
                            │  ┌───────────────┐  │
                            │  │  MQTT Client  │  │
                            │  │  - Publisher  │  │
                            │  │  - Subscriber │  │
                            │  └───────────────┘  │
                            └───────────┬─────────┘
                                        │
                            ┌───────────┴───────────┐
                            │                       │
                            ▼                       ▼
                    ┌───────────────┐       ┌───────────────┐
                    │  MQTT Broker  │       │   DATABASE    │
                    │ (HiveMQ Cloud) │       │  (PostgreSQL) │
                    │   :1883       │       │   (Supabase)  │
                    └───────┬───────┘       └───────────────┘
                            │
                            │ MQTT
                            ▼
┌─────────────────────────────────────────────────────────────────────────┐
│                           CAMADA IoT (BORDA)                            │
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐       │
│  │    SALA 1       │  │    SALA 2       │  │    SALA N       │       │
│  │  ┌───────────┐  │  │  ┌───────────┐  │  │  ┌───────────┐  │       │
│  │  │  ESP32    │  │  │  │  ESP32    │  │  │  │  ESP32    │  │       │
│  │  │           │  │  │  │           │  │  │  │           │  │       │
│  │  │ ○ Verde   │  │  │  │ ○ Verde   │  │  │  │ ○ Verde   │  │       │
│  │  │ ○ Amarelo │  │  │  │ ○ Amarelo │  │  │  │ ○ Amarelo │  │       │
│  │  │ ○ Vermelho│  │  │  │ ○ Vermelho│  │  │  │ ○ Vermelho│  │       │
│  │  │           │  │  │  │           │  │  │  │           │  │       │
│  │  │ [BOTAO]   │  │  │  │ [BOTAO]   │  │  │  │ [BOTAO]   │  │       │
│  │  └───────────┘  │  │  └───────────┘  │  │  └───────────┘  │       │
│  └─────────────────┘  └─────────────────┘  └─────────────────┘       │
└─────────────────────────────────────────────────────────────────────────┘
```

### 3.1.2 Fluxo de Dados - Ciclo de Vida de uma Reserva

```
    USUÁRIO              BACKEND              BANCO          ESP32
       │                   │                   │               │
       │ 1. Criar reserva  │                   │               │
       │ ───────────────> │                   │               │
       │                   │ 2. Validar horários│               │
       │                   │ ────────────────> │               │
       │                   │ <───────────────  │               │
       │                   │ 3. Verificar       │               │
       │                   │    conflitos       │               │
       │                   │                   │               │
       │ <─────────────── │ 4. Reserva criada │               │
       │   Confirmação    │                   │               │
       │                   │                   │               │
       │                   │ 5. Agendar timer  │               │
       │                   │    (até hora_inicio)│             │
       │                   │                   │               │
       │                   │ 6. No horário:     │               │
       │                   │    publish mqtt   │               │
       │                   │ ────────────────> │               │
       │                   │                   │ 7. LED amarelo│
       │                   │ 8. Iniciar timer │               │
       │                   │    10 minutos     │               │
       │                   │                   │               │
       │                   │                   │ 9. Pressiona   │
       │                   │                   │    botão      │
       │                   │                   │ <─────────────│
       │                   │ 10. Publish status│               │
       │                   │     "ocupada"     │               │
       │                   │ <─────────────── │               │
       │                   │ 11. Cancela timer │               │
       │                   │ 12. Atualiza BD   │               │
       │                   │                   │               │
       │                   │                   │ 13. LED vermelho│
       │                   │                   │               │
       │                   │ 14. No horário fim│               │
       │                   │     publish mqtt  │               │
       │                   │ ────────────────> │               │
       │                   │                   │ 15. LED verde  │
```

### 3.1.3 Estrutura de Tópicos MQTT

```
Tópicos por sala (scalável):

senac/biblioteca/sala{ID}/status     → PUBLICADO pelo ESP32 (livre|reservada|ocupada)
senac/biblioteca/sala{ID}/reserva     → PUBLICADO pelo Backend (reservada)
senac/biblioteca/sala{ID}/liberar     → PUBLICADO pelo Backend (liberar)

Exemplo prático:
- senac/biblioteca/sala1/status   (ESP32 publica: "livre")
- senac/biblioteca/sala1/reserva   (Backend publica: "reservada")
- senac/biblioteca/sala1/liberar   (Backend publica: "liberar")
```

### 3.1.4 Componentes da Borda (IoT)

| Componente | Especificação | Justificativa |
|-----------|---------------|---------------|
| **ESP32 DevKit** | 240MHz, WiFi + Bluetooth, 4MB Flash | Processamento suficiente, WiFi integrado, baixo custo (~R$35) |
| **LED Verde** | Pino GPIO 2, 330Ω resistor | Indica sala livre |
| **LED Amarelo** | Pino GPIO 21, 330Ω resistor | Indica sala reservada (timer ativo) |
| **LED Vermelho** | Pino GPIO 5, 330Ω resistor | Indica sala ocupada |
| **Botão Push** | Pino GPIO 4, INPUT_PULLUP | Confirma presença do aluno |
| **Protoboard** | 400 pontos | Prototipagem e conexões |

**Esquema de Conexão:**
```
ESP32 GPIO 2  ─────┐
                    ├───┤ Resistor 330Ω ──────┬──► LED Verde (+) ── GND
ESP32 GPIO 21 ─────┤
                    ├───┤ Resistor 330Ω ──────┬──► LED Amarelo (+) ── GND
ESP32 GPIO 5  ─────┤
                    ├───┤ Resistor 330Ω ──────┬──► LED Vermelho (+) ── GND
                    
ESP32 GPIO 4  ───────────────────────────────► BOTÃO (PULLUP interno)
                                            ── GND (via botão)
```

### 3.1.5 Justificativa dos Protocolos

| Protocolo | Camada | Justificativa |
|-----------|--------|---------------|
| **MQTT** | ESP32 ↔ Backend | Protocolo leve ideal para IoT. Baixo consumo de banda e energia. Permite comunicação bidirecional em tempo real. O ESP32 não suporta conexões HTTP persistentes de forma eficiente. QoS 0 para mensagens não críticas. |
| **REST/HTTPS** | App ↔ Backend | Padrão da indústria para APIs. Stateless, facilita escalabilidade horizontal. Compatível com qualquer cliente (mobile, web). Permite cache e autenticação via tokens JWT. |
| **WebSocket** | Backend ↔ Frontend (opcional) | Para atualizações em tempo real no app, substituindo polling constante. |

---

## 3.2 Requisitos Funcionais

| ID | Requisito | Descrição |
|----|-----------|-----------|
| RF-01 | Visualizar status das salas | O sistema deve exibir em tempo real se uma sala está livre, reservada ou ocupada. |
| RF-02 | Criar reserva | O aluno pode criar uma reserva informando: nome, sala, data, horário de início e fim. |
| RF-03 | Validar conflito de horários | O sistema não deve permitir reservas em horários sobrepostos na mesma sala. |
| RF-04 | Timer automático de 10 minutos | Ao chegar o horário da reserva, o sistema aguarda 10 minutos. Se o aluno não pressionar o botão na sala, a reserva é cancelada automaticamente. |
| RF-05 | Confirmar presença via botão | O botão físico na sala confirma a presença do aluno, mudando o LED para vermelho. |
| RF-06 | Atualização visual via LEDs | O ESP32 deve exibir o estado da sala através de LEDs (verde = livre, amarelo = reservada, vermelho = ocupada). |
| RF-07 | Verificação de reservas | A recepcionista pode consultar no sistema se um aluno possui reserva válida. |
| RF-08 | Cancelar reserva | O aluno pode cancelar sua própria reserva antes do horário de início. |
| RF-09 | Encerrar ocupação | Ao final da reserva ou ao pressionar o botão em sala ocupada, o sistema encerra a ocupação. |

---

## 3.3 Requisitos Não Funcionais

| ID | Requisito | Descrição | Meta |
|----|-----------|-----------|------|
| RNF-01 | Latência de atualização | Tempo entre ação (botão/reserva) e atualização nos LEDs/app. | < 500ms |
| RNF-02 | Disponibilidade | Sistema deve estar disponível durante horário de funcionamento da biblioteca. | 99% no horário comercial |
| RNF-03 | Capacidade | Suportar múltiplas salas simultaneamente. | Scalável via tópicos MQTT |
| RNF-04 | Segurança da comunicação | Dados transmitidos via HTTPS (app-backend) e MQTT sobre TCP. | TLS para produção |
| RNF-05 | Persistência | Dados de reservas mantidos em banco de dados relacional. | PostgreSQL (Supabase) |
| RNF-06 | Custo de hospedagem | Solução deve funcionar com recursos gratuitos. | Custo zero com Supabase + HiveMQ Cloud |
| RNF-07 | Manutenibilidade | Código modular, separação clara entre camadas. | Arquitetura MVC no backend |
| RNF-08 | Escalabilidade | Fácil adição de novas salas sem modificar código existente. | Tópicos MQTT dinâmicos |

---

## 3.4 Cibersegurança e Governança (LGPD)

### 3.4.1 Estratégia de Security by Design

A arquitetura foi pensada seguindo os princípios de **Security by Design**, garantindo que a segurança não seja uma camada adicional, mas sim parteintrínseca do sistema desde sua concepção.

#### 3.4.1.1 Autenticação e Autorização

| Camada | Mecanismo | Implementação |
|--------|-----------|---------------|
| App/Web | JWT (JSON Web Token) | Tokens de curta duração (1h) com refresh token |
| Backend | Middleware de validação | Verificação de token em todas as requisições |
| MQTT | Client ID único | Cada ESP32 possui ID único e não pode publicar em tópicos de outras salas |
| API | Rate Limiting | Prevenção de ataques de força bruta |

#### 3.4.1.2 Proteção contra Manipulação

```
┌─────────────────────────────────────────────────────────────────┐
│                    PREVENÇÃO DE BYPASS                           │
└─────────────────────────────────────────────────────────────────┘

ATAQUE: Aluno tenta "simular" presença sem ir à sala
─────────────────────────────────────────────────────────────────
Proteção:
1. O botão físico é o único mecanismo de confirmação
2. ESP32 não aceita comandos via MQTT para simular presença
3. Timer de 10 min garante presença real

ATAQUE: Aluno tenta reservar sala já ocupada
─────────────────────────────────────────────────────────────────
Proteção:
1. Backend valida status da sala em tempo real
2. Conflitos de horário bloqueados no banco de dados
3. Reservas só podem ser feitas em salas com status "livre"

ATAQUE: Acesso não autorizado aos dados
─────────────────────────────────────────────────────────────────
Proteção:
1. HTTPS em todas as comunicações
2. Autenticação via JWT
3. Validação de CORS no backend
4. Sanitização de inputs (prevenção SQL Injection/XSS)

ATAQUE: ESP32 é substituído por dispositivo malicioso
─────────────────────────────────────────────────────────────────
Proteção:
1. Client ID único e fixo no código (não configurável)
2. MAC address do ESP32 pode ser vinculado ao cadastro
3. Tópicos MQTT específicos por sala (não há acesso cruzado)
```

#### 3.4.1.3 Validação de Entrada

```javascript
// Exemplo de validação no Backend
function validarReserva(dados) {
    // Sanitização
    const nome = sanitize(dados.nome); // Remove caracteres especiais
    const salaId = parseInt(dados.salaId); // Garante número
    const horaInicio = new Date(dados.horaInicio);
    const horaFim = new Date(dados.horaFim);

    // Validações
    if (!nome || nome.length < 2) return false;
    if (!salaId || salaId < 1) return false;
    if (horaFim <= horaInicio) return false;
    if (horaInicio < new Date()) return false; // Não permite reservas no passado

    return true;
}
```

### 3.4.2 Tratamento Ético dos Logs (LGPD)

#### 3.4.2.1 Dados Coletados

| Dado | Finalidade | Base Legal |
|------|------------|------------|
| Nome do aluno | Identificação da reserva | Consentimento do titular |
| Horário de acesso | Auditoria e segurança | Legítimo interesse da instituição |
| Status da sala | Funcionamento do sistema | Necessidade técnica |

#### 3.4.2.2 Política de Logs

```
┌─────────────────────────────────────────────────────────────────┐
│                    ESTRUTURA DE LOGS                            │
└─────────────────────────────────────────────────────────────────┘

Logs de Acesso (Backend):
├── timestamp        (data/hora)
├── ip_usuario      (endereço IP)
├── acao            (criar_reserva, cancelar, consultar)
├── id_usuario      (hash anônimo)
├── id_sala         (identificador)
└── resultado       (sucesso/falha)

Logs do ESP32:
├── timestamp       (data/hora)
├── id_dispositivo (identificador único)
├── evento         (conectar, status_mudanca, botao_pressionado)
└── novo_status    (livre/reservada/ocupada)

POLÍTICA DE RETENÇÃO:
- Logs de acesso: 90 dias (LGPD - finalidade de auditoria)
- Após 90 dias: anonimização (hash dos dados pessoais)
- Backup: criptografado, retention de 1 ano
```

#### 3.4.2.3 Princípios LGPD Aplicados

| Princípio | Implementação |
|-----------|---------------|
| **Finalidade** | Dados usados apenas para funcionamento do sistema e auditoria |
| **Necessidade** | Coleta mínima - apenas dados essenciais |
| **Transparência** | Usuário informado sobre coleta via Termo de Uso |
| **Segurança** | Dados criptografados em repouso e em trânsito |
| **Responsabilidade** | Backend com logs de auditoria completos |
| **Eliminarão** | Usuário pode solicitar exclusão de seus dados |

#### 3.4.2.4 Medidas Técnicas de Proteção

```
┌─────────────────────────────────────────────────────────────────┐
│                    MEDIDAS DE PROTEÇÃO                           │
└─────────────────────────────────────────────────────────────────┘

DADOS EM TRÂNSITO:
├── App → Backend: HTTPS/TLS 1.3
├── Backend → MQTT Broker: TLS (produção)
└── ESP32 → MQTT: TCP nativo (ambiente controlado)

DADOS EM REPOUSO:
├── Banco de dados: Criptografia AES-256 (Supabase)
├── Backups: Criptografados
└── Logs: Hash de dados pessoais para anonimização

CONTROLES DE ACESSO:
├── Princípio do menor privilégio
├── Autenticação multifator (opcional para admins)
├── Logs de auditoria em todas as operações
└── Revisão periódica de acessos
```

---

## 3.5 Resumo da Arquitetura Técnica

| Aspecto | Solução Escolhida | Justificativa |
|---------|-------------------|---------------|
| Dispositivo IoT | ESP32 DevKit | Custo-benefício, WiFi integrado, comunidade ativa |
| Protocolo IoT | MQTT | Leve, ideal para IoT, pub/sub nativo |
| Broker MQTT | HiveMQ Cloud | Gratuito, gerenciado, alta disponibilidade |
| Backend | Node.js + Express | JavaScript full-stack, grande ecossistema |
| API | REST/HTTPS | Padrao da industria, stateless, facil integracao |
| Banco de Dados | PostgreSQL (Supabase) | Relacional, gratuito, backup automático |
| Frontend Mobile | Expo + React Native | Multiplataforma, gratuito |
| Frontend Web | React.js | Componentização, mercado de trabalho |
| Hospedagem | Supabase + Render/Netlify | Tier gratuito adequado para projeto acadêmico |
| Custo Total | R$ 0 (hospedagem) + R$ ~35/hardware | Projeto de baixo custo |

---

**Autores:** [Nomes da equipe]  
**Instituição:** [Nome da Faculdade]  
**Data:** [Data de entrega]  
**Versão:** 1.0