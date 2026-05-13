# ============================================================
# Core Arduino Build System (Shared)
# ============================================================
# Este arquivo contém a lógica comum para compilar e enviar
# sketches Arduino usando arduino-cli.
# ============================================================

# Variáveis padrão (podem ser sobrescritas no Makefile local ou via .board_config)
BOARD ?= arduino:avr:uno
PORT ?= /dev/ttyUSB0
BAUD ?= 115200
SKETCH ?= $(shell ls *.ino 2>/dev/null | head -n 1)

# Aliases comuns para facilitar a troca
# Ex: make compile BOARD=mega
ifeq ($(BOARD),uno)
  override BOARD = arduino:avr:uno
endif
ifeq ($(BOARD),mega)
  override BOARD = arduino:avr:mega
endif
ifeq ($(BOARD),esp32)
  override BOARD = esp32:esp32:esp32
endif
ifeq ($(BOARD),esp32s3)
  override BOARD = esp32:esp32:esp32s3
endif

# Cores
RED    = \033[0;31m
GREEN  = \033[0;32m
YELLOW = \033[1;33m
NC     = \033[0m

# Caminhos
BUILD_DIR = ./build

.PHONY: help compile upload monitor all clean info ports lib-list lib-install \
        lib-search list-boards set-board set-port

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
	@echo "║  make ports      - Listar portas disponíveis        ║"
	@echo "║  make list-boards- Listar placas disponíveis        ║"
	@echo "║  make set-board BOARD=... - Definir placa manual    ║"
	@echo "║  make set-port PORT=...   - Definir porta manual    ║"
	@echo "║  make lib-list   - Listar bibliotecas instaladas    ║"
	@echo "║  make lib-search NAME=... - Buscar biblioteca       ║"
	@echo "║  make lib-install NAME=... - Instalar biblioteca    ║"
	@echo "╚══════════════════════════════════════════════════════╝"

compile:
	@if [ -z "$(SKETCH)" ]; then echo "$(RED)❌ Erro: Nenhum .ino encontrado!$(NC)"; exit 1; fi
	@echo "$(YELLOW)🔨 Compilando: $(SKETCH)$(NC)"
	@echo "$(YELLOW)📦 Placa: $(BOARD)$(NC)"
	arduino-cli compile -b $(BOARD) --build-path $(BUILD_DIR)
	@if [ $$? -eq 0 ]; then \
		echo "$(GREEN)✅ Compilação bem sucedida!$(NC)"; \
	else \
		echo "$(RED)❌ Falha na compilação!$(NC)"; \
		exit 1; \
	fi

upload: compile
	@echo "$(YELLOW)📤 Enviando para $(PORT)...$(NC)"
	arduino-cli upload -b $(BOARD) -p $(PORT) --input-dir $(BUILD_DIR)
	@if [ $$? -eq 0 ]; then \
		echo "$(GREEN)✅ Upload concluído!$(NC)"; \
	else \
		echo "$(RED)❌ Falha no upload!$(NC)"; \
		exit 1; \
	fi

monitor:
	@echo "$(YELLOW)📡 Monitor em $(PORT) ($(BAUD) baud)$(NC)"
	@echo "$(YELLOW)Pressione Ctrl+C para sair$(NC)"
	arduino-cli monitor -p $(PORT) -c baudrate=$(BAUD)

all: upload monitor

clean:
	@echo "$(YELLOW)🧹 Limpando...$(NC)"
	rm -rf $(BUILD_DIR)
	@echo "$(GREEN)✅ Limpeza concluída!$(NC)"

info:
	@echo "$(YELLOW)📋 Status do projeto:$(NC)"
	@echo "  Sketch: $(SKETCH)"
	@echo "  Placa:  $(BOARD)"
	@echo "  Porta:  $(PORT)"
	@echo "  Baud:   $(BAUD)"
	@echo "  Diretório: $(PWD)"

ports:
	@echo "$(YELLOW)🔌 Portas seriais disponíveis:$(NC)"
	@arduino-cli board list
	@echo ""
	@ls -la /dev/ttyUSB* /dev/ttyACM* 2>/dev/null || echo "Nenhuma porta encontrada"

list-boards:
	@echo "$(YELLOW)📋 Placas disponíveis (Cores instalados):$(NC)"
	@arduino-cli board listall | grep -E "arduino:avr|esp32:esp32|^ " | head -30

lib-list:
	@echo "$(YELLOW)📚 Bibliotecas instaladas:$(NC)"
	@arduino-cli lib list

lib-search:
	@if [ -z "$(NAME)" ]; then \
		echo "$(RED)❌ Use: make lib-search NAME=nome_da_biblioteca$(NC)"; \
		exit 1; \
	fi
	@echo "$(YELLOW)🔍 Buscando: $(NAME)$(NC)"
	@arduino-cli lib search "$(NAME)"

lib-install:
	@if [ -z "$(NAME)" ]; then \
		echo "$(RED)❌ Use: make lib-install NAME=nome_da_biblioteca$(NC)"; \
		exit 1; \
	fi
	@echo "$(YELLOW)📦 Instalando: $(NAME)$(NC)"
	@arduino-cli lib install "$(NAME)"
	@echo "$(GREEN)✅ Biblioteca instalada!$(NC)"

set-board:
	@if [ -z "$(BOARD)" ]; then \
		echo "$(RED)❌ Use: make set-board BOARD=arduino:avr:uno$(NC)"; \
		exit 1; \
	fi
	@echo "BOARD=$(BOARD)" > .board_config
	@echo "$(GREEN)✅ Placa configurada: $(BOARD)$(NC)"

set-port:
	@if [ -z "$(PORT)" ]; then \
		echo "$(RED)❌ Use: make set-port PORT=/dev/ttyUSB0$(NC)"; \
		exit 1; \
	fi
	@echo "PORT=$(PORT)" > .port_config
	@echo "$(GREEN)✅ Porta configurada: $(PORT)$(NC)"

# Carregar configurações salvas
-include .board_config
-include .port_config