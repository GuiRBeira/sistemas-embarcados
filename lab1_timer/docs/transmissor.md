# Documentação do Transmissor (Lab 1)

O transmissor é responsável por coletar dados (simulados ou de sensores) e enviá-los periodicamente para o receptor utilizando interrupções de hardware para garantir a precisão temporal.

## Arquivos
- `transmissor.ino`: Sketch principal contendo a configuração de hardware e loops.
- `Comunicacao.h`: Classe que gerencia a formatação e o envio das mensagens.

## Funcionamento Técnico

### 1. Sistema de Temporização (Timers)
O código utiliza a biblioteca `TimerInterrupt` para evitar o uso de `delay()`, que é bloqueante.
- **Timer de Hardware (Timer1):** Configurado para gerar uma interrupção a cada **50ms**.
- **Handler (`TimerHandler`):** Função chamada pela interrupção que executa o `isr_timer.run()`.
- **Timer Lógico (`ISR_Timer`):** Criado sobre o timer de hardware, está configurado para disparar a função `enviarDadosPeriodicamente()` a cada **2000ms (2 segundos)**.

### 2. Protocolo de Mensagem
As mensagens são enviadas via Serial no formato:
`TIMESTAMP|TIPO|DADO|ID`
Exemplo: `14500|SENSOR|512|7`

### 3. Funcionalidades da Classe `Comunicacao`
- `enviarMensagem(tipo, conteudo)`: Monta a string no protocolo e incrementa o contador de IDs.
- `transmissaoPeriodica()`: Lê o valor do pino `A0` e envia como tipo `SENSOR`.
- `processarEntradaSerial()`: 
    - Lê o que o usuário digita no Monitor Serial.
    - Se começar com `ACK`, valida a resposta do receptor.
    - Se começar com `MSG:`, envia como tipo `CUSTOM`.
    - Caso contrário, envia como tipo `TEXTO`.

## Configuração de Hardware Recomendada
- Arduinos da família AVR (Uno, Nano, Mega).
- Baud rate: **115200**.
- Pino `A0` conectado a um sensor ou potenciômetro (opcional).
