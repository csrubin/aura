#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#ifdef ESP32
    #include <WiFi.h>
#else
    #include <ESP8266WiFi.h>
#endif
#include "fauxmoESP.h"
#include <IRsend.h>    // IR library 
#include <IRrecv.h>
#include <IRutils.h>
#include <debug.h>
#define PIN 14
//
//// Parameter 1 = number of pixels in strip
//// Parameter 2 = pin number (most are valid)
//// Parameter 3 = pixel type flags, add together as needed:
////   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
////   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
////   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
////   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(14, PIN, NEO_GRB + NEO_KHZ800);

    
// Rename the credentials.sample.h file to credentials.h and 
// edit it according to your router configuration
#include "credentials.h"

fauxmoESP fauxmo;

// -----------------------------------------------------------------------------

#define SERIAL_BAUDRATE     115200

#define ID_TVS       "both tvs"
#define ID_LGTV       "LG"
#define ID_LGMUTE     "LG mute"
#define ID_LGVOLUP    "LG volume up"
#define ID_LGVOLDOWN  "LG volume down"

#define ID_SAMTV       "Samsung"
#define ID_SAMMUTE     "Samsung mute"
#define ID_SAMVOLUP    "Samsung volume up"
#define ID_SAMVOLDOWN  "Samsung volume down"

const uint16_t kIrLed = 5;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).

int lgpower = 0x0020DF10EF;
int lgup    = 0x0020DF40BF;
int lgdown  = 0x0020DFC03F;
int lgmute  = 0x0020DF906F;
int input = 0x0020DFD02F;

int sampower = 0x00E0E040BF;
int samup    = 0x00E0E0E01F;
int samdown  = 0x00E0E0D02F;
int sammute  = 0x00E0E0F00F;

IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.
//IRrecv irrecv(kRecvPin);

// -----------------------------------------------------------------------------
//turn on/off the tv by sending IR command
void toggleLG(int);
void toggleSam(int);
void volumeLG(int, unsigned char);
void volumeSam(int, unsigned char);
void flash(int);

// -----------------------------------------------------------------------------
// Wifi
// -----------------------------------------------------------------------------

void wifiSetup() {

    // Set WIFI module to STA mode
    WiFi.mode(WIFI_STA);

    // Connect
    Serial.printf("[WIFI] Connecting to %s ", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    // Wait
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(100);
    }
    Serial.println();

    // Connected!
    Serial.printf("[WIFI] STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());

}

void setup() {
  
    strip.begin();
    strip.setBrightness(100);
    strip.show(); // Initialize all pixels to 'off'
    for(int16_t j=0; j<strip.numPixels(); j++){
      strip.setPixelColor(j,0,0,0);
      strip.show();
    }
    
    // Init serial port and clean garbage
    Serial.begin(SERIAL_BAUDRATE);
    Serial.println();
    Serial.println();

    //initialize the IR 
    IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.
    //IRrecv irrecv(kRecvPin);
    irsend.begin();
    //irrecv.enableIRIn();  // Start the receiver

    // Wifi
    wifiSetup();

    // By default, fauxmoESP creates it's own webserver on the defined port
    // The TCP port must be 80 for gen3 devices (default is 1901)
    // This has to be done before the call to enable()
    fauxmo.createServer(true); // not needed, this is the default value
    fauxmo.setPort(80); // This is required for gen3 devices

    // You have to call enable(true) once you have a WiFi connection
    // You can enable or disable the library at any moment
    // Disabling it will prevent the devices from being discovered and switched
    fauxmo.enable(true);

    // You can use different ways to invoke alexa to modify the devices state:
    // "Alexa, turn yellow lamp on"
    // "Alexa, turn on yellow lamp
    // "Alexa, set yellow lamp to fifty" (50 means 50% of brightness, note, this example does not use this functionality)

    // Add virtual devices
    fauxmo.addDevice(ID_LGTV     );
    fauxmo.addDevice(ID_LGMUTE   );
    fauxmo.addDevice(ID_LGVOLUP  );
    fauxmo.addDevice(ID_LGVOLDOWN);
    fauxmo.addDevice(ID_SAMTV     );
    fauxmo.addDevice(ID_SAMMUTE   );
    fauxmo.addDevice(ID_SAMVOLUP  );
    fauxmo.addDevice(ID_SAMVOLDOWN);
    fauxmo.addDevice(ID_TVS);

    fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state, unsigned char value) {
        
        // Callback when a command from Alexa is received. 
        // You can use device_id or device_name to choose the element to perform an action onto (relay, LED,...)
        // State is a boolean (ON/OFF) and value a number from 0 to 255 (if you say "set kitchen light to 50%" you will receive a 128 here).
        // Just remember not to delay too much here, this is a callback, exit as soon as possible.
        // If you have to do something more involved here set a flag and process it in your main loop.
        
        Serial.printf("[MAIN] Device #%d (%s) state: %s value: %d\n", device_id, device_name, state ? "ON" : "OFF", value);

        // Checking for device_id is simpler if you are certain about the order they are loaded and it does not change.
        // Otherwise comparing the device_name is safer.

        if (strcmp(device_name, ID_LGTV)==0) {
            toggleLG(lgpower);
        } else if (strcmp(device_name, ID_LGMUTE)==0) {
            toggleLG(lgmute);
        } else if (strcmp(device_name, ID_LGVOLUP)==0) {
            volumeLG(lgup, value);
        } else if (strcmp(device_name, ID_LGVOLDOWN)==0) {
            volumeLG(lgdown, value);
        } else if (strcmp(device_name, ID_SAMTV)==0) {
            toggleSam(sampower);
        } else if (strcmp(device_name, ID_SAMMUTE)==0) {
            toggleSam(sammute);
        } else if (strcmp(device_name, ID_SAMVOLUP)==0) {
            volumeSam(samup, value);
        } else if (strcmp(device_name, ID_SAMVOLDOWN)==0) {
            volumeSam(samdown, value);
        } else if (strcmp(device_name, ID_TVS)==0) {
            toggleSam(sampower);
            delay(50);
            toggleLG(lgpower);
        }
    });

}

