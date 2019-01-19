#include <Arduino.h>
#include <SPI.h>
#include "RA8875.h"
#include <Adafruit_NeoPixel.h>
#include "fonts/minipixel_24.c"
#include <Wire.h>

#define PIN 6
#define DRIVERWANTSBLOCK 1
// Library only supports hardware SPI at this time
// Connect SCLK to UNO Digital #13 (Hardware SPI clock)
// Connect MISO to UNO Digital #12 (Hardware SPI MISO)
// Connect MOSI to UNO Digital #11 (Hardware SPI MOSI)
#define RA8875_INT 3
#define RA8875_CS 4
#define RA8875_RESET 9
#define CHANNELS 27 //Subject to change
//rpm, tps, fuel time, ignition angle

/*LEGEND for incoming data
 * DAQ
 * x = x accel              -- CHANNEL 25   
 * y = y accel              -- CHANNEL 26
 * z = z accel              -- CHANNEL 27
 * 
 * ECU
 * rpm = rpm                -- CHANNEL 1
 * tps = tps                -- CHANNEL 2
 * ftime = fueltime         -- CHANNEL 3
 * igangl = ignitionangle   -- CHANNEL 4
 * baro = barometer         -- CHANNEL 5
 * map = map                -- CHANNEL 6
 * lambda = lambda          -- CHANNEL 7
 * a1 = analog 1            -- CHANNEL 8
 * a2 = analog 2            -- CHANNEL 9
 * a3 = analog 3            -- CHANNEL 10
 * a4 = analog 4            -- CHANNEL 11
 * a5 = analog 5            -- CHANNEL 12
 * a6 = analog 6            -- CHANNEL 13
 * a7 = analog 7            -- CHANNEL 14
 * a8 = analog 8            -- CHANNEL 15
 * f1 = frequency 1         -- CHANNEL 16
 * f2 = frequency 2         -- CHANNEL 17
 * f3 = frequency 3         -- CHANNEL 18
 * f4 = frequency 4         -- CHANNEL 19
 * bat = battery            -- CHANNEL 20
 * atmp = airtemp           -- CHANNEL 21
 * ctm = coolanttemp        -- CHANNEL 22
 * t1 = thermistor 1        -- CHANNEL 23
 * t2 = thermistor 2        -- CHANNEL 24
 */
 
RA8875 tft = RA8875(RA8875_CS, RA8875_RESET);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, PIN, NEO_GRBW + NEO_KHZ800); 

typedef struct{
    String key;
    String value;
}Data;
Data DATA[CHANNELS];
//For Testing Only
int rpm = 0;
//End Testing Only


static bool whichDisplay = true;
//true == main display
//false == second display

void setup() {
    Serial.begin(1000000);
    Serial.println("<Arduino is ready>");
    //Graphics Shit
    //Get Device Value
    /* Initialise the display using 'RA8875_480x272' or 'RA8875_800x480' */
    //Setup for Adafruit_RA8875
    Wire.begin(8);
    Wire.onReceive(readInput);
    tft.begin(RA8875_800x480);
    tft.GPIOX(true);      // Enable TFT - display enable tied to GPIOX
    tft.setRotation(2);
    strip.begin();
    strip.show();
    strip.setBrightness(5); 
    if(whichDisplay)
      setupMainDisplayLabels();
    else
      setupSecondDisplayLabels();
}

