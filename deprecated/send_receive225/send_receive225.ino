

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
int power=0x0020DF10EF;
int up=0x0020DF40BF;
int down=0x0020DFC03F;
int mute=0x0020DF906F;
int input=0x0020DFD02F;


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
  //prepareJSONfromFS();
}

void loop() {
  if (irrecv.decode(&results)) {
    // print() & println() can't handle printing long longs. (uint64_t)
    serialPrintUint64(results.value,HEX);
    Serial.println("");
    irrecv.resume();  // Receive the next value
  }
  //irsend.sendNEC(power,32);
  Serial.println("turn power on!");
  delay(1000);

  //irsend.sendNEC(up,32);
  Serial.println("turn volume up!");
  delay(1000);

  //irsend.sendNEC(down,32);
  Serial.println("turn volume down!");
  delay(1000);

  //irsend.sendNEC(mute,32);
  Serial.println("turn volume off!");
  delay(1000);

  //irsend.sendNEC(input,32);
  Serial.println("change input!");
  delay(1000); 
}

//

