# Plataforma de Desenvolvimento - Sistemas Embarcados

Este repositório contém um framework modular para o desenvolvimento de laboratórios e projetos Arduino, utilizando `arduino-cli` e um sistema unificado de build baseado em `Makefiles`.

---

## 🏗️ Estrutura do Workspace

```text
.
├── arduino-manager.sh     # Script interativo de gerenciamento global
├── scripts/               # Lógica central do sistema de build
│   ├── arduino.mk         # Build core (compilação, upload, monitor)
│   └── lab.mk             # Gerenciamento de múltiplos projetos por lab
├── templates/             # Templates para novos projetos
├── lab1_timer/            # Exemplo de laboratório implementado
│   ├── Makefile           # Ponteiro para scripts/lab.mk
│   ├── transmissor/       # Projeto individual
│   │   ├── Makefile       # Ponteiro para scripts/arduino.mk
│   │   └── ...
│   └── ...
└── README.md              # Este manual
```

---

## 🚀 Como Utilizar

### 1. Script Interativo (Recomendado)
Para uma experiência fluida sem precisar decorar comandos, execute o gerenciador na raiz do repositório:
```bash
./arduino-manager.sh
```
*   **O que ele faz:** Busca automaticamente seus laboratórios (`lab*`) e projetos. Permite selecionar, compilar, fazer upload e monitorar a serial através de menus simples.

### 2. Comandos via Terminal (Make)
Se preferir o terminal diretamente, você pode navegar até qualquer pasta e usar os comandos básicos:

**Dentro de um projeto (ex: `lab1_timer/transmissor`):**
*   `make compile`: Apenas compila o código.
*   `make upload`: Compila e envia para a placa.
*   `make monitor`: Abre o monitor serial (115200 baud).
*   `make all`: Faz tudo (compile + upload + monitor).
*   `make info`: Mostra a placa e porta configuradas.

**Dentro de um lab (ex: `lab1_timer`):**
*   `make compile-all`: Tenta compilar todos os projetos do lab para checar erros.
*   `make clean-all`: Remove os arquivos temporários de build de todos os projetos.

---

## 🛠️ Configurações (Board e Port)

Por padrão, o sistema utiliza:
- **Placa:** `arduino:avr:uno`
- **Porta:** `/dev/ttyUSB0`

**Para alterar permanentemente em um projeto:**
Crie ou edite o arquivo `.board_config` ou `.port_config` dentro da pasta do projeto com:
```makefile
BOARD=arduino:avr:nano
PORT=/dev/ttyACM0
```
*(O `arduino-manager.sh` também permite verificar as portas disponíveis através do comando `p`)*.

---

## ➕ Criando um Novo Laboratório

Para replicar esta estrutura em novos laboratórios (ex: `lab2_adc`):

1.  **Crie a pasta do laboratório:**
    ```bash
    mkdir lab2_adc
    ```
2.  **Adicione o Makefile do Lab:**
    Copie ou crie um arquivo `lab2_adc/Makefile` com o conteúdo:
    ```makefile
    include ../scripts/lab.mk
    ```
3.  **Crie a pasta de um projeto:**
    ```bash
    mkdir lab2_adc/leitura_potenciometro
    ```
4.  **Adicione o Makefile do Projeto:**
    Copie ou crie um arquivo `lab2_adc/leitura_potenciometro/Makefile` com o conteúdo:
    ```makefile
    include ../../scripts/arduino.mk
    ```
5.  **Crie seu `.ino`**: O sistema identificará automaticamente seu código.

---

## 📦 Dependências

*   **[arduino-cli](https://arduino.github.io/arduino-cli/latest/)**: O motor por trás de toda a automação.
*   **Make**: Utilitário padrão de build em sistemas Unix/Linux.
