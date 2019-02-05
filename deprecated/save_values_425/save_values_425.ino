#ifndef UNIT_TEST
#include <Arduino.h>
#endif
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <IRsend.h>

// An IR detector/demodulator is connected to GPIO pin 14(D5 on a NodeMCU
// board).
const uint16_t kRecvPin = 14;
const uint16_t kIrLed = 5;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).
int power;
int up;
int down;
int mute;
int input;

IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.
IRrecv irrecv(kRecvPin);

decode_results results;

void setup() {
  Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
  irsend.begin();
  irrecv.enableIRIn();  // Start the receiver
  while (!Serial)  // Wait for the serial connection to be establised.
    delay(200);
  Serial.println();
  Serial.print("IRrecvDemo is now running and waiting for IR message on Pin ");
  Serial.println(kRecvPin);
}

void loop() {

  
  
  

  Serial.println("hit the power button");
  serialPrintUint64(results.value,HEX);
  while(results.value==0){
    delay(100);
    ESP.wdtFeed();
    yield();
    Serial.println("in the loooooooop");
    if (irrecv.decode(&results)) {
    // print() & println() can't handle printing long longs. (uint64_t)
    serialPrintUint64(results.value,HEX);
    Serial.println("");
    irrecv.resume();  // Receive the next value
  }
    power=results.value;
    
  }
  Serial.print("power: ");
  Serial.println(power);
  results.clear();
    Serial.println("hit the mute button");
  serialPrintUint64(results.value,HEX);
  while(results.value==0){
    delay(100)
    ESP.wdtFeed();
    yield();
    Serial.println("in the loooooooop");
    if (irrecv.decode(&results)) {
    // print() & println() can't handle printing long longs. (uint64_t)
    serialPrintUint64(results.value,HEX);
    Serial.println("");
    irrecv.resume();  // Receive the next value
  }
    mute=results.value;
  }
  Serial.print("mute: ");
  Serial.println(mute);
  results.clear();
  
    Serial.println("hit the up button");
  serialPrintUint64(results.value,HEX);
  while(results.value==0){
    delay(100);
    ESP.wdtFeed();
    yield();
    Serial.println("in the loooooooop");
    if (irrecv.decode(&results)) {
    // print() & println() can't handle printing long longs. (uint64_t)
    serialPrintUint64(results.value,HEX);
    Serial.println("");
    irrecv.resume();  // Receive the next value
  }
    up=results.value;
  }
  Serial.print("up: ");
  Serial.println(up);
  results.clear();

    Serial.println("hit the down button");
  serialPrintUint64(results.value,HEX);
  while(results.value==0){
    delay(100);
    ESP.wdtFeed();
    yield();
    Serial.println("in the loooooooop");
    if (irrecv.decode(&results)) {
    // print() & println() can't handle printing long longs. (uint64_t)
    serialPrintUint64(results.value,HEX);
    Serial.println("");
    irrecv.resume();  // Receive the next value
  }
    down=results.value;
  }
  Serial.print("down: ");
  Serial.println(down);
  results.clear();

    Serial.println("hit the power button");
  serialPrintUint64(results.value,HEX);
  while(results.value==0){
    delay(100);
    ESP.wdtFeed();
    yield();
    Serial.println("in the loooooooop");
    if (irrecv.decode(&results)) {
    // print() & println() can't handle printing long longs. (uint64_t)
    serialPrintUint64(results.value,HEX);
    Serial.println("");
    irrecv.resume();  // Receive the next value
  }
    input=results.value;
  }
  Serial.print("input: ");
  Serial.println(input);
  results.clear();
  
  
  //irsend.sendNEC(0x0020DF10EF,32);
  delay(100);
}
