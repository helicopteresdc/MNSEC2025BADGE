#include <Adafruit_NeoPixel.h>
#define PIN 6
#define NUMPIXELS 4
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
#define DELAYVAL 500
int counter = 0;
void setup() {
  pixels.begin();
}

void loop() {
  counter++;
  // pixels.clear();
  if (counter % 4 == 0) {
    pixels.fill(pixels.Color(0, 150, 0), 0, 4);
  }
  if (counter % 4 == 1) {
    pixels.fill(pixels.Color(150, 0, 0), 0, 4);
  }
  if (counter % 4 == 2) {
    pixels.fill(pixels.Color(0, 0, 0), 0, 4);
  }

  if (counter % 4 == 3) {
    pixels.clear();
  }
  pixels.show();
  delay(DELAYVAL);
}