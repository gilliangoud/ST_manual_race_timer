#define ledPin  13                  // LED connected to digital pin 13
#define startPin 0                  // start button on pin 4
#define stopPin1 22                   // stop button on pin 5
#define stopPin2 23                   // stop button 2 on pin 6
#define stopPin3 24                   // stop button 3 on pin 7
#define stopPin4 25                   // stop button 4 on pin 8
#define stopPin5 26                   // stop button on pin 5
#define stopPin6 27                   // stop button 2 on pin 6
#define stopPin7 28                   // stop button 3 on pin 7
#define stopPin8 29                   // stop button 4 on pin 8
#define modeSwitch 30               // the mode selector, if the signal pulled to ground sceond mode is enabeled

#define maxlaps 30
#define maxlanes 9

#define versions 1.5
#define lastUpdate "oktober 2017."

#include <Bounce2.h>                // Library for Bounce of switches

Bounce startSwitch = Bounce();
Bounce stopSwitch1 = Bounce();
Bounce stopSwitch2 = Bounce();
Bounce stopSwitch3 = Bounce();
Bounce stopSwitch4 = Bounce();
Bounce stopSwitch5 = Bounce();
Bounce stopSwitch6 = Bounce();
Bounce stopSwitch7 = Bounce();
Bounce stopSwitch8 = Bounce();
Bounce modeSwitchS = Bounce();

int modeSelector;
long previousMillis;
long startTime;
int raceNr = 1;
boolean statusStartSwitch;
int value = LOW;                    // previous value of the LED
int blinking;                       // condition for blinking - timer is timing
long interval = 100;                // blink interval - change to suit

int raceTime[maxlanes][maxlaps];
int lap[maxlaps];

void setup() {
  Serial.begin(9600);

  pinMode(ledPin, OUTPUT);
  pinMode(startPin, INPUT_PULLUP);
  pinMode(stopPin1, INPUT_PULLUP);
  pinMode(stopPin2, INPUT_PULLUP);
  pinMode(stopPin3, INPUT_PULLUP);
  pinMode(stopPin4, INPUT_PULLUP);
  pinMode(stopPin5, INPUT_PULLUP);
  pinMode(stopPin6, INPUT_PULLUP);
  pinMode(stopPin7, INPUT_PULLUP);
  pinMode(stopPin8, INPUT_PULLUP);
  
  pinMode(modeSwitch, INPUT_PULLUP);

  startSwitch.attach(startPin);
  startSwitch.interval(3);
  stopSwitch1.attach(stopPin1);
  stopSwitch1.interval(20);
  stopSwitch2.attach(stopPin2);
  stopSwitch2.interval(20);
  stopSwitch3.attach(stopPin3);
  stopSwitch3.interval(20);
  stopSwitch4.attach(stopPin4);
  stopSwitch4.interval(20);
  stopSwitch5.attach(stopPin5);
  stopSwitch5.interval(20);
  stopSwitch6.attach(stopPin6);
  stopSwitch6.interval(20);
  stopSwitch7.attach(stopPin7);
  stopSwitch7.interval(20);
  stopSwitch8.attach(stopPin8);
  stopSwitch8.interval(20);
  
  modeSwitchS.attach(modeSwitch);
  modeSwitchS.interval(3);

  Serial.println("ST manual race timer by Gillian Goud.");
  Serial.print("Software version: "); Serial.print(versions); Serial.print("     last update: "); Serial.println(lastUpdate);

  sdinit();

  Serial.println("");
  Serial.println("Ready");
  Serial.println("");
}

void loop() {
  modeSwitchS.update();
  if (modeSwitchS.fell()) {
    modeSelector = 1;
  } else if (modeSwitchS.rose()) {
    modeSelector = 0;
  }

  startSwitch.update();stopSwitch1.update();stopSwitch2.update();stopSwitch3.update();stopSwitch4.update();stopSwitch5.update();stopSwitch6.update();stopSwitch7.update();stopSwitch8.update();
  
  if (startSwitch.fell()) {
    statusStartSwitch = !statusStartSwitch;
  }

  if (statusStartSwitch == true && blinking == false) {
    startTimer();
  }

  if (blinking == true && statusStartSwitch == false) {stopTimer();}
  
  else if (stopSwitch1.fell() && blinking == true) {logtime(int(1));}
  else if (stopSwitch2.fell() && blinking == true) {logtime(int(2));}
  else if (stopSwitch3.fell() && blinking == true) {logtime(int(3));}
  else if (stopSwitch4.fell() && blinking == true) {logtime(int(4));}
  else if (stopSwitch5.fell() && blinking == true) {logtime(int(5));}
  else if (stopSwitch6.fell() && blinking == true) {logtime(int(6));}
  else if (stopSwitch7.fell() && blinking == true) {logtime(int(7));}
  else if (stopSwitch8.fell() && blinking == true) {logtime(int(8));}
  

  // blinking section
  if ( (millis() - previousMillis > interval) ) {
    if (blinking == true) {
      previousMillis = millis();
      if (value == LOW){value = HIGH;}
      else{value = LOW;}
      digitalWrite(ledPin, value);
    } else {digitalWrite(ledPin, LOW);}
  }
  // /blinking section
}

void startTimer(){
  // record starting point and enable blinking
  startTime = millis();
  blinking = true;
  
  Serial.println("Timer started");

  // emptying the array for refilling.
  for (int i = 0; i < maxlaps; i++) {
    for (int j = 0; j < maxlanes; j++) {
      raceTime[j][i] = 0;
    }
  }
  for (int i = 0; i < maxlaps; i++) {
    lap[i] = 0;
  }
}

void stopTimer(){
  // show in serial
  Serial.print("Timer stopped at ");Serial.print(convertMillis(getElapsedTime()));Serial.print(" for race ");Serial.println(raceNr);
  savetosd();
  
  // reset
  startTime = 0;
  blinking = false;
  
  // leave at bottom
  raceNr ++;
}

long getElapsedTime() {
  long elapsedMillis =   millis() - startTime;
  return elapsedMillis;
}

String convertMillis(long elapsedMillis){
  int seconds = elapsedMillis / 1000L;                // divide by 1000 to convert to seconds
  int minutes = seconds / 60;
  while (seconds >= 60) {
    seconds = seconds - 60;
  }
  long fractional = (int)(elapsedMillis % 1000L);
  String fracString;
  if (fractional == 0) {fracString = "000";}
  else if (fractional < 10) {fracString = String("00") + String(fractional);}
  else if (fractional < 100) {fracString = String("0") + String(fractional);}
  else {fracString = String(fractional);}

  String elapsedTime = String(minutes) + String(":") + String(seconds) + String(".") + String(fracString);
  //elapsedMillis = 0;
  return elapsedTime;  
}

void logtime (int button) {
  long elapsedMillis =   millis() - startTime;
  lap[button] ++;
  int thislap = lap[button];
  raceTime[button][thislap] = elapsedMillis;
  String temp = convertMillis(elapsedMillis);
  Serial.print("Button "); Serial.print(button); Serial.print(" was pressed and logged at: "); Serial.println(temp);
}

long getLapMillis(int lane, int lap){
  int previousLap = lap - 1;
  long previousElapsedMillis = raceTime[lane][previousLap];
  long currentElapsedMillis = raceTime[lane][lap];
  long lapMillis = currentElapsedMillis - previousElapsedMillis;

  return lapMillis;  
}

