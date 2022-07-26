#include "server.h"
#include "settings.h"

void setup()
{
  Serial.begin(SERIAL_BAUD);

  Serial.println();
  Serial.println("KSControl v0.1.0");
  Serial.print("Connecting to WiFi");

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  initServer();
}

void loop()
{
  handleClient();
}
