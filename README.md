🕶️ Fursuit Smart Glasses – Arduino K0419V12 owo

A DIY smart glasses project made by a furry, for fursuits! UwU
Uses Arduino UNO R4 WiFi + the K0419V12 sensor kit to give you a cute interactive HUD with lots of sensors. Totally modifiable for cosplay, robotics, or any nerdy fun stuff~

✨ Features

OLED menu with rotary encoder infinite scroll

Touch sensor shortcuts (jump to temp page uwu~)

Heart rate monitoring (BPM) – see your own “pawsome” beats 💓

Temperature (DS18B20)

Flame & IR sensor alerts 🔥

Laser (VL53L0X) & Ultrasonic distance (HC-SR04)

Speed sensor readings

Real-time US Eastern time via WiFi/NTP

Multiple menu pages: scroll to see all your stats

🛠️ What You’ll Need

Arduino UNO R4 WiFi

K0419V12 Sensor Kit:

MAX30105 Heart Rate

DS18B20 Temp

Flame sensor

IR sensor

VL53L0X Laser distance

Ultrasonic distance sensor

Speed sensor

Touch sensor

Rotary encoder

OLED 128x64 I2C display

Breadboard & jumper wires

📌 Wiring Overview
Sensor	Arduino Pins
OLED	VCC, GND, SDA, SCL
MAX30105 Heart	3.3V/5V, GND, SDA, SCL
DS18B20 Temp	5V, GND, D6
Flame	5V, GND, A0
IR	5V, GND, D5
Laser VL53L0X	5V, GND, SDA, SCL
Ultrasonic	5V, GND, D11(TRIG), D12(ECHO)
Speed	5V, GND, D2
Touch	5V, GND, D7
Rotary Encoder	D8(CLK), D9(DT), D10(SW)
💻 Software / Libraries

Arduino IDE (latest)

Install via Library Manager:

Adafruit SSD1306

Adafruit GFX

MAX30105

Adafruit VL53L0X

OneWire

DallasTemperature

NTPClient

WiFiS3

📝 Installation

Clone this repo:

git clone https://github.com/yourusername/fursuit-smart-glasses.git

Open smartglasses.ino in Arduino IDE.

Install all required libraries.

Connect your Arduino UNO R4 WiFi via USB-C.

Update your WiFi SSID in the code:

char ssid[] = "Your_WiFi_Name";

Upload the sketch uwu~

Power everything via Arduino or 5V supply.

Rotate the rotary to scroll pages and press to select. Touch to jump to temp page~

🎮 How It Works

Scroll the menu: turn the rotary encoder to view different sensor pages

Select a page: press the rotary encoder button

Touch shortcut: instantly jump to temperature page uwu~

OLED displays: all your stats live: heart, temp, flame, IR, laser, ultrasonic, speed, WiFi

⚠️ Disclaimers

⚠️ This project is made for fun and educational purposes!

Not medical grade (heart rate is for fun uwu)

Avoid staring into the laser

Use correct voltage (5V/3.3V) to avoid fried circuits

Authors are not responsible for damages or injuries

Test on a breadboard before installing in a fursuit

💖 Supporting / Donations

This project is free and open source for all furries and nerds~
If you enjoy it or want to support future upgrades, tips and contributions are loved! UwU

📝 License

MIT License – free for all, modify and share as you wish 💖

MIT License
Copyright (c) 2026 Your Name

Permission is hereby granted, free of charge, to any person obtaining a copy...
