# ============================================================
# Lab Build System (Shared)
# ============================================================
# Este arquivo gerencia múltiplos projetos dentro de um Lab.
# ============================================================

# Encontra subdiretórios que contêm arquivos .ino
PROJECTS = $(shell find . -maxdepth 2 -name "*.ino" -exec dirname {} \; | sed 's|^./||' | sort)

.PHONY: all compile-all clean-all $(PROJECTS)

all: compile-all

compile-all:
	@echo "🔨 Compilando todos os projetos do $(shell basename $(CURDIR))..."
	@for dir in $(PROJECTS); do \
		echo "\n➡️  Entrando em $$dir..."; \
		$(MAKE) -C $$dir compile || exit 1; \
	done

clean-all:
	@for dir in $(PROJECTS); do \
		$(MAKE) -C $$dir clean; \
	done

# Atalho para rodar 'make all' em um projeto específico (ex: make transmissor)
$(PROJECTS):
	$(MAKE) -C $@ all
