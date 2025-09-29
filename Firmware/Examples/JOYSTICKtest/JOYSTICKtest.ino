#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <Preferences.h>

#define TFT_CS 7
#define TFT_RST 6
#define TFT_DC 4
#define TFT_MOSI 8
#define TFT_SCLK 9
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

Preferences preferences;

#define ADC_PINBUT 1
#define ADC_PINJOY 3
#define ADC_BITS 12
#define ADC_ATTEN ADC_11db
#define SAMPLE_PERIOD_MS 200

// Recalibration hold window at boot (ms)
#define RECAL_HOLD_MS 2000

// Direction match tolerance (ADC counts)
#define TOL 140

int JoystickVal;
int ButtonVal;

// Calibrated ladder values (defaults = "unset")
uint16_t aVal = 4096;
uint16_t bVal = 4096;
uint16_t upVal = 4096;
uint16_t downVal = 4096;
uint16_t rightVal = 4096;
uint16_t leftVal = 4096;
uint16_t switchVal = 4096;

int joyState = 0;

// -------------------- NVS helpers --------------------
void SetJoystick() {
  if (preferences.begin("my_app", false)) {
    preferences.putUInt("upVal", upVal);
    preferences.putUInt("downVal", downVal);
    preferences.putUInt("rightVal", rightVal);
    preferences.putUInt("leftVal", leftVal);
    preferences.putUInt("switchVal", switchVal);
    preferences.putUInt("aVal", aVal);
    preferences.putUInt("bVal", bVal);
    preferences.end();
    Serial.println("Joystick values saved.");
  } else {
    Serial.println("Failed to open NVS for writing.");
  }
}

void GetJoystick() {
  if (preferences.begin("my_app", true)) {
    upVal     = preferences.getUInt("upVal", 4096);
    downVal   = preferences.getUInt("downVal", 4096);
    rightVal  = preferences.getUInt("rightVal", 4096);
    leftVal   = preferences.getUInt("leftVal", 4096);
    switchVal = preferences.getUInt("switchVal", 4096);
    aVal      = preferences.getUInt("aVal", 4096);
    bVal      = preferences.getUInt("bVal", 4096);
    preferences.end();

    Serial.println("Loaded from NVS:");
    Serial.printf("up=%u down=%u right=%u left=%u switch=%u A=%u B=%u\n",
                  upVal, downVal, rightVal, leftVal, switchVal, aVal, bVal);
  } else {
    Serial.println("Failed to open NVS for reading.");
  }
}

bool valuesUnset() {
  return (upVal == 4096 || downVal == 4096 || rightVal == 4096 ||
          leftVal == 4096 || switchVal == 4096 || aVal == 4096 || bVal == 4096);
}

static inline bool within(uint16_t v, uint16_t c, uint16_t tol) {
  return (v >= (c > tol ? c - tol : 0)) && (v <= (c + tol));
}

// -------------------- UI helpers --------------------
void title2(const char* line1, const char* line2) {
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.setCursor(10, 30); tft.print(line1);
  tft.setCursor(10, 50); tft.print(line2);
}

void showCapture(const char* label, uint16_t val) {
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.setCursor(10, 70);
  tft.print(label);
  tft.print(" value: ");
  tft.print(val);
}

