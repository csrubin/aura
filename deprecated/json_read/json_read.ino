#include <FS.h>
#include <ArduinoJson.h>

#define INFO "/User/csrubin/Documents/Arduino/data"
int power;
int up;
int down;
int mute;
int input;
#define path "/Users/csrubin/Documents/Arduino/data/info.json"

        
void setup() {
  // put your setup code here, to run once:
if (SPIFFS.begin()) {
  Serial.println("mounted file system");

  // parse json config file
  File jsonFile = GetFile(path);
  if (jsonFile) {
    // Allocate a buffer to store contents of the file.
    size_t size = jsonFile.size();
    std::unique_ptr<char[]> jsonBuf(new char[size]);
    jsonFile.readBytes(jsonBuf.get(), size);

    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.parseObject(jsonBuf.get());
    if (json.success()) {
      power=json["power"];
      up=json["up"];
      down=json["down"];
      mute=json["mute"];
      input=json["input"];
    } else {
      Serial.println("failed to load json config");
    }
    jsonFile.close();
  }
}


}

void loop() {
  // put your main code here, to run repeatedly:
  
}

// file io
File GetFile(String fileName) {
  File textFile;
  if (SPIFFS.exists(fileName)) {
    textFile = SPIFFS.open(fileName, "r");
  }
  return textFile;
}
