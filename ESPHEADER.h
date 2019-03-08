#ifndef ESPHEADER_h
#define ESPHEADER_h
#include "Arduino.h"
#include <OneWire.h>
#include <WiFi.h>
#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h" // legacy include: `#include "SSD1306.h"
#include <WiFiClient.h>
#include <FS.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <HTTPClient.h>
#include <Adafruit_INA219.h>
#include "driver/adc.h"
extern "C" {
  #include "freertos/FreeRTOS.h"
  #include "freertos/timers.h"
}
#include <AnalogPHMeter.h>
#include <EEPROM.h>
#include <ESPmDNS.h>
#include <PubSubClient.h>
#include <AsyncMqttClient.h>
#include "images.h"
#include "DATACLASS.h"
#endif
