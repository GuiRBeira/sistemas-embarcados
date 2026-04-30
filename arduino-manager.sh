#!/bin/bash

# ============================================================
# ARDUINO GLOBAL MANAGER
# ============================================================
# Script dinâmico para gerenciar múltiplos Labs e Projetos
# Adaptável para Arduino Uno, Mega, ESP32 e mais.
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

start_menu() {
    CURRENT_LAB=""
    CURRENT_PROJ=""
    show_header
    echo "1. Gerenciar Laboratórios / Projetos"
    echo "2. Criar Novo Laboratório/Projeto"
    echo "3. Gerenciar Bibliotecas (Instalar/Buscar)"
    echo "4. Configurar Ambiente (Cores/ESP32)"
    echo "q. Sair"
    echo ""
    read -p "Escolha: " choice

    case $choice in
        1) select_lab ;;
        2) create_wizard ;;
        3) library_menu ;;
        4) $ROOT_DIR/scripts/setup.sh; read -p "Enter..."; start_menu ;;
        q) exit 0 ;;
        *) start_menu ;;
    esac
}

library_menu() {
    show_header
    echo -e "${YELLOW}--- GERENCIADOR DE BIBLIOTECAS ---${NC}"
    echo "1. Buscar Biblioteca"
    echo "2. Instalar Biblioteca"
    echo "3. Listar Instaladas"
    echo "b. Voltar"
    echo ""
    read -p "Escolha: " lchoice

    case $lchoice in
        1) 
            read -p "Digite o nome para busca: " search_name
            arduino-cli lib search "$search_name"
            read -p "Pressione Enter para continuar..."
            library_menu
            ;;
        2)
            read -p "Digite o nome exato para instalar: " lib_name
            arduino-cli lib install "$lib_name"
            read -p "Pressione Enter para continuar..."
            library_menu
            ;;
        3)
            arduino-cli lib list
            read -p "Pressione Enter para continuar..."
            library_menu
            ;;
        b) start_menu ;;
        *) library_menu ;;
    esac
}

create_wizard() {
    show_header
    echo -e "${YELLOW}--- CRIAR NOVO PROJETO ---${NC}"
    read -p "Nome do Lab (ex: lab3_sensores): " lab_name
    read -p "Nome do Projeto (ex: dht11_test): " proj_name

    LAB_PATH="$ROOT_DIR/$lab_name"
    PROJ_PATH="$LAB_PATH/$proj_name"

    # Criar Lab se não existir
    if [ ! -d "$LAB_PATH" ]; then
        mkdir -p "$LAB_PATH"
        echo "include ../scripts/lab.mk" > "$LAB_PATH/Makefile"
    fi

    # Criar Projeto
    if [ -d "$PROJ_PATH" ]; then
        echo -e "${RED}Erro: Projeto já existe!${NC}"
    else
        mkdir -p "$PROJ_PATH"
        echo "include ../../scripts/arduino.mk" > "$PROJ_PATH/Makefile"
        echo -e "void setup() {\n  Serial.begin(115200);\n}\n\nvoid loop() {\n  \n}" > "$PROJ_PATH/$proj_name.ino"
        echo -e "${GREEN}✅ Projeto criado com sucesso em $PROJ_PATH${NC}"
    fi
    read -p "Pressione Enter para voltar..."
    start_menu
}

select_lab() {
    local labs=($(find "$ROOT_DIR" -maxdepth 1 -type d -name "lab*" | sort))
    
    if [ ${#labs[@]} -eq 0 ]; then
        echo -e "${RED}Nenhum laboratório encontrado (padrão: lab*)${NC}"
        read -p "Pressione Enter..."
        start_menu
        return
    fi

    show_header
    echo "Selecione o Laboratório:"
    for i in "${!labs[@]}"; do
        echo "$((i+1)). $(basename ${labs[$i]})"
    done
    echo "b. Voltar"
    echo "q. Sair"

    read -p "Escolha: " choice
    if [ "$choice" == "q" ]; then exit 0; fi
    if [ "$choice" == "b" ]; then start_menu; return; fi
    
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
        select_lab
        return
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
        echo "  b. Trocar Placa (Set Board)"
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
            b) board_menu ;;
            s) select_project; return ;;
            l) CURRENT_LAB=""; select_lab; return ;;
            q) exit 0 ;;
            *) echo -e "${RED}Comando inválido!${NC}" ;;
        esac
        echo ""
        read -p "Pressione Enter para continuar..."
    done
}

board_menu() {
    show_header
    echo "Selecione o perfil da placa:"
    echo "1. Arduino Uno (arduino:avr:uno)"
    echo "2. Arduino Mega (arduino:avr:mega)"
    echo "3. ESP32 DevKit (esp32:esp32:esp32)"
    echo "4. ESP32-S3 (esp32:esp32:esp32s3)"
    echo "5. Manual (Digitar FQBN)"
    echo "b. Voltar"
    read -p "Escolha: " bchoice

    case $bchoice in
        1) make -C "$CURRENT_PROJ" set-board BOARD=uno ;;
        2) make -C "$CURRENT_PROJ" set-board BOARD=mega ;;
        3) make -C "$CURRENT_PROJ" set-board BOARD=esp32 ;;
        4) make -C "$CURRENT_PROJ" set-board BOARD=esp32s3 ;;
        5) read -p "Digite o FQBN: " fqbn; make -C "$CURRENT_PROJ" set-board BOARD=$fqbn ;;
        b) return ;;
        *) board_menu ;;
    esac
}

# Iniciar
start_menu
