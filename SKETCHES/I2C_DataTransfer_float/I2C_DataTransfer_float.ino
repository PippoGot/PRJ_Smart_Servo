#include <Wire.h>

#define I2C_ADDR  0x42   // indirizzo dello slave
#define REG_ADDR  0x03   // registro da aggiornare

void setup() {
  Serial.begin(115200);

  // Inizializza I2C come master a 400kHz (SDA=21, SCL=22 su ESP32)
  Wire.begin(21, 22, 400000);  

  Serial.println("ESP32 I2C Master pronto");
  Serial.println("Scrivi un numero float e premi INVIO:");
}

void loop() {
  // Se ci sono dati disponibili sul terminale
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n'); // legge fino a invio
    input.trim(); // rimuove spazi o ritorni a capo

    if (input.length() > 0) {
      float value = input.toFloat();  // converte in float

      // --- CONVERTI FLOAT IN BYTE ---
      uint8_t data[4];
      memcpy(data, &value, sizeof(float));

      // --- SCRITTURA I2C ---
      Wire.beginTransmission(I2C_ADDR);
      Wire.write(REG_ADDR);     // indirizzo del registro
      Wire.write(data, 4);      // invia 4 byte del float
      byte error = Wire.endTransmission();

      if (error == 0) {
        Serial.print("✅ Float scritto: ");
        Serial.println(value, 6);
      } else {
        Serial.print("❌ Errore di scrittura: ");
        Serial.println(error);
        return;
      }

      delay(50); // piccola pausa

      // --- LETTURA I2C ---
      Wire.beginTransmission(I2C_ADDR);
      Wire.write(REG_ADDR);
      error = Wire.endTransmission(false); // repeated start

      if (error == 0) {
        Wire.requestFrom(I2C_ADDR, (uint8_t)4); // chiedi 4 byte
        if (Wire.available() == 4) {
          uint8_t recvData[4];
          for (int i = 0; i < 4; i++) {
            recvData[i] = Wire.read();
          }

          float readValue;
          memcpy(&readValue, recvData, sizeof(float)); // ricostruisci float

          Serial.print("📖 Float letto dal registro: ");
          Serial.println(readValue, 6);
        } else {
          Serial.println("⚠️ Lettura fallita (dati insufficienti)");
        }
      } else {
        Serial.print("❌ Errore di lettura: ");
        Serial.println(error);
      }

      Serial.println("Scrivi un nuovo numero float e premi INVIO:");
    }
  }
}
