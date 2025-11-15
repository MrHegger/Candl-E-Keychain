#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "media.h"
#include "driver/adc.h"
#include "esp_sleep.h"

// ------------------- Config -------------------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1

#define IGNITION_TIME 200
#define EXTINGUISH_TIME 3000
#define IDLE_TIMEOUT 20000

#define WAKE_BUTTON_PIN D1       // <-- D9 (BOOT button)
#define BUTTON_PIN_BITMASK (1ULL << WAKE_BUTTON_PIN)

const float MIN_SCALE = 0.2;
const float MAX_SCALE = 1.0;
const unsigned long FRAME_INTERVAL = 100;

// ------------------- Globals -------------------
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

float currentScale = 0.0;
float batteryScale = 1.0;
int currentFrame = 0;
unsigned long lastFrameTime = 0;
unsigned long flameOnTime = 0;
bool flameActive = false;

RTC_DATA_ATTR int bootCount = 0;


// ------------------- Easing -------------------
float easeInOutSine(float t) {
  return -0.5 * (cos(PI * t) - 1);
}


// ------------------- Battery Read -------------------
float readBatteryVoltage() {
  uint32_t sum = 0;
  for (int i = 0; i < 16; i++) {
    sum += analogReadMilliVolts(A0);
  }
  // attenuation 1/2 → measured voltage * 2
  return (2.0f * (sum / 16)) / 1000.0f;
}

float batteryToScale(float vbatt) {
  // Adjust these thresholds to match your battery
  const float VBATT_MIN = 3.3;
  const float VBATT_MAX = 4.2;

  float t = (vbatt - VBATT_MIN) / (VBATT_MAX - VBATT_MIN);
  t = constrain(t, 0.0, 1.0);

  return MIN_SCALE + t * (MAX_SCALE - MIN_SCALE);
}


// ------------------- Bitmap Draw -------------------
void drawBitmapFrameScaled(const unsigned char* bitmap, float scale) {
  display.clearDisplay();
  for (int y = 0; y < SCREEN_HEIGHT; y++) {
    for (int x = 0; x < SCREEN_WIDTH; x++) {
      uint8_t byteValue = pgm_read_byte(bitmap + (y * (SCREEN_WIDTH / 8)) + (x / 8));
      bool pixelOn = byteValue & (0x80 >> (x & 7));
      if (pixelOn) {
        int scaledX = int(x * scale);
        if (scaledX >= 0 && scaledX < SCREEN_WIDTH) {
          display.drawPixel(scaledX, y, WHITE);
        }
      }
    }
  }
  display.display();
}


// ------------------- Animation -------------------
void animateScale(float fromScale, float toScale, unsigned long duration) {
  unsigned long startTime = millis();
  while (true) {
    unsigned long now = millis();
    float t = float(now - startTime) / float(duration);
    if (t > 1.0) t = 1.0;

    float easedT = easeInOutSine(t);
    currentScale = fromScale + (toScale - fromScale) * easedT;

    if (now - lastFrameTime >= FRAME_INTERVAL) {
      currentFrame = (currentFrame + 1) % epd_bitmap_allArray_LEN;
      lastFrameTime = now;
    }

    drawBitmapFrameScaled(epd_bitmap_allArray[currentFrame], currentScale);

    if (t >= 1.0) break;
    delay(10);
  }
}


// ------------------- Flame Control -------------------
void turnOffFlame() {
  flameActive = false;
  currentScale = 0;
  display.clearDisplay();
  display.ssd1306_command(SSD1306_DISPLAYOFF);
}

void igniteFlame() {
  display.ssd1306_command(SSD1306_DISPLAYON);

  float vbatt = readBatteryVoltage();
  batteryScale = batteryToScale(vbatt);

  animateScale(0.0, MAX_SCALE, IGNITION_TIME);
  animateScale(MAX_SCALE, batteryScale, 300);

  flameOnTime = millis();
  flameActive = true;
}


// ------------------- Deep Sleep -------------------
void goDeepSleep() {
  display.ssd1306_command(SSD1306_DISPLAYOFF);

  Serial.println("Going into deep sleep...");
  delay(100);

  esp_deep_sleep_enable_gpio_wakeup((1ULL << WAKE_BUTTON_PIN), ESP_GPIO_WAKEUP_GPIO_LOW); // wake on LOW

  esp_deep_sleep_start();
}


// ------------------- Wake Reason Debug -------------------
void printWakeReason() {
  esp_sleep_wakeup_cause_t reason = esp_sleep_get_wakeup_cause();

  switch (reason) {
    case ESP_SLEEP_WAKEUP_EXT0: Serial.println("Wakeup: EXT0 (button)"); break;
    case ESP_SLEEP_WAKEUP_EXT1: Serial.println("Wakeup: EXT1"); break;
    case ESP_SLEEP_WAKEUP_TIMER: Serial.println("Wakeup: timer"); break;
    default: Serial.printf("Wakeup from reset or unknown: %d\n", reason); break;
  }
}


// ------------------- Setup -------------------
void setup() {
  Serial.begin(115200);
  delay(500);

  ++bootCount;
  Serial.println("Boot count: " + String(bootCount));
  printWakeReason();

  pinMode(WAKE_BUTTON_PIN, INPUT_PULLUP);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 init failed!");
    for (;;);
  }

  display.clearDisplay();
  display.display();

  igniteFlame();
}


// ------------------- Loop -------------------
void loop() {
  if (!flameActive) return;

  unsigned long now = millis();

  if (now - lastFrameTime >= FRAME_INTERVAL) {
    currentFrame = (currentFrame + 1) % epd_bitmap_allArray_LEN;
    lastFrameTime = now;
    drawBitmapFrameScaled(epd_bitmap_allArray[currentFrame], currentScale);
  }

  if (now - flameOnTime > IDLE_TIMEOUT) {
    animateScale(currentScale, 0.0, EXTINGUISH_TIME);
    turnOffFlame();
    goDeepSleep();
  }
}
