#include <Wire.h>

#define I2C_ADDR  0x42   // indirizzo dello slave
#define REG_ADDR  0x20   // registro da aggiornare

void setup() {
  Serial.begin(115200);

  // Inizializza I2C come master a 400kHz (SDA=21, SCL=22 su ESP32)
  Wire.begin(21, 22, 400000);  

  Serial.println("ESP32 I2C Master pronto");
  Serial.println("Scrivi un numero intero e premi INVIO:");
}

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input.length() > 0) {
      uint16_t value = input.toInt();  // usa uint16_t direttamente
      if (value > 65535) {
        Serial.println("⚠️ Inserisci un valore tra 0 e 65535");
        return;
      }

      // --- CONVERTI IL VALORE IN BYTE ---
      uint8_t data[2];
      memcpy(data, &value, sizeof(uint16_t)); // copia i byte in memoria

      // --- SCRITTURA I2C ---
      Wire.beginTransmission(I2C_ADDR);
      Wire.write(REG_ADDR);      
      Wire.write(data, 2);      // invia i 2 byte
      byte error = Wire.endTransmission();

      if (error == 0) {
        Serial.print("✅ Valore scritto: ");
        Serial.println(value);
      } else {
        Serial.print("❌ Errore di scrittura: ");
        Serial.println(error);
        return;
      }

      delay(50);

      // --- LETTURA I2C ---
      Wire.beginTransmission(I2C_ADDR);
      Wire.write(REG_ADDR);
      error = Wire.endTransmission(false); // repeated start

      if (error == 0) {
        Wire.requestFrom(I2C_ADDR, (uint8_t)2);
        if (Wire.available() == 2) {
          uint8_t recvData[2];
          recvData[0] = Wire.read();
          recvData[1] = Wire.read();

          uint16_t readValue;
          memcpy(&readValue, recvData, sizeof(uint16_t)); // ricostruisci il valore

          Serial.print("📖 Valore letto dal registro: ");
          Serial.println(readValue);
        } else {
          Serial.println("⚠️ Lettura fallita (dati insufficienti)");
        }
      } else {
        Serial.print("❌ Errore di lettura: ");
        Serial.println(error);
      }

      Serial.println("Scrivi un nuovo numero e premi INVIO:");
    }
  }
}