// -------------------- Calibration (runs ONLY in setup) --------------------
void runCalibrationWizardInSetup() {
  // Start with your initial screen
  title2("Joystick: Up", "Calibrating");
  joyState = 1;

  // Calibration loop runs INSIDE setup; blocks until joyState reaches 9
  while (joyState != 9) {
    JoystickVal = analogRead(ADC_PINJOY);
    ButtonVal   = analogRead(ADC_PINBUT);

    switch (joyState) {
      case 1: // UP
        if (JoystickVal < 4095 && upVal >= JoystickVal) {
          upVal = JoystickVal;
          showCapture("Up", upVal);
          Serial.println(upVal);
          joyState = 2;
          delay(800);
          title2("Joystick: Down", "Calibrating");
        }
        break;

      case 2: // DOWN
        if (JoystickVal < 4095 && downVal >= JoystickVal) {
          downVal = JoystickVal;
          showCapture("Down", downVal);
          Serial.println(downVal);
          joyState = 3;
          delay(800);
          title2("Joystick: Right", "Calibrating");
        }
        break;

      case 3: // RIGHT
        if (JoystickVal < 4095 && rightVal >= JoystickVal) {
          rightVal = JoystickVal;
          showCapture("Right", rightVal);
          Serial.println(rightVal);
          joyState = 4;
          delay(800);
          title2("Joystick: Left", "Calibrating");
        }
        break;

      case 4: // LEFT
        if (JoystickVal < 4095 && leftVal >= JoystickVal) {
          leftVal = JoystickVal;
          showCapture("Left", leftVal);
          Serial.println(leftVal);
          joyState = 5;
          delay(800);
          title2("Joystick: Switch", "Calibrating");
        }
        break;

      case 5: // SWITCH (press)
        if (JoystickVal < 4095 && switchVal >= JoystickVal) {
          switchVal = JoystickVal;
          showCapture("Switch", switchVal);
          Serial.println(switchVal);
          joyState = 6;
          delay(800);
          title2("Button: A", "Calibrating");
        }
        break;

      case 6: // A (from ADC_PINBUT ladder)
        if (ButtonVal < 4095 && aVal >= ButtonVal) {
          aVal = ButtonVal;
          showCapture("A", aVal);
          Serial.println(aVal);
          joyState = 7;
          delay(800);
          title2("Button: B", "Calibrating");
        }
        break;

      case 7: // B (from ADC_PINBUT ladder)
        if (ButtonVal < 4095 && bVal >= ButtonVal) {
          bVal = ButtonVal;
          showCapture("B", bVal);
          Serial.println(bVal);
          joyState = 8;
          delay(600);
          title2("Joystick Values", "Saving...");
        }
        break;

      case 8:
        SetJoystick();
        joyState = 9;
        title2("Calibration", "Complete");
        delay(800);
        break;
    }

    delay(10); // small yield
  }
}

// -------------------- Setup / Loop --------------------
void setup() {
  Serial.begin(115200);
  delay(100);

  analogReadResolution(ADC_BITS);
  analogSetPinAttenuation(ADC_PINJOY, ADC_ATTEN);
  analogSetPinAttenuation(ADC_PINBUT, ADC_ATTEN);

  tft.initR(INITR_GREENTAB);
  tft.setRotation(3);
  tft.fillScreen(ST77XX_BLACK);

  // Read any existing values
  // GetJoystick();

  // Force recal if joystick kept pressed at boot for ~2s
  bool forceRecal = false;
  uint32_t t0 = millis();
  while (millis() - t0 < RECAL_HOLD_MS) {
    int v = analogRead(ADC_PINJOY);
    // crude range for "press" on many 5-way ladders (tune if needed)
    if (v > 1800 && v < 2600) { forceRecal = true; break; }
    delay(10);
  }

  if (valuesUnset() || forceRecal) {
    runCalibrationWizardInSetup();  // <— calibration runs entirely in setup
  } else {
    title2("Calibration", "Skipped (NVS)");
    delay(600);
  }

  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(10, 30);
  tft.setTextColor(ST77XX_WHITE);
  tft.print("Ready");
}

void loop() {
  JoystickVal = analogRead(ADC_PINJOY);
  ButtonVal   = analogRead(ADC_PINBUT);

  // Example: classify based on calibrated peaks + tolerance
  if (within(JoystickVal, switchVal, TOL)) {
    Serial.println("switch");
  } else if (within(JoystickVal, leftVal, TOL)) {
    Serial.println("left");
  } else if (within(JoystickVal, rightVal, TOL)) {
    Serial.println("right");
  } else if (within(JoystickVal, upVal, TOL)) {
    Serial.println("up");
  } else if (within(JoystickVal, downVal, TOL)) {
    Serial.println("down");
  }

  // Your A/B button ladder on ADC_PINBUT
  if (within(ButtonVal, aVal, TOL)) {
    Serial.println("a");
  } else if (within(ButtonVal, bVal, TOL)) {
    Serial.println("b");
  }

  delay(SAMPLE_PERIOD_MS);
}
