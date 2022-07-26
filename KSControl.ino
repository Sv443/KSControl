#include "server.h"
#include "settings.h"

void setup()
{
  Serial.begin(SERIAL_BAUD);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  Serial.println();
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED)
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
