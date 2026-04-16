// ============================================================
// Configurações obrigatórias para a biblioteca TimerInterrupt
// ============================================================
// Estas definições DEVEM vir antes do #include
#define TIMER_INTERRUPT_DEBUG 0
#define _TIMERINTERRUPT_LOGLEVEL_ 0

// --- Seleção do Timer Hardware ---
// Para Arduino Uno, Nano, Mega, etc. vamos usar o Timer1
// A biblioteca usa estas definições para saber qual timer ativar
#define USE_TIMER_1     true
#define USE_TIMER_2     false
#define USE_TIMER_3     false
#define USE_TIMER_4     false
#define USE_TIMER_5     false

// ============================================================
// Inclusão das Bibliotecas
// ============================================================
// Arquivos principais da biblioteca (seguindo a recomendação do README)
#include "TimerInterrupt.h"   // Deve ser incluído em apenas um arquivo .ino
#include "ISR_Timer.h"        // Para os timers baseados em ISR
#include "Comunicacao.h"      // Sua classe de comunicação

// ============================================================
// Objetos Globais
// ============================================================
Comunicacao comunicador;

// Objeto que gerenciará os timers baseados em ISR
ISR_Timer isr_timer;

// ============================================================
// Função Handler do Timer Hardware
// ============================================================
// Esta função será chamada pelo Timer1 em um intervalo fixo (ex: 50ms)
// Dentro dela, chamamos isr_timer.run() para processar todos os
// outros timers que criaremos.
void TimerHandler() {
  isr_timer.run();  // <--- CRUCIAL: É aqui que seus timers são executados
}

// ============================================================
// Callback para a Tarefa Periódica (envio a cada 2 segundos)
// ============================================================
void enviarDadosPeriodicamente() {
  // Chama o método da sua classe que envia a mensagem
  comunicador.transmissaoPeriodica();
}

// ============================================================
// Setup
// ============================================================
void setup() {
  Serial.begin(115200);
  Serial.println(F("================================="));
  Serial.println(F("SISTEMA TRANSMISSOR - LABORATÓRIO 1"));
  Serial.println(F("Usando TimerInterrupt Library v1.8.0"));
  Serial.println(F("================================="));

  // --- 1. Inicializar o Timer Hardware (Timer1) ---
  // O objeto global ITimer1 é fornecido pela biblioteca
  ITimer1.init();

  // --- 2. Configurar o Timer Hardware para chamar o Handler em intervalos fixos ---
  // Vamos usar um intervalo de 50ms (50000 microssegundos) para o timer base.
  // Este intervalo alto (50ms) é recomendado para placas AVR de 16MHz.
  // O Handler será chamado estritamente a cada 50ms.
  if (ITimer1.attachInterruptInterval(50, TimerHandler)) {
    Serial.println(F("✓ Timer Hardware (Timer1) configurado com sucesso!"));
  } else {
    Serial.println(F("✗ Falha ao configurar Timer Hardware!"));
  }

  // --- 3. Configurar o Timer Lógico (ISR_Timer) para a tarefa desejada ---
  // Agora configuramos um timer que usará o hardware timer como base.
  // Queremos enviar dados a cada 2 segundos (2000ms).
  // setInterval(intervalo_em_ms, funcao_callback);
  isr_timer.setInterval(2000, enviarDadosPeriodicamente);

  Serial.println(F("Timer lógico configurado para enviar dados a cada 2 segundos."));
  Serial.println();
}

// ============================================================
// Loop Principal
// ============================================================
void loop() {
  // NÃO é necessário chamar isr_timer.run() aqui. Ele já é chamado
  // dentro do TimerHandler(), na interrupção.
  
  // Processa entrada do Monitor Serial (mensagens customizadas ou ACKs)
  comunicador.processarEntradaSerial();
  
  // Pequeno delay para dar tempo ao processador
  delay(10);
}