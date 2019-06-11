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
#define RA8875_CS 10
#define RA8875_RESET 9
#define CHANNELS 27 //Subject to change
///*LEGEND for incoming data
// * Fastest way to check incoming signals is to use one char to indicate the signal from
// * DAQ
// * x = x accel              -- CHANNEL 0   
// * y = y accel              -- CHANNEL 1
// * z = z accel              -- CHANNEL 2
// * r = rpm                  -- CHANNEL 3
// * s = speed                -- CHANNEL 4
// * l = latitude             -- CHANNEL 5
// * g = longitude            -- CHANNEL 6
// * o = oiltemp              -- CHANNEL 7
// * p = oilpressure          -- CHANNEL 8
// * t = enginetemp           -- CHANNEL 9
// * F = FuelTemp             -- CHANNEL 10
// * A = AFR                  -- CHANNEL 11
// * T = TPS                  -- CHANNEL 12
// * I = IAT                  -- CHANNEL 13
// * M = MAP                  -- CHANNEL 14
// * B = Battery              -- CHANNEL 15
// * 1 = Front Left           -- CHANNEL 16
// * 2 = Front Right          -- CHANNEL 17
// * 3 = Back Left            -- CHANNEL 18
// * 4 = Back Right           -- CHANNEL 19
// * a = Front Brake          -- CHANNEL 20
// * b = Rear Brake           -- CHANNEL 21
// */
// /* LEGEND for what colours/warnings mean 
//  *  RED =
//  *  BLUE =
//  *  GREEN =
//  */

RA8875 tft = RA8875(RA8875_CS, RA8875_RESET);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, PIN, NEO_GRBW + NEO_KHZ800); 

typedef struct{
  String value;
}Data;

Data DATA[CHANNELS]; //Data structure for storing all the data
//For Testing Only
int rpm = 0; //End Testing Only
static int whichDisplay = 0; //1 == main display //0 == second display
int buttonPin = 2;

