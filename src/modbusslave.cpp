#include <Arduino.h>
#include <ModbusRTUSlave.h> 
#include "mytype32.h"
#include "portdef.h"
#include "proc.h"
#include "modbusslave.h"

ModbusRTUSlave modbus(Serial0, -1); // serial port, driver enable pin for rs-485
extern UMESH_INFO umi;
//-----------------------------------------------------------------------------
void sensorhubmodbus_init(void)
{
  modbus.configureHoldingRegisters(umi._wb, 256);     // unsigned 16 bit integer array of input register values, number of input registers
  modbus.begin(HILPEL_ID, 9600);
}
//-----------------------------------------------------------------------------
void sensorhub_poll(void)
{
  modbus.poll();
  himpellive.pollcount = modbus.getpollcount();
}

