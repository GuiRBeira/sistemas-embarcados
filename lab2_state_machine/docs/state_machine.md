stateDiagram-v2
    [*] --> NORMAL
    
    state NORMAL {
        [*] --> Exibindo
        Exibindo --> Verificando: tick do relógio
        Verificando --> Disparando: horario == alarme
        Verificando --> Exibindo
    }
    
    state AJUSTE {
        [*] --> SelecionaCampo
        SelecionaCampo --> IncrementaDecrementa: eixo Y movimento
        IncrementaDecrementa --> SelecionaCampo: confirma ou timeout
    }
    
    state ALARME_DISPARADO {
        [*] --> Tocando
        Tocando --> DesligadoManual: botão SW
        Tocando --> DesligadoTimeout: passou 5s
        DesligadoManual --> [*]
        DesligadoTimeout --> [*]
    }
    
    NORMAL --> AJUSTE: botão SW longo (>1s)
    AJUSTE --> NORMAL: botão SW curto
    NORMAL --> ALARME_DISPARADO: horario == alarme
    ALARME_DISPARADO --> NORMAL: (desligado)