void setup() {
    Serial.begin(115200);
    Serial.println("<Arduino is ready>"); /* Initialise the display using 'RA8875_480x272' or 'RA8875_800x480' */
    //Setup for Adafruit_RA8875
    Wire.begin(8);
    Wire.onReceive(readInput);
    tft.begin(RA8875_800x480);
    tft.displayOn(true);
    tft.backlight(true);
    //tft.begin(Adafruit_800x480);
    tft.GPIOX(true);      // Enable TFT - display enable tied to GPIOX
    tft.setRotation(2);
    strip.begin();
    strip.show();
    strip.setBrightness(5);
    pinMode(buttonPin, INPUT);
}
void setupMainDisplayLabels(){ //Just setup for all the labels
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
void setupSecondDisplayLabels(){ //Just setup for all the labels
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
bool state = true;
void loop(){
  if(state){
      setupMainDisplayLabels();
      displayPrimaryData(); //Main Display
  }
  else{
      setupSecondDisplayLabels();
      displaySecondaryData(); //Secondary Display
  }
  checkButton();
}

int lastButtonState = LOW;
int buttonState;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;
unsigned long firstTime = 0;
unsigned long timeHeld = 0;
bool wait = false;

int checkButton(){
  int reading = digitalRead(buttonPin);
  if(reading == HIGH && lastButtonState == LOW)
    firstTime = millis();
  timeHeld = millis() - firstTime;
  if(reading == LOW && lastButtonState == HIGH){
  if(timeHeld < 400){
      tft.clearActiveWindow();
      tft.clearMemory();
      tft.begin(RA8875_800x480);
      tft.GPIOX(true);      // Enable TFT - display enable tied to GPIOX
      tft.setRotation(2);
      delay(1000);
      state = !state;
    }
    else if(timeHeld > 3000)
      Serial.println("3 second hold");
  }
  lastButtonState = reading;
}

void displayPrimaryData(){ //Dispays data for the main driving display
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
    LED(DATA[3]);
}

void displayGear(){ //Just for testing
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

void displayWaterTemp(){//This function just for testing
    tft.setCursor(0, 0);  
    tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
    tft.write("55.0");
}

void displayKPH(){ //This function just for testing
    tft.setFontScale(3); 
    tft.setCursor(30, 230);  
    tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
    tft.write("125");
    tft.setFontScale(2); 
}

void displayRPM(){ //This function just for testing
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

void displayOilTemp(){ //This function just for testing
    tft.setCursor(680, 0);  
    tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
    tft.write("14.0");
}

void displaySecondaryData(){//Displays all data on the secondary display
    //AccelX
    char buf[10];
    tft.setCursor(630, 0);  
    tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
    (DATA[0].value).toCharArray(buf, 5);
    Serial.println(DATA[0].value);
    tft.write(buf);
    //AccelY
    tft.setCursor(630, 40);  
    (DATA[1].value).toCharArray(buf, 5);
    tft.write(buf);
    //AccelZ
    tft.setCursor(630, 80);  
    (DATA[2].value).toCharArray(buf, 5);
    tft.write(buf);
    //Display LAT
    tft.setCursor(600, 120);
    (DATA[5].value).toCharArray(buf, 10);
    tft.write(buf);
    //Display LONG
    tft.setCursor(600, 160);  
    (DATA[6].value).toCharArray(buf, 10);
    tft.write(buf);
    //FrontL
    tft.setCursor(630, 240);  
    (DATA[16].value).toCharArray(buf, 5);
    tft.write(buf);
    //FrontR
    tft.setCursor(630, 280);  
    (DATA[17].value).toCharArray(buf, 5);
    tft.write(buf);
    //RearL
    tft.setCursor(630, 320);  
    (DATA[18].value).toCharArray(buf, 5);
    tft.write(buf);
    //RearR
    tft.setCursor(630, 360);  
    (DATA[19].value).toCharArray(buf, 5);
    tft.write(buf);
    //Front Brake
    tft.setCursor(630, 400);  
    (DATA[20].value).toCharArray(buf, 5);
    tft.write(buf);
    //Rear Brake
    tft.setCursor(630, 440);  
    (DATA[21].value).toCharArray(buf, 5);
    tft.write(buf);
    //Oil P.
    tft.setCursor(130, 0);  
    (DATA[8].value).toCharArray(buf, 5);
    tft.write(buf);
    //Oil T
    tft.setCursor(130, 40);  
    (DATA[7].value).toCharArray(buf, 5);
    tft.write(buf);
    //Engine T
    tft.setCursor(130, 80);  
    (DATA[9].value).toCharArray(buf, 5);
    tft.write(buf);
    //Fuel T
    tft.setCursor(130, 120);  
    (DATA[10].value).toCharArray(buf, 5);
    tft.write(buf);
    //AFR
    tft.setCursor(130, 200);  
    (DATA[11].value).toCharArray(buf, 5);
    tft.write(buf);
    //MAP
    tft.setCursor(130, 240);  
    (DATA[14].value).toCharArray(buf, 5);
    tft.write(buf);
    //TPS
    tft.setCursor(130, 280);  
    (DATA[12].value).toCharArray(buf, 5);
    tft.write(buf);
    //IAT
    tft.setCursor(130, 320);  
    (DATA[13].value).toCharArray(buf, 5);
    tft.write(buf);
    //Battery
    tft.setCursor(140, 400);  
    (DATA[15].value).toCharArray(buf, 6);
    tft.write(buf); 
}

//Get Data and store
void readInput(int howMany){ //Get all data and store in the data structure as string values
  String data = "";
  while(Wire.available() > 0){
    char c = Wire.read();
    data += c;
  }
  String val = "";
  for(int i = 1; i < data.length(); i++)
    val += (String)data[i];
  if(data[0] == 'x')
    DATA[0].value = val;
  else if(data[0] == 'y')
    DATA[1].value = val;
  else if(data[0] == 'z')
    DATA[2].value = val;
  else if(data[0] == 'r')
    DATA[3].value = val;
  else if(data[0] == 's')
    DATA[4].value = val;
  else if(data[0] == 'l'){
    DATA[5].value = "";
    DATA[5].value = val;
  }
  else if(data[0] == 'g'){
    DATA[6].value = "";
    DATA[6].value = val;
  }
  else if(data[0] == 'o')
    DATA[7].value = val;
  else if(data[0] == 'p')
    DATA[8].value = val;
  else if(data[0] == 't')
    DATA[9].value = val;
  else if(data[0] == 'F')
    DATA[10].value = val;
  else if(data[0] == 'A')
    DATA[11].value = val;
  else if(data[0] == 'T')
    DATA[12].value = val;
  else if(data[0] == 'I')
    DATA[13].value = val;
  else if(data[0] == 'M')
    DATA[14].value = val;
  else if(data[0] == 'B')
    DATA[15].value = val;
  else if(data[0] == '1')
    DATA[16].value = val;
  else if(data[0] == '2')
    DATA[17].value = val;
  else if(data[0] == '3')
    DATA[18].value = val;
  else if(data[0] == '4')
    DATA[19].value = val;
  else if(data[0] == 'a')
    DATA[20].value = val;
  else if(data[0] == 'b')
    DATA[21].value = val;
  else if(data[0] == 'L'){//Get extra decimals for latitude
    if(DATA[5].value.length() < 10)//Just so it doesnt add on more because the rates of arrival for data is always slightly different
      DATA[5].value += val;
  }else if(data[0] == 'G'){//Get extra decimals for longitude
    if(DATA[6].value.length() < 10)//Just so it doesnt add on more because the rates of arrival for data is always slightly different
      DATA[6].value += val;
  }
}

//LED CODE
void blockIncrementLED() { //Just for making the blue appear as a block
  for (int i = 10; i < 16; i++)
    strip.setPixelColor(i, 0, 0, 255);
}

void LED (int rpm) {//Actually determines the number of LEDS on based on the RPM value
  int numberoflights = (int)(rpm - 10000) * 0.0064);        
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
