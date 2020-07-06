# ESP32-Infrared-Blaster

**ESP32 based heater control.**

I needed to be able to switch on a couple of heaters in a remote location.
since I have no feedback from the heater, I need the room temperature to see if the heater is on or off.

The heater commands (or any if you change the msg in the script) are given, and the temperature is recieved through dedicated MQTT channels.
This allows for easy integration in homeassistant/node-red...


**Functionality**
- Connect to MQQT Server, listen to messages
- When recieved blink (fast!!) IR signal
- Measure temperature

** Parts List **
- ESP32
- 3 IR Led
- IR Reciever (for recording the signals from the remote)
- dht11
- 

**To Do**
- Check for disconnecting internet connection (disconnects after 2/3 days)
- Change Delay() for something less blocking
