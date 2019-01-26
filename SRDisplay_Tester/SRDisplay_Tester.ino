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
 * Fastest way to check incoming signals isnto use one char to indicate the signal
 * DAQ
 * x = x accel              -- CHANNEL 0   
 * y = y accel              -- CHANNEL 1
 * z = z accel              -- CHANNEL 2
 * r = rpm                  -- CHANNEL 3
 * s = speed                -- CHANNEL 4
 * l = latitude             -- CHANNEL 5
 * g = longitude            -- CHANNEL 6
 * o = oiltemp              -- CHANNEL 7
 * p = oilpressure          -- CHANNEL 8
 * t = enginetemp           -- CHANNEL 9
 * F = FuelTemp             -- CHANNEL 10
 * A = AFR                  -- CHANNEL 11
 * T = TPS                  -- CHANNEL 12
 * I = IAT                  -- CHANNEL 13
 * M = MAP                  -- CHANNEL 14
 * B = Battery              -- CHANNEL 15
 * 1 = Front Left           -- CHANNEL 16
 * 2 = Front Right          -- CHANNEL 17
 * 3 = Back Left            -- CHANNEL 18
 * 4 = Back Right           -- CHANNEL 19
 * a = Front Brake          -- CHANNEL 20
 * b = Rear Brake           -- CHANNEL 21
 * u = UTC                  -- CHANNEL 22
 */
