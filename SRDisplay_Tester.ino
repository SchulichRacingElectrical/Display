#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_RA8875.h"
#include "ctype.h"

// Library only supports hardware SPI at this time
//
// Connect SCLK to UNO Digital #13 (Hardware SPI clock)
// Connect MISO to UNO Digital #12 (Hardware SPI MISO)
// Connect MOSI to UNO Digital #11 (Hardware SPI MOSI)
#define RA8875_INT 3
#define RA8875_CS 10
#define RA8875_RESET 9
//rpm, tps, fuel time, ignition angle

Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS, RA8875_RESET);

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
    data[0].key = "rpm";
    data[0].value = "6500";
    data[1].key = "test";
    data[1].value = "450.0";
    //Graphics Shit
    //Get Device Value
    /* Initialise the display using 'RA8875_480x272' or 'RA8875_800x480' */
    if (!tft.begin(RA8875_800x480)) {
      Serial.println("RA8875 Not Found!");
      while (1);
    }else{
      Serial.println("RA8875 start");
    }

    tft.displayOn(true);
    tft.GPIOX(true);      // Enable TFT - display enable tied to GPIOX
    tft.PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
    tft.PWM1out(255);
    tft.fillScreen(RA8875_BLACK);
  
    /* Switch to text mode */  
    tft.textMode();   
}
int counter = 0;
int flag = 10;

void loop() {
   if (rpm >= 12500){
    rpm = 0;
   }else{
    rpm += 53;
   }
  displayTester();
}



void printData(){ 
 for (int i = 0; i < CHANNELS; i++){
  if(data[i].key == "rpm"){
    //tft.textEnlarge(1);  
    //tft.setTextSize(3);
    tft.textSetCursor(0, 90);  
    tft.textColor(RA8875_WHITE, RA8875_BLACK);
    tft.textWrite("RPM");
    tft.textSetCursor(350, 90);
    tft.textColor(RA8875_WHITE, RA8875_BLACK);
    String t_s = String(data[i].value);                                
    char buf[t_s.length() + 1];
    t_s.toCharArray(buf, t_s.length()+1);
    tft.textWrite(buf);    
  }else if (data[i].key == "test"){
    tft.textEnlarge(1);  
    tft.textSetCursor(0, 40);  
    tft.textColor(RA8875_WHITE, RA8875_BLACK);
    tft.textWrite("TEST");
    tft.textSetCursor(350, 40);
    tft.textColor(RA8875_WHITE, RA8875_BLACK);
    String t_s = String(data[i].value);                                
    char buf[t_s.length() + 1];
    t_s.toCharArray(buf, t_s.length()+1);
    tft.textWrite(buf);
  }
 }
}

void displayTester(){
    tft.textEnlarge(1); 
    tft.textSetCursor(15, 90);  
    tft.textColor(RA8875_WHITE, RA8875_BLACK);
    tft.textWrite("55.0");
    tft.textSetCursor(15, 120);  
    tft.textColor(RA8875_WHITE, RA8875_BLACK);
    tft.textWrite("FUEL RATE");

    tft.textSetCursor(15, 170);  
    tft.textColor(RA8875_WHITE, RA8875_BLACK);
    tft.textWrite("125");
    tft.textSetCursor(15, 200);  
    tft.textColor(RA8875_WHITE, RA8875_BLACK);
    tft.textWrite("KPH");



    if (flag = 1 && rpm < 10000){
      tft.textSetCursor(15, 250);  
      tft.textColor(RA8875_WHITE, RA8875_BLACK);
      tft.textWrite("     ");
    }else if (flag = 0  && rpm < 1000){
      tft.textSetCursor(15, 250);  
      tft.textColor(RA8875_WHITE, RA8875_BLACK);
      tft.textWrite("     ");     
    }
      tft.textSetCursor(15, 250);  
      tft.textColor(RA8875_WHITE, RA8875_BLACK);
      String rpm_s = String(rpm);
      char buf[rpm_s.length() + 1];
      rpm_s.toCharArray(buf, rpm_s.length()+1);
      tft.textWrite(buf);
    
    if (rpm >= 10000){
      flag = 1;      
    }else{
      flag = 0;
    }

    tft.textSetCursor(15, 280);  
    tft.textColor(RA8875_WHITE, RA8875_BLACK);
    tft.textWrite("RPM");

    tft.textSetCursor(700, 90);  
    tft.textColor(RA8875_WHITE, RA8875_BLACK);
    tft.textWrite("14.0");
    tft.textSetCursor(700, 120);  
    tft.textColor(RA8875_WHITE, RA8875_BLACK);
    tft.textWrite("AFR");

    tft.textSetCursor(700, 170);  
    tft.textColor(RA8875_WHITE, RA8875_BLACK);
    tft.textWrite("65.0");
    tft.textSetCursor(700, 200);  
    tft.textColor(RA8875_WHITE, RA8875_BLACK);
    tft.textWrite("OIL P.");

    tft.textSetCursor(700, 250);  
    tft.textColor(RA8875_WHITE, RA8875_BLACK);
    tft.textWrite("15.4");
    tft.textSetCursor(700, 280);  
    tft.textColor(RA8875_WHITE, RA8875_BLACK);
    tft.textWrite("MAP");

    tft.textEnlarge(4);  
    tft.textTransparent(RA8875_RED);
    tft.textSetCursor(350, 120);  
    tft.textColor(RA8875_WHITE, RA8875_BLACK);
    tft.textWrite("N");
    
    int percentage = rpm/12500.0*533;

    tft.fillRect(133,440, 533, 20, RA8875_BLACK);
    tft.fillRect(133,440, percentage, 20, RA8875_WHITE);
}
