#ifndef COMUNICACAO_FULL_DUPLEX_H
#define COMUNICACAO_FULL_DUPLEX_H

#include <Arduino.h>

class Comunicacao {
  private:
    // --- Atributos de Transmissão ---
    int contadorMensagens;
    String ultimaMensagemEnviada;

    // --- Atributos de Recepção ---
    String bufferRecepcao;
    int totalMensagensRecebidas;
    int mensagensValidasRecebidas;

    // Método privado para extrair campos da mensagem recebida (Protocolo: timestamp|tipo|dado|id)
    void extrairCampos(String msg, String& timestamp, String& tipo, 
                       String& dado, String& id) {
      int idx1 = msg.indexOf('|');
      int idx2 = msg.indexOf('|', idx1 + 1);
      int idx3 = msg.indexOf('|', idx2 + 1);
      
      timestamp = msg.substring(0, idx1);
      tipo = msg.substring(idx1 + 1, idx2);
      dado = msg.substring(idx2 + 1, idx3);
      id = msg.substring(idx3 + 1);
    }

  public:
    Comunicacao() : 
      contadorMensagens(0), 
      ultimaMensagemEnviada(""),
      bufferRecepcao(""), 
      totalMensagensRecebidas(0), 
      mensagensValidasRecebidas(0) {}

    // --- MÉTODOS DE TRANSMISSÃO ---

    // Envia mensagem no formato do protocolo: milis|tipo|dado|id
    void enviarMensagem(String tipo, String conteudo) {
      String mensagem = String(millis()) + "|" + 
                       tipo + "|" + 
                       conteudo + "|" + 
                       String(contadorMensagens++);
      
      // Envia para a Serial (para o outro nó receber)
      Serial.println(mensagem);
      ultimaMensagemEnviada = mensagem;
      
      // Mostra feedback no console (opcional, pode ser prefixado para não confundir parser do outro lado)
      // Serial.print("[TX] "); Serial.println(mensagem);
    }

    // Chamada pelo timer para envio automático
    void transmissaoPeriodica() {
      int valorSensor = analogRead(A0);
      enviarMensagem("SENSOR", String(valorSensor));
    }

    // Envia confirmação de recebimento (Protocolo: ACK|id|tipo|milis)
    void enviarConfirmacao(String idMensagem, String tipo) {
      String ack = "ACK|" + idMensagem + "|" + tipo + "|" + String(millis());
      Serial.println(ack);
    }

    // --- MÉTODOS DE RECEPÇÃO E PROCESSAMENTO ---

    // Lê dados da serial e acumula no buffer até encontrar uma nova linha
    void processarComunicacao() {
      while (Serial.available()) {
        char c = Serial.read();
        if (c == '\n') {
          analisarEntrada(bufferRecepcao);
          bufferRecepcao = "";
        } else if (c != '\r') {
          bufferRecepcao += c;
        }
      }
    }

    // Decide se a entrada é uma mensagem do outro nó, um ACK ou comando do usuário
    void analisarEntrada(String entrada) {
      entrada.trim();
      if (entrada.length() == 0) return;

      // Conta o número de separadores para validar se segue o protocolo
      int pipes = 0;
      for (int i = 0; i < entrada.length(); i++) {
        if (entrada[i] == '|') pipes++;
      }

      // Caso 1: Segue o formato do protocolo (3 pipes)
      if (pipes == 3) {
        if (entrada.startsWith("ACK|")) {
          // Recebemos uma confirmação de uma mensagem que enviamos
          Serial.print("✅ [INFO] ACK recebido: ");
          Serial.println(entrada);
        } else {
          // Recebemos uma mensagem nova de outro nó
          totalMensagensRecebidas++;
          mensagensValidasRecebidas++;
          
          String timestamp, tipo, dado, id;
          extrairCampos(entrada, timestamp, tipo, dado, id);
          
          Serial.println("─────────────────────────────────");
          Serial.print("📥 FULL-DUPLEX: Recebido #");
          Serial.println(mensagensValidasRecebidas);
          Serial.println("   Timestamp: " + timestamp + "ms");
          Serial.println("   Tipo: " + tipo + " | Dado: " + dado + " | ID: " + id);
          Serial.println("─────────────────────────────────");
          
          // Responde com ACK para confirmar o recebimento
          enviarConfirmacao(id, tipo);
        }
      } 
      // Caso 2: Comando do usuário para enviar mensagem customizada (ex: MSG:Texto)
      else if (entrada.startsWith("MSG:")) {
        String msg = entrada.substring(4);
        enviarMensagem("CUSTOM", msg);
      }
      // Caso 3: Entrada arbitrária do usuário tratada como mensagem de TEXTO
      else {
        enviarMensagem("TEXTO", entrada);
      }
    }

    // Exibe estatísticas de TX e RX periodicamente
    void mostrarEstatisticas() {
      static unsigned long ultimoRelatorio = 0;
      if (millis() - ultimoRelatorio > 20000) { // A cada 20 segundos
        ultimoRelatorio = millis();
        Serial.println("\n╔════════════════════════════════════════╗");
        Serial.println("║       STATUS DO NÓ FULL-DUPLEX       ║");
        Serial.print("║  Enviadas: "); Serial.print(contadorMensagens); Serial.println("                     ║");
        Serial.print("║  Recebidas: "); Serial.print(mensagensValidasRecebidas); 
        Serial.print("/"); Serial.print(totalMensagensRecebidas); Serial.println(" validas              ║");
        
        if (totalMensagensRecebidas > 0) {
          float taxa = (mensagensValidasRecebidas * 100.0) / totalMensagensRecebidas;
          Serial.print("║  Taxa RX: "); Serial.print(taxa, 1); Serial.println("%                      ║");
        }
        Serial.println("╚════════════════════════════════════════╝\n");
      }
    }

    int getMensagensValidas() { return mensagensValidasRecebidas; }
};

#endif
