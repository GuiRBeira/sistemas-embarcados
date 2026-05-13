#include "input_utils.h"

// Variáveis internas para debounce e joystick
static unsigned long tempoPressionado = 0;
static const int LIMIAR_LONGO = 1000; 
static EstadoBotao estadoBotaoInterno = BOTAO_ESPERA;

static int ultimoValorX = 512;
static int ultimoValorY = 512;
static const int deadzone = 100;
static unsigned long tempoUltimoMovimento = 0;
static const unsigned int atrasoMovimento = 250;

void initInputs() {
  pinMode(PIN_SW, INPUT_PULLUP);
}

EstadoBotao processarBotao() {
  bool pressionado = !digitalRead(PIN_SW);
  unsigned long agora = millis();

  switch (estadoBotaoInterno) {
    case BOTAO_ESPERA:
      if (pressionado) {
        estadoBotaoInterno = BOTAO_PRECIONADO;
        tempoPressionado = agora;
      }
      break;

    case BOTAO_PRECIONADO:
      if (!pressionado) {
        // Soltou rápido: clique curto
        if (agora - tempoPressionado >= 50) {
          estadoBotaoInterno = BOTAO_CONFIRMADO_CURTO; // Vai para o estado de confirmação
          return BOTAO_CONFIRMADO_CURTO;
        } else {
          estadoBotaoInterno = BOTAO_ESPERA;
        }
      } else {
        // Ainda segurando: clique longo
        if (agora - tempoPressionado >= LIMIAR_LONGO) {
          estadoBotaoInterno = BOTAO_CONFIRMADO_LONGO; // Vai para o estado de confirmação
          return BOTAO_CONFIRMADO_LONGO;
        }
      }
      break;

    case BOTAO_CONFIRMADO_CURTO:
    case BOTAO_CONFIRMADO_LONGO:
      // ESTADO DE TRAVA: Só volta a aceitar cliques quando você soltar o botão
      if (!pressionado) {
        estadoBotaoInterno = BOTAO_ESPERA;
      }
      return BOTAO_ESPERA; 
      break;
  }
  return BOTAO_ESPERA;
}

bool lerJoystick(int &deltaX, int &deltaY) {
  int vrx = analogRead(PIN_VRX);
  int vry = analogRead(PIN_VRY);
  unsigned long agora = millis();
  bool mudou = false;

  deltaX = 0;
  deltaY = 0;

  if (agora - tempoUltimoMovimento > atrasoMovimento) {
    if (abs(vrx - 512) > deadzone) {
      deltaX = (vrx < 512 - deadzone) ? -1 : 1;
      mudou = true;
    }
    if (abs(vry - 512) > deadzone) {
      deltaY = (vry < 512 - deadzone) ? -1 : 1;
      mudou = true;
    }
    if (mudou) tempoUltimoMovimento = agora;
  }
  return mudou;
}
