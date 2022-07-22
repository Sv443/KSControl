#include "env.h"

#ifndef String
  #include <Arduino.h>
#endif

#define HTTP_PORT 80
#define HTTP_TIMEOUT 5000 // TODO

#define SERIAL_BAUD 115200
#define INTERNAL_JSON_MEM_SIZE 1000

/*
 *  Json parametric POST REST request with ArduinoJSON library
 *  by Mischianti Renzo <https://www.mischianti.org>
 *
 *  https://www.mischianti.org/
 *
 */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>

ESP8266WebServer server(80);

// Serving Hello world
void setRoom()
{
  String postBody = server.arg("plain");
  Serial.println(postBody);

  DynamicJsonDocument doc(512);
  DeserializationError error = deserializeJson(doc, postBody);
  if (error)
  {
    // if the file didn't open, print an error:
    Serial.print(F("Error parsing JSON "));
    Serial.println(error.c_str());

    String msg = error.c_str();

    server.send(400, F("text/html"),
                "Error in parsin json body! <br>" + msg);
  }
  else
  {
    JsonObject postObj = doc.as<JsonObject>();

    Serial.print(F("HTTP Method: "));
    Serial.println(server.method());

    if (server.method() == HTTP_POST)
    {
      if (postObj.containsKey("name") && postObj.containsKey("type"))
      {

        Serial.println(F("done."));

        // Here store data or doing operation

        // Create the response
        // To get the status of the result you can get the http status so
        // this part can be unusefully
        DynamicJsonDocument doc(512);
        doc["status"] = "OK";

        Serial.print(F("Stream..."));
        String buf;
        serializeJson(doc, buf);

        server.send(201, F("application/json"), buf);
        Serial.print(F("done."));
      }
      else
      {
        DynamicJsonDocument doc(512);
        doc["status"] = "KO";
        doc["message"] = F("No data found, or incorrect!");

        Serial.print(F("Stream..."));
        String buf;
        serializeJson(doc, buf);

        server.send(400, F("application/json"), buf);
        Serial.print(F("done."));
      }
    }
  }
}

// Define routing
void restServerRouting()
{
  server.on("/", HTTP_GET, []()
            { server.send(200, F("text/html"),
                          F("Welcome to the REST Web Server")); });
  // handle post request
  server.on(F("/setRoom"), HTTP_POST, setRoom);
}

// Manage not found URL
void handleNotFound()
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++)
  {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup(void)
{
  Serial.begin(SERIAL_BAUD);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Activate mDNS this is used to be able to connect to the server
  // with local DNS hostmane esp8266.local
  if (MDNS.begin("esp8266"))
  {
    Serial.println("MDNS responder started");
  }

  // Set server routing
  restServerRouting();
  // Set not found response
  server.onNotFound(handleNotFound);
  // Start server
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void)
{
  server.handleClient();
}

// /**
//  * @struct JsonParseRes The result of parsing a JSON string
//  * @property error Whether there was an error
//  * @property message A message from the server
//  * @property errorMsg An error message (if error=true)
//  */
// struct JsonParseRes {
//   bool error;
//   String message;
//   String errorMsg;
// };

// WiFiServer server(HTTP_PORT);

// String header;
// String body;

// void setup() {
//   Serial.begin(115200);

//   Serial.print("Connecting to WiFi");
//   WiFi.begin(WIFI_SSID, WIFI_PASS);
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }
//   Serial.println("");
//   Serial.print("Connected to WiFi as ");
//   Serial.println(WiFi.localIP());
//   server.begin();
// }

// void loop() {
//   WiFiClient client = server.available();

//   if (client)
//   {
//     Serial.println("Client available");
//     String currentLine = "";
//     bool isPayload = false;

//     while(client.connected())
//     {
//       if(client.available())
//       {
//         char c = client.read();
//         if(c == '\n')
//           Serial.write("\\n\n");
//         else if(c == '\r')
//           Serial.write("\\r\r");
//         else
//           Serial.write(c);

//         if(isPayload)
//           body += c;
//         else
//           header += c;

//         if(c == '\n')
//         {
//           if(currentLine.length() == 0)
//           {
//             Serial.println("cur line empty");
//             if(isPayload)
//             {
//               Serial.println("body time");
//               parseRequest(client, header, body);
//               break;
//             }

//             isPayload = true;
//           }

//           currentLine = "";
//         } else if (c != '\r') {
//           currentLine += c;
//         }
//       }
//     }
//     header = "";
//     body = "";
//     client.stop();

//     Serial.println("Client disconnected");
//     Serial.println("");
//   }
// }

// void parseRequest(WiFiClient client, String header, String body)
// {
//   Serial.println("----");
//   Serial.println("Header");
//   Serial.println(header);
//   Serial.println("\nBody");
//   Serial.println(body);
//   Serial.println("----");
// }

// JsonParseRes parseJson(String jsonString)
// {
//   unsigned int jsonLength = jsonString.length() + 1;

//   DynamicJsonDocument doc(INTERNAL_JSON_MEM_SIZE);
//   char json[jsonLength];
//   jsonString.toCharArray(json, jsonLength);

//   Serial.println();
//   Serial.println("JSON");
//   Serial.println(json);

//   DeserializationError error = deserializeJson(doc, json);

//   JsonParseRes result = { false, "", "" };

//   if(error)
//   {
//     result.error = true;
//     result.errorMsg = error.f_str();
//   }
//   else
//   {
//     bool err = doc["error"];
//     String msg = doc["message"];

//     result.error = err;
//     result.message = msg;
//   }

//   return result;
// }
