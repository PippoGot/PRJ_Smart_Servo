#include <Wire.h>

void setup() {
  Wire.begin(-1, -1, 400000);   // Arduino come master
  Serial.begin(115200);
  delay(2000);
  Serial.println("=== Scansione I2C in corso... ===");
}

void loop() {
  byte error, address;
  int nDevices = 0;

  for(address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("Trovato dispositivo I2C all'indirizzo 0x");
      if (address < 16)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.println(" !");
      nDevices++;
    }
    else if (error == 4) {
      Serial.print("Errore sconosciuto all'indirizzo 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }
  }

  if (nDevices == 0)
    Serial.println("Nessun dispositivo I2C trovato");
  else
    Serial.println("Scansione completata");

  delay(3000); // attende 3s prima di ripetere
}
