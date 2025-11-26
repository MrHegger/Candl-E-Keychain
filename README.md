# Candl-E Keychain 🔥  
**ESP32 Digital Candle Animation**

Candl-E is a tiny digital flame that lives on your keychain. Tap the button and it springs to life, flickering and dancing before gently fading out when it gets sleepy.  
This project turns the XIAO ESP32-C3, a small display, and a LiPo battery into a cozy animated pocket flame with smart power behavior.

---

## ✨ Features

- 🔥 **Smooth flame animation** using frame cycling and sine-based easing  
- 🔋 **Flame size reflects battery level** (shrinks as the battery gets low)  
- 💤 **Idle auto-fade** and transition into deep sleep  
- 🔘 **Instant wake** via button press  
- ⚡ **Efficient power usage** (deep sleep + display off when idle)

---

## 🧠 How It Works

- On button press, the device wakes and ignites the flame using a smooth scale animation.  
- The battery voltage is read and mapped to a flame size, giving the digital flame a “mood” based on power level.  
- Frame-by-frame flame animation plays at a fixed interval for a natural flicker.  
- After a set idle timeout, the flame gracefully shrinks and “goes out,” then the ESP32 enters deep sleep.  
- The same button serves as the wake trigger.

---

## 🛠️ Bill of Materials (BOM)

- **Seeed Studio XIAO ESP32-C3**  
- **SSD1306 display module** (128x32)  
- **LiPo Battery** LP552035, 3.7V, 350mAh  
- **2 × 220kΩ resistors** (for safe battery voltage reading)  

---

## 📦 3D Model

3D-printable enclosure (Womp):  
https://beta.womp.com/preview/l16ip302v2di4zah

---

## 📁 Code Overview

- **Animation scaling** for ignition and extinguish effects  
- **Battery voltage reading** with averaging for stability  
- **Adaptive flame size** using voltage-to-scale mapping  
- **Deep sleep mode** for long battery life  
- **Bitmap rendering** of animated frames with horizontal scaling  
- **Button wake-up** using ESP32 GPIO wake trigger  

---

## 🚀 Getting Started

1. Download and Install the latest version of Arduino IDE according to your operating system
2. Add ESP32 board package to your Arduino IDE
Navigate to File > Preferences, and fill "Additional Boards Manager URLs" with the url below: https://jihulab.com/esp-mirror/espressif/arduino-esp32.git
Navigate to Tools > Board > Boards Manager..., type the keyword "esp32" in the search box, select the latest version of esp32, and install it.
4. Select your board and port
Navigate to Tools > Board > ESP32 Arduino and select "XIAO_ESP32C3". The list of board is a little long and you need to roll to the buttom to reach it.
Navigate to Tools > Port and select the serial port name of the connected XIAO ESP32C3. This is likely to be COM3 or higher (COM1 and COM2 are usually reserved for hardware serial ports).
6. Install required libraries:  
   - `Adafruit_GFX`  
   - `Adafruit_SSD1306`  
7. Wire up the SSD1306 display and battery.  
8. Flash the code to the **XIAO ESP32-C3** using Arduino IDE.
9. Power on and tap the button—enjoy your pocket flame!

---

## 🔑 License

MIT — feel free to modify and improve!

---

Made with 🔥 and ✨
