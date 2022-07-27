#include "server.h"

// vars

ESP8266WebServer server(SERVER_PORT);

struct AcState state = {true, 20, "AUTO", "AUTO"};

int modeMapLen = 5;
String modeMap[5] = {"AUTO", "COOL", "DRY", "FAN", "HEAT"};
int fanMapLen = 4;
String fanMap[4] = {"AUTO", "LOW", "MEDIUM", "HIGH"};

// ------------------------- #MARKER server -------------------------

// routes
void handleSetState();

void initServer()
{
  EEPROM.begin(EEPROM_SIZE);

  server.on("/state", handleSetState);

  server.begin();
  Serial.print("Started server on port ");
  Serial.println(SERVER_PORT);
}

void handleClient()
{
  server.handleClient();
}

void respond(int status, String body)
{
  server.send(status, "application/json", body);
}

void respondError(int status, String message)
{
  respond(status, "{\"error\":true,\"message\":\"" + message + "\"}");
}

// ------------------------- #MARKER IR -------------------------

int getIRData(AcState state)
{
  int num = 0x50000000;

  auto addBitFieldVal = [&](int n, int val, int pos){
    return n | (val << pos);
  };

  int mode = 0;
  for(int i = 0; i < modeMapLen; i++)
    if(modeMap[i] == state.mode)
      mode = i;
  num = addBitFieldVal(num, mode, 0);
  // Serial.println("Mode: " + String(mode) + " > " + String(num));

  if(state.enabled)
    num = addBitFieldVal(num, 1, 3);
  // Serial.println("Enabled: " + String(num));

  int fan = 0;
  for(int i = 0; i < fanMapLen; i++)
    if(fanMap[i] == state.fan)
      fan = i;
  num = addBitFieldVal(num, fan, 4);
  // Serial.println("Fan: " + String(fan) + " > " + String(num));

  num = addBitFieldVal(num, state.temp - 16, 8);
  // Serial.println("Temp: " + String(num));

  return num;
}

void sendIR(AcState state)
{
  Serial.print("> Sending state via IR // enabled=");
  Serial.print(state.enabled ? "true" : "false");
  Serial.print("; temp=");
  Serial.print(state.temp);
  Serial.print("; mode=");
  Serial.print(state.mode);
  Serial.print("; fan=");
  Serial.println(state.fan);

  int irData = getIRData(state);

  Serial.println("IR Data: " + String(irData));
}

// ------------------------- #MARKER EEPROM -------------------------

void setState(AcState state)
{
  EEPROM.put(EADDR_STATE, state);
  if(!EEPROM.commit())
    Serial.println("---- Error while updating local state in EEPROM ----");
}

AcState getState()
{
  struct AcState st;
  EEPROM.get(EADDR_STATE, st);
  return st;
}


// ------------------------- #MARKER routes -------------------------

void handleSetState()
{
  if(server.args() < 1)
    return respondError(400, "Please provide URL parameters");

  auto hasArg = [&](String name)
  {
    return server.hasArg(name);
  };

  auto isBetween = [&](int val, int min, int max){
    return val >= min && val <= max;
  };

  auto includes = [&](String name, String val){
    if(name == "mode")
      for(int i = 0; i < modeMapLen; i++)
        if(modeMap[i] == val)
          return true;
    else if(name == "fan")
      for(int i = 0; i < fanMapLen; i++)
        if(fanMap[i] == val)
          return true;
    return false;
  };

  auto argValid = [&](String name)
  {
    if(!hasArg(name))
      return false;

    String val = server.arg(name);

    if(name == "enabled" && (val == "true" || val == "false"))
      return true;
    else if(name == "temp" && isBetween(val.toInt(), 16, 30))
      return true;
    else if(name == "mode" && includes("mode", val))
      return true;
    else if(name == "fan" && includes("fan", val))
      return true;

    return false;
  };

  auto argInvalid = [&](String name)
  {
    return respondError(400, "Argument \\\"" + name + "\\\" is invalid. Please refer to the documentation.");
  };

  if(hasArg("enabled"))
  {
    if(argValid("enabled"))
      state.enabled = server.arg("enabled") == "true";
    else
      return argInvalid("enabled");
  }
  if(hasArg("temp"))
  {
    if(argValid("temp"))
      state.temp = server.arg("temp").toInt();
    else
      return argInvalid("temp");
  }
  if(hasArg("mode"))
  {
    if(argValid("mode"))
      state.mode = server.arg("mode");
    else
      return argInvalid("mode");
  }
  if(hasArg("fan"))
  {
    if(argValid("fan"))
      state.fan = server.arg("fan");
    else
      return argInvalid("fan");
  }

  setState(state);
  sendIR(state);

  respond(200, "{\"message\":\"Success\"}");
}
