# ESP32 Infrared Blaster

**A Remote-Control Solution for IR-Operated Heaters**

This project provides a semi-reliable way to remotely control heaters in locations where direct feedback is unavailable. By integrating room temperature monitoring, you can infer the operational status of your heater.

Commands for controlling the heater (or any device, if modified) are transmitted and received via dedicated MQTT channels, enabling seamless integration with tools like **Home Assistant**, **Node-RED**, and **MySQL**. In this setup, Node-RED also manages the user interface, including buttons for various functionalities and a real-time temperature chart, while logging data to a MariaDB database for further analysis.

---

## Key Features

- **MQTT Integration**: Connects to the MQTT server and processes messages.
- **IR Signal Transmission**: Sends IR commands corresponding to specific messages with fast blinking IR signals.
- **Temperature Monitoring**: Measures room temperature and humidity for status feedback.
- **Data Transmission**: Publishes temperature and humidity readings to the MQTT server.
- **Test Functionality**: Responds to test messages for validation purposes.

---

## Update: Version 2

After more than five years of continuous operation (HOORAY!), Version 2 introduces significant improvements:

- **Sensor Upgrade**: Replaced the DHT11 sensor with the more reliable **SHT3x** sensor for accurate humidity readings.
- **Improved Wi-Fi Management**: Integrated **WiFiManager** (by Tzapu) to replace the basic WiFi configuration.
- **Library Update**: Switched to **IRremoteESP8266.h**, offering a simpler and fully functional IR library for ESP32.
- **Reduced Blocking Delays**: Eliminated most blocking delays to prevent signal overlap.
- **Enhanced IR Capability**: Added four IR LEDs in parallel for better signal coverage.

---

## Hardware Requirements

To get started, you'll need the following components:

- **ESP32**: The core of the project.
- **4x IR LEDs**: For transmitting IR signals.
- **IR Receiver**: To capture and record signals from your remote control.
- **SHT3x Temperature/Humidity Sensor**: For environmental monitoring.
