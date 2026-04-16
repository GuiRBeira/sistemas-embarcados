# ============================================================
# Core Arduino Build System (Shared)
# ============================================================
# Este arquivo contém a lógica comum para compilar e enviar
# sketches Arduino usando arduino-cli.
# ============================================================

# Variáveis padrão (podem ser sobrescritas no Makefile local)
BOARD ?= arduino:avr:uno
PORT ?= /dev/ttyUSB0
BAUD ?= 115200
SKETCH ?= $(shell ls *.ino 2>/dev/null | head -n 1)

# Cores
RED    = \033[0;31m
GREEN  = \033[0;32m
YELLOW = \033[1;33m
NC     = \033[0m

# Caminhos
BUILD_DIR = ./build

.PHONY: help compile upload monitor all clean info ports lib-list lib-install

help:
	@echo "╔══════════════════════════════════════════════════════╗"
	@echo "║           ARDUINO CORE MK - COMANDOS                ║"
	@echo "╠══════════════════════════════════════════════════════╣"
	@echo "║  make compile    - Compilar o sketch                ║"
	@echo "║  make upload     - Compilar e fazer upload          ║"
	@echo "║  make monitor    - Abrir monitor serial             ║"
	@echo "║  make all        - Compilar + upload + monitor      ║"
	@echo "║  make clean      - Limpar arquivos temporários      ║"
	@echo "║  make info       - Mostrar configurações atuais     ║"
	@echo "║  make lib-install NAME=... - Instalar biblioteca    ║"
	@echo "╚══════════════════════════════════════════════════════╝"

compile:
	@if [ -z "$(SKETCH)" ]; then echo "$(RED)Erro: Nenhum .ino encontrado!$(NC)"; exit 1; fi
	@echo "$(YELLOW)🔨 Compilando: $(SKETCH)$(NC)"
	arduino-cli compile -b $(BOARD) --build-path $(BUILD_DIR)
	@if [ $$? -eq 0 ]; then echo "$(GREEN)✅ Sucesso!$(NC)"; else exit 1; fi

upload: compile
	@echo "$(YELLOW)📤 Enviando para $(PORT)...$(NC)"
	arduino-cli upload -b $(BOARD) -p $(PORT) --input-dir $(BUILD_DIR)
	@if [ $$? -eq 0 ]; then echo "$(GREEN)✅ Upload concluído!$(NC)"; else exit 1; fi

monitor:
	@echo "$(YELLOW)📡 Monitor em $(PORT) ($(BAUD) baud)$(NC)"
	arduino-cli monitor -p $(PORT) -c baudrate=$(BAUD)

all: upload monitor

clean:
	@echo "$(YELLOW)🧹 Limpando...$(NC)"
	rm -rf $(BUILD_DIR)

info:
	@echo "$(YELLOW)Status:$(NC)"
	@echo "  Projeto: $(SKETCH)"
	@echo "  Placa:   $(BOARD)"
	@echo "  Porta:   $(PORT)"

ports:
	@arduino-cli board list

lib-install:
	@if [ -z "$(NAME)" ]; then echo "Use: make lib-install NAME=..."; exit 1; fi
	arduino-cli lib install "$(NAME)"

# Configurações dinâmicas (persistem em arquivos ocultos)
set-board:
	@echo "BOARD=$(VALUE)" > .board_config
set-port:
	@echo "PORT=$(VALUE)" > .port_config

-include .board_config
-include .port_config
