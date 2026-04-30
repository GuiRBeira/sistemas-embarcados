#ifndef INPUT_UTILS_H
#define INPUT_UTILS_H

#include <Arduino.h>
#include "types.h"
#include "pins.h"

void initInputs();
EstadoBotao processarBotao();
bool lerJoystick(int &deltaX, int &deltaY);

#endif
