#include "Freenove_WS2812_Lib_for_ESP32.h"
#include "ac.h"
#include "const.h"
#include <ESPAsyncWebServer.h>
#include <IRremoteESP8266.h>
#include <WiFi.h>

// Initialize the rgb led on pin 18
Freenove_ESP32_WS2812 strip = Freenove_ESP32_WS2812(1, 18, 0, TYPE_GRB);

AsyncWebServer server(SERVERHOST);
ClimateControlImpl ac;

/**
 * Sets the LED strip color by choosing from an index of predefined colors.
 * @param index: The index of the color in m_color array.
 */
void selectColor(int index) {
  strip.setBrightness(10);
  for (int i = 0; i < 1; i++) {
    strip.setLedColorData(i, M_COLOR[index][0], M_COLOR[index][1],
                          M_COLOR[index][2]);
    strip.show();
    delay(100);
  }
}

/**
 * Initializes the service by setting up the serial port, LED strip and WiFi
 * connection.
 */
void init_service() {
  delay(10);
  Serial.begin(115200);
  strip.begin();
  delay(100);
  strip.setBrightness(10);
  delay(100);
  selectColor(0);
  delay(100);
  WiFi.begin(SSID, WIFI_PW);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  selectColor(1);
  Serial.println(WiFi.localIP());
}

/**
 * The setup function runs once when you press reset or power the board.
 * It is used to initialize variables, input and output pin modes, and to set up
 * libraries.
 */
void setup() {
  init_service();

  // Setup endpoint for controlling AC
  server.on("/setac", HTTP_POST, [](AsyncWebServerRequest *request) {
    int params = request->params();
    String param;
    Serial.println();
    for (int i = 0; i < params; i++) {
      AsyncWebParameter *p = request->getParam(i);
      if (p->isPost()) {
        param = p->name().c_str();
        Serial.printf("param: %s -> %s; ", param, p->value().c_str());
        if (param == "state")
          ac.stateFromString(p->value().c_str());
        else if (param == "mode")
          ac.modeFromString(p->value().c_str());
        else if (param == "temp")
          ac.setTemp(p->value().toInt());
        else if (param == "fanspeed")
          ac.modeFromString(p->value().c_str());
        else
          Serial.printf("invalid param: %s -> %s", param, p->value().c_str());
      }
    }
    ac.sendCmd();
    request->send(200, "text/plain", "OK");
  });

  server.begin();
}

// Nope....
void loop() {}
