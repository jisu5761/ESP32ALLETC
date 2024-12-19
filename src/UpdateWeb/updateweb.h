#ifndef __UPDATEWEB_H__
#define __UPDATEWEB_H__

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include "portdef.h"


void check_update(void);
#endif