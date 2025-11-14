#ifndef __MODBUSSLAVE_H__
#define __MODBUSSLAVE_H__
#define HILPEL_ID        0x50

#pragma pack(push)  /* push current alignment to stack */
#pragma pack(1) 


#pragma pack(pop)   /* restore original alignment from stack */


extern  UHIMPERINFO uhi;

void sensorhubmodbus_init(void);
void sensorhub_poll(void);

#endif