RA8875 tft = RA8875(RA8875_CS, RA8875_RESET);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, PIN, NEO_GRBW + NEO_KHZ800); 
typedef struct{String key;String value;}Data;
Data DATA[CHANNELS];
//For Testing Only
int rpm = 0;//End Testing Only
static bool whichDisplay = false;//true == main display//false == second display
void setup() {
    Serial.begin(115200);
    Serial.println("<Arduino is ready>");/* Initialise the display using 'RA8875_480x272' or 'RA8875_800x480' */
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
void setupSecondDisplayLabels(){
    tft.setFontScale(1);
    tft.setCursor(500, 0);  
    tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
    tft.write("AccelX:");
    tft.setCursor(500, 40);  
    tft.write("AccelY:");
    tft.setCursor(500, 80);  
    tft.write("AccelZ:");
    tft.setCursor(500, 120);
    tft.write("Lat:");
    tft.setCursor(500, 160);
    tft.write("Long:");

    tft.setCursor(500, 240);
    tft.write("FrontL:");
    tft.setCursor(500, 280);
    tft.write("FrontR:");
    tft.setCursor(500, 320);
    tft.write("RearL:");
    tft.setCursor(500, 360);
    tft.write("RearR:");
    tft.setCursor(500, 400);
    tft.write("FBrake:");
    tft.setCursor(500, 440);
    tft.write("RBrake:");
    
    tft.setCursor(0, 0);
    tft.write("Oil P:");
    tft.setCursor(0, 40);
    tft.write("Oil T:");
    tft.setCursor(0, 80);
    tft.write("Eng T:");
    tft.setCursor(0, 120);
    tft.write("Fuel T:");
    
    tft.setCursor(0, 200);
    tft.write("AFR:");
    tft.setCursor(0, 240);
    tft.write("MAP:");
    tft.setCursor(0, 280);
    tft.write("TPS:");
    tft.setCursor(0, 320);
    tft.write("IAT:");
    tft.setCursor(0, 400);
    tft.write("Battery:");
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
    displayWaterTemp();
    displayKPH();
    displayOilTemp();
    tft.clearMemory(true);
    if(checkGearChange())
      displayGear();
    LED(rpm);
  }else{
    //AccelX
    char buf[7];
    tft.setCursor(610, 0);  
    tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
    (DATA[0].value).toCharArray(buf, 7);
    tft.write("    ");
    tft.write(buf);
    //AccelY
    tft.setCursor(610, 40);  
    (DATA[1].value).toCharArray(buf, 7);
    tft.write("    ");
    tft.write(buf);
    //AccelZ
    tft.setCursor(610, 80);  
    (DATA[2].value).toCharArray(buf, 7);
    tft.write("    ");
    tft.write(buf);
    //Display LAT
    tft.setCursor(610, 120);
    (DATA[5].value).toCharArray(buf, 7);
    tft.write("    ");
    tft.write(buf);
    //Display LONG
    tft.setCursor(610, 160);  
    (DATA[6].value).toCharArray(buf, 7);
    tft.write("    ");
    tft.write(buf);
    //FrontL
    tft.setCursor(610, 240);  
    (DATA[16].value).toCharArray(buf, 7);
    tft.write("    ");
    tft.write(buf);
    //FrontR
    tft.setCursor(610, 280);  
    (DATA[17].value).toCharArray(buf, 7);
    tft.write("    ");
    tft.write(buf);
    //RearL
    tft.setCursor(610, 320);  
    (DATA[18].value).toCharArray(buf, 7);
    tft.write("    ");
    tft.write(buf);
    //RearR
    tft.setCursor(610, 360);  
    (DATA[19].value).toCharArray(buf, 7);
    tft.write("    ");
    tft.write(buf);
    //Front Brake
    tft.setCursor(610, 400);  
    (DATA[20].value).toCharArray(buf, 7);
    tft.write("    ");
    tft.write(buf);
    //Rear Brake
    tft.setCursor(610, 440);  
    (DATA[21].value).toCharArray(buf, 7);
    tft.write("    ");
    tft.write(buf);
    //Oil P.
    tft.setCursor(110, 0);  
    (DATA[8].value).toCharArray(buf, 7);
    tft.write("    ");
    tft.write(buf);
    //Oil T
    tft.setCursor(110, 40);  
    (DATA[7].value).toCharArray(buf, 7);
    tft.write("    ");
    tft.write(buf);
    //Engine T
    tft.setCursor(110, 80);  
    (DATA[9].value).toCharArray(buf, 7);
    tft.write("    ");
    tft.write(buf);
    //Fuel T
    tft.setCursor(110, 120);  
    (DATA[10].value).toCharArray(buf, 7);
    tft.write("    ");
    tft.write(buf);
    //AFR
    tft.setCursor(110, 200);  
    (DATA[11].value).toCharArray(buf, 7);
    tft.write("    ");
    tft.write(buf);
    //MAP
    tft.setCursor(110, 240);  
    (DATA[14].value).toCharArray(buf, 7);
    tft.write("    ");
    tft.write(buf);
    //TPS
    tft.setCursor(110, 280);  
    (DATA[12].value).toCharArray(buf, 7);
    tft.write("    ");
    tft.write(buf);
    //IAT
    tft.setCursor(110, 320);  
    (DATA[13].value).toCharArray(buf, 7);
    tft.write("    ");
    tft.write(buf);
    //Battery
    tft.setCursor(120, 400);  
    (DATA[15].value).toCharArray(buf, 7);
    tft.write("    ");
    tft.write(buf); 
    //UTC
    tft.setCursor(120, 440);  
    (DATA[22].value).toCharArray(buf, 7);
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
bool checkGearChange(){ //Only update gear if there is a change, otherwise there is too much overhead
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
      DATA[0].key = "x";
      DATA[0].value = val;
    }
    else if(data[0] == 'y'){
      DATA[1].key = "y";
      DATA[1].value = val;
    }
    else if(data[0] == 'z'){
      DATA[2].key = "z";
      DATA[2].value = val;
    }
    else if(data[0] == 'r'){
      DATA[3].key = "r";
      DATA[3].value = val;
    }
    else if(data[0] == 's'){
      DATA[4].key = "s";
      DATA[4].value = val;
    }
    else if(data[0] == 'l'){
      DATA[5].key = "l";
      DATA[5].value = val;
    }
    else if(data[0] == 'g'){
      DATA[6].key = "g";
      DATA[6].value = val;
    }
    else if(data[0] == 'o'){
      DATA[7].key = "o";
      DATA[7].value = val;
    }
    else if(data[0] == 'p'){
      DATA[8].key = "p";
      DATA[8].value = val;
    }
    else if(data[0] == 't'){
      DATA[9].key = "t";
      DATA[9].value = val;
    }
    else if(data[0] == 'F'){
      DATA[10].key = "F";
      DATA[10].value = val;
    }
    else if(data[0] == 'A'){
      DATA[11].key = "A";
      DATA[11].value = val;
    }
    else if(data[0] == 'T'){
      DATA[12].key = "T";
      DATA[12].value = val;
    }
    else if(data[0] == 'I'){
      DATA[13].key = "I";
      DATA[13].value = val;
    }
    else if(data[0] == 'M'){
      DATA[14].key = "M";
      DATA[14].value = val;
    }
    else if(data[0] == 'B'){
      DATA[15].key = "B";
      DATA[15].value = val;
    }
    else if(data[0] == '1'){
      DATA[16].key = "1";
      DATA[16].value = val;
    }
    else if(data[0] == '2'){
      DATA[17].key = "2";
      DATA[17].value = val;
    }
    else if(data[0] == '3'){
      DATA[18].key = "3";
      DATA[18].value = val;
    }
    else if(data[0] == '4'){
      DATA[19].key = "4";
      DATA[19].value = val;
    }
    else if(data[0] == 'a'){
      DATA[20].key = "a";
      DATA[20].value = val;
    }
    else if(data[0] == 'b'){
      DATA[21].key = "b";
      DATA[21].value = val;
    }
  }
  
//  char sep = ':';
//  char dataSep  = ',';
//  String keys[CHANNELS];
//  String vals[CHANNELS];
//  int j = 0;
//  int i = 0;
//  String key = "";
//  String value = "";
//  
//  for(; i < CHANNELS; i++){
//    key = "";
//    value = "";
//    bool flag = 0; //starting at a key
//    while(data[j] != sep && data[j] != '\0'){ //Just get the key
//      if(data[j] == dataSep || data[j] == ' ' || data[j] == sep)
//        j++;
//      key += data[j];
//      j++;
//    }
//    keys[i] = key;
//    while(data[j] != dataSep && data[j] != '\0'){//Get the value{
//      if(data[j] == sep || data[j] == ' ' || data[j] == dataSep)
//        j++;
//      value += data[j];
//      j++;
//    }
//    j++;//To remove spaces
//    vals[i] = value;
//  }
//  
//  while(count < i){
//    DATA[count].key = keys[count];
//    DATA[count].value = vals[count];
//    count++;
//  }
//  if(count == 24)
//    count = 0;
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
