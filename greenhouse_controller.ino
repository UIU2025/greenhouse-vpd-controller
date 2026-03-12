#include <DHT.h>
#include <EEPROM.h>

// ----- Pin Definitions -----
#define DHTPIN_IN    2      // Inside DHT22 data
#define DHTPIN_OUT   10     // Outside DHT22 data
#define HEATER       3      // Gas burner relay
#define HUMIDIFIER   4      // Humidifier relay
#define INTAKE       8      // Intake fan relay
#define EXHAUST      9      // Exhaust fan relay
#define BUTTON       7      // Reset button (to GND)
#define LDR_PIN      A0     // Light sensor

#define DHTTYPE DHT22
DHT dht_in(DHTPIN_IN, DHTTYPE);
DHT dht_out(DHTPIN_OUT, DHTTYPE);

// ----- Safety & Target Limits -----
const float H_MIN = 40.0;        // Minimum safe humidity (%)
const float H_MAX = 80.0;        // Maximum safe humidity (%)

// Temperature limits (°C)
const float T_MIN_DAY   = 20.0;
const float T_MAX_DAY   = 26.0;
const float T_MIN_NIGHT = 16.0;
const float T_MAX_NIGHT = 18.0;

// VPD ranges (kPa)
const float VPD_DAY_START_LOW  = 0.6;
const float VPD_DAY_START_HIGH = 1.0;
const float VPD_DAY_END_LOW    = 1.0;
const float VPD_DAY_END_HIGH   = 1.4;
const float VPD_NIGHT_LOW      = 0.4;
const float VPD_NIGHT_HIGH     = 0.8;

// LDR threshold (adjust after testing)
const int LDR_THRESHOLD = 500;   // Above = day, below = night

// Safe outside temperature range for ventilation (°C)
const float OUT_TEMP_MIN = 10.0;
const float OUT_TEMP_MAX = 30.0;

// EEPROM address for day counter
const int DAY_ADDR = 0;

// ----- Global Variables -----
unsigned long dayCount = 0;
unsigned long lastDayMillis = 0;
int lastButtonState = HIGH;

void setup() {
  pinMode(HEATER, OUTPUT);
  pinMode(HUMIDIFIER, OUTPUT);
  pinMode(INTAKE, OUTPUT);
  pinMode(EXHAUST, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);

  // All relays start OFF
  digitalWrite(HEATER, LOW);
  digitalWrite(HUMIDIFIER, LOW);
  digitalWrite(INTAKE, LOW);
  digitalWrite(EXHAUST, LOW);

  dht_in.begin();
  dht_out.begin();

  // Read day count from EEPROM
  dayCount = EEPROM.read(DAY_ADDR);
  if (dayCount > 365) dayCount = 0;   // Sanity check for first boot

  lastDayMillis = millis();
}

void loop() {
  float inTemp = dht_in.readTemperature();
  float inHum  = dht_in.readHumidity();
  float outTemp = dht_out.readTemperature();
  float outHum  = dht_out.readHumidity();

  // If inside sensor fails, do nothing (safety)
  if (isnan(inTemp) || isnan(inHum)) return;

  // ----- Day/Night Detection -----
  bool isDay = analogRead(LDR_PIN) > LDR_THRESHOLD;

  // ----- Reset Button (debounced) -----
  int buttonState = digitalRead(BUTTON);
  if (buttonState == LOW && lastButtonState == HIGH) {
    delay(50);                         // simple debounce
    if (digitalRead(BUTTON) == LOW) {
      dayCount = 0;
      EEPROM.write(DAY_ADDR, 0);       // save reset to EEPROM
      lastDayMillis = millis();         // restart day counter
    }
  }
  lastButtonState = buttonState;

  // ----- Advance Day Counter (every 24h) -----
  if (millis() - lastDayMillis >= 86400000UL) {
    dayCount++;
    EEPROM.write(DAY_ADDR, dayCount);  // save new count
    lastDayMillis += 86400000UL;
  }

  // ----- Determine Target VPD Range & Temp Limits -----
  float vpdLow, vpdHigh, tMin, tMax;
  if (isDay) {
    float progress = (dayCount < 60) ? (float)dayCount / 60.0 : 1.0;
    vpdLow  = VPD_DAY_START_LOW  + (VPD_DAY_END_LOW  - VPD_DAY_START_LOW)  * progress;
    vpdHigh = VPD_DAY_START_HIGH + (VPD_DAY_END_HIGH - VPD_DAY_START_HIGH) * progress;
    tMin = T_MIN_DAY;
    tMax = T_MAX_DAY;
  } else {
    vpdLow  = VPD_NIGHT_LOW;
    vpdHigh = VPD_NIGHT_HIGH;
    tMin = T_MIN_NIGHT;
    tMax = T_MAX_NIGHT;
  }

  // ----- Calculate Inside VPD -----
  float es_in = 0.6108 * exp(17.27 * inTemp / (inTemp + 237.3));
  float ea_in = es_in * inHum / 100.0;
  float vpd_in = es_in - ea_in;

  // ----- Smart Ventilation (continuous when beneficial) -----
  bool shouldVent = false;
  if (!isnan(outTemp) && !isnan(outHum) && outTemp >= OUT_TEMP_MIN && outTemp <= OUT_TEMP_MAX) {
    float es_out = 0.6108 * exp(17.27 * outTemp / (outTemp + 237.3));
    float ea_out = es_out * outHum / 100.0;
    float vpd_out = es_out - ea_out;

    // Ventilate if outside air helps VPD or temperature
    if ((vpd_in < vpdLow && vpd_out > vpd_in) ||
        (vpd_in > vpdHigh && vpd_out < vpd_in) ||
        (inTemp > tMax && outTemp < inTemp) ||
        (inTemp < tMin && outTemp > inTemp)) {
      shouldVent = true;
    }
  }

  digitalWrite(INTAKE, shouldVent ? HIGH : LOW);
  digitalWrite(EXHAUST, shouldVent ? HIGH : LOW);

  // ----- Heater & Humidifier Control -----
  // Heater turns on if:
  //   - VPD too low (need to raise VPD) AND temperature below max limit, OR
  //   - Temperature below minimum (safety override)
  bool needHeater = (vpd_in < vpdLow && inTemp < tMax) || (inTemp < tMin);

  // Humidifier turns on if VPD too high (need to lower VPD) AND humidity below max limit
  bool needHumidifier = (vpd_in > vpdHigh && inHum < H_MAX);

  digitalWrite(HEATER, needHeater ? HIGH : LOW);
  digitalWrite(HUMIDIFIER, needHumidifier ? HIGH : LOW);

  delay(2000);   // Wait 2 seconds before next reading
}
