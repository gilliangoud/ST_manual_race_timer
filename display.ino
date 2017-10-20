#if(displayEnable)
#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"
#define I2C_ADDRESS 0x3C
SSD1306AsciiAvrI2c oled;

void displayInit() {
// Initialize, optionally clear the screen
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
  oled.setFont(System5x7);  
  oled.clear();                 // clear screen
  oled.setScroll(true);
  //oled.set2X();
  //oled.println("2X demo");
  oled.set1X();
}

void printScreen(String message){
    oled.print(message);
}

void printlnScreen(String message){
    oled.println(message);
}

void printScreenBig(String message){
    oled.set2X();
    oled.println(message);
    oled.set1X();
}
#endif

