

#ifndef UNIT_TEST
#include <Arduino.h>
#endif
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <IRsend.h>
#include <FS.h>
#include <ArduinoJson.h>
int power;
int up;
int down;
int mute;
int input;
#define path "/info.json"


// An IR detector/demodulator is connected to GPIO pin 14(D5 on a NodeMCU
// board).
const uint16_t kRecvPin = 14;
const uint16_t kIrLed = 5;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).



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
  if (SPIFFS.begin()) {
  Serial.println("mounted file system");

  // parse json config file
  File jsonFile = GetFile(path);
  if (jsonFile) {
    Serial.println("if(jsonFile)");
    // Allocate a buffer to store contents of the file.
    size_t size = jsonFile.size();
    std::unique_ptr<char[]> jsonBuf(new char[size]);
    jsonFile.readBytes(jsonBuf.get(), size);

    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.parseObject(jsonBuf.get());
    
    if (json.success()) {
      Serial.println("if(json.success())");
      power=json["power"];
      Serial.print("power: ");
      Serial.println(power);
      up=json["up"];
      down=json["down"];
      mute=json["mute"];
      input=json["input"];
    } else {
      power=json["power"];
      up=json["up"];
      down=json["down"];
      mute=json["mute"];
      input=json["input"];
      Serial.println("failed to load json config");
    }
    jsonFile.close();
  }
}
}

void loop() {
  if (irrecv.decode(&results)) {
    // print() & println() can't handle printing long longs. (uint64_t)
    serialPrintUint64(results.value,HEX);
    Serial.println("");
    irrecv.resume();  // Receive the next value
  }
  irsend.sendNEC(power,32);
  Serial.println("turn power on!");
  Serial.print("power: ");
  Serial.println(power);
  delay(2000);

  irsend.sendNEC(up,32);
  Serial.println("turn volume up!");
  Serial.print("up: ");
  Serial.println(up);
  delay(2000);

  irsend.sendNEC(down,32);
  Serial.println("turn volume down!");
  Serial.print("down: ");
  Serial.println(down);
  delay(2000);

  irsend.sendNEC(mute,32);
  Serial.println("turn volume off!");
  Serial.print("mute: ");
  Serial.println(mute);
  delay(2000);

  irsend.sendNEC(input,32);
  Serial.println("change input!");
  Serial.print("input: ");
  Serial.println(input);
  delay(2000); 
}

// file io
File GetFile(String fileName) {
  File textFile;
  if (SPIFFS.exists(fileName)) {
    textFile = SPIFFS.open(fileName, "r");
  }
  return textFile;
}
