# SmartBib - Salas de Reunião Inteligentes (IoT/ESP32)

## 1. Identificação
* **Projeto:** SmartBib - Sistema Inteligente de Ocupação
* **Equipe:** Pedro Henrique Cordeiro, Heloyse Silva, Ágata Maria, Rafaela Dayana e Guilherme Pereira
* **Turma/Período:** CST em Análise e Desenvolvimento de Sistemas - 4º Período (Noite)
* **UCs Integradas:** IoT, Comportamento do Consumidor, Cloud Computing, Segurança da Informação, Engenharia de Software, Inglês.

## 2. Links de Acesso
* 🔗 **Repositório GitHub:**
* Repositório do backend: https://github.com/henriquepedrohttp/backend-smartbib
* Repositório frontend: https://github.com/Heloyse19/smartbib
* 📄 **Apresentação:** https://www.canva.com/design/DAHL12OVdRo/T5EdJquxx5ayXncmIdCXrQ/edit
* ⚡ **Simulação:** https://wokwi.com/projects/466111485385615361
* 📋 **Quadro Kanban:** https://trello.com/invite/b/65e08e1234c51791fe772df3/ATTI83fd4be7c50d4d2744a32ae58d4fa714B276A610/pi-senac

## 3. Documento de Requisitos Simplificado
* **Problema:** Dificuldade na gestão visual e remota da ocupação das salas de estudo da biblioteca do Senac, gerando ociosidade ou conflito de reservas.
* **Escopo do MVP:** Um dispositivo embarcado com ESP32 posicionado em cada sala, sinalizando fisicamente o status (Livre, Reservada, Ocupada) por meio de LEDs, com controle local via Push Button e sincronização em tempo real com um backend (NestJS) através do protocolo MQTT.
* **Requisitos Funcionais (RF):**
  * O sistema deve alternar o status da sala entre LIVRE, RESERVADA e OCUPADA.
  * O ESP32 deve atualizar os LEDs indicativos de acordo com o estado recebido.
  * O usuário deve poder alterar o status da sala fisicamente via botão físico (com debounce de 50ms).
  * O ESP32 deve reportar o estado atual para o broker MQTT sempre que houver alteração.
* **Requisitos Não-Funcionais (RNF):**
  * O intervalo de amostragem/resposta do botão deve ser inferior a 100ms.
  * Credenciais de rede e tokens MQTT não devem ficar expostos no código-fonte (uso de minimização de dados e arquivos de configuração).

## 4. Mapeamento de UCs e Marcas Formativas

### Integração com UCs
| Conceito | UC Relacionada | Onde está evidenciado no projeto |
| :--- | :--- | :--- |
| Firmware e controle de Hardware (ESP32) | IoT & Artefato | Código fonte (`SmartBib_Sala.ino`) e circuito. Fail-safe garantindo debounce do botão. |
| Jornada do Usuário e Persona | Comportamento do Consumidor | Na documentação da Persona (Anexo X) e nos slides da apresentação. |
| Comunicação MQTT (Pub/Sub) e RESTful | Engenharia de Software & APIs | Estrutura de tópicos MQTT mapeada neste README e uso de broker. |
| Dashboard e Mensageria na Nuvem | Cloud Computing | Comunicação do ESP32 com o Broker HiveMQ na nuvem. |
| Proteção de Credenciais e LGPD | Segurança da Informação | Uso de `config.h.example` isolado. Ausência de dados sensíveis dos alunos no hardware. |
| Abstract / Documentação Técnica | Inglês | Seção de Abstract nos slides e variáveis declaradas no código-fonte. |

### Marcas Formativas Senac
* **Domínio técnico-científico:** Rigor na montagem do circuito com ESP32 e lógica de estados no firmware.
* **Autonomia e Resolução de Problemas:** Tratamento de debouncing do botão via software e reconexão autônoma do WiFi/MQTT em caso de queda.
* **Visão Crítica, Ética e Segurança:** Ausência de hardcoding de chaves de rede para garantir aderência às boas práticas de segurança e LGPD.
* **Comunicação e Colaboração:** Organização ágil da equipe (divisão de tarefas da API, hardware e frontend).
* **Atitude Empreendedora e Inovadora:** MVP viável que resolve uma dor real da biblioteca do Senac.

## 5. Esboços e Diagramas Técnicos

### Mapeamento de Hardware
| Componente | Pino | Função |
| :--- | :--- | :--- |
| ESP32 DevKit | -- | Microcontrolador com WiFi |
| LED Verde | GPIO 2 | Sala LIVRE |
| LED Amarelo | GPIO 21 | Sala RESERVADA (aguardando confirmação) |
| LED Vermelho | GPIO 5 | Sala OCUPADA |
| Push Button | GPIO 4 | Alterna estado de ocupação (`INPUT_PULLUP`) |

*(Adicione aqui a imagem do Diagrama de Arquitetura Lógica e o Esquema Elétrico/Print do Wokwi)*
![Diagrama de Arquitetura](link_da_imagem)
<img width="397" height="462" alt="image" src="https://github.com/user-attachments/assets/f04e36b1-7d2b-4aa9-94b9-fa91a81137db" />

## 6. Dossiê de Evidências (Mídia)
*(Adicione fotos nítidas do seu circuito montado na protoboard e prints do Serial Monitor)*
* 📸 Imagem do circuito físico
* 💻 Print do Serial Monitor enviando o JSON
* 📊 Print do Dashboard em nuvem atualizando o status das salas

## 7. Instruções de Execução e Configuração

**Segurança da Informação:** Não versione o arquivo contendo as credenciais da sua rede. Para executar este projeto:

1. Renomeie o arquivo `config.h.example` para `config.h`.
2. O arquivo `config.h` já está adicionado ao `.gitignore`.
3. Preencha suas credenciais locais no `config.h`:
   ```cpp
   // config.h
   const char* WIFI_SSID = "SEU_WIFI_AQUI";
   const char* WIFI_PASSWORD = "SUA_SENHA_AQUI";
