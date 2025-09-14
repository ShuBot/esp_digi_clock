/*
 
*/
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

#include <WiFi.h>
#include "time.h"

#define SERIAL_DEBUG 1
#if SERIAL_DEBUG
#define PRINT(...) Serial.print(__VA_ARGS__)
#define PRINTS(...) Serial.println(__VA_ARGS__)
#endif
// Defining size, and output pins
// GPIO18	CLK

// GPIO23	DIN
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES   4   // Total number of LED modules connected in parallel.
#define CS_PIN        5   // GPIO5	CS
// #define DM_INTENSITY  0   // Dot-Matrix LED Intensity Range: 0-15
int DM_INTENSITY = 0;

MD_Parola Display = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

const char* ssid     = "ShuBot11";
const char* password = "mochubabu99";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 19800;      // GMT+5:30 Offset time for IST
const int   daylightOffset_sec = 0;
unsigned long startTime = 0;

uint8_t scrollSpeed = 75;    // default frame delay value - 25
textEffect_t scrollEffect = PA_SCROLL_DOWN;
textPosition_t scrollAlign = PA_CENTER;
uint16_t scrollPause = 60000; // in milliseconds

// Global message buffers shared by Serial and Scrolling functions
#define	BUF_SIZE	10
char curMessage[BUF_SIZE] = { "xxx" };
char waitMessage[BUF_SIZE] = { "-----" };
char newMessage[BUF_SIZE] = { "Hello!" };
bool newMessageAvailable = true;

void readSerial(void)
{
  static char *cp = newMessage;

  while (Serial.available())
  {
    *cp = (char)Serial.read();
    if ((*cp == '\n') || (cp - newMessage >= BUF_SIZE-2)) // end of message character or full buffer
    {
      *cp = '\0'; // end the string
      // restart the index for next filling spree and flag we have a message waiting
      cp = newMessage;
      newMessageAvailable = true;
    }
    else  // move char pointer to next position
      cp++;
  }
}

void setup(){
  Display.begin();
  Display.setIntensity(DM_INTENSITY);
  Display.displayText(curMessage, scrollAlign, scrollSpeed, scrollPause, scrollEffect, scrollEffect);
  // Display.displayClear();

  if(SERIAL_DEBUG) {Serial.begin(115200);}
  // Connect to Wi-Fi
  PRINT("Connecting to ");
  PRINTS(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    PRINT(".");
  }
  PRINTS("");
  PRINTS("WiFi connected.");
  
  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();
  
  startTime = millis();
  //disconnect WiFi as it's no longer needed
  // WiFi.disconnect(true);
  // WiFi.mode(WIFI_OFF);
}

void loop(){

  if (Display.displayAnimate())
  {
      Display.displayReset();
      printLocalTime();
      delay(10);
    
  }
  // readSerial();
}

void printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    PRINTS("Failed to obtain time");
    strcpy(curMessage, waitMessage);
  } else {
    PRINTS("Local time obtained successfully");
    PRINTS(&timeinfo, "%A, %B %d %Y %H:%M:%S");
    
    // Change DM_INTENSITY as per hour of day.
    if(timeinfo.tm_hour >= 6 && timeinfo.tm_hour <= 17) {
      DM_INTENSITY = 5;
    } else {
      DM_INTENSITY = 0;
    }
    Display.setIntensity(DM_INTENSITY);
    
    strftime(newMessage, BUF_SIZE, "%H:%M", &timeinfo);
    strcpy(curMessage, newMessage);
  }

  // PRINT("Day of week: ");
  // PRINTS(&timeinfo, "%A");
  // PRINT("Month: ");
  // PRINTS(&timeinfo, "%B");
  // PRINT("Day of Month: ");
  // PRINTS(&timeinfo, "%d");
  // PRINT("Year: ");
  // PRINTS(&timeinfo, "%Y");
  // PRINT("Hour: ");
  // PRINTS(&timeinfo, "%H");
  // PRINT("Hour (12 hour format): ");
  // PRINTS(&timeinfo, "%I");
  // PRINT("Minute: ");
  // PRINTS(&timeinfo, "%M");
  // PRINT("Second: ");
  // PRINTS(&timeinfo, "%S");
  // int hour = timeinfo.tm_hour;

  // sprintf(newMessage, "Time is Money");  :%S
  // delay(2000);
  // strftime(newMessage, BUF_SIZE, "%d.%m.%Y", &timeinfo);
  // strcpy(curMessage, newMessage);
}
