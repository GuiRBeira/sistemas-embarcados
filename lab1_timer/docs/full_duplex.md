# Documentação do Nó Full-Duplex (Lab 1)

O nó Full-Duplex combina as capacidades de transmissão e recepção em um único código, permitindo que dois dispositivos atuem como pares idênticos (peers) em uma comunicação bidirecional simultânea.

## Arquivos
- `full_duplex.ino`: Sketch unificado com o loop de eventos e configuração de múltiplos timers.
- `Comunicacao.h`: Classe estendida que lida com tráfego bidirecional, buffers de recepção, envio de ACKs e estatísticas.

## Funcionamento Técnico

### 1. Multitarefa com Timers
O sistema utiliza interrupções de hardware para gerenciar tarefas paralelas de forma não-bloqueante:
- **Timer de Hardware (Timer1):** Configurado com um "tick" base de **50ms**.
- **Tarefa de Transmissão:** Dispara `transmissaoPeriodica()` a cada **2 segundos** para envio automático de dados de sensores.
- **Tarefa de Status (Blink):** Alterna o estado do LED nativo (`LED_BUILTIN`) a cada **1 segundo** para indicar que a CPU não está travada.

### 2. Fluxo de Comunicação Unificado
O método `processarComunicacao()` é executado continuamente no `loop()`, diferenciando três tipos de tráfego na porta Serial:
1.  **Mensagens do Protocolo (`timestamp|tipo|dado|id`):** Dados recebidos da outra placa. O nó extrai os campos, exibe no monitor e responde automaticamente com um **ACK**.
2.  **Confirmações (`ACK|id|tipo|timestamp`):** Quando este nó recebe um ACK, ele o identifica como uma confirmação de sucesso de um envio anterior.
3.  **Comandos do Usuário:** 
    - Entradas começando com `MSG:` enviam o conteúdo como tipo `CUSTOM`.
    - Entradas de texto simples são enviadas como tipo `TEXTO`.

### 3. Painel de Estatísticas
A cada **20 segundos**, o sistema imprime um relatório consolidado no monitor serial:
- **TX:** Contador de mensagens enviadas.
- **RX:** Contador de mensagens recebidas (Válidas / Total).
- **Taxa de Sucesso:** Percentual de integridade das mensagens recebidas.

## Conexão e Teste
Para testar o modo Full-Duplex entre dois Arduinos:
1.  **Fiação:** Conecte `TX` de uma placa no `RX` da outra e vice-versa. Ligue os `GND` de ambas.
2.  **Monitor Serial:** Configure para **115200 baud**.
3.  **Observação:** Ambas as placas estarão enviando e recebendo dados ao mesmo tempo. Você verá os ACKs cruzados no monitor.
