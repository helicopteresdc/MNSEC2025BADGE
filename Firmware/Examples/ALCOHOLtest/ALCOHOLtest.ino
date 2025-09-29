#include <Arduino.h>
#include <Adafruit_GFX.h>     // Core graphics library
#include <Adafruit_ST7735.h>  // Hardware-specific library for ST7735
#include <SPI.h>
#define TFT_CS 7
#define TFT_RST 6
#define TFT_DC 4
#define TFT_MOSI 8
#define TFT_SCLK 9

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);


#define ADC_PINBUT 1  // Use 0..4 for ADC1 on ESP32-C3 (2 = ADC1_CH2)
#define SAMPLE_PERIOD_MS 200
#define ADC_BITS 12         // 12-bit resolution (0..4095)
#define ADC_ATTEN ADC_11db  // ~0..3.3V full-scale (use ADC_6db for ~0..2.2V)

#define ADC_PIN 2  // <-- Using GPIO2
#define SAMPLES 16  // Average samples to reduce noise
#define PRINT_PERIODMS 200

const float VOLTAGE_DIVIDER_RATIO = 1.0f;
int rawAlc = 0;
int prevrawAlc = 0;
int countState = 0;
int ButtonVal = 0;
void setup() {
  Serial.begin(115200);
  while (!Serial) { /* Wait for USB Serial */
  }
  analogReadResolution(12);  // 0..4095
  analogSetPinAttenuation(ADC_PIN, ADC_11db);
  analogSetPinAttenuation(ADC_PINBUT, ADC_ATTEN);  // pick ADC_6db if your signal <~2.2V
  tft.initR(INITR_GREENTAB);                       // Init ST7735S chip, green tab
  tft.setRotation(3);
  Serial.println(F("Initialized"));
  tft.fillScreen(ST77XX_BLACK);
}

uint16_t readAveragedRaw(uint8_t pin, uint8_t nSamples) {
  uint32_t acc = 0;
  for (uint8_t i = 0; i < nSamples; i++) {
    acc += analogRead(pin);
    delayMicroseconds(200);
  }
  return (uint16_t)(acc / nSamples);
}

unsigned long lastPrint = 0;

void loop() {
  ButtonVal = analogRead(ADC_PINBUT);
  if (ButtonVal < 100) {
    countState = 1;
  }

  if (countState == 1) {
    Serial.println("a");
    tft.fillScreen(ST77XX_BLACK);
    tft.setTextColor(ST77XX_GREEN);
    tft.setCursor(0, 20);
    tft.setTextSize(2);
    tft.println("BUSINESS CARD");
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(1);
    tft.setCursor(0, 40);
    tft.println("Name: Nisdegtereg");
    tft.println("");
    tft.println("Company: Beelog Tech");
    tft.println("");
    tft.println("Position: CEO");
    tft.println("");
    tft.println("Mobile: 99782920");
    delay(5000);
    tft.fillScreen(ST77XX_BLACK);
    countState = 0;
  } else {
    if (millis() - lastPrint >= PRINT_PERIODMS) {
      lastPrint = millis();
      int16_t raw = readAveragedRaw(ADC_PIN, SAMPLES);
      Serial.print(F("IN:"));
      Serial.print(raw);
      raw = raw - 2000;
      if (raw < 0) {
        rawAlc = 0;
      }
      if (0 < raw && raw <= 150) {
        rawAlc = 1;
      }
      if (150 < raw && raw <= 300) {
        rawAlc = 2;
      }
      if (300 < raw && raw <= 450) {
        rawAlc = 3;
      }
      if (450 < raw && raw <= 600) {
        rawAlc = 4;
      }

      if (600 < raw && raw <= 750) {
        rawAlc = 5;
      }
      if (750 < raw && raw <= 900) {
        rawAlc = 6;
      }

      if (900 < raw && raw <= 1050) {
        rawAlc = 7;
      }
      if (1050 < raw) {
        rawAlc = 8;
      }

      Serial.print(F(" RAW:"));
      Serial.println(raw);
    }
    if (rawAlc == prevrawAlc) {
      tft.setTextColor(ST77XX_WHITE);
      tft.setTextSize(1);
      tft.setCursor(40, 10);
      tft.println("Alcohol sensor");
      tft.setCursor(70, 106);
      tft.println("beer");
      tft.setTextSize(6);
      tft.setCursor(66, 42);
      tft.setTextColor(ST77XX_GREEN);
      tft.println(rawAlc);
    } else {
      tft.fillScreen(ST77XX_BLACK);
      prevrawAlc = rawAlc;
    }
  }
}
