# Documentação do Projeto - Laboratório 1 (Sistemas Embarcados)

Este projeto consiste em um sistema de comunicação serial entre dois dispositivos (Transmissor e Receptor) utilizando interrupções de hardware para temporização precisa.

## Estrutura da Documentação

- [**Transmissor**](transmissor.md): Detalha como os dados são coletados e enviados a cada 2 segundos via Timer/Interrupts.
- [**Receptor**](receptor.md): Explica o processo de recepção assíncrona, validação de dados e geração de ACKs (confirmações).
- [**Full-Duplex**](full_duplex.md): Implementação unificada onde o nó transmite e recebe simultaneamente (peer-to-peer).

---

## Como Executar

1.  **Conexão Física:**
    - Conecte o **TX** do Transmissor ao **RX** do Receptor.
    - Conecte o **RX** do Transmissor ao **TX** do Receptor.
    - Compartilhe o **GND** entre ambos os microcontroladores.
2.  **Upload:**
    - Para modo Mestre/Escravo: Use `lab1_timer/transmissor/` em uma placa e `lab1_timer/receptor/` na outra.
    - Para modo Peer-to-Peer: Use `lab1_timer/full_duplex/` em ambas as placas.
3.  **Monitoramento:**
    - Abra o Monitor Serial a **115200 baud**.

## Dependências
- Biblioteca [**TimerInterrupt**](https://github.com/khoih-prog/TimerInterrupt) (v1.8.0 ou superior).
