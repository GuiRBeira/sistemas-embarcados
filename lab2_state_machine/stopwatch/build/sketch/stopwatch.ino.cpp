#include <Arduino.h>
#line 1 "/home/gui/Documents/Projects/sistemas-embarcados/lab2_state_machine/stopwatch/stopwatch.ino"
// ============================================================
// Laboratório 2 - Alarme com Máquina de Estados
// Disciplina: Sistemas Embarcados
// 
// Descrição: Cronômetro (minutos:segundos) com ajuste via joystick
// e alarme com buzzer. Utiliza máquina de estados no loop principal.
// ============================================================

#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Pinos do joystick
#define PIN_VRX A0
#define PIN_VRY A1
#define PIN_SW  2

// Buzzer
#define PIN_BUZZER 9

// ===== Variáveis de estado =====
typedef enum {
  ESTADO_NORMAL,
  ESTADO_AJUSTE,
  ESTADO_ALARME_ATIVO
} EstadoPrincipal;

EstadoPrincipal estadoAtual = ESTADO_NORMAL;

// ===== Variáveis de tempo =====
unsigned long ultimoTick = 0;
unsigned long tickInterval = 1000;  // 1 segundo

// Horário atual (minutos e segundos)
int minutosAtual = 0;
int segundosAtual = 0;

// Horário do alarme
int minutosAlarme = 0;
int segundosAlarme = 0;

// Flag para evitar disparo repetido
bool alarmeDisparado = false;

// ===== Variáveis do modo ajuste =====
// Campos: 0 = min atual, 1 = seg atual, 2 = min alarme, 3 = seg alarme
int campoSelecionado = 0;
const char* campos[] = {"Min Atual", "Seg Atual", "Min Alarme", "Seg Alarme"};

// ===== Variáveis do botão com debounce via máquina de estados =====
typedef enum {
  BOTAO_ESPERA,
  BOTAO_PRECIONADO,
  BOTAO_CONFIRMADO_CURTO,
  BOTAO_CONFIRMADO_LONGO
} EstadoBotao;

EstadoBotao estadoBotao = BOTAO_ESPERA;
unsigned long tempoPressionado = 0;
int tempoCurtoMax = 700;      // ms para considerar curto
int tempoLongoMin = 800;      // ms para considerar longo

// ===== Variáveis do alarme ativo =====
unsigned long inicioAlarme = 0;
const int duracaoAlarme = 5000;  // 5 segundos
bool alarmeInterrompido = false;

// ===== Joystick =====
int vrx = 0, vry = 0;
int ultimoValorX = 512, ultimoValorY = 512; // para detecção de movimento
const int deadzone = 100;
bool movimentoX = false, movimentoY = false;
unsigned long tempoUltimoMovimento = 0;
unsigned int atrasoMovimento = 250; // ms para evitar leitura muito rápida

// ===== Funções auxiliares =====
#line 82 "/home/gui/Documents/Projects/sistemas-embarcados/lab2_state_machine/stopwatch/stopwatch.ino"
void atualizarDisplay();
#line 117 "/home/gui/Documents/Projects/sistemas-embarcados/lab2_state_machine/stopwatch/stopwatch.ino"
void atualizarTempo();
#line 129 "/home/gui/Documents/Projects/sistemas-embarcados/lab2_state_machine/stopwatch/stopwatch.ino"
void verificarAlarme();
#line 144 "/home/gui/Documents/Projects/sistemas-embarcados/lab2_state_machine/stopwatch/stopwatch.ino"
void tratarBotao();
#line 206 "/home/gui/Documents/Projects/sistemas-embarcados/lab2_state_machine/stopwatch/stopwatch.ino"
void tratarJoystick();
#line 262 "/home/gui/Documents/Projects/sistemas-embarcados/lab2_state_machine/stopwatch/stopwatch.ino"
void tratarAlarmeAtivo();
#line 279 "/home/gui/Documents/Projects/sistemas-embarcados/lab2_state_machine/stopwatch/stopwatch.ino"
void setup();
#line 305 "/home/gui/Documents/Projects/sistemas-embarcados/lab2_state_machine/stopwatch/stopwatch.ino"
void loop();
#line 82 "/home/gui/Documents/Projects/sistemas-embarcados/lab2_state_machine/stopwatch/stopwatch.ino"
void atualizarDisplay() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  switch (estadoAtual) {
    case ESTADO_NORMAL:
      display.println("=== RELOGIO ===");
      display.print("Hora: %02d:%02d\n", minutosAtual, segundosAtual);
      display.print("Alarme: %02d:%02d\n", minutosAlarme, segundosAlarme);
      if (alarmeDisparado) display.println("ALARME!!!");
      break;

    case ESTADO_AJUSTE:
      display.println("=== AJUSTE ===");
      display.print("Campo: %s\n", campos[campoSelecionado]);
      display.print("Min: %02d  Seg: %02d\n", minutosAtual, segundosAtual);
      display.print("Alarme: %02d:%02d\n", minutosAlarme, segundosAlarme);
      display.println("Eixo X muda campo");
      display.println("Eixo Y + / -");
      display.println("SW curto confirma");
      display.println("SW longo cancela");
      break;

    case ESTADO_ALARME_ATIVO:
      display.println("*** ALARME ***");
      display.print("Hora: %02d:%02d\n", minutosAtual, segundosAtual);
      display.println("Pressione SW p/ parar");
      break;
  }
  display.display();
}

