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
SubEstadoAjuste subEstadoAjuste = SEL_CAMPO;
unsigned long tempoUltimaAtividade = 0;
const int timeoutSubEstado = 3000; // 3s para voltar a selecionar campo

int campoSelecionado = 0;
const char* campos[] = {"Min Atual", "Seg Atual", "Min Alarme", "Seg Alarme"};

// ===== Funções de Lógica =====

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
  // Zera o tempo conforme solicitado
  minutosAtual = 0;
  segundosAtual = 0;
}

void tratarAcoesBotao(EstadoBotao b) {
  if (b == BOTAO_CONFIRMADO_CURTO) {
    if (estadoAtual == ESTADO_AJUSTE) {
      // AJUSTE --> NORMAL: botão SW curto
      estadoAtual = ESTADO_NORMAL;
      alarmeDisparado = false;
      atualizarDisplay(estadoAtual, subEstadoAjuste, minutosAtual, segundosAtual, minutosAlarme, segundosAlarme, campoSelecionado, campos, alarmeDisparado);
    } else if (estadoAtual == ESTADO_ALARME_ATIVO) {
      // ALARME_DISPARADO --> NORMAL: botão SW (DesligadoManual)
      pararAlarme();
      atualizarDisplay(estadoAtual, subEstadoAjuste, minutosAtual, segundosAtual, minutosAlarme, segundosAlarme, campoSelecionado, campos, alarmeDisparado);
    }
  } else if (b == BOTAO_CONFIRMADO_LONGO) {
    if (estadoAtual == ESTADO_NORMAL) {
      // NORMAL --> AJUSTE: botão SW longo
      estadoAtual = ESTADO_AJUSTE;
      subEstadoAjuste = SEL_CAMPO;
      campoSelecionado = 0;
      atualizarDisplay(estadoAtual, subEstadoAjuste, minutosAtual, segundosAtual, minutosAlarme, segundosAlarme, campoSelecionado, campos, alarmeDisparado);
    } else if (estadoAtual == ESTADO_ALARME_ATIVO) {
      // Também desliga o alarme no longo por conveniência
      pararAlarme();
      atualizarDisplay(estadoAtual, subEstadoAjuste, minutosAtual, segundosAtual, minutosAlarme, segundosAlarme, campoSelecionado, campos, alarmeDisparado);
    }
  }
}

void tratarAcoesJoystick(int dx, int dy) {
  tempoUltimaAtividade = millis();
  
  if (dx != 0) { 
    // SelecionaCampo (Eixo X)
    subEstadoAjuste = SEL_CAMPO;
    campoSelecionado += dx;
    if (campoSelecionado < 0) campoSelecionado = 3;
    if (campoSelecionado > 3) campoSelecionado = 0;
  }
  
  if (dy != 0) { 
    // IncrementaDecrementa (Eixo Y)
    subEstadoAjuste = INC_VALOR;
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

  atualizarDisplay(estadoAtual, subEstadoAjuste, minutosAtual, segundosAtual, 
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
      
      // Verifica alarme somente quando o tempo muda
      if (!alarmeDisparado && minutosAtual == minutosAlarme && segundosAtual == segundosAlarme) {
        dispararAlarme();
      }
      
      atualizarDisplay(estadoAtual, subEstadoAjuste, minutosAtual, segundosAtual, 
                       minutosAlarme, segundosAlarme, campoSelecionado, 
                       campos, alarmeDisparado);
    }
  }

  // 2. Processar Entradas
  // Se o alarme estiver ativo, verificamos a pressão bruta do botão para parada instantânea
  if (estadoAtual == ESTADO_ALARME_ATIVO && !digitalRead(PIN_SW)) {
      pararAlarme();
      atualizarDisplay(estadoAtual, subEstadoAjuste, minutosAtual, segundosAtual, 
                       minutosAlarme, segundosAlarme, campoSelecionado, 
                       campos, alarmeDisparado);
  }

  EstadoBotao b = processarBotao();
  tratarAcoesBotao(b);

  if (estadoAtual == ESTADO_AJUSTE) {
    // Timeout do sub-estado IncrementaDecrementa --> SelecionaCampo
    if (subEstadoAjuste == INC_VALOR && agora - tempoUltimaAtividade >= timeoutSubEstado) {
      subEstadoAjuste = SEL_CAMPO;
      atualizarDisplay(estadoAtual, subEstadoAjuste, minutosAtual, segundosAtual, 
                       minutosAlarme, segundosAlarme, campoSelecionado, 
                       campos, alarmeDisparado);
    }

    int dx, dy;
    if (lerJoystick(dx, dy)) {
      tratarAcoesJoystick(dx, dy);
      atualizarDisplay(estadoAtual, subEstadoAjuste, minutosAtual, segundosAtual, 
                       minutosAlarme, segundosAlarme, campoSelecionado, 
                       campos, alarmeDisparado);
    }
  }

  // 3. Lógica de Estados
  switch (estadoAtual) {
    case ESTADO_NORMAL:
      // A verificação do alarme agora só ocorre no "tick" do relógio (movido para cima)
      break;

    case ESTADO_ALARME_ATIVO:
      if (agora - inicioAlarme >= duracaoAlarme) {
        pararAlarme();
        atualizarDisplay(estadoAtual, subEstadoAjuste, minutosAtual, segundosAtual, 
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