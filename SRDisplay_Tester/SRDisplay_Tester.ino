#include <Arduino.h>
#include <SPI.h>
#include "RA8875.h"
#include <Adafruit_NeoPixel.h>
#include "fonts/minipixel_24.c"

#define PIN 6
#define DRIVERWANTSBLOCK 1

// Library only supports hardware SPI at this time
//
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
    Serial.begin(10000000);
    Serial.println("<Arduino is ready>");
    data[0].key = "rpm";
    data[0].value = "6500";
    data[1].key = "test";
    data[1].value = "450.0";
    //Graphics Shit
    //Get Device Value
    /* Initialise the display using 'RA8875_480x272' or 'RA8875_800x480' */
    tft.begin(RA8875_800x480);
    tft.GPIOX(true);      // Enable TFT - display enable tied to GPIOX
    //tft.fillScreen(RA8875_BLACK);
    tft.setRotation(2);
    strip.begin();
    strip.show();
    strip.setBrightness(5);
    tft.setFont(&minipixel_24);
    
    tft.setFontScale(12);
    tft.setTextColor(RA8875_WHITE);//background transparent!!!
    tft.setCursor(CENTER, CENTER);
    tft.println("N");  
    
    const tFont* n = NULL;
    tft.setFont(n);
    tft.setFontScale(1.6); 
}
int counter = 0;
int flag = 10;

void loop() 
{
  if (rpm >= 12500)
    rpm = 0;
  else
    rpm += 150;
  displayTester();
}

void displayTester(){
    tft.setFontScale(1); 
    tft.setCursor(15, 90);  
    tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
    tft.print("55.0");
    tft.setCursor(15, 120);  
    tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
    tft.print("FUEL RATE");

    tft.setCursor(15, 170);  
    tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
    tft.print("125");
    tft.setCursor(15, 200);  
    tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
    tft.print("KPH");



    if (flag = 1 && rpm < 10000){
      tft.setCursor(15, 250);  
      tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
      tft.print("     ");
    }else if (flag = 0  && rpm < 1000){
      tft.setCursor(15, 250);  
      tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
      tft.print("     ");     
    }
      tft.setCursor(15, 250);  
      tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
      String rpm_s = String(rpm);
      char buf[rpm_s.length() + 1];
      rpm_s.toCharArray(buf, rpm_s.length()+1);
      tft.print(buf);
    
    if (rpm >= 10000){
      flag = 1;      
    }else{
      flag = 0;
    }

    tft.setCursor(15, 280);  
    tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
    tft.print("RPM");

    tft.setCursor(700, 90);  
    tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
    tft.print("14.0");
    tft.setCursor(700, 120);  
    tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
    tft.print("AFR");

    tft.setCursor(700, 170);  
    tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
    tft.print("65.0");
    tft.setCursor(700, 200);  
    tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
    tft.print("OIL P.");

    tft.setCursor(700, 250);  
    tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
    tft.print("15.4");
    tft.setCursor(700, 280);  
    tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
    tft.print("MAP");

    int percentage = rpm/12500.0*533;

    tft.fillRect(133,440, 533, 20, RA8875_BLACK);
    tft.fillRect(133,440, percentage, 20, RA8875_WHITE);

    LED(rpm);
}

void blockIncrementLED() 
{
  for (int i = 10; i < 16; i++)
    strip.setPixelColor(i, 0, 0, 255);
}

void LED (int rpm) 
{
  int numberoflights = (int)(rpm * 0.00152);        //0.00152 = 1/657
  
  if (numberoflights > strip.numPixels()) 
    numberoflights = strip.numPixels();
    
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
}
