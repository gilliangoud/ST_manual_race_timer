#define MAX_LANES  11 // The max amount of lanes in the race + 1
#define MAX_LAPS 15
long laneLapTimeElapsed[MAX_LANES][MAX_LAPS];
#include <Bounce2.h>
#define raceBusyLedPin  13
#define pwrSensorPin  17
const byte buttonPins[] = {4, 5, 6, 7, 8, 9, 10, 11, 12, 14, 15, 16}; // start, stop , lane 1 - 10
#define NUMBUTTONS sizeof(buttonPins)

Bounce button[NUMBUTTONS] = Bounce();
bool timerStarted = false;
long startTime;
long tempStartTime = 0;
int laneLapCounter[MAX_LANES];

// display
#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"
#define I2C_ADDRESS 0x3C
#define RST_PIN 4
SSD1306AsciiAvrI2c oled;
// end_display
// printer
#include "Adafruit_Thermal.h"
#include "SoftwareSerial.h"
#define TX_PIN 3 // Arduino transmit  YELLOW WIRE  labeled RX on printer
#define RX_PIN 2 // Arduino receive   GREEN WIRE   labeled TX on printer
SoftwareSerial mySerial(RX_PIN, TX_PIN);
Adafruit_Thermal printer(&mySerial);
// end_printer

void setup() {
  // display
#if RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS, RST_PIN);
#else // RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
#endif // RST_PIN >= 0
  oled.setFont(System5x7);
  oled.setScrollMode(SCROLL_MODE_AUTO);
  oled.clear();
  oled.println("Made by Goud-IT");
  // end_display
  // printer
  mySerial.begin(9600);
  printer.begin();
  printer.justify('L');
  printer.setSize('S');
  // end_printer

  Serial.begin(9600);
  pinMode(raceBusyLedPin, OUTPUT);
  pinMode(pwrSensorPin, INPUT);
  for (int i = 0; i < NUMBUTTONS; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
    button[i] = Bounce();
    button[i].attach(buttonPins[i]);
    button[i].interval(5);
  }

  printMessage("Timer ready");
  if (digitalRead(pwrSensorPin) == LOW) {
    printMessage("No AC power attached");
    printMessage("Use AC power to print");
  } else {
    printMessage("Printer ready");
  }
}

void loop() {
  for (int i = 0; i < NUMBUTTONS; i++) {
    button[i].update();
  }

  if (button[0].fell() && timerStarted == false) {
    startTimer(millis());
  }

  for (int i = 2; i < NUMBUTTONS; i++) {
    if (button[i].fell() && timerStarted == true && (millis() - startTime) > 3000) {
      recordLapPress(i - 1);
    }
    if (button[i].fell() && timerStarted == false){
      if (tempStartTime > 0){
        startTimer(tempStartTime);
      } else {
        tempStartTime = millis();
      }
    }
  }

  if (tempStartTime > 0 && (millis() - tempStartTime) > 500) {
    tempStartTime = 0;
  }

  if (button[1].fell()) {
    if (timerStarted == true) {
      stopTimer();
    }
    if (digitalRead(pwrSensorPin) == HIGH) {
      printRace();
    } else {
      printMessage("No AC power attached");
      printMessage("Connect power to print");
    }
  }
}

void recordLapPress(int lane) {
  laneLapCounter[lane]++;
  laneLapTimeElapsed[lane][laneLapCounter[lane]] = millis() - startTime;
  printMessage(lapTimeMessage(lane, millis() - startTime));
}

void printMessage(String message) {
  Serial.println(message);
  oled.println(message);
}

void printRace() {
  printMessage("Printing race");
  printer.println("Timer stopped at " + msToTimeConverter(millis()) + " since beginning");
  for (int i = 1; i < MAX_LANES; i++) {
    if (laneLapCounter[i] != 0) {
      printer.println("Lane: " + (String)i + " laps counted: " + (String)laneLapCounter[i]);
      printer.println("Final time: " + msToTimeConverter(laneLapTimeElapsed[i][laneLapCounter[i]]));
    }
  }
  printer.feed(4);
}

void stopTimer() {
  printMessage("Stopped at:" + msToTimeConverter(millis() - startTime));
  timerStarted = false;
}

void startTimer(long stime) {
  oled.clear();
  tempStartTime = 0;
  startTime = stime;
  timerStarted = true;
  for (int i = 0; i < MAX_LANES; i++) {
    laneLapCounter[i] = 0;
  }
  printMessage("Timer started");
}

String lapTimeMessage(int lane, long elapsedMillis) {
  return (String)lane + ": lap " + (String)laneLapCounter[lane] + ": " + msToTimeConverter(elapsedMillis);
}

String msToTimeConverter(long elapsedTime) {
  int seconds = elapsedTime / 1000L;
  int minutes = seconds / 60;

  while (seconds >= 60) {
    seconds = seconds - 60;
  }
  String secondsString = (seconds < 10)? "0" + String(seconds): String(seconds);

  int fractional = (int)(elapsedTime % 1000L);
  String fracString = "";
  if (fractional == 0) {
    fracString = "000";
  }
  else if (fractional < 10) {
    fracString = "00";
  }
  else if (fractional < 100) {
    fracString = "0";
  }
  fracString += (String) fractional;

  return "0" + (String) minutes + ':' + secondsString + "." + fracString;
}
