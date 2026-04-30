#ifndef DISPLAY_UTILS_H
#define DISPLAY_UTILS_H

#include <Adafruit_SSD1306.h>
#include "types.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C

// Declaração externa para que outros arquivos possam usar
extern Adafruit_SSD1306 display;

bool initDisplay();
void atualizarDisplay(EstadoPrincipal estado, 
                      int minAt, int segAt, 
                      int minAl, int segAl, 
                      int campoSel, const char* campos[],
                      bool alarmeDisparado);

#endif
