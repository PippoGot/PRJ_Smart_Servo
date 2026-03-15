#include <Wire.h>
#include <math.h>

// --- Indirizzi I2C degli slave ---
#define I2C_ADDR_1  0x50
#define I2C_ADDR_2  0x51
#define I2C_ADDR_3  0x52
#define REG_WRITE   0x20

const unsigned long period = 10;  // 10 ms
unsigned long lastTime = 0;

// Parametri della sinusoide
const float freq = M_PI / 5.0;    // frequenza angolare
const float Ts = 0.01;            // 10 ms
const float amplitude = 4065.0 / 2.0;
const float offset = 4065.0 / 2.0;

float t = 0.0;

// --- Pin I2C hardware (per ATmega2560: SDA=20, SCL=21)
const uint8_t PIN_SDA = 20;
const uint8_t PIN_SCL = 21;

// 🔧 Funzione per reset hardware del bus I2C
void resetI2CBus() {
  Serial.println("🔄 Reset hardware I2C...");

  Wire.end();

  pinMode(PIN_SCL, OUTPUT);
  pinMode(PIN_SDA, INPUT_PULLUP);

  // 1. Genera fino a 9 clock su SCL per liberare SDA
  for (int i = 0; i < 9; i++) {
    if (digitalRead(PIN_SDA) == HIGH) break;
    digitalWrite(PIN_SCL, HIGH);
    delayMicroseconds(5);
    digitalWrite(PIN_SCL, LOW);
    delayMicroseconds(5);
  }

  // 2. Forza condizione di STOP
  pinMode(PIN_SDA, OUTPUT);
  digitalWrite(PIN_SDA, LOW);
  delayMicroseconds(5);
  digitalWrite(PIN_SCL, HIGH);
  delayMicroseconds(5);
  digitalWrite(PIN_SDA, HIGH);
  delayMicroseconds(5);

  // 3. Ripristina bus
  Wire.begin();
  Wire.setClock(400000);

  Serial.println("✅ Bus I2C resettato");
}

// --- Scrittura con timeout e reset hardware se necessario
bool writeI2CwithTimeout(uint8_t addr, uint8_t reg, uint16_t value) {
  uint8_t low = value & 0xFF;
  uint8_t high = (value >> 8) & 0xFF;

  unsigned long start = micros();

  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.write(low);
  Wire.write(high);
  byte error = Wire.endTransmission();

  if ((micros() - start > 1000UL) || error != 0) {
    Serial.print("⚠️ Errore/timeout I2C (codice ");
    Serial.print(error);
    Serial.print(") su device 0x");
    Serial.println(addr, HEX);

    resetI2CBus();  // reset fisico

    // Ritento una sola volta
    Wire.beginTransmission(addr);
    Wire.write(reg);
    Wire.write(low);
    Wire.write(high);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.println("✅ Retry riuscito");
      return true;
    } else {
      Serial.print("❌ Fallimento dopo retry su 0x");
      Serial.print(addr, HEX);
      Serial.print(" (errore ");
      Serial.print(error);
      Serial.println(")");
      return false;
    }
  }

  return true;
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(100000);
  Serial.println("✅ Generatore sinusoidale I2C per 3 dispositivi avviato");
}

void loop() {
  unsigned long now = millis();
  if (now - lastTime >= period) {
    lastTime = now;

    // Calcolo sinusoide
    float val_f = offset + amplitude * sin(freq * t);
    uint16_t val = (uint16_t) round(val_f);

    // Scrive su tutti i dispositivi
    bool ok1 = writeI2CwithTimeout(I2C_ADDR_1, REG_WRITE, val);
    bool ok2 = writeI2CwithTimeout(I2C_ADDR_2, REG_WRITE, val);
    bool ok3 = writeI2CwithTimeout(I2C_ADDR_3, REG_WRITE, val);

    if (ok1 && ok2 && ok3) {
      Serial.print("I2C -> 0x");
      Serial.print(I2C_ADDR_1, HEX);
      Serial.print(", 0x");
      Serial.print(I2C_ADDR_2, HEX);
      Serial.print(", 0x");
      Serial.print(I2C_ADDR_3, HEX);
      Serial.print(" [0x");
      Serial.print(REG_WRITE, HEX);
      Serial.print("] = ");
      Serial.println(val);
    }

    t += Ts;
  }
}
