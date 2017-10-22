#if (sdEnable == true)
#include "SD.h"
//#include "SdFat.h"
#include"SPI.h"

//SdFat SD;

//const int SD_CS_PIN = 7;


#define SD_CS_PIN 10

void sdinit() {
  Serial.print("Initializing SD card...");
  pinMode(SD_CS_PIN, OUTPUT);

  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("Card failed, or not present... Stopping here...");
    #if(displayEnable)
    printScreenBig("NO SD HERE!");
    #endif
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
  #if(displayEnable)
  printlnScreen("SD card loaded");
  #endif

  if(SD.exists("race1.CSV")){
    #if(displayEnable)
    printlnScreen("Race 1 already on SD,");
    printlnScreen("New raceData will be");
    printlnScreen("written underneath.");
    #endif
  }

  /*
  if (SD.exists("data.CSV")) {
    Serial.println("Data.csv already exists, going on.");
  } else {
    Serial.println("Data.csv is not here! Creating it now...");
    raceData = SD.open("data.CSV", FILE_WRITE);
    if (raceData) {
      raceData.println("sep=;");
      raceData.close(); // close the file
    }
  }*/
}

void savetosd(){
  File raceData;
  String filename = "race" + String(raceNr) + ".CSV";
  raceData = SD.open(filename, FILE_WRITE);
  
  raceData.print("Race");raceData.print(";");raceData.print(raceNr);raceData.print(";");raceData.print("Stoppped at");raceData.print(";;");raceData.println(convertMillis(getElapsedTime()));
  raceData.println("");
  
  for (int i = 0; i < maxlaps; i++) {
    if (i == 0){
      raceData.print(";");
      for (int j = 1; j < maxlanes; j++) {
        raceData.print("Lane ");raceData.print(j);raceData.print(";");
      }
      raceData.println("");
    } else {
      if (raceTime[1][i] == 0 && raceTime[2][i] == 0 && raceTime[3][i] == 0 && raceTime[4][i] == 0 && raceTime[5][i] == 0){} else {
          for (int j = 0; j < maxlanes; j++) {
            if (j == 0){
              raceData.print("Lap ");raceData.print(i);raceData.print(";");
            } else {
                String temp;
                if(raceTime[j][i] != 0){
                  temp = convertMillis(getLapMillis(j,i));
                }
                raceData.print(temp);raceData.print(";");
            }
          }
          raceData.println("");
      }
    }
  }
  raceData.println("");
  for (int i = 0; i < maxlanes; i++) {
    String temp;
    if (i == 0){temp = "End:";}
    for (int j = 1; j < maxlaps; j++) {
      int nextLap = j + 1;
      if(raceTime[i][j] != 0 && raceTime[i][nextLap] == 0){
        temp = convertMillis(raceTime[i][j]);
      }
    }
    raceData.print(temp);raceData.print(";");
  }
  raceData.println("");
  raceData.close(); // close the file
  #if(displayEnable)
    printlnScreen("Written to SD.");
  #endif
}
#endif