// Atualiza o tempo atual (incrementa a cada segundo)
void atualizarTempo() {
  segundosAtual++;
  if (segundosAtual >= 60) {
    segundosAtual = 0;
    minutosAtual++;
    if (minutosAtual >= 60) {
      minutosAtual = 0;
    }
  }
}

// Verifica se o horário atual atingiu o alarme
void verificarAlarme() {
  if (!alarmeDisparado && 
      minutosAtual == minutosAlarme && 
      segundosAtual == segundosAlarme) {
    // Dispara alarme
    estadoAtual = ESTADO_ALARME_ATIVO;
    alarmeDisparado = true;
    inicioAlarme = millis();
    alarmeInterrompido = false;
    tone(PIN_BUZZER, 2000);  // 2kHz
    atualizarDisplay();
  }
}

// Trata o botão com máquina de estados (deve ser chamada frequentemente)
void tratarBotao() {
  bool pressionado = !digitalRead(PIN_SW);  // HIGH quando pressionado (pull-up)
  unsigned long agora = millis();

  switch (estadoBotao) {
    case BOTAO_ESPERA:
      if (pressionado) {
        estadoBotao = BOTAO_PRECIONADO;
        tempoPressionado = agora;
      }
      break;

    case BOTAO_PRECIONADO:
      if (!pressionado) {
        // Soltou: verifica duração
        if (agora - tempoPressionado <= tempoCurtoMax) {
          estadoBotao = BOTAO_CONFIRMADO_CURTO;
        } else if (agora - tempoPressionado >= tempoLongoMin) {
          estadoBotao = BOTAO_CONFIRMADO_LONGO;
        } else {
          estadoBotao = BOTAO_ESPERA;
        }
      } else if (agora - tempoPressionado >= tempoLongoMin) {
        // Ainda pressionado e já passou do limiar de longo
        estadoBotao = BOTAO_CONFIRMADO_LONGO;
      }
      break;

    case BOTAO_CONFIRMADO_CURTO:
      // Ação de curto
      if (estadoAtual == ESTADO_AJUSTE) {
        // Confirma ajuste e sai
        estadoAtual = ESTADO_NORMAL;
        alarmeDisparado = false;  // reset disparo ao mudar horário
        atualizarDisplay();
      } else if (estadoAtual == ESTADO_ALARME_ATIVO) {
        // Interrompe alarme imediatamente
        noTone(PIN_BUZZER);
        estadoAtual = ESTADO_NORMAL;
        alarmeDisparado = false;
        atualizarDisplay();
      }
      estadoBotao = BOTAO_ESPERA;
      break;

    case BOTAO_CONFIRMADO_LONGO:
      // Ação de longo
      if (estadoAtual == ESTADO_NORMAL) {
        estadoAtual = ESTADO_AJUSTE;
        campoSelecionado = 0;
        atualizarDisplay();
      } else if (estadoAtual == ESTADO_AJUSTE) {
        // Cancela ajuste (sai sem salvar)
        estadoAtual = ESTADO_NORMAL;
        atualizarDisplay();
      }
      estadoBotao = BOTAO_ESPERA;
      break;
  }
}

