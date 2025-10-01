#include <Wire.h>

// Indirizzo di default dello slave (può essere sovrascritto da ogni comando)
#define DEFAULT_I2C_ADDR 0x42

void setup() {
  Serial.begin(115200);

  // Inizializza I2C come master a 400kHz (SDA=20, SCL=21 su ATmega2560)
  Wire.begin();
  Wire.setClock(400000);

  Serial.println("ATmega2560 I2C Master pronto");
  Serial.println("Formato input: ADDR OPER REG TIPO [VALORE]");
  Serial.println("Esempi:");
  Serial.println("  0x42 r 0x20 u16");
  Serial.println("  0x42 w 0x21 u8 200");
  Serial.println("  0x42 w 0x30 f 3.1416");
  Serial.println("  0x42 w 0x40 i32 -12345678");
}

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input.length() > 0) {
      // --- PARSING ---
      int firstSpace = input.indexOf(' ');
      int secondSpace = input.indexOf(' ', firstSpace + 1);
      int thirdSpace = input.indexOf(' ', secondSpace + 1);
      int fourthSpace = input.indexOf(' ', thirdSpace + 1);

      if (firstSpace < 0 || secondSpace < 0) {
        Serial.println("⚠️ Formato non valido. Usa: ADDR OPER REG TIPO [VALORE]");
        return;
      }

      String addrStr = input.substring(0, firstSpace);
      String operStr = input.substring(firstSpace + 1, secondSpace);
      String regStr = input.substring(secondSpace + 1, thirdSpace);
      String typeStr;
      String valStr;

      if (fourthSpace > 0) {
        typeStr = input.substring(thirdSpace + 1, fourthSpace);
        valStr = input.substring(fourthSpace + 1);
      } else {
        typeStr = input.substring(thirdSpace + 1);
      }

      // --- Indirizzo slave ---
      uint8_t i2cAddr = (uint8_t) strtol(addrStr.c_str(), NULL, 0);

      // --- Registro ---
      uint8_t regAddr = (uint8_t) strtol(regStr.c_str(), NULL, 0);

      // --- Determina la dimensione del dato ---
      uint8_t buffer[4];  // max 4 byte (float / i32)
      size_t dataLen = 0;

      if (typeStr.equalsIgnoreCase("u8")) {
        dataLen = 1;
      } 
      else if (typeStr.equalsIgnoreCase("u16")) {
        dataLen = 2;
      } 
      else if (typeStr.equalsIgnoreCase("i32")) {
        dataLen = 4;
      } 
      else if (typeStr.equalsIgnoreCase("f")) {
        dataLen = 4;
      } 
      else {
        Serial.println("⚠️ Tipo non valido. Usa: u8, u16, i32, f");
        return;
      }

      // --- SCRITTURA ---
      if (operStr.equalsIgnoreCase("w")) {
        if (valStr.length() == 0) {
          Serial.println("⚠️ Devi specificare un valore per la scrittura");
          return;
        }

        if (typeStr.equalsIgnoreCase("u8")) {
          uint8_t val = (uint8_t) valStr.toInt();
          memcpy(buffer, &val, sizeof(val));
        } 
        else if (typeStr.equalsIgnoreCase("u16")) {
          uint16_t val = (uint16_t) valStr.toInt();
          memcpy(buffer, &val, sizeof(val));
        } 
        else if (typeStr.equalsIgnoreCase("i32")) {
          int32_t val = (int32_t) valStr.toInt();
          memcpy(buffer, &val, sizeof(val));
        } 
        else if (typeStr.equalsIgnoreCase("f")) {
          float val = valStr.toFloat();
          memcpy(buffer, &val, sizeof(val));
        }

        Wire.beginTransmission(i2cAddr);
        Wire.write(regAddr);
        Wire.write(buffer, dataLen);
        byte error = Wire.endTransmission();

        if (error == 0) {
          Serial.print("✅ Scritto su registro 0x");
          Serial.print(regAddr, HEX);
          Serial.print(" -> ");
          Serial.println(valStr);
        } else {
          Serial.print("❌ Errore di scrittura: ");
          Serial.println(error);
          return;
        }

        delay(50);
      }

      // --- LETTURA ---
      Wire.beginTransmission(i2cAddr);
      byte error = Wire.endTransmission(false); // repeated start

      if (error == 0) {
        Wire.beginTransmission(i2cAddr);
        Wire.write(regAddr);
        error = Wire.endTransmission(false);
        if (error != 0) {
          Serial.print("❌ Errore setup lettura: ");
          Serial.println(error);
          return;
        }

        Wire.requestFrom(i2cAddr, (uint8_t)dataLen);
        if (Wire.available() == dataLen) {
          uint8_t recvData[4];
          for (size_t i = 0; i < dataLen; i++) {
            recvData[i] = Wire.read();
          }

          Serial.print("📖 Valore letto da registro 0x");
          Serial.print(regAddr, HEX);
          Serial.print(": ");

          if (typeStr.equalsIgnoreCase("u8")) {
            uint8_t v;
            memcpy(&v, recvData, sizeof(v));
            Serial.println(v);
          } 
          else if (typeStr.equalsIgnoreCase("u16")) {
            uint16_t v;
            memcpy(&v, recvData, sizeof(v));
            Serial.println(v);
          } 
          else if (typeStr.equalsIgnoreCase("i32")) {
            int32_t v;
            memcpy(&v, recvData, sizeof(v));
            Serial.println(v);
          } 
          else if (typeStr.equalsIgnoreCase("f")) {
            float v;
            memcpy(&v, recvData, sizeof(v));
            Serial.println(v, 6);
          }
        } else {
          Serial.println("⚠️ Lettura fallita (dati insufficienti)");
        }
      } else {
        Serial.print("❌ Errore di lettura: ");
        Serial.println(error);
      }

      Serial.println("Scrivi un nuovo comando:");
    }
  }
}