void loop() {

    // fauxmoESP uses an async TCP server but a sync UDP server
    // Therefore, we have to manually poll for UDP packets
    fauxmo.handle();

    // This is a sample code to output free heap every 5 seconds
    // This is a cheap way to detect memory leaks
    static unsigned long last = millis();
    if (millis() - last > 5000) {
        last = millis();
        Serial.printf("[MAIN] Free heap: %d bytes\n", ESP.getFreeHeap());
    }

    // If your device state is changed by any other means (MQTT, physical button,...)
    // you can instruct the library to report the new state to Alexa on next request:
    // fauxmo.setState(ID_YELLOW, true, 255);

}

void toggleLG(int code)
{
  Serial.println("Sending IR toggle command to LG");  
  irsend.sendNEC(code, 32); 
  flash(4);
}

void toggleSam(int code)
{
  Serial.println("Sending IR toggle command to Samsung"); 
  irsend.sendSAMSUNG(code, 32); 
  flash(4);
}

void volumeLG(int code, unsigned char value)
{
  if (value != 255)
  {
    Serial.println("Sending IR command to change LG volume"); 
    int count = ( int(value) * 100 )/255;
    for(int i=0;i<count;i++)
    {
      irsend.sendNEC(code, 32);
      delay(10);
    } 
    
  }
  flash(4);
}

void volumeSam(int code, unsigned char value)
{
  if (value != 255)
  {
    Serial.println("Sending IR command to change LG volume"); 
    int count = ( int(value) * 100 )/255;
    for(int i=0;i<count;i++)
    {
      irsend.sendSAMSUNG(code, 32, 2);
      delay(50);
    } 
  }
  flash(4);
}

void flash(int skip){
  for(int16_t i=0; i<255-skip; i+=skip){
    for(int16_t j=0; j<strip.numPixels(); j++){
      strip.setPixelColor(j,0,0,i);
      strip.show();
    }}
    for(int16_t j=0; j<strip.numPixels(); j++){
      strip.setPixelColor(j,0,0,255);
      strip.show();
    }
      for(int16_t i=255; i>skip; i=i-skip){
    for(int16_t j=0; j<strip.numPixels(); j++){
      strip.setPixelColor(j,0,0,i);
      strip.show();
    }}
    for(int16_t j=0; j<strip.numPixels(); j++){
      strip.setPixelColor(j,0,0,0);
      strip.show();
    }}
