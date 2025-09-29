#include <Adafruit_GFX.h>     // Core graphics library
#include <Adafruit_ST7735.h>  // Hardware-specific library for ST7735
#include "fonts.h"
#include <SPI.h>
#define TFT_CS 7
#define TFT_RST 6
#define TFT_DC 4
#define TFT_MOSI 8
#define TFT_SCLK 9


Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

float p = 3.1415926;

void setup(void) {
  Serial.begin(115200);
  Serial.print(F("Hello! ST77xx TFT Test"));
  tft.initR(INITR_GREENTAB);  // Init ST7735S chip, green tab
  tft.setRotation(3);
  Serial.println(F("Initialized"));
  tftPrintTest();
  tft.drawRoundRect(6, 6, 150, 26, 2, ST77XX_WHITE);
  delay(1000);
}

void loop() {
}


void testdrawtext(char *text, uint16_t color) {
  tft.setCursor(0, 0);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
}


void tftPrintTest() {
  tft.setCursor(0, 10);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_MAGENTA);
  tft.setTextSize(2);
  tft.println(" 1. Sponsor");
  tft.println("");
  tft.println(" 2. Sensor");
  tft.println("");
  tft.println(" 3. Game");
  tft.println("");
  tft.println(" 4. Namecard");
}
