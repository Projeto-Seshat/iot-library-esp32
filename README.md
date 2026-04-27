# SmartBib - Sistema de Gestão de Salas de Estudo (IoT + Web)

[cite_start]Este projeto faz parte da Entrega U1 da Unidade Curricular da matéria de IOT, Ideação e Arquitetura do MVP[cite: 1, 2]. [cite_start]O objetivo é modernizar o acesso às salas de reunião da biblioteca da Faculdade Senac Pernambuco utilizando Internet das Coisas (IoT) e protocolos de comunicação em tempo real[cite: 51, 65, 66].

## 👥 Equipe
* [cite_start]Ágata Maria Ferraz de Oliveira [cite: 55]
* [cite_start]Guilherme Pereira da Silva [cite: 56]
* [cite_start]Heloyse Silva Santos [cite: 57]
* [cite_start]Pedro Henrique Cordeiro Ferreira [cite: 58]
* [cite_start]Rafaela Dayana da Silva [cite: 59]

## 🎯 1. Escopo e Temática
[cite_start]O projeto foca no nicho de **Gestão de Fluxo** e **Experiência do Usuário**[cite: 8, 11]. [cite_start]O SmartBib transforma as cabines de estudo em espaços inteligentes, eliminando a incerteza sobre a ocupação das salas e facilitando a rotatividade justa entre os estudantes[cite: 67, 72].

## 🔍 2. Fase de Imersão e Definição
### [cite_start]Problema e "Dores" [cite: 15]
* [cite_start]**Falta de Transparência:** Alunos perdem tempo indo até a biblioteca para encontrar salas trancadas ou com "reservas fantasmas"[cite: 105, 106].
* [cite_start]**Gestão Manual:** Atualmente, a conferência de ocupação depende de intervenção humana, gerando gargalos[cite: 77, 221].

### [cite_start]Proposta de Valor [cite: 18]
[cite_start]Oferecer visibilidade em tempo real do status de ocupação (Verde/Livre e Vermelho/Ocupada) tanto no local físico quanto em um painel web centralizado, otimizando o uso do Smart Campus[cite: 68, 80, 134].

## 🏗️ 3. Arquitetura e Requisitos Técnicos
### [cite_start]Componentes da Solução [cite: 21]
* [cite_start]**Borda (IoT):** ESP32 integrado a um botão físico para alternância de estado e LEDs de sinalização visual[cite: 23, 36].
* [cite_start]**Comunicação (Redes):** Utilização do protocolo **MQTT** para eventos em tempo real, garantindo baixa latência na atualização do status[cite: 25].
* [cite_start]**Backend e Banco de Dados:** Processamento dos dados recebidos do broker para persistência e alimentação do dashboard[cite: 26, 27].
* [cite_start]**Painel Web:** Interface de monitoramento para visualização rápida da taxa de ocupação das salas[cite: 94, 96].

### [cite_start]Requisitos de Sistema [cite: 28]
* [cite_start]**Funcionais:** Registro de ocupação via botão, alteração de cores dos LEDs e exibição em tempo real no painel[cite: 30].
* [cite_start]**Não Funcionais:** Baixa latência na comunicação via MQTT e escalabilidade para múltiplas salas[cite: 31].


## 🛠️ 4. Protótipo Funcional (Prova de Conceito)
[cite_start]O protótipo inicial demonstra a integração completa entre o hardware e o sistema de monitoramento[cite: 34, 41]:
1. **Ação:** O usuário pressiona o botão físico na sala.
2. **Hardware:** O ESP32 inverte o estado da sala e acende o LED correspondente (Verde -> Livre / Vermelho -> Ocupada).
3. **Rede:** Uma mensagem JSON é publicada via MQTT para o broker.
4. **Dashboard:** O painel web atualiza instantaneamente o ícone da sala correspondente.

## 📁 Estrutura do Repositório
* `/SmartBib_Sala`: Código-fonte (.ino) para o ESP32.
* `/Docs`: Documentação analítica e diagramas de arquitetura.
* `/Web`: Frontend do painel de monitoramento.

---
[cite_start]*Faculdade Senac Pernambuco - Análise e Desenvolvimento de Sistemas (ADS)* [cite: 51, 102]