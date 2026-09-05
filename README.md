# 🌡️ ESP8266 Local Environment Dashboard

A standalone, local IoT weather station built with the ESP8266 and a DHT11 sensor. This project turns the ESP8266 into its own Wi-Fi router (SoftAP) and hosts a sleek, responsive dark-mode web dashboard to display real-time temperature and humidity data.

Instead of relying on the cloud or external Wi-Fi, you connect directly to the ESP8266 from your phone or laptop.

## ✨ Features

*   **Standalone Wi-Fi Hotspot:** Operates entirely offline using ESP8266 SoftAP mode.
*   **Modern Web UI:** A mobile-friendly, auto-refreshing dark-mode dashboard styled with CSS.
*   **Non-Blocking Architecture:** Uses `millis()` timers to serve web pages, read sensors, and animate LEDs simultaneously without crashing or freezing.
*   **Breathing LED:** Smooth, hardware-PWM fading effect on an external LED to indicate active status.
*   **Heartbeat Monitor:** Blinking onboard blue LED to confirm the main loop is running.

---

## 🛠️ Hardware Requirements

1.  **ESP8266** (NodeMCU or similar)
2.  **DHT11** Temperature & Humidity Sensor
3.  **1x LED** (Any color, for the breathing effect)
4.  **1x Resistor** (220Ω to 1kΩ)
5.  Breadboard and jumper wires

---

## 🔌 Wiring Guide

Because the ESP8266 is sensitive to which pins are pulled high/low during the boot process, this project uses guaranteed safe pins.

| Component | Pin Name | ESP8266 Pin | Notes |
| :--- | :--- | :--- | :--- |
| **DHT11 Data (S)** | Data | **D1** (GPIO 5) | Sensor communication |
| **DHT11 VCC** | + / VCC | **3.3V** (or 5V/VIN) | Power for the sensor |
| **DHT11 GND** | - / GND | **GND** | Ground |
| **External LED (Anode)** | Long Leg | **D5** (GPIO 14) | PWM pin for breathing fade |
| **External LED (Cathode)** | Short Leg | **GND** | Connect through resistor to GND |

> **Note:** The onboard blue LED is internally hardwired to **D4 (GPIO 2)** and is active-low (setting it to `LOW` turns it on).

---

## 💻 Software Setup

### 1. Install the Arduino IDE & ESP8266 Core
If you haven't already, install the Arduino IDE and add the ESP8266 board manager URL to your preferences:
`http://arduino.esp8266.com/stable/package_esp8266com_index.json`

### 2. Install Required Libraries
Go to **Sketch > Include Library > Manage Libraries** and install:
*   **DHT sensor library** by Adafruit (Ensure you also install the *Adafruit Unified Sensor* dependency if prompted).

### 3. Flash the Code
1. Open the `.ino` file in the Arduino IDE.
2. Select your specific ESP8266 board from the **Tools > Board** menu.
3. Set the baud rate in the Serial Monitor to `115200`.
4. Upload the code. *(If using a bare ESP-01 or HW-597B adapter, remember to pull GPIO 0 / D3 to GND during boot to enter flash mode).*

---

## 🚀 How to Use It

1.  **Power it up:** Plug the ESP8266 into a power bank, USB adapter, or computer. You will see the external LED start to "breathe" and the onboard LED blink.
2.  **Connect to Wi-Fi:** Open the Wi-Fi settings on your phone or computer and connect to the network:
    *   **SSID:** `ESP8266_Dashboard`
    *   **Password:** `password123`
3.  **Open the Dashboard:** Open any web browser and go to the default ESP8266 access point IP address:
    *   🔗 **`http://192.168.4.1`**

The dashboard will automatically update every 5 seconds with fresh environmental data!

---

## ⚙️ How the Code Works

This project avoids the infamous `delay()` function. Using `delay()` halts the microcontroller, which crashes the web server and corrupts DHT11 timing. 

Instead, it uses state machines based on `millis()`:
*   **The Web Server:** Listens continuously for incoming client requests.
*   **The DHT11 Timer:** Safely queries the sensor exactly once every 2.5 seconds in the background and stores the data in global variables (`t_global`, `h_global`).
*   **The LED Timers:** Adjusts the PWM brightness of the external LED every 4 milliseconds for a buttery-smooth fade, while toggling the onboard LED every 1000 milliseconds.

---

## 🐛 Troubleshooting

*   **Dashboard shows `--` instead of numbers:** The ESP8266 cannot communicate with the DHT11. Check your wiring, ensure Data is on D1, and try moving the sensor's VCC to 5V (VIN) if it is underpowered.
*   **Wi-Fi network doesn't appear:** Ensure your power supply can provide enough current. The ESP8266 draws a significant spike of power when initializing the Wi-Fi radio.
