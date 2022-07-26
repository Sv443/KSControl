#include "server.h"

struct AcState state = {};

int modeMapLen = 5;
String modeMap[5] = {"AUTO", "COOL", "DRY", "FAN", "HEAT"};
int fanMapLen = 4;
String fanMap[4] = {"AUTO", "LOW", "MEDIUM", "HIGH"};

// ------------------------- #MARKER server -------------------------

ESP8266WebServer server(SERVER_PORT);

void handleSetState();

void initServer()
{
  state.enabled = true;
  state.temp = 20;
  state.mode = "AUTO";
  state.fan = "AUTO";

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

  int num = 0x50000000;

  auto addBitFieldVal = [=](int val, int pos){
    return num | (val << pos);
  };

  int mode = 0;
  for(int i = 0; i < modeMapLen; i++)
  {
    if(modeMap[i] == state.mode)
      mode = i;
  }
  num = addBitFieldVal(mode, 0);
  Serial.println("Mode: " + String(mode) + " > " + String(num));

  if(state.enabled)
    num = addBitFieldVal(1, 3);
  Serial.println("Enabled: " + String(num));

  int fan = 0;
  for(int i = 0; i < fanMapLen; i++)
  {
    if(fanMap[i] == state.fan)
      fan = i;
  }
  num = addBitFieldVal(fan, 4);
  Serial.println("Fan: " + String(fan) + " > " + String(num));

  num = addBitFieldVal(state.temp - 16, 8);
  Serial.println("Temp: " + String(num));

  Serial.println("IR Data: " + String(num));
}

// ------------------------- #MARKER EEPROM -------------------------

void setState(AcState state)
{

}

AcState getState()
{

}


// ------------------------- #MARKER routes -------------------------

void handleSetState()
{
  if (server.args() < 1)
  {
    respondError(400, "Please provide URL parameters");
    return;
  }

  auto hasArg = [&](String name)
  {
    return server.hasArg(name);
  };

  auto argValid = [&](String name)
  {
    if (!hasArg(name))
      return false;

    String val = server.arg(name);

    auto isBetween = [&](int val, int min, int max)
    {
      return val >= min && val <= max;
    };

    auto includes = [&](String name, String val)
    {
      if (name == "mode")
        for (int i = 0; i < modeMapLen; i++)
        {
          if (modeMap[i] == val)
            return true;
        }
      else if (name == "fan")
        for (int i = 0; i < fanMapLen; i++)
        {
          if (fanMap[i] == val)
            return true;
        }
      return false;
    };

    if (name == "enabled" && (val == "true" || val == "false"))
      return true;
    else if (name == "temp" && isBetween(val.toInt(), 16, 30))
      return true;
    else if (name == "mode" && includes("mode", val))
      return true;
    else if (name == "fan" && includes("fan", val))
      return true;

    return false;
  };

  auto argInvalid = [&](String name)
  {
    return respondError(400, "Argument \\\"" + name + "\\\" is invalid. Please refer to the documentation.");
  };

  if (hasArg("enabled"))
  {
    if (argValid("enabled"))
      state.enabled = server.arg("enabled") == "true";
    else
      argInvalid("enabled");
  }
  if (hasArg("temp"))
  {
    if (argValid("temp"))
      state.temp = server.arg("temp").toInt();
    else
      argInvalid("temp");
  }
  if (hasArg("mode"))
  {
    if (argValid("mode"))
      state.mode = server.arg("mode");
    else
      argInvalid("mode");
  }
  if (hasArg("fan"))
  {
    if (argValid("fan"))
      state.fan = server.arg("fan");
    else
      argInvalid("fan");
  }

  sendIR(state);

  respond(200, "{\"message\":\"Success\"}");
}
