# Documentação de Modularização - Stopwatch

A modularização dividiu o código monolítico em componentes reutilizáveis e fáceis de testar.

## 🏗️ Arquitetura

### 1. Camada de Hardware (`pins.h` e `types.h`)
- **Objetivo**: Abstrair o hardware e os estados.
- **Benefício**: Facilita a migração para outras placas (ex: se o Buzzer mudar de pino 9 para 10, altera-se apenas em um lugar).

### 2. Módulo de Interface (`display_utils`)
- **Funções**: `initDisplay()` e `atualizarDisplay()`.
- **Destaque**: Uso de `extern Adafruit_SSD1306 display` para permitir que o objeto global seja compartilhado mantendo a implementação escondida no `.cpp`.

### 3. Módulo de Entrada (`input_utils`)
- **Lógica de Joystick**: Agora retorna deltas (`dx`, `dy`) baseados em zonas mortas e intervalos de tempo, evitando leituras frenéticas.
- **Máquina de Estados do Botão**: O debounce foi isolado. O arquivo principal apenas pergunta `processarBotao()` e recebe o estado final (Curto, Longo ou Espera).

### 4. Orquestrador (`stopwatch.ino`)
- O `loop()` agora é uma leitura clara de alto nível:
  1. Verifica o tempo.
  2. Processa entradas.
  3. Atualiza a máquina de estados.
  4. Renderiza o display.

---

## 🛠️ Como Adicionar Novas Funcionalidades

### Adicionar um novo componente (ex: LED de Alarme)
1. Defina o pino em `pins.h`.
2. Adicione a inicialização no `setup()` do `.ino`.
3. Crie uma função de controle (ex: `toggleLed()`) ou adicione ao módulo de hardware apropriado.

### Adicionar um novo Estado (ex: MODO_CRONOMETRO)
1. Adicione o novo valor no enum `EstadoPrincipal` em `types.h`.
2. Adicione o `case` correspondente no `switch` do `stopwatch.ino` e no `atualizarDisplay()` em `display_utils.cpp`.
