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

// ---------------- WiFi Time ----------------
char ssid[] = "Vidor_WiFi";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", -5*3600);

// ---------------- Pins ----------------
#define FLAME_PIN 4
#define IR_PIN 5
#define SPEED_PIN 2
#define TOUCH_PIN 7

#define ROTARY_CLK 8
#define ROTARY_DT 9
#define ROTARY_SW 10

#define ULTRASONIC_TRIG 3
#define ULTRASONIC_ECHO 11

long readUltrasonicCM() {
  digitalWrite(ULTRASONIC_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIG, LOW);

  long duration = pulseIn(ULTRASONIC_ECHO, HIGH);
  long cm = duration / 29 / 2;
  return cm;
}

volatile int speedCount = 0;
void speedISR(){ speedCount++; }

// ---------------- Menu ----------------
int menuIndex = 0;
bool inMenu = true;

const int menuItems = 4;

int lastCLK;

// ---------------- Heart BPM ----------------
unsigned long lastBeat = 0;
int bpm = 0;
bool beatDetected = false;
const int THRESHOLD = 50000;

// ---------------- Sensor Values ----------------
int flame;
int ir;
int speed;
float tempF;
int laserDist;
long redValue;

// ---------------- Setup ----------------
void setup() {

  Serial.begin(9600);

  pinMode(FLAME_PIN, INPUT);
  pinMode(IR_PIN, INPUT);
  pinMode(SPEED_PIN, INPUT_PULLUP);
  pinMode(TOUCH_PIN, INPUT);

  pinMode(ROTARY_CLK, INPUT);
  pinMode(ROTARY_DT, INPUT);
  pinMode(ROTARY_SW, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(SPEED_PIN), speedISR, RISING);

  lastCLK = digitalRead(ROTARY_CLK);

  Wire.begin();

  // OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  // Heart sensor
  if(heartSensor.begin(Wire, I2C_SPEED_STANDARD))
    heartSensor.setup();

  // Laser
  laser.begin();

  ds18b20.begin();

  // WiFi
  int status = WiFi.begin(ssid);
  while(status != WL_CONNECTED){
    delay(1000);
    status = WiFi.begin(ssid);
  }

  timeClient.begin();
}

// ---------------- Read Sensors ----------------
void readSensors(){

  flame = digitalRead(FLAME_PIN);
  ir = digitalRead(IR_PIN);

  speed = speedCount;
  speedCount = 0;

  ds18b20.requestTemperatures();
  tempF = ds18b20.getTempCByIndex(0)*9/5 + 32;

  VL53L0X_RangingMeasurementData_t measure;
  laser.rangingTest(&measure, false);

  if(measure.RangeStatus != 4)
    laserDist = measure.RangeMilliMeter;
  else
    laserDist = -1;

  redValue = heartSensor.getRed();

  unsigned long now = millis();

  if(redValue > THRESHOLD && !beatDetected){
    beatDetected = true;
    bpm = 60000 / (now - lastBeat);
    lastBeat = now;
  }

  if(redValue < THRESHOLD)
    beatDetected = false;
}

// ---------------- Rotary ----------------
void checkEncoder() {
  static int lastDT = LOW;
  int clk = digitalRead(ROTARY_CLK);
  int dt  = digitalRead(ROTARY_DT);

  // detect change on CLK
  if(clk != lastCLK) {
    if(dt != clk) menuIndex++;   // clockwise
    else menuIndex--;            // counterclockwise

    // wrap around
    if(menuIndex < 0) menuIndex = menuItems - 1;
    if(menuIndex >= menuItems) menuIndex = 0;

    Serial.print("Menu Index: "); Serial.println(menuIndex);
  }

  lastCLK = clk;
}

// ---------------- Button ----------------
void checkButton(){

  if(digitalRead(ROTARY_SW) == LOW){

    delay(200);

    if(inMenu)
      inMenu = false;
    else
      inMenu = true;
  }
}

// ---------------- Menu Screen ----------------
void drawMenu(){

  String items[4] = {
    "Environment",
    "Distance",
    "Heart Rate",
    "System"
  };

  display.setTextSize(1);

  for(int i=0;i<menuItems;i++){

    display.setCursor(0,i*15);

    if(i==menuIndex)
      display.print("> ");
    else
      display.print("  ");

    display.println(items[i]);
  }
}

// ---------------- Pages ----------------
void drawPage(){

  display.setTextSize(1);
  display.setCursor(0,0);

  switch(menuIndex){

    case 0:
      display.println("Temp F:");
      display.println(tempF);

      display.println("Speed:");
      display.println(speed);

      display.println("Flame:");
      display.println(flame);
    break;

    case 1:
      display.println("Laser mm:");
      display.println(laserDist);

      display.println("IR:");
      display.println(ir);
    break;

    case 2:
      display.println("Heart BPM:");
      display.println(bpm);

      display.println("Raw:");
      display.println(redValue);
    break;

    case 3:
      display.println("IP:");
      display.println(WiFi.localIP());

      display.println("Touch:");
      display.println(digitalRead(TOUCH_PIN));

      timeClient.update();

      display.println("Time:");
      display.println(timeClient.getFormattedTime());
    break;

    case 4:
      display.println("Laser mm:");
      display.println(laserDist);
      display.println("Ultrasonic cm:");
      display.println(ultrasonicDist);
      display.println("IR:");
      display.println(ir);
    break;

  }
}

// ---------------- Loop ----------------
void loop(){

  readSensors();

  checkEncoder();
  checkButton();

  display.clearDisplay();

  if(inMenu)
    drawMenu();
  else
    drawPage();

  display.display();

  delay(100);
}
