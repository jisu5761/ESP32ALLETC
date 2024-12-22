#include "updateweb/updateweb.h"
#include <otadrive_esp.h>

const char* host = "esp32";
const char* ssid = "SWOOMUPDATE";
const char* password = "12341234";

void check_update(void) 
{
  static uint8_t scount,updateflag;
  uint32_t update_time = 0;
  Serial.println("Update Web Server");
  // Connect to WiFi network
  OTADRIVE.setInfo(APIKEY, FW_VER);

  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    digitalWrite(STLED_PIN,!digitalRead(STLED_PIN));          
    Serial.print(".");
    if (++ scount > 10) 
    {
      return;
    }
  }
  auto inf = OTADRIVE.updateFirmwareInfo();

  // update firmware if newer available
  if (inf.available)
  {
    Serial.printf("\nNew version available, %dBytes, %s\n", inf.size, inf.version.c_str());
    OTADRIVE.updateFirmware();
  }
  else
  {
    Serial.printf("\nNo newer version\n");
  }
}