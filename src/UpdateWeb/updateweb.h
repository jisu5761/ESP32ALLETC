#ifndef __UPDATEWEB_H__
#define __UPDATEWEB_H__

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include "portdef.h"


#define APIKEY		"5f3f6c29-435a-4cca-92fc-3d971e549834"
#define FW_VER 		"v@4.0.1"

void check_update(void);
#endif