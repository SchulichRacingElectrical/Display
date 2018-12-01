#include <Arduino.h>
#include <SPI.h>
#include "RA8875.h"
#include <Adafruit_NeoPixel.h>
#include "fonts/minipixel_24.c"

#define PIN 6
#define DRIVERWANTSBLOCK 1
// Library only supports hardware SPI at this time
// Connect SCLK to UNO Digital #13 (Hardware SPI clock)
// Connect MISO to UNO Digital #12 (Hardware SPI MISO)
// Connect MOSI to UNO Digital #11 (Hardware SPI MOSI)
#define RA8875_INT 3
#define RA8875_CS 10
#define RA8875_RESET 9
//rpm, tps, fuel time, ignition angle

RA8875 tft = RA8875(RA8875_CS, RA8875_RESET);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, PIN, NEO_GRBW + NEO_KHZ800); 

typedef struct
{
    String key;
    String value;
}Data;
const int CHANNELS = 2; 
Data data[CHANNELS];
//For Testing Only
int rpm = 0;
//End Testing Only

void setup() {
    Serial.begin(1000000);
    Serial.println("<Arduino is ready>");
//    data[0].key = "rpm";
//    data[0].value = "6500";
//    data[1].key = "test";
//    data[1].value = "450.0";
    //Graphics Shit
    //Get Device Value
    /* Initialise the display using 'RA8875_480x272' or 'RA8875_800x480' */
    //Setup for Adafruit_RA8875
    tft.begin(RA8875_800x480);
    tft.GPIOX(true);      // Enable TFT - display enable tied to GPIOX
    tft.setRotation(2);
    strip.begin();
    strip.show();
    strip.setBrightness(5); 
    setupLabels();
}

void setupLabels(){
    tft.setFontScale(1); 
    tft.setCursor(0, 50);  
    tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
    tft.write("WaterTemp");
    tft.setCursor(685, 50);  
    tft.write("OilTemp");
    tft.setFontScale(2); 
    tft.setCursor(715, 430);  
    tft.write("RPM");
    tft.setCursor(0, 430);  
    tft.write("KPH");
}

int counter = 0;
int flag = 10;

void loop() {
  if(rpm >= 12500)
    rpm = 0;
  else
    rpm += 5;
  displayTester();
}

void displayTester(){
    displayWaterTemp();
    displayKPH();
    displayRPM();
    displayOilTemp();
    if(checkGearChange())
      displayGear();
    LED(rpm);
    delay(1); //Gives a tiny amount of recovery time for the buffer. Do not remove.
}

void displayGear(){
    static int count = 1;
    //const tFont* n = tft.getFont();
    const tFont* n = NULL;
    tft.setFont(&minipixel_24);
    tft.setFontScale(20);
    tft.setTextColor(RA8875_WHITE);//background transparent!!!
    tft.setCursor(CENTER, CENTER);
    String gear = String(count);
    char buf[2];
    gear.toCharArray(buf, 2);
    tft.write("  ");
    tft.setCursor(CENTER, CENTER);
    tft.write(buf);  
    tft.setFontScale(2);
    tft.setFont(n);
    tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
    count++;
    if(count > 6)
      count = 1;
}

bool checkGearChange() //Only update gear if there is a change, otherwise there is too much overhead
{
  if(rpm == 12500)
    return true;
  else
    return false;  
}

void displayWaterTemp(){
    tft.setCursor(0, 0);  
    tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
    tft.write("55.0");
}

void displayKPH(){
    tft.setFontScale(3); 
    tft.setCursor(30, 230);  
    tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
    tft.write("125");
    tft.setFontScale(2); 
}

void displayRPM(){
    tft.setFontScale(3); 
    if (flag = 1 && rpm < 10000){
      tft.setCursor(600, 230);  
      tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
      tft.write("      ");
    }
    else if (flag = 0  && rpm < 1000){
      tft.setCursor(600, 230);  
      tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
      tft.write("      ");      
    }
    tft.setCursor(620, 230);  
    tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
    String rpm_s = String(rpm);
    char buf[rpm_s.length() + 1];
    rpm_s.toCharArray(buf, rpm_s.length()+1);
    tft.write(buf);
    flag = (rpm >= 10000)?1:0;
    tft.setFontScale(2); 
}

void displayOilTemp(){
    tft.setCursor(680, 0);  
    tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
    tft.write("14.0");
}

void blockIncrementLED() {
  for (int i = 10; i < 16; i++)
    strip.setPixelColor(i, 0, 0, 255);
}

void LED (int rpm) {
  int numberoflights = (int)((rpm - 10000) * 0.0064);        
  if (numberoflights > strip.numPixels()) 
    numberoflights = 0;
  for (int i = 0; i < strip.numPixels(); i++) {
     if (i < numberoflights && i < 6)
        strip.setPixelColor(i, 0, 255, 0);
     else if (i < numberoflights && i < 10)
        strip.setPixelColor(i, 255, 0, 0);
     else if (i < numberoflights && i < 16 && DRIVERWANTSBLOCK) {
        blockIncrementLED();
        break;
     } 
     else if (i < numberoflights && i < 16)
       strip.setPixelColor(i, 0, 0, 255);
     else
       strip.setPixelColor(i, 0);
    }
    strip.show();
    flag = true;
}
