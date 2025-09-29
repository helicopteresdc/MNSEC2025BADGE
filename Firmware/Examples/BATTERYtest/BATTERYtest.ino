#include <Arduino.h>

#define ADC_PIN 0   // Use 0..4 for ADC1 on ESP32-C3 (2 = ADC1_CH2)
#define SAMPLE_PERIOD_MS 200
#define ADC_BITS 12         // 12-bit resolution (0..4095)
#define ADC_ATTEN ADC_11db  // ~0..3.3V full-scale (use ADC_6db for ~0..2.2V)
int BatteryVal;
void setup() {
  Serial.begin(115200);
  analogReadResolution(ADC_BITS);
  analogSetPinAttenuation(ADC_PIN, ADC_ATTEN);  // pick ADC_6db if your signal <~2.2V
}

void loop() {
  BatteryVal = analogRead(ADC_PIN);
  Serial.println(BatteryVal);
  delay(1000);
}
