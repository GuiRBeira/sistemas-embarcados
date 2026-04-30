#include <Arduino.h>
#line 1 "/home/gui/Documents/Projects/sistemas-embarcados/lab2_state_machine/stopwatch/stopwatch.ino"
// ============================================================
// Laboratório 2 - Alarme com Máquina de Estados (Modularizado)
// ============================================================

#include <Wire.h>
#include "pins.h"
#include "types.h"
#include "display_utils.h"
#include "input_utils.h"

// ===== Variáveis Globais de Tempo =====
unsigned long ultimoTick = 0;
const unsigned long tickInterval = 1000;
int minutosAtual = 0, segundosAtual = 0;
int minutosAlarme = 0, segundosAlarme = 5;

// ===== Variáveis Globais de Estado =====
EstadoPrincipal estadoAtual = ESTADO_NORMAL;
bool alarmeDisparado = false;
unsigned long inicioAlarme = 0;
const int duracaoAlarme = 5000;

// Configuração do modo ajuste
int campoSelecionado = 0;
const char* campos[] = {"Min Atual", "Seg Atual", "Min Alarme", "Seg Alarme"};

// ===== Funções de Lógica =====

#line 29 "/home/gui/Documents/Projects/sistemas-embarcados/lab2_state_machine/stopwatch/stopwatch.ino"
void atualizarRelogio();
#line 38 "/home/gui/Documents/Projects/sistemas-embarcados/lab2_state_machine/stopwatch/stopwatch.ino"
void dispararAlarme();
#line 45 "/home/gui/Documents/Projects/sistemas-embarcados/lab2_state_machine/stopwatch/stopwatch.ino"
void pararAlarme();
#line 51 "/home/gui/Documents/Projects/sistemas-embarcados/lab2_state_machine/stopwatch/stopwatch.ino"
void tratarAcoesBotao(EstadoBotao b);
#line 76 "/home/gui/Documents/Projects/sistemas-embarcados/lab2_state_machine/stopwatch/stopwatch.ino"
void tratarAcoesJoystick(int dx, int dy);
#line 94 "/home/gui/Documents/Projects/sistemas-embarcados/lab2_state_machine/stopwatch/stopwatch.ino"
void setup();
#line 112 "/home/gui/Documents/Projects/sistemas-embarcados/lab2_state_machine/stopwatch/stopwatch.ino"
void loop();
#line 29 "/home/gui/Documents/Projects/sistemas-embarcados/lab2_state_machine/stopwatch/stopwatch.ino"
void atualizarRelogio() {
  segundosAtual++;
  if (segundosAtual >= 60) {
    segundosAtual = 0;
    minutosAtual++;
    if (minutosAtual >= 60) minutosAtual = 0;
  }
}

void dispararAlarme() {
  estadoAtual = ESTADO_ALARME_ATIVO;
  alarmeDisparado = true;
  inicioAlarme = millis();
  tone(PIN_BUZZER, 2000);
}

void pararAlarme() {
  noTone(PIN_BUZZER);
  estadoAtual = ESTADO_NORMAL;
  alarmeDisparado = false;
}

void tratarAcoesBotao(EstadoBotao b) {
  if (b == BOTAO_CONFIRMADO_CURTO) {
    if (estadoAtual == ESTADO_AJUSTE) {
      estadoAtual = ESTADO_NORMAL;
      alarmeDisparado = false;
      atualizarDisplay(estadoAtual, minutosAtual, segundosAtual, minutosAlarme, segundosAlarme, campoSelecionado, campos, alarmeDisparado);
    } else if (estadoAtual == ESTADO_ALARME_ATIVO) {
      pararAlarme();
      atualizarDisplay(estadoAtual, minutosAtual, segundosAtual, minutosAlarme, segundosAlarme, campoSelecionado, campos, alarmeDisparado);
    }
  } else if (b == BOTAO_CONFIRMADO_LONGO) {
    if (estadoAtual == ESTADO_NORMAL) {
      estadoAtual = ESTADO_AJUSTE;
      campoSelecionado = 0;
      atualizarDisplay(estadoAtual, minutosAtual, segundosAtual, minutosAlarme, segundosAlarme, campoSelecionado, campos, alarmeDisparado);
    } else if (estadoAtual == ESTADO_AJUSTE) {
      estadoAtual = ESTADO_NORMAL;
      atualizarDisplay(estadoAtual, minutosAtual, segundosAtual, minutosAlarme, segundosAlarme, campoSelecionado, campos, alarmeDisparado);
    } else if (estadoAtual == ESTADO_ALARME_ATIVO) {
      pararAlarme();
      atualizarDisplay(estadoAtual, minutosAtual, segundosAtual, minutosAlarme, segundosAlarme, campoSelecionado, campos, alarmeDisparado);
    }
  }
}

