#include "arduino_secrets.h"
#include "thingProperties.h"
#include <Arduino_MKRIoTCarrier.h>
MKRIoTCarrier carrier;

int moistPin = A5;
String lightState;

uint32_t lightsOn = carrier.leds.Color(82, 118, 115);
uint32_t lightsOff = carrier.leds.Color(0, 0, 0);

void setup() {
    Serial.begin(9600);
    delay(1500);
   
    initProperties();
   
    ArduinoCloud.begin(ArduinoIoTPreferredConnection);
    setDebugMessageLevel(2);
    ArduinoCloud.printDebugInfo();
   
    while (ArduinoCloud.connected() != 1) {
      ArduinoCloud.update();
      delay(500);
    }
   
    delay(500);
    CARRIER_CASE = true;
    carrier.begin();
    carrier.display.setRotation(0);
    delay(1500);
}

void loop() {
    ArduinoCloud.update();
    
    temperature = carrier.Env.readTemperature();
    humidity = carrier.Env.readHumidity();
    
    int raw_moisture = analogRead(moistPin);
    moisture = map(raw_moisture, 0, 1023, 100, 0);
   
    while (!carrier.Light.colorAvailable()) {
      delay(5);
    }
    int none;
    carrier.Light.readColor(none, none, none, light);
   
    delay(100);
}

	
void onArtificialLightChange() {
  if (artificial_light == true) {
    carrier.leds.fill(lightsOn, 0, 5);
    carrier.leds.show();
    lightState = "LIGHTS: ON";
  } else {
    carrier.leds.fill(lightsOff, 0, 5);
    carrier.leds.show();
    lightState = "LIGHTS: OFF";
  }
  updateScreen();
}

	
void updateScreen() {
  carrier.display.fillScreen(ST77XX_BLACK);
  carrier.display.setTextColor(ST77XX_WHITE);
  carrier.display.setTextSize(3);
 
  carrier.display.setCursor(40, 130);
  carrier.display.print(lightState);
}



