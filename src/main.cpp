//************************************************************
// this is a simple example that uses the painlessMesh library
//
// 1. sends a silly message to every node on the mesh at a random time between 1 and 5 seconds
// 2. prints anything it receives to Serial.print
//
//
//************************************************************
#include <arduino.h>
#include "mytype32.h"
#include "portdef.h"
#include "proc.h"
#include <esp_task_wdt.h>



void setup() {
  waterlevel_init();

}

void loop() {
  // it will run the user scheduler as well
  static  uint32_t  t2ms,t100ms,t200ms,t500ms;
  uint32_t  currentMillis;
  check_serial();
  currentMillis = millis();
  if((currentMillis - t2ms) > 9)
  {
    t2ms = currentMillis;
    read_key();
    
  }
  if((currentMillis - t100ms) > 99)
  {
    t100ms = currentMillis;
    control_report();
  }
  if((currentMillis - t200ms) > 149)
  {
    t200ms = currentMillis;
    esp_task_wdt_reset();
    scan_SUR26A();
  }
  if((currentMillis - t500ms) > 499)
  {
    t500ms = currentMillis;
    send_himpercommand();
    digitalWrite(STLED,!digitalRead(STLED));
  }  
}

