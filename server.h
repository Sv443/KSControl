#pragma once

#ifndef String
  #include <Arduino.h>
#endif

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP_EEPROM.h>

#include "settings.h"

struct AcState
{
  bool enabled;
  unsigned int temp;
  String mode;
  String fan;
};

// server
void initServer();
void handleClient();
void respond(int status, String body);
void respondError(int status, String message);

// IR
void sendIR(AcState state);

// EEPROM
void setState(AcState state);
AcState getState();

// routes
void handleSetState();
