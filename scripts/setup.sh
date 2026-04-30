#!/bin/bash

# ============================================================
# SETUP - SISTEMAS EMBARCADOS
# ============================================================
# Instala os cores necessários e configura o arduino-cli
# ============================================================

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo -e "${YELLOW}🚀 Iniciando configuração do ambiente...${NC}"

# 1. Configurar URLs Adicionais (ESP32)
echo -e "${YELLOW}🌐 Configurando URLs para ESP32...${NC}"
arduino-cli config init --overwrite
arduino-cli config add board_manager.additional_urls https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

# 2. Atualizar Index
echo -e "${YELLOW}🔄 Atualizando índice de placas...${NC}"
arduino-cli core update-index

# 3. Instalar Cores
echo -e "${YELLOW}📦 Instalando core AVR (Arduino Uno/Mega)...${NC}"
arduino-cli core install arduino:avr

echo -e "${YELLOW}📦 Instalando core ESP32...${NC}"
arduino-cli core install esp32:esp32

echo -e "${GREEN}✅ Ambiente configurado com sucesso!${NC}"
echo -e "Use 'make list-boards' em qualquer projeto para ver as placas disponíveis."