void tratarAcoesJoystick(int dx, int dy) {
  if (dx != 0) { // Mudar campo
    campoSelecionado += dx;
    if (campoSelecionado < 0) campoSelecionado = 3;
    if (campoSelecionado > 3) campoSelecionado = 0;
  }
  if (dy != 0) { // Mudar valor
    int delta = dy;
    switch (campoSelecionado) {
      case 0: minutosAtual = (minutosAtual + delta + 60) % 60; break;
      case 1: segundosAtual = (segundosAtual + delta + 60) % 60; break;
      case 2: minutosAlarme = (minutosAlarme + delta + 60) % 60; break;
      case 3: segundosAlarme = (segundosAlarme + delta + 60) % 60; break;
    }
  }
}

// ===== SETUP =====
void setup() {
  Serial.begin(115200);
  
  if (!initDisplay()) {
    Serial.println("Erro OLED");
    while(1);
  }
  
  initInputs();
  pinMode(PIN_BUZZER, OUTPUT);
  noTone(PIN_BUZZER);

  atualizarDisplay(estadoAtual, minutosAtual, segundosAtual, 
                   minutosAlarme, segundosAlarme, campoSelecionado, 
                   campos, alarmeDisparado);
}

// ===== LOOP PRINCIPAL =====
void loop() {
  unsigned long agora = millis();

  // 1. Tick do Relógio
  if (agora - ultimoTick >= tickInterval) {
    ultimoTick = agora;
    if (estadoAtual == ESTADO_NORMAL) {
      atualizarRelogio();
      atualizarDisplay(estadoAtual, minutosAtual, segundosAtual, 
                       minutosAlarme, segundosAlarme, campoSelecionado, 
                       campos, alarmeDisparado);
    }
  }

  // 2. Processar Entradas
  EstadoBotao b = processarBotao();
  tratarAcoesBotao(b);

  if (estadoAtual == ESTADO_AJUSTE) {
    int dx, dy;
    if (lerJoystick(dx, dy)) {
      tratarAcoesJoystick(dx, dy);
      atualizarDisplay(estadoAtual, minutosAtual, segundosAtual, 
                       minutosAlarme, segundosAlarme, campoSelecionado, 
                       campos, alarmeDisparado);
    }
  }

  // 3. Lógica de Estados
  switch (estadoAtual) {
    case ESTADO_NORMAL:
      if (!alarmeDisparado && minutosAtual == minutosAlarme && segundosAtual == segundosAlarme) {
        dispararAlarme();
        atualizarDisplay(estadoAtual, minutosAtual, segundosAtual, 
                         minutosAlarme, segundosAlarme, campoSelecionado, 
                         campos, alarmeDisparado);
      }
      break;

    case ESTADO_ALARME_ATIVO:
      if (agora - inicioAlarme >= duracaoAlarme) {
        pararAlarme();
        atualizarDisplay(estadoAtual, minutosAtual, segundosAtual, 
                         minutosAlarme, segundosAlarme, campoSelecionado, 
                         campos, alarmeDisparado);
      }
      break;
      
    case ESTADO_AJUSTE:
      // Já tratado nas entradas
      break;
  }

  delay(10);
}
