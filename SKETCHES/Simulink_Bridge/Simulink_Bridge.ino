#include <Wire.h>

const int device1 = 0x50;
const int device2 = 0x51;
const int device3 = 0x52;

const uint8_t REG_WRITE = 0x20; // registro per scrittura
const uint8_t REG_READ  = 0x21; // registro per lettura

int32_t valuesToI2C[3];
int32_t valuesFromI2C[3];

unsigned long lastTime = 0;
const unsigned long period = 10; // 10ms

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(400000);
}

void writeI2C(uint8_t addr, uint8_t reg, int32_t value) {
  uint8_t buffer[4];
  memcpy(buffer, &value, sizeof(value));

  Wire.beginTransmission(addr);
  Wire.write(reg);        // registro
  Wire.write(buffer, 4);  // dati
  Wire.endTransmission();

  delay(1);
}

int32_t readI2C(uint8_t addr, uint8_t reg) {
  uint8_t buffer[4] = {0};
  
  Wire.beginTransmission(addr);
  Wire.write(reg);              // registro da leggere
  byte error = Wire.endTransmission(false);  // restart senza rilascio del bus

  if (error != 0) return 0;
  
  Wire.requestFrom(addr, (uint8_t)4);
  for (int i = 0; i < 4; i++) {
    buffer[i] = Wire.read();
  }

  int32_t value = 0;
  memcpy(&value, buffer, sizeof(value));
  
  return value;
}

void loop() {
  const int devices[3] = {device1, device2, device3};
  static int currentDevice = 0;

  if(millis() - lastTime >= period) {
    lastTime = millis();
      
    if (Serial.available() >= 12) {
      // 1. Lettura valori da Simulink
      for (int i = 0; i < 3; i++) {
        uint8_t b[4];
        for (int j = 0; j < 4; j++) {
          b[j] = Serial.read();
        }
        memcpy(&valuesToI2C[i], b, 4);
      }

  
      for (int i = 0; i < 3; i++) {
        if (i == 1) continue;
        writeI2C(devices[i], REG_WRITE, valuesToI2C[i]);
        valuesFromI2C[i] = readI2C(devices[i], REG_READ);
      }
  
/*
      // DEBUG
      for (int i = 0; i < 3; i++) {
        valuesFromI2C[i] = valuesToI2C[i];
      }
*/
          
      // 4. Invio dati a Simulink
      Serial.write('A'); // header
      for (int i = 0; i < 3; i++) {
        uint8_t b[4];
        valuesFromI2C[1] = 0;
        memcpy(b, &valuesFromI2C[i], 4);
        Serial.write(b, 4);
      }
      Serial.print('\n');
    }
  }
}