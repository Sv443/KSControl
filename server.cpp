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
  Serial.print(">> Pushing state : enabled=");
  Serial.print(state.enabled ? "true" : "false");
  Serial.print("; temp=");
  Serial.print(state.temp);
  Serial.print("; mode=");
  Serial.print(state.mode);
  Serial.print("; fan=");
  Serial.println(state.fan);
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
        for (int i = 0; i < 5; i++)
        {
          if (modeMap[i] == val)
            return true;
        }
      else if (name == "fan")
        for (int i = 0; i < 4; i++)
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