void setupMainDisplayLabels(){
    //tft.setFontScale(1); 
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

void setupSecondDisplayLabels(){
    //tft.setFontScale(1); 
    tft.setCursor(0, 0);  
    tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
    tft.write("AccelX:");
    tft.setCursor(0, 50);  
    tft.write("AccelY:");
    tft.setCursor(0, 100);  
    tft.write("AccelZ:");
}

int counter = 0;
int flag = 10;

void loop(){
  if(whichDisplay){
    if(rpm >= 12600)
      rpm = 0;
    else
      rpm += 50;
    displayRPM();
    if(checkWaterTempChange())
      displayWaterTemp();
    if(checkKPHChange())
      displayKPH();
    if(checkOilTempChange())
      displayOilTemp();
    tft.clearMemory(true);
    if(checkGearChange())
      displayGear();
    //  LED(rpm);
  }else{
    char buf[7];
    tft.setCursor(120, 0);  
    tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
    (DATA[25].value).toCharArray(buf, 7);
    //tft.write("    ");
    tft.write(buf);

    tft.setCursor(120, 50);  
    tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
    (DATA[26].value).toCharArray(buf, 7);
    //tft.write("    ");
    tft.write(buf);

    tft.setCursor(120, 100);  
    tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
    (DATA[27].value).toCharArray(buf, 7);
    tft.write("    ");
    tft.write(buf);
  }
}

void displayGear(){
    static int count = 1;
    tft.setFont(&minipixel_24);
    tft.setFontScale(19);
    tft.setCursor(CENTER, CENTER);
    String gear = String(count);
    char buf[2];
    gear.toCharArray(buf, 2);
    tft.write("    ");
    tft.setCursor(CENTER, CENTER);
    tft.write(buf,2);  
    tft.setFontScale(2);
    tft.setFont(INT);
    count++;
    if(count > 6)
      count = 1;
}

bool checkGearChange() //Only update gear if there is a change, otherwise there is too much overhead
{
  if(rpm == 12600)
    return true;
  else
    return false;  
}

void displayWaterTemp(){
    tft.setCursor(0, 0);  
    tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
    tft.write("55.0");
}

bool checkWaterTempChange(){
  return true;
}

void displayKPH(){
    tft.setFontScale(3); 
    tft.setCursor(30, 230);  
    tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
    tft.write("125");
    tft.setFontScale(2); 
}

bool checkKPHChange(){
  return true;
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

bool checkRPMChange(){
  return true;
}

void displayOilTemp(){
    tft.setCursor(680, 0);  
    tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
    tft.write("14.0");
}

bool checkOilTempChange(){
  return true;
}

//Get string data into the struct. 

void readInput(int howMany){
  String data = "";
  static int count = 0;
  while(Wire.available() > 0){
    char c = Wire.read();
    data += c;
  }
  String formattedData = "";
  if(data[0] == '{'){
    for(int i = 0; i < data.length(); i++)//Get rid of {} chars.
      if(data[i] != '{' && data[i] != '}')
        formattedData += data[i];
    data = formattedData;
  }
  else{
    String val = "";
    for(int i = 1; i < data.length(); i++)
      val += (String)data[i];
    if(data[0] == 'x'){
      DATA[25].key = "x";
      DATA[25].value = val;
    }
    else if(data[0] == 'y'){
      DATA[26].key = "y";
      DATA[26].value = val;
    }
    else if(data[0] == 'z'){
      DATA[27].key = "z";
      DATA[27].value = val;
    }
  }
  
  char sep = ':';
  char dataSep  = ',';
  String keys[CHANNELS];
  String vals[CHANNELS];
  int j = 0;
  int i = 0;
  String key = "";
  String value = "";
  
  for(; i < CHANNELS; i++){
    key = "";
    value = "";
    bool flag = 0; //starting at a key
    while(data[j] != sep && data[j] != '\0'){ //Just get the key
      if(data[j] == dataSep || data[j] == ' ' || data[j] == sep)
        j++;
      key += data[j];
      j++;
    }
    keys[i] = key;
    while(data[j] != dataSep && data[j] != '\0'){//Get the value{
      if(data[j] == sep || data[j] == ' ' || data[j] == dataSep)
        j++;
      value += data[j];
      j++;
    }
    j++;//To remove spaces
    vals[i] = value;
  }
  
  while(count < i){
    DATA[count].key = keys[count];
    DATA[count].value = vals[count];
    count++;
  }
  if(count == 24)
    count = 0;
}

//LED CODE

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
