#ifndef TYPES_H
#define TYPES_H

// Estados Principais do Sistema
typedef enum {
  ESTADO_NORMAL,
  ESTADO_AJUSTE,
  ESTADO_ALARME_ATIVO
} EstadoPrincipal;

// Sub-estados do modo de ajuste
typedef enum { SEL_CAMPO, INC_VALOR } SubEstadoAjuste;

// Estados da Máquina de Estados do Botão (Debounce)
typedef enum {
  BOTAO_ESPERA,
  BOTAO_PRECIONADO,
  BOTAO_CONFIRMADO_CURTO,
  BOTAO_CONFIRMADO_LONGO
} EstadoBotao;

#endif
