// ============================================================
// Configurações obrigatórias para a biblioteca TimerInterrupt
// ============================================================
#define TIMER_INTERRUPT_DEBUG 0
#define _TIMERINTERRUPT_LOGLEVEL_ 0

// --- Seleção do Timer Hardware ---
// Usamos o Timer1 como base para as interrupções
#define USE_TIMER_1     true
#define USE_TIMER_2     false
#define USE_TIMER_3     false
#define USE_TIMER_4     false
#define USE_TIMER_5     false

// ============================================================
// Inclusão das Bibliotecas
// ============================================================
#include "TimerInterrupt.h"
#include "ISR_Timer.h"
#include "Comunicacao.h"

// ============================================================
// Objetos Globais
// ============================================================
Comunicacao comunicador; // Objeto que gerencia TX/RX simultâneo
ISR_Timer isr_timer;    // Gerenciador de timers lógicos

// ============================================================
// Funções de Interrupção e Callbacks
// ============================================================

// Handler chamado pelo timer de hardware a cada 50ms
void TimerHandler() {
  isr_timer.run();
}

// Tarefa 1: Transmissão periódica de dados (Sensor)
void tarefaTransmissao() {
  comunicador.transmissaoPeriodica();
}

// Tarefa 2: Feedback visual (Blink do LED nativo)
void tarefaBlink() {
  static bool estadoLed = false;
  estadoLed = !estadoLed;
  digitalWrite(LED_BUILTIN, estadoLed);
}

// ============================================================
// Setup do Sistema
// ============================================================
void setup() {
  // Inicialização da Serial (115200 baud é recomendado para comunicações rápidas)
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  
  Serial.println(F("═══════════════════════════════════════════"));
  Serial.println(F("    LAB 1: NÓ EM OPERAÇÃO FULL-DUPLEX     "));
  Serial.println(F("    Transmissão e Recepção Simultâneas    "));
  Serial.println(F("═══════════════════════════════════════════"));

  // 1. Inicializar o Timer de Hardware (Timer1)
  ITimer1.init();

  // 2. Vincular o Handler ao Timer com intervalo base de 50ms
  // Este intervalo é o tempo de tick para o isr_timer
  if (ITimer1.attachInterruptInterval(50, TimerHandler)) {
    Serial.println(F("✓ Timer Hardware (50ms tick) configurado."));
  } else {
    Serial.println(F("✗ Erro ao configurar Timer Hardware!"));
  }

  // 3. Configurar Tarefas Periódicas (Timers Lógicos)
  
  // Envia dados do "sensor" a cada 2 segundos
  isr_timer.setInterval(2000, tarefaTransmissao);
  Serial.println(F("✓ Tarefa TX configurada (intervalo: 2s)"));
  
  // Pisca o LED de status a cada 1 segundo
  isr_timer.setInterval(1000, tarefaBlink);
  Serial.println(F("✓ Tarefa Blink configurada (intervalo: 1s)"));

  Serial.println(F("\nSistema pronto!"));
  Serial.println(F("Digite 'MSG:sua_mensagem' para enviar dados customizados."));
  Serial.println(F("═══════════════════════════════════════════\n"));
}

// ============================================================
// Loop Principal
// ============================================================
void loop() {
  // A recepção e o processamento de entrada (user/outra placa)
  // devem acontecer o mais rápido possível no loop.
  comunicador.processarComunicacao();
  
  // Mostra as estatísticas de tráfego a cada período definido na classe
  comunicador.mostrarEstatisticas();
  
  // Pequeno delay para estabilidade, mas o processamento de interrupção
  // continua ocorrendo em background.
  delay(10);
}
