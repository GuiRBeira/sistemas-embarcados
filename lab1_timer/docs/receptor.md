# Documentação do Receptor (Lab 1)

O receptor é responsável por escutar as mensagens enviadas pelo transmissor, validar o formato, extrair os dados e enviar uma resposta de confirmação (ACK).

## Arquivos
- `receptor.ino`: Sketch principal que gerencia os callbacks e o processamento de loop.
- `Comunicacao.h`: Classe responsável pelo buffer de recepção, parsing e estatísticas.

## Funcionamento Técnico

### 1. Recepção Não-Bloqueante
O receptor não usa `Serial.readString()`, que pode travar a execução. Em vez disso, ele utiliza um buffer:
- A função `processarRecepcao()` lê cada caractere individualmente.
- Quando encontra um caractere de nova linha (`\n`), a mensagem completa é enviada para `processarMensagem()`.

### 2. Parsing e Validação
A classe `Comunicacao` verifica se a mensagem possui exatamente 3 separadores `|`.
- Se válida, os campos `TIMESTAMP`, `TIPO`, `DADO` e `ID` são extraídos.
- Uma confirmação é enviada imediatamente no formato: `ACK|ID|TIPO|TIMESTAMP_ATUAL`.

### 3. Temporização e Feedback
A biblioteca `TimerInterrupt` é utilizada para tarefas de monitoramento:
- **Timer de Status (1s):** Pisca o LED interno (`LED_BUILTIN`) para indicar que o sistema está operacional.
- **Timer de Relatório (30s):** Imprime no log o número total de mensagens processadas.
- **Estatísticas Automáticas:** A cada 10 segundos, o sistema imprime uma tabela com a taxa de sucesso da comunicação.

## Estatísticas Calculadas
- **Mensagens Recebidas:** Total de tentativas de recebimento.
- **Mensagens Válidas:** Quantidade que passou no teste do protocolo.
- **Taxa de Sucesso:** Percentual de integridade dos dados recebidos.

## Configuração de Hardware Recomendada
- Arduinos da família AVR (Uno, Nano, Mega).
- Baud rate: **115200**.
- LED interno funcional.
