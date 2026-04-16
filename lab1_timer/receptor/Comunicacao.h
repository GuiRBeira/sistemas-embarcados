#ifndef COMUNICACAO_RECEPTOR_H
#define COMUNICACAO_RECEPTOR_H

#include <Arduino.h>

class Comunicacao {
  private:
    String bufferRecepcao;
    int totalMensagens;
    int mensagensValidas;
    
    // Método privado para extrair campos da mensagem
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
    Comunicacao() : bufferRecepcao(""), totalMensagens(0), mensagensValidas(0) {}
    
    // Processa dados recebidos pela serial
    void processarRecepcao() {
      while (Serial.available()) {
        char c = Serial.read();
        if (c == '\n') {
          processarMensagem(bufferRecepcao);
          bufferRecepcao = "";
          totalMensagens++;
        } else if (c != '\r') {  // Ignora carriage return
          bufferRecepcao += c;
        }
      }
    }
    
    // Analisa e valida a mensagem recebida
    void processarMensagem(String msg) {
      // Valida formato básico (deve conter 3 separadores '|')
      int count = 0;
      for (int i = 0; i < msg.length(); i++) {
        if (msg[i] == '|') count++;
      }
      
      if (count == 3 && msg.length() > 0) {
        mensagensValidas++;
        
        String timestamp, tipo, dado, id;
        extrairCampos(msg, timestamp, tipo, dado, id);
        
        Serial.println("─────────────────────────────────");
        Serial.print("📥 Mensagem #");
        Serial.print(mensagensValidas);
        Serial.println(" recebida:");
        Serial.println(msg);
        Serial.println("  ⏱️  Timestamp: " + timestamp + " ms");
        Serial.println("  📋 Tipo: " + tipo);
        Serial.println("  📊 Dado: " + dado);
        Serial.println("  🆔 ID: " + id);
        Serial.println("─────────────────────────────────");
        
        // Envia confirmação de volta
        enviarConfirmacao(id, tipo);
      } else if (msg.length() > 0) {
        Serial.print("⚠️ Mensagem mal formatada: ");
        Serial.println(msg);
      }
    }
    
    // Envia confirmação de recebimento
    void enviarConfirmacao(String idMensagem, String tipo) {
      String ack = "ACK|" + idMensagem + "|" + tipo + "|" + String(millis());
      Serial.print("📤 ");
      Serial.println(ack);
    }
    
    // Estatísticas do sistema
    void mostrarEstatisticas() {
      static unsigned long ultimoRelatorio = 0;
      if (millis() - ultimoRelatorio > 10000) { // A cada 10 segundos
        ultimoRelatorio = millis();
        Serial.println("═════════════════════════════════");
        Serial.print("📊 ESTATÍSTICAS: ");
        Serial.print(mensagensValidas);
        Serial.print("/");
        Serial.print(totalMensagens);
        Serial.println(" mensagens válidas");
        
        if (totalMensagens > 0) {
          float taxa = (mensagensValidas * 100.0) / totalMensagens;
          Serial.print("📈 Taxa de sucesso: ");
          Serial.print(taxa);
          Serial.println("%");
        }
        Serial.println("═════════════════════════════════");
      }
    }
    
    // Retorna número de mensagens válidas recebidas
    int getMensagensValidas() {
      return mensagensValidas;
    }
};

#endif