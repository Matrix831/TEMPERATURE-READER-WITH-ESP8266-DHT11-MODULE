# ESP8266 Local Environment Dashboard

A standalone, local IoT weather station built with the ESP8266 and a DHT11 sensor. This project turns the ESP8266 microcontroller into an independent Wi-Fi router (SoftAP) and hosts a responsive, dark-mode web dashboard to display real-time temperature and humidity data. 

Instead of relying on cloud services or external Wi-Fi networks, users can connect directly to the ESP8266 from a mobile device or laptop to monitor environmental metrics locally.

> **[Insert Photo 1 Here: e.g., A wide shot of the completed physical project]**

## Features

* **Standalone Wi-Fi Hotspot:** Operates entirely offline using the ESP8266 SoftAP mode.
* **Modern Web UI:** A mobile-friendly, auto-refreshing dark-mode dashboard styled with CSS.
* **Non-Blocking Architecture:** Utilizes `millis()` timers to serve web pages, read sensors, and animate LEDs simultaneously without freezing the microcontroller.
* **Hardware Status Indicators:** Features a hardware-PWM fading effect on an external LED and a blinking onboard LED to visually confirm system health and active background loops.

---

## Hardware Requirements

1. **ESP8266** (NodeMCU or similar development board)
2. **DHT11** Temperature & Humidity Sensor
3. **1x LED** (For the external breathing effect)
4. **1x Resistor** (220Ω to 1kΩ, for the external LED)
5. Breadboard and standard jumper wires

> **[Insert Photo 2 Here: e.g., A close-up of the breadboard wiring and hardware assembly]**

---

## Sensor Calibration and Error Margin

The DHT11 is a cost-effective sensor, but it carries a standard factory error margin of ±2°C for temperature and ±5% for humidity. Over time or in specific environments, this drift can be larger.

To ensure data reliability, the sensor in this project was calibrated against a certified reference meter.

Calibration Documentation:

>[DIY Meter Output 1](link-to-diy-photo-1.jpg)

>[DIY Meter Output 2](link-to-diy-photo-2.jpg)

>[Legitimate Meter Result 1](link-to-legit-photo-1.jpg)

>[Legitimate Meter Result 2](link-to-legit-photo-2.jpg)


---

## Wiring Guide

Because the ESP8266 is sensitive to which pins are pulled high or low during the boot process, this project uses guaranteed safe GPIO pins.

| Component | Pin Name | ESP8266 Pin | Notes |
| :--- | :--- | :--- | :--- |
| **DHT11 Data (S)** | Data | **D1 (GPIO 5)** | Sensor communication |
| **DHT11 VCC** | + / VCC | **3.3V** (or 5V/VIN) | Power for the sensor |
| **DHT11 GND** | - / GND | **GND** | System ground |
| **External LED (Anode)** | Long Leg | **D5 (GPIO 14)** | PWM pin for breathing fade |
| **External LED (Cathode)** | Short Leg | **GND** | Connect through resistor to GND |

*Note: The onboard blue LED is internally hardwired to D4 (GPIO 2) and operates as active-low (setting it to `LOW` turns it on).*

---

## Software Setup

### 1. Install the Arduino IDE & ESP8266 Core
If not already installed, download the Arduino IDE and add the ESP8266 board manager URL to your preferences:
`http://arduino.esp8266.com/stable/package_esp8266com_index.json`

### 2. Install Required Libraries
Navigate to **Sketch > Include Library > Manage Libraries** and install:
* **DHT sensor library** by Adafruit (Ensure the *Adafruit Unified Sensor* dependency is also installed).

### 3. Flash the Microcontroller
1. Open the `.ino` file in the Arduino IDE.
2. Select your specific ESP8266 board from the **Tools > Board** menu.
3. Set the baud rate in the Serial Monitor to `115200`.
4. Upload the code. 

---

## How to Use It

1. **Power Initialization:** Plug the ESP8266 into a power bank, USB adapter, or computer. The external LED will begin to "breathe" and the onboard LED will blink, indicating a successful boot.
2. **Connect to the Network:** Open the Wi-Fi settings on your device and connect to the local access point:
    * **SSID:** `ESP8266`
    * **Password:** `12345678`
3. **Access the Dashboard:** Open any web browser and navigate to the default ESP8266 gateway IP address:
    * 🔗 **`http://192.168.4.1`**

The dashboard will automatically refresh every 5 seconds to display the latest calibrated data.

> **[Insert Photo 4 Here: e.g., A screenshot of the dark-mode web dashboard showing the final temperature and humidity]**

---

## System Architecture

This project strictly avoids the `delay()` function. Using `delay()` halts the microcontroller's processing, which causes web server timeouts and corrupts DHT11 timing protocols. 

Instead, the system utilizes state machines based on the `millis()` function:
* **The Web Server:** Listens continuously for incoming client HTML requests without blocking other tasks.
* **The DHT11 Timer:** Safely queries the sensor exactly once every 2.5 seconds in the background, applies the calibration offsets, and stores the optimized data in global variables (`t_global`, `h_global`).
* **The LED Timers:** Adjusts the PWM brightness of the external LED every 4 milliseconds for a fluid fade, while toggling the onboard LED every 1000 milliseconds.

---

## Troubleshooting

* **Dashboard displays `--` instead of numbers:** The ESP8266 is failing to communicate with the DHT11. Verify wiring, ensure the Data pin is routed to D1, and try switching the sensor's VCC to 5V (VIN) if it appears underpowered.
* **Wi-Fi network fails to broadcast:** Ensure the power supply can provide sufficient current (at least 500mA). The ESP8266 draws a significant power spike when initializing the internal Wi-Fi radio.
