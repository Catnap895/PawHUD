#include <Wire.h>
#include <WiFiS3.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "MAX30105.h"
#include <Adafruit_VL53L0X.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// ---------------- OLED ----------------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ---------------- Sensors ----------------
MAX30105 heartSensor;
Adafruit_VL53L0X laser = Adafruit_VL53L0X();
OneWire oneWire(6);
DallasTemperature ds18b20(&oneWire);

// ---------------- WiFi & Time ----------------
char ssid[] = "Vidor_WiFi";
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", -5*3600);

// ---------------- Pins ----------------
#define FLAME_PIN 4
#define IR_PIN 5
#define SPEED_PIN 2
#define TOUCH_PIN 7

#define ULTRASONIC_TRIG 3
#define ULTRASONIC_ECHO 11

#define JOY_X A0
#define JOY_SW 10

volatile int speedCount = 0;
void speedISR() { speedCount++; }

// ---------------- Menu ----------------
int menuIndex = 0;
int selectedPage = 0;
const int menuItems = 5;
bool inMenu = true;

// ---------------- Heart BPM ----------------
unsigned long lastBeat = 0;
int bpm = 0;
bool beatDetected = false;
const int THRESHOLD = 50000;

// ---------------- Sensor Values ----------------
int flame, ir, speed, laserDist;
float tempF;
long redValue, ultrasonicDist;

// ---------------- Read Ultrasonic ----------------
long readUltrasonicCM() {
  digitalWrite(ULTRASONIC_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIG, LOW);
  long duration = pulseIn(ULTRASONIC_ECHO, HIGH);
  return duration / 29 / 2; // cm
}

// ---------------- Setup ----------------
void setup() {
  Serial.begin(9600);

  pinMode(FLAME_PIN, INPUT);
  pinMode(IR_PIN, INPUT);
  pinMode(SPEED_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(SPEED_PIN), speedISR, RISING);

  pinMode(TOUCH_PIN, INPUT);
  pinMode(ULTRASONIC_TRIG, OUTPUT);
  pinMode(ULTRASONIC_ECHO, INPUT);

  pinMode(JOY_SW, INPUT_PULLUP);

  Wire.begin();

  // OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  // Heart
  if(heartSensor.begin(Wire, I2C_SPEED_STANDARD)) heartSensor.setup();

  // Laser
  laser.begin();

  // Temp
  ds18b20.begin();

  // WiFi
  int status = WiFi.begin(ssid);
  while(status != WL_CONNECTED){ delay(1000); status = WiFi.begin(ssid);}
  timeClient.begin();
}

// ---------------- Read Sensors ----------------
void readSensors() {
  flame = digitalRead(FLAME_PIN);
  ir = digitalRead(IR_PIN);

  speed = speedCount;
  speedCount = 0;

  ds18b20.requestTemperatures();
  tempF = ds18b20.getTempCByIndex(0)*9/5 + 32;

  VL53L0X_RangingMeasurementData_t measure;
  laser.rangingTest(&measure, false);
  laserDist = (measure.RangeStatus != 4) ? measure.RangeMilliMeter : -1;

  redValue = heartSensor.getRed();

  ultrasonicDist = readUltrasonicCM();

  unsigned long now = millis();
  if(redValue > THRESHOLD && !beatDetected){
    beatDetected = true;
    bpm = 60000 / (now - lastBeat);
    lastBeat = now;
  }
  if(redValue < THRESHOLD) beatDetected = false;
}

// ---------------- Joystick Menu ----------------
void checkJoystick() {
  int joyX = analogRead(JOY_X);

  // Scroll Right
  if(joyX > 600){
    menuIndex++;
    if(menuIndex >= menuItems) menuIndex = 0;
    delay(150);
  }
  // Scroll Left
  else if(joyX < 400){
    menuIndex--;
    if(menuIndex < 0) menuIndex = menuItems - 1;
    delay(150);
  }

  // Press to select
  if(digitalRead(JOY_SW) == LOW){
    selectedPage = menuIndex;
    inMenu = false;
    delay(200);
  }
}

// ---------------- Draw Menu ----------------
void drawMenu(){
  String items[5] = {"Environment","Distance","Heart Rate","System","Extra"};
  display.setTextSize(1);
  for(int i=0;i<menuItems;i++){
    display.setCursor(0,i*12);
    if(i==menuIndex) display.print("> ");
    else display.print("  ");
    display.println(items[i]);
  }
}

// ---------------- Draw Pages ----------------
void drawPage(){
  display.setTextSize(1);
  display.setCursor(0,0);
  switch(selectedPage){
    case 0:
      display.println("Temp F: " + String(tempF));
      display.println("Speed: " + String(speed));
      display.println("Flame: " + String(flame));
      break;
    case 1:
      display.println("Laser mm: " + String(laserDist));
      display.println("IR: " + String(ir));
      display.println("Ultrasonic cm: " + String(ultrasonicDist));
      break;
    case 2:
      display.println("Heart BPM: " + String(bpm));
      display.println("Raw: " + String(redValue));
      break;
    case 3:
      display.println("IP: " + WiFi.localIP().toString());
      display.println("Touch: " + digitalRead(TOUCH_PIN));
      timeClient.update();
      display.println("Time: " + timeClient.getFormattedTime());
      break;
    case 4:
      display.println("Extra Sensor Page");
      break;
  }
}

// ---------------- Loop ----------------
void loop(){
  readSensors();
  checkJoystick();

  display.clearDisplay();
  if(inMenu) drawMenu();
  else drawPage();
  display.display();

  delay(100);

  if(ultrasonicDist >= 32){
    display.println("Safe Distance");
  }
  if(ultrasonicDist <= 31){
    display.println("Danger!");
  }
}
