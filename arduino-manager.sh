#!/bin/bash

# ============================================================
# ARDUINO GLOBAL MANAGER
# ============================================================
# Script dinâmico para gerenciar múltiplos Labs e Projetos
# ============================================================

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CURRENT_LAB=""
CURRENT_PROJ=""

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

show_header() {
    clear
    echo -e "${BLUE}╔══════════════════════════════════════════════════════╗${NC}"
    echo -e "${BLUE}║            SISTEMAS EMBARCADOS - MANAGER             ║${NC}"
    echo -e "${BLUE}╚══════════════════════════════════════════════════════╝${NC}"
    if [ ! -z "$CURRENT_LAB" ]; then
        echo -e "${YELLOW}📍 Lab: $(basename $CURRENT_LAB)${NC}"
    fi
    if [ ! -z "$CURRENT_PROJ" ]; then
        echo -e "${YELLOW}📂 Proj: $(basename $CURRENT_PROJ)${NC}"
    fi
    echo ""
}

select_lab() {
    local labs=($(find "$ROOT_DIR" -maxdepth 1 -type d -name "lab*" | sort))
    
    if [ ${#labs[@]} -eq 0 ]; then
        echo -e "${RED}Nenhum laboratório encontrado (padrao: lab*)${NC}"
        read -p "Pressione Enter..."
        return 1
    fi

    show_header
    echo "Selecione o Laboratório:"
    for i in "${!labs[@]}"; do
        echo "$((i+1)). $(basename ${labs[$i]})"
    done
    echo "q. Sair"

    read -p "Escolha: " choice
    if [ "$choice" == "q" ]; then exit 0; fi
    
    local idx=$((choice-1))
    if [ $idx -ge 0 ] && [ $idx -lt ${#labs[@]} ]; then
        CURRENT_LAB="${labs[$idx]}"
        CURRENT_PROJ=""
        select_project
    else
        echo -e "${RED}Opção inválida!${NC}"
        sleep 1
        select_lab
    fi
}

select_project() {
    local projs=($(find "$CURRENT_LAB" -maxdepth 2 -name "*.ino" -exec dirname {} \; | sort | uniq))
    
    if [ ${#projs[@]} -eq 0 ]; then
        echo -e "${RED}Nenhum projeto (.ino) encontrado neste Lab.${NC}"
        CURRENT_LAB=""
        read -p "Pressione Enter..."
        return 1
    fi

    show_header
    echo "Selecione o Projeto:"
    for i in "${!projs[@]}"; do
        echo "$((i+1)). $(basename ${projs[$i]})"
    done
    echo "b. Voltar aos Labs"
    echo "q. Sair"

    read -p "Escolha: " choice
    if [ "$choice" == "q" ]; then exit 0; fi
    if [ "$choice" == "b" ]; then CURRENT_LAB=""; select_lab; return; fi
    
    local idx=$((choice-1))
    if [ $idx -ge 0 ] && [ $idx -lt ${#projs[@]} ]; then
        CURRENT_PROJ="${projs[$idx]}"
        main_loop
    else
        echo -e "${RED}Opção inválida!${NC}"
        sleep 1
        select_project
    fi
}

main_loop() {
    while true; do
        show_header
        echo "Comandos para o projeto $(basename $CURRENT_PROJ):"
        echo "  c. Compilar (make compile)"
        echo "  u. Upload (make upload)"
        echo "  m. Monitor Serial (make monitor)"
        echo "  a. All (Compile + Upload + Monitor)"
        echo "  i. Info"
        echo "  p. Listar Portas"
        echo ""
        echo "  s. Selecionar outro Projeto"
        echo "  l. Selecionar outro Lab"
        echo "  q. Sair"
        echo ""
        read -p "Comando: " cmd

        case $cmd in
            c) make -C "$CURRENT_PROJ" compile ;;
            u) make -C "$CURRENT_PROJ" upload ;;
            m) make -C "$CURRENT_PROJ" monitor ;;
            a) make -C "$CURRENT_PROJ" all ;;
            i) make -C "$CURRENT_PROJ" info ;;
            p) make -C "$CURRENT_PROJ" ports ;;
            s) select_project; return ;;
            l) CURRENT_LAB=""; select_lab; return ;;
            q) exit 0 ;;
            *) echo -e "${RED}Comando inválido!${NC}" ;;
        esac
        echo ""
        read -p "Pressione Enter para continuar..."
    done
}

# Iniciar
select_lab
