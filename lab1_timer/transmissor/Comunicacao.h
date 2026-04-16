#ifndef COMUNICACAO_TRANSMISSOR_H
#define COMUNICACAO_TRANSMISSOR_H

#include <Arduino.h>

class Comunicacao {
  private:
    int contadorMensagens;
    unsigned long ultimoEnvio;
    String ultimaMensagem;
    
  public:
    Comunicacao() : contadorMensagens(0), ultimoEnvio(0), ultimaMensagem("") {}
    
    // Envia mensagem formatada com timestamp
    void enviarMensagem(String tipo, String conteudo) {
      String mensagem = String(millis()) + "|" + 
                       tipo + "|" + 
                       conteudo + "|" + 
                       String(contadorMensagens++);
      
      Serial.println(mensagem);
      ultimaMensagem = mensagem;
      
      // Debug local
      Serial.print("[TX] ");
      Serial.println(mensagem);
    }
    
    // Processa transmissão periódica (chamada pelo timer)
    void transmissaoPeriodica() {
      // Simula leitura de sensor (pino A0) ou gera dado aleatório
      int valorSensor = analogRead(A0);
      enviarMensagem("SENSOR", String(valorSensor));
    }
    
    // Processa entrada da Serial (pode ser resposta do receptor ou comando do usuário)
    void processarEntradaSerial() {
      if (Serial.available()) {
        String entrada = Serial.readStringUntil('\n');
        entrada.trim();
        
        if (entrada.length() == 0) return;

        // Se a entrada começar com "ACK", identificamos como resposta do receptor
        if (entrada.startsWith("ACK")) {
          Serial.print("[INFO] Resposta recebida: ");
          Serial.println(entrada);
          Serial.println("-> Confirmação de recebimento validada.");
        } 
        // Se for um comando especial de mensagem (ex: MSG:conteudo)
        else if (entrada.startsWith("MSG:")) {
          String msg = entrada.substring(4);
          enviarMensagem("CUSTOM", msg);
        }
        // Caso contrário, envia o texto inteiro como mensagem do tipo TEXTO
        else {
          enviarMensagem("TEXTO", entrada);
        }
      }
    }
    
    // Método para enviar mensagem customizada programaticamente
    void enviarMensagemCustomizada(String tipo, String conteudo) {
      enviarMensagem(tipo, conteudo);
    }
};

#endif