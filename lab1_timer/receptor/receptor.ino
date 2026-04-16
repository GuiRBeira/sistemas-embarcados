// ============================================================
// Configurações obrigatórias para a biblioteca TimerInterrupt
// ============================================================
#define TIMER_INTERRUPT_DEBUG 0
#define _TIMERINTERRUPT_LOGLEVEL_ 0

// --- Seleção do Timer Hardware ---
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
Comunicacao comunicador;
ISR_Timer isr_timer;

// ============================================================
// Função Handler do Timer Hardware
// ============================================================
void TimerHandler() {
  isr_timer.run();  // Processa todos os timers lógicos
}

// ============================================================
// Callbacks para os Timers Lógicos
// ============================================================

// Timer de status: pisca o LED a cada 1 segundo
void timerStatusCallback() {
  static bool estadoLed = false;
  estadoLed = !estadoLed;
  digitalWrite(LED_BUILTIN, estadoLed);
}

// Timer de relatório: mostra estatísticas a cada 30 segundos
void timerRelatorioCallback() {
  Serial.println("💡 Sistema ativo - " + String(comunicador.getMensagensValidas()) + 
                 " mensagens processadas até agora");
}

// ============================================================
// Setup
// ============================================================
void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  
  Serial.println("═════════════════════════════════");
  Serial.println("  SISTEMA RECEPTOR - LABORATÓRIO 1");
  Serial.println("  Aguardando dados do transmissor");
  Serial.println("═════════════════════════════════");

  // --- 1. Inicializar o Timer Hardware (Timer1) ---
  ITimer1.init();

  // --- 2. Configurar o Timer Hardware (intervalo base de 50ms) ---
  if (ITimer1.attachInterruptInterval(50, TimerHandler)) {
    Serial.println("✓ Timer Hardware (Timer1) configurado com sucesso!");
  } else {
    Serial.println("✗ Falha ao configurar Timer Hardware!");
  }

  // --- 3. Configurar os Timers Lógicos ---
  // IMPORTANTE: setInterval retorna o timerId (int), NÃO um booleano!
  // Por isso não podemos usar 'if' para verificar sucesso.
  
  // Timer de status: a cada 1 segundo (1000ms)
  isr_timer.setInterval(1000, timerStatusCallback);
  Serial.println("✓ Timer de status configurado (LED a cada 1s)");
  
  // Timer de relatório: a cada 30 segundos (30000ms)
  isr_timer.setInterval(30000, timerRelatorioCallback);
  Serial.println("✓ Timer de relatório configurado (30s)");

  Serial.println();
  Serial.println("Sistema pronto. Aguardando mensagens...");
  Serial.println();
}

// ============================================================
// Loop Principal
// ============================================================
void loop() {
  // Processa as mensagens recebidas pela serial
  comunicador.processarRecepcao();
  
  // Mostra estatísticas
  comunicador.mostrarEstatisticas();
  
  delay(10);
}