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

#define EEPROM_SIZE 29

// AC state (29B)
#define EADDR_STATE 0

// ------------------------
// bool   - 1 byte
// int    - 4 bytes
// uint   - 4 bytes
// long   - 8 bytes
// String - 12 bytes
// struct - add sizes

// server
void initServer();
void handleClient();
void respond(int status, String body);
void respondError(int status, String message);

// IR
int getIRData(AcState state);
void sendIR(AcState state);

// EEPROM
void setState(AcState state);
AcState getState();

// routes
void handleSetState();
