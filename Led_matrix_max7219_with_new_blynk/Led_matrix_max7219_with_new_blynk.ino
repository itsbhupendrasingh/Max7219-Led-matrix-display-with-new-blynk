#define BLYNK_TEMPLATE_ID "Template ID"
#define BLYNK_TEMPLATE_NAME "Template Name"
#define BLYNK_AUTH_TOKEN "Token"

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#define BLYNK_PRINT Serial


char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "********";
char pass[] = "********";

// Hardware setup
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 8

#define CLK_PIN   D5 
#define DATA_PIN  D7 
#define CS_PIN    D8 
//#define CS_PIN    15 // or SS

// HARDWARE SPI
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

// Scrolling parameters
uint8_t frameDelay = 25;  // default frame delay value
textPosition_t scrollEffect = PA_LEFT; // default scroll effect is left
uint8_t scrollSpeed = 5; // default scroll speed value (change as needed)
bool scrollDirectionLeft = false; // flag to determine the scroll direction
bool invertDisplay = false; // flag to determine if the display is inverted
uint8_t brightnessLevel = 0; // default brightness level (range: 1-15)

// Global message buffers shared by Blynk and Scrolling functions
#define BUF_SIZE  512
char curMessage[BUF_SIZE];
char newMessage[BUF_SIZE];
bool newMessageAvailable = false;


void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  delay(1000);
  P.begin();
  P.displayClear();
  P.displaySuspend(false);

  delay(1000);
  Blynk.begin(auth, ssid, pass);
  delay(2000);
  P.displayScroll(curMessage, scrollEffect, scrollDirectionLeft ? PA_SCROLL_RIGHT : PA_SCROLL_LEFT, frameDelay);

  // Register a virtual pin handler for receiving messages from Blynk
  Blynk.virtualWrite(V0, "Hi, You are Connected with NuttyFi Wifi Board!");

  // Set up the initial message as the IP address
  sprintf(curMessage, "Connected with Blynk2.0 Cloud!");
  Serial.println(curMessage);
}

void loop() {
  // Run Blynk
  Blynk.run();

  // Check if a new message is available and update the display
  if (newMessageAvailable) {
    strcpy(curMessage, newMessage);
    newMessageAvailable = false;
    P.displayReset();
  }

  // Animate the display
  if (P.displayAnimate()) {
    if (newMessageAvailable) {
      strcpy(curMessage, newMessage);
      newMessageAvailable = false;
    }
    P.displayReset();
  }
}

// Blynk virtual pin handler for receiving messages
BLYNK_WRITE(V0) {
  strncpy(newMessage, param.asString(), BUF_SIZE);
  newMessageAvailable = true;
}

// Blynk virtual pin handler for receiving scroll speed
BLYNK_WRITE(V4) {
  scrollSpeed = param.asInt();
  frameDelay = map(scrollSpeed, 0, 130, 130, 0);
  P.setSpeed(frameDelay); // Update the scroll speed
}

// Blynk virtual pin handler for receiving scroll direction
BLYNK_WRITE(V2) {
  scrollDirectionLeft = param.asInt();
  P.displayClear();
  P.displayScroll(curMessage, scrollEffect, scrollDirectionLeft ? PA_SCROLL_RIGHT : PA_SCROLL_LEFT, frameDelay);
}

// Blynk virtual pin handler for receiving invert display
BLYNK_WRITE(V1) {
  invertDisplay = param.asInt();
  P.displayClear();
  P.setInvert(invertDisplay);
}

// Blynk virtual pin handler for receiving brightness level
BLYNK_WRITE(V3) {
  brightnessLevel = param.asInt();
  P.setIntensity(brightnessLevel); // Set the brightness level
}
