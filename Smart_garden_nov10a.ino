#include "arduino_secrets.h"
#include "thingProperties.h"
#include <Arduino_MKRIoTCarrier.h>
#include <Pushsafer.h>
#include <WiFiClient.h>
MKRIoTCarrier carrier;

int moistPin = A5;
String lightState;

uint32_t lightsOn = carrier.leds.Color(82, 118, 115);
uint32_t lightsOff = carrier.leds.Color(0, 0, 0);

char PushsaferKey[] = SECRET_KEY;

WiFiClient client;
Pushsafer pushsafer(PushsaferKey, client);

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
  CARRIER_CASE = false;
  carrier.begin();
  carrier.display.setRotation(0);
  setupStartScreen();
  delay(1500);
}

void loop() {
  ArduinoCloud.update();
  carrier.Buttons.update();

  temperature = carrier.Env.readTemperature();
  humidity = carrier.Env.readHumidity();

  int raw_moisture = analogRead(moistPin);
  moisture = map(raw_moisture, 0, 1023, 100, 0);

  while (!carrier.Light.colorAvailable()) {
    delay(5);
  }
  int none;
  carrier.Light.readColor(none, none, none, light);

  if (carrier.Buttons.onTouchDown(TOUCH0)) {
    // display measured info
    setupInfoScreen();
  }

  if (carrier.Buttons.onTouchDown(TOUCH1)) {
    // display latest plant_report
    carrier.display.fillScreen(ST77XX_BLACK);
    carrier.display.setTextColor(ST77XX_WHITE);
    carrier.display.setTextSize(2);

    carrier.display.setCursor(20, 110);
    carrier.display.print(plant_report);
  }

  if (carrier.Buttons.onTouchDown(TOUCH2)) {
    carrier.display.fillScreen(ST77XX_BLACK);
    carrier.display.setTextColor(ST77XX_WHITE);
    carrier.display.setTextSize(2);

    carrier.display.setCursor(30, 110);
    carrier.display.print("Moisture: ");
    carrier.display.print(moisture);
  }

  if (carrier.Buttons.onTouchDown(TOUCH3)) {
    carrier.display.fillScreen(ST77XX_BLACK);
    carrier.display.setTextColor(ST77XX_WHITE);
    carrier.display.setTextSize(2);

    carrier.display.setCursor(30, 110);
    carrier.display.print("Light: ");
    carrier.display.print(light);
  }
  
  if (carrier.Buttons.onTouchDown(TOUCH4)) {
    if (artificial_light == true) {
      artificial_light = false;
    } else {
      artificial_light = true;
    }
    onArtificialLightChange();
  }
  
  checkConditions();

  delay(100);
}


void onArtificialLightChange() {
  if (artificial_light == true) {
    carrier.leds.fill(lightsOn, 0, 5);
    carrier.leds.show();
  } else {
    carrier.leds.fill(lightsOff, 0, 5);
    carrier.leds.show();
  }
}

void setupInfoScreen() {
  carrier.display.fillScreen(ST77XX_BLACK);
  carrier.display.setTextColor(ST77XX_WHITE);
  carrier.display.setTextSize(2);

  carrier.display.setCursor(40, 80);
  carrier.display.print("Temp: ");
  carrier.display.print(temperature);
  carrier.display.print(" C");
  carrier.display.setCursor(40, 100);
  carrier.display.print("Humid: ");
  carrier.display.print(humidity);
  carrier.display.print(" %");
  carrier.display.setCursor(40, 120);
  carrier.display.print("Moist: ");
  carrier.display.print(moisture);
  carrier.display.setCursor(40, 140);
  carrier.display.print("Light: ");
  carrier.display.print(light);
}

void setupStartScreen() {
  carrier.display.fillScreen(ST77XX_BLACK);
  carrier.display.setTextColor(ST77XX_WHITE);
  carrier.display.setTextSize(2);

  carrier.display.setCursor(70, 70);
  carrier.display.print("Hey there!");
  carrier.display.setCursor(70, 90);
  carrier.display.print("Check plants");
  carrier.display.setCursor(70, 110);
  carrier.display.print("state from");
  carrier.display.setCursor(70, 130);
  carrier.display.print("left bottom");
  carrier.display.setCursor(70, 150);
  carrier.display.print("<- button.");
}

void checkConditions() {
  if (moisture < 20) {
    plant_report = "Give me water.";
    pushMessage(plant_report);
    
  } else if (temperature < 16) {
    plant_report = "It's too cold.";
    pushMessage(plant_report);
    
  } else if (temperature > 30) {
    plant_report = "It's too hot.";
    pushMessage(plant_report);
    
  } else if (light >= 200) {
    plant_report = "Too much light.";
    pushMessage(plant_report);
    
  } else if (light < 10) {
    plant_report = "Give me light.";
    pushMessage(plant_report);
    
  } else if (humidity < 15) {
    plant_report = "Not humid enough.";
    pushMessage(plant_report);
    
  } else {
    plant_report = "Plant is doing fine!";
  }
  
}

void pushMessage(String message) {
  struct PushSaferInput input;
  input.title = "Hey there!";
  input.message = message;
  input.sound = "";
  input.vibration = "1";
  input.icon = "1";
  input.iconcolor = "#FFCCCC";
  input.priority = "1";
  input.device = "a";
  input.url = "";
  input.urlTitle = "";
  input.picture = "";
  input.picture2 = "";
  input.picture3 = "";
  input.time2live = "";
  input.retry = "";
  input.expire = "";
  input.answer = "";
  
  Serial.println(pushsafer.sendEvent(input));
  Serial.println("Sent");
}


