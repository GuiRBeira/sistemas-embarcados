#include "display_utils.h"
#include <Wire.h>
#include <stdio.h>

// Definição do objeto display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

bool initDisplay() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    return false;
  }
  display.clearDisplay();
  display.display();
  return true;
}

void desenharHeader(const char* titulo) {
  display.drawRect(0, 0, 128, 16, SSD1306_WHITE);
  display.setCursor(4, 4);
  display.setTextSize(1);
  display.println(titulo);
}

void atualizarDisplay(EstadoPrincipal estado, 
                      SubEstadoAjuste subEstado,
                      int minAt, int segAt, 
                      int minAl, int segAl, 
                      int campoSel, const char* campos[],
                      bool alarmeDisparado) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  char buf[32];

  switch (estado) {
    case ESTADO_NORMAL:
      display.invertDisplay(false);
      desenharHeader("RELOGIO");
      
      // Relógio Principal (Tamanho Grande)
      display.setTextSize(2);
      sprintf(buf, "%02d:%02d", minAt, segAt);
      display.setCursor(35, 25);
      display.println(buf);
      
      // Alarme no rodapé
      display.setTextSize(1);
      display.drawFastHLine(0, 48, 128, SSD1306_WHITE);
      sprintf(buf, "Alarme: %02d:%02d", minAl, segAl);
      display.setCursor(20, 53);
      display.println(buf);
      break;

    case ESTADO_AJUSTE:
      display.invertDisplay(false);
      desenharHeader("MODO AJUSTE");
      
      // Indica qual campo está sendo editado
      display.setCursor(0, 20);
      display.print(subEstado == SEL_CAMPO ? "> " : "* ");
      display.print("Editando:"); 
      display.println(campos[campoSel]);
      
      display.drawFastHLine(0, 30, 128, SSD1306_WHITE);

      // Valores atuais
      display.setCursor(0, 36);
      sprintf(buf, "Relogio: %02d:%02d", minAt, segAt);
      display.println(buf);
      sprintf(buf, "Alarme : %02d:%02d", minAl, segAl);
      display.println(buf);

      // Dica de navegação
      display.setCursor(0, 56);
      display.setTextSize(1);
      if (subEstado == SEL_CAMPO) display.print("X:Mover | Y:Ajustar");
      else display.print("Alterando Valor...");
      break;

    case ESTADO_ALARME_ATIVO:
      // Efeito Strobo (Pisca invertendo a cada 250ms)
      display.invertDisplay((millis() / 250) % 2);
      
      desenharHeader("!!! ALERTA !!!");
      
      display.setTextSize(2);
      display.setCursor(35, 25);
      display.println("ALARME");
      
      display.setTextSize(1);
      display.setCursor(15, 50);
      display.println("PRESSIONE BOTAO");
      break;
  }
  display.display();
}
