# ============================================================
# Lab Build System (Evolved)
# ============================================================
# Gerenciamento avançado de múltiplos projetos em um Lab.
# ============================================================

# Cores
GREEN  = \033[0;32m
RED    = \033[0;31m
YELLOW = \033[1;33m
BLUE   = \033[0;34m
NC     = \033[0m

# Encontra subdiretórios que contêm arquivos .ino
PROJECTS = $(shell find . -maxdepth 2 -name "*.ino" -exec dirname {} \; | sed 's|^./||' | sort)

.PHONY: all compile-all clean-all info-all help $(PROJECTS)

help:
	@echo -e "${BLUE}╔══════════════════════════════════════════════════════╗${NC}"
	@echo -e "${BLUE}║            LAB MANAGER - COMANDOS                    ║${NC}"
	@echo -e "${BLUE}╠══════════════════════════════════════════════════════╣${NC}"
	@echo -e "  make compile-all   - Compila todos os projetos"
	@echo -e "  make clean-all     - Limpa builds de todos"
	@echo -e "  make info-all      - Mostra config de cada projeto"
	@echo -e "  make <projeto>     - Compila e envia um específico"
	@echo -e "${BLUE}╚══════════════════════════════════════════════════════╝${NC}"

all: compile-all

compile-all:
	@echo -e "${YELLOW}🚀 Iniciando compilação em lote para: $(shell basename $(CURDIR))${NC}"
	@echo -e "${YELLOW}------------------------------------------------------${NC}"
	@failed=""; \
	passed=""; \
	for dir in $(PROJECTS); do \
		echo -e "${BLUE}➡️  Compilando: $$dir...${NC}"; \
		if $(MAKE) -C $$dir compile BOARD=$(BOARD) PORT=$(PORT) > /dev/null 2>&1; then \
			echo -e "   ${GREEN}✅ SUCESSO${NC}"; \
			passed="$$passed $$dir"; \
		else \
			echo -e "   ${RED}❌ FALHA${NC}"; \
			failed="$$failed $$dir"; \
		fi; \
	done; \
	echo -e "${YELLOW}------------------------------------------------------${NC}"; \
	echo -e "${YELLOW}📊 RELATÓRIO FINAL:${NC}"; \
	if [ ! -z "$$passed" ]; then echo -e "   ${GREEN}Passaram:${NC}$$passed"; fi; \
	if [ ! -z "$$failed" ]; then \
		echo -e "   ${RED}Falharam:${NC}$$failed"; \
		exit 1; \
	else \
		echo -e "   ${GREEN}✨ Todos os projetos estão OK!${NC}"; \
	fi

clean-all:
	@echo -e "${YELLOW}🧹 Limpando todos os projetos...${NC}"
	@for dir in $(PROJECTS); do \
		$(MAKE) -C $$dir clean > /dev/null; \
	done
	@echo -e "${GREEN}✅ Tudo limpo!${NC}"

info-all:
	@echo -e "${YELLOW}📋 Configurações atuais do Lab:${NC}"
	@for dir in $(PROJECTS); do \
		echo -n "  [$$dir]: "; \
		$(MAKE) -s -C $$dir info | grep -E "Placa:|Porta:" | tr '\n' ' '; \
		echo ""; \
	done

# Atalho para rodar 'make all' em um projeto específico (ex: make stopwatch)
$(PROJECTS):
	@echo -e "${BLUE}🔨 Executando projeto individual: $@${NC}"
	$(MAKE) -C $@ all