// Lê joystick e aplica ajustes no modo AJUSTE
void tratarJoystick() {
  vrx = analogRead(PIN_VRX);
  vry = analogRead(PIN_VRY);
  unsigned long agora = millis();

  // Detecta movimento em X (seleção de campo)
  if (abs(vrx - ultimoValorX) > deadzone && agora - tempoUltimoMovimento > atrasoMovimento) {
    if (vrx < (512 - deadzone)) {
      // Move para esquerda? Vamos definir: X negativo -> campo anterior
      campoSelecionado--;
      if (campoSelecionado < 0) campoSelecionado = 3;
    } else if (vrx > (512 + deadzone)) {
      // X positivo -> próximo campo
      campoSelecionado++;
      if (campoSelecionado > 3) campoSelecionado = 0;
    }
    ultimoValorX = vrx;
    tempoUltimoMovimento = agora;
    atualizarDisplay();
  }

  // Detecta movimento em Y (ajuste do valor)
  if (abs(vry - ultimoValorY) > deadzone && agora - tempoUltimoMovimento > atrasoMovimento) {
    int delta = (vry < (512 - deadzone)) ? -1 : (vry > (512 + deadzone) ? 1 : 0);
    if (delta != 0) {
      // Aplica mudança conforme campo selecionado
      switch (campoSelecionado) {
        case 0: // min atual
          minutosAtual += delta;
          if (minutosAtual < 0) minutosAtual = 59;
          if (minutosAtual > 59) minutosAtual = 0;
          break;
        case 1: // seg atual
          segundosAtual += delta;
          if (segundosAtual < 0) segundosAtual = 59;
          if (segundosAtual > 59) segundosAtual = 0;
          break;
        case 2: // min alarme
          minutosAlarme += delta;
          if (minutosAlarme < 0) minutosAlarme = 59;
          if (minutosAlarme > 59) minutosAlarme = 0;
          break;
        case 3: // seg alarme
          segundosAlarme += delta;
          if (segundosAlarme < 0) segundosAlarme = 59;
          if (segundosAlarme > 59) segundosAlarme = 0;
          break;
      }
      atualizarDisplay();
    }
    ultimoValorY = vry;
    tempoUltimoMovimento = agora;
  }
}

// Máquina de estados do alarme ativo
void tratarAlarmeAtivo() {
  unsigned long agora = millis();
  if (alarmeInterrompido) {
    // já foi interrompido por botão (mas o botão já mudou estado)
    return;
  }
  // Verifica se tempo esgotou
  if (agora - inicioAlarme >= duracaoAlarme) {
    noTone(PIN_BUZZER);
    estadoAtual = ESTADO_NORMAL;
    alarmeDisparado = false;
    atualizarDisplay();
  }
  // O botão curto já trata interrupção (veja tratarBotao)
}

// ===== SETUP =====
void setup() {
  Serial.begin(115200);
  
  // Inicializa display
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("Falha no display OLED");
    for(;;);
  }
  display.clearDisplay();
  display.display();

  // Configura pinos
  pinMode(PIN_SW, INPUT_PULLUP);
  pinMode(PIN_BUZZER, OUTPUT);
  noTone(PIN_BUZZER);

  // Valores iniciais
  minutosAtual = 0;
  segundosAtual = 0;
  minutosAlarme = 0;
  segundosAlarme = 5;  // só para testar rapidamente, pode mudar

  atualizarDisplay();
}

// ===== LOOP PRINCIPAL - MÁQUINA DE ESTADOS =====
void loop() {
  unsigned long agora = millis();

  // Atualiza o tempo a cada segundo (independente de estado)
  if (agora - ultimoTick >= tickInterval) {
    ultimoTick = agora;
    if (estadoAtual == ESTADO_NORMAL) {
      atualizarTempo();
      atualizarDisplay();
    }
  }

  // Lê botão e joystick (sempre)
  tratarBotao();
  if (estadoAtual == ESTADO_AJUSTE) {
    tratarJoystick();
  }

  // Máquina de estados principal
  switch (estadoAtual) {
    case ESTADO_NORMAL:
      // Verifica se deve disparar alarme
      if (!alarmeDisparado && 
          minutosAtual == minutosAlarme && 
          segundosAtual == segundosAlarme) {
        estadoAtual = ESTADO_ALARME_ATIVO;
        alarmeDisparado = true;
        inicioAlarme = millis();
        tone(PIN_BUZZER, 2000);
        atualizarDisplay();
      }
      break;

    case ESTADO_AJUSTE:
      // Já tratado via tratarJoystick e tratarBotao
      break;

    case ESTADO_ALARME_ATIVO:
      tratarAlarmeAtivo();
      break;
  }

  // Pequeno delay para evitar leitura excessiva (não interfere nos timings)
  delay(20);
}
