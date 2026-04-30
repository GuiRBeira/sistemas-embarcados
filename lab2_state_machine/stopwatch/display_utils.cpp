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

void atualizarDisplay(EstadoPrincipal estado, 
                      int minAt, int segAt, 
                      int minAl, int segAl, 
                      int campoSel, const char* campos[],
                      bool alarmeDisparado) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  char buf[32];

  switch (estado) {
    case ESTADO_NORMAL:
      display.println("=== RELOGIO ===");
      sprintf(buf, "Hora: %02d:%02d", minAt, segAt);
      display.println(buf);
      sprintf(buf, "Alarme: %02d:%02d", minAl, segAl);
      display.println(buf);
      if (alarmeDisparado) display.println("ALARME!!!");
      break;

    case ESTADO_AJUSTE:
      display.println("=== AJUSTE ===");
      display.print("Campo: "); display.println(campos[campoSel]);
      sprintf(buf, "Min: %02d  Seg: %02d", minAt, segAt);
      display.println(buf);
      sprintf(buf, "Alarme: %02d:%02d", minAl, segAl);
      display.println(buf);
      display.println("X: muda campo");
      display.println("Y: ajusta valor");
      display.println("SW: curto p/ sair");
      break;

    case ESTADO_ALARME_ATIVO:
      display.println("*** ALARME ***");
      sprintf(buf, "Hora: %02d:%02d", minAt, segAt);
      display.println(buf);
      display.println("Pressione SW p/ parar");
      break;
  }
  display.display();
}
