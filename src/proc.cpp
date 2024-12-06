#include <arduino.h>
#include <Wire.h>
#include "painlessMesh.h"
#include "eeprom.h"
#include <ESP32Time.h>
#include "SoftwareSerial.h"
#include <Arduino_JSON.h>
#include "mytype32.h"
#include "portdef.h"
#include "proc.h"
#include <esp_task_wdt.h>


UBIT ctrl_flag;
UBIT ctrl_flag1;
ESP32Time rtc(0);  // offset in seconds GMT+1
long epochTime;

UHIMPERINFO uh;

SLAVERECORDSET slaveget;
SLAVERECORDSET slaveset;
// SLAVERECORD slaverecord[256];
SLAVERECORD slaverecordunit;
USLAVEREQ   uslavereq;
HIMPELLIVE   himpellive;
UMASTERSET   uslaveset;
uint8_t mpptstatus[12];
uint8_t resetstatus[12];

Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;
// uint8_t blebuf[256],bf_ptr,br_ptr;

// HardwareSerial  himpelSerial(0);
EspSoftwareSerial::UART himpelSerial;
EspSoftwareSerial::UART rempteSerial;

// User stub

SimpleList<uint32_t> nodes;
bool calc_delay = false;

//Number for this node
int nodeNumber = 2;
//String to send to other nodes with sensor readings
String readings;

String makestringmsg(void)
{
  UMASTERCOM ums;
  String str = "";
 
  for(int i = 0;i < (12); i++)    //*28
  {
    if(ums._b[i] < 16)
      str = str + "0" + String(ums._b[i],HEX);
    else
      str = str + String(ums._b[i],HEX);
  }
    
  return str;
}

String inttohexstring(uint16_t num,uint8_t lng)
{
  String str;
  str = String(num,HEX);
  while(str.length() < lng)
    str = '0'+str;
  // str += '^';
  return str;
}

// String getReadings () 
// {
//   JSONVar jsonReadings;
//   jsonReadings["type"] = 0;
//   jsonReadings["epoch"] = (int)slaverecord[himpellive.r_recordpos].epoch;
//   jsonReadings["id"] = himpellive.slaveid;
//   String str = "";
//   for(int i = 0; i < 12;  i++)
//   {
//     str+= inttohexstring(slaverecord[himpellive.r_recordpos].s[i].ch1vin,4);
//     str+= inttohexstring(slaverecord[himpellive.r_recordpos].s[i].ch1vout,4);
//     str+= inttohexstring(slaverecord[himpellive.r_recordpos].s[i].ch1iout,4);
//     str+= inttohexstring(slaverecord[himpellive.r_recordpos].s[i].ch1pwm,4);
//     str+= inttohexstring(slaverecord[himpellive.r_recordpos].s[i].ch1gfdp / 100,2);
//     str+= inttohexstring(slaverecord[himpellive.r_recordpos].s[i].ch1gfdm / 100,2);
//     str+= inttohexstring(slaverecord[himpellive.r_recordpos].s[i].ch1tmp,2);
//     str+= inttohexstring(slaverecord[himpellive.r_recordpos].s[i].ch1mode,2);
//     str+= inttohexstring(slaverecord[himpellive.r_recordpos].s[i].ch2vin,4);
//     str+= inttohexstring(slaverecord[himpellive.r_recordpos].s[i].ch2vout,4);
//     str+= inttohexstring(slaverecord[himpellive.r_recordpos].s[i].ch2iout,4);
//     str+= inttohexstring(slaverecord[himpellive.r_recordpos].s[i].ch2pwm,4);
//     str+= inttohexstring(slaverecord[himpellive.r_recordpos].s[i].ch2gfdp / 100,2);
//     str+= inttohexstring(slaverecord[himpellive.r_recordpos].s[i].ch2gfdm / 100,2);
//     str+= inttohexstring(slaverecord[himpellive.r_recordpos].s[i].ch2tmp,2);
//     str+= inttohexstring(slaverecord[himpellive.r_recordpos].s[i].ch2mode,2);    
//     str+= inttohexstring(slaverecord[himpellive.r_recordpos].s[i].currenthigh,4);    
//     str+= inttohexstring(slaverecord[himpellive.r_recordpos].s[i].setstatus,2);    
//   }

//   jsonReadings["msg"] = str;

//   // for(int i = 0; i < 12; i ++)
//   //   Serial.printf( "i=%d v = %d\r\n",i, slaverecord[himpellive.r_recordpos].s[i].ch1vin);

//   jsonReadings["ver"] = VER_INFO;  
//   readings = JSON.stringify(jsonReadings);
//   return readings;
// }

constexpr char hexmap[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

String hexChar(unsigned char srcData)
{
  String destBuf = "";
  destBuf = String(hexmap[(srcData & 0xF0) >> 4]);
  destBuf += String(hexmap[srcData & 0x0F]); 
  return destBuf;
}

void sendMessage(USLAVERECORD usla) 
{
  String hexstring = "";
  JSONVar jsonReadings;
  jsonReadings["type"] = 0;
  //jsonReadings["epoch"] = (int)slaverecord[himpellive.r_recordpos].epoch;
  for (int i = 0; i < RECORDSIZE;i ++)
    hexstring += String(hexChar(usla._b[i]));
  jsonReadings["msg"] = hexstring;
  hexstring = JSON.stringify(jsonReadings);
  
  mesh.sendSingle(himpellive.rootid,hexstring);
#ifdef MESH_DEBUG  
  Serial.println(hexstring);
#endif  
}

String getSetReadings () 
{
  JSONVar jsonReadings;
  jsonReadings["type"] = 1;
  jsonReadings["mnode"] = himpellive.masterid;
  jsonReadings["sid"] = himpellive.slaveid;
  jsonReadings["epoch"] = rtc.getEpoch();

  for(int i = 0; i < 12;  i++)
  {
    jsonReadings["data"][i]["oid"] = (i + 1);    
    jsonReadings["data"][i]["instart_volt"] = slaveset.s[i].STARTVOLTAGE;
    jsonReadings["data"][i]["instop_volt"] = slaveset.s[i].STOPVOLTAGE;
    jsonReadings["data"][i]["outlim_vol"] = slaveset.s[i].OVEROUTVOLTAGE;
    jsonReadings["data"][i]["overstp_cur"] = slaveset.s[i].OVERCURRENT;
    jsonReadings["data"][i]["bypass_cur"] = slaveset.s[i].BYPASSCURRENT;
    jsonReadings["data"][i]["outlim_cur"] = slaveset.s[i].LIMITCURRENT;
    jsonReadings["data"][i]["lim_temp"] = slaveset.s[i].TEMPLIMIT;
    jsonReadings["data"][i]["revise_volt_ch1"] = slaveset.s[i].VOLTAGECORRECTRATIOCH1;
    jsonReadings["data"][i]["revise_cur_ch1"] = slaveset.s[i].CURRENTCORRECTRATIOCH1;    
    jsonReadings["data"][i]["revise_volt_ch2"] = slaveset.s[i].VOLTAGECORRECTRATIOCH2;
    jsonReadings["data"][i]["revise_cur_ch2"] = slaveset.s[i].CURRENTCORRECTRATIOCH2;
    jsonReadings["data"][i]["strtdly_time"] = slaveset.s[i].INITDELAYTIME;
    jsonReadings["data"][i]["swpuls_width"] = slaveset.s[i].SWUNITPULSETIME;
    jsonReadings["data"][i]["duty_maxlim"] = slaveset.s[i].DUTYLIMIT;
    jsonReadings["data"][i]["singlestp_time"] = slaveset.s[i].SINGLEBLOCKTIME;
    jsonReadings["data"][i]["single_dutywidth"] = slaveset.s[i].SINGLEUNITDUTY;
    jsonReadings["data"][i]["singleon_time"] = slaveset.s[i].SINGLERUNTIME;
    jsonReadings["data"][i]["smpptdly_time"] = slaveset.s[i].SMPPTDELAYTIME;
    jsonReadings["data"][i]["smpptduty_wigth"] = slaveset.s[i].SMPPTUNITDUTY;
    jsonReadings["data"][i]["smpptduty_uptime"] = slaveset.s[i].SMPPTDUTYINCTIME;
    jsonReadings["data"][i]["smppt_powergap"] = slaveset.s[i].SMPPTPOWERMARGIN;
    jsonReadings["data"][i]["smppt_pwr_comtime"] = slaveset.s[i].SMPPTPOWERCOMPTIME;
    jsonReadings["data"][i]["mpptlim_cur"] = slaveset.s[i].WORKINGCURRENTLIMIT;
    jsonReadings["data"][i]["mpptoff_cur"] = slaveset.s[i].CURRENTLEDOFFLEVEL;
    jsonReadings["data"][i]["gfd_vol"] = slaveset.s[i].GFDVOLTAGE;
    jsonReadings["data"][i]["mppton_cur"] = slaveset.s[i].CURRHIGHMPPT;
    jsonReadings["data"][i]["ch1_da_revise"] = slaveset.s[i].CH1DACRATIO;
    jsonReadings["data"][i]["ch2_da_revise"] = slaveset.s[i].CH2DACRATIO;
    jsonReadings["data"][i]["temp_width"] = slaveset.s[i].TEMPZONE;
    jsonReadings["data"][i]["scan_time"] = slaveset.s[i].SCANTIME;
    jsonReadings["data"][i]["up_current"] = slaveset.s[i].UPCURRENT;
    jsonReadings["data"][i]["down_current"] = slaveset.s[i].DOWNCURRENT;
    jsonReadings["data"][i]["optimal_pwmtime"] = slaveset.s[i].STARTDURATION;
    readings = JSON.stringify(jsonReadings);
  }
  // Serial.println(readings);
  return readings;
}
void sendSetMessage() {
  himpellive.readmsg = getSetReadings();
  // Serial.printf("root id = %lu\r\n",himpellive.rootid);
  // Serial.printf("Sec = %d\r\n",rtc.getSecond());
  mesh.sendSingle(himpellive.rootid,himpellive.readmsg);
#ifdef MESH_DEBUG    
  Serial.println(himpellive.readmsg);
#endif  

}

// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("Received from %u msg=%s\n", from, msg.c_str());
  int tmpint,oid;
  JSONVar myObject = JSON.parse(msg.c_str());



  int ack = myObject["err"];
  // int id = myObject["id"];
  // epochTime = myObject["epoch"];
  // Serial.printf("epochTime = %d\r\n", epochTime);  
  // if(epochTime > 1685712654)    //2023???? June 2???? Friday PM 1:30:54
  //   rtc.setTime(epochTime);


//   if(id != himpellive.slaveid)
//   {
// #ifdef CONSOLE_DEBUG      
//     Serial.printf("id mismatch id = %d set id=%s\r\n", id,himpellive.slaveid);  
// #endif  
//     return;
//   }
//   else
//     Serial.println("id matched");


  // Serial.printf("ack = %d\r\n",ack);

  // if(!epochset_flag)
  {
    epochTime = myObject["epoch"];
    if(epochTime > 1685712654)    //2023???? June 2???? Friday PM 1:30:54
    {
      rtc.setTime(epochTime);
      epochset_flag = 1;
    }          
  }
}

void newConnectionCallback(uint32_t nodeId) {
  himpellive.rootid = mesh.getRootId(mesh.asNodeTree());
  if(himpellive.rootid)
  {
    rootconnect_flag = 1;
#ifdef MESH_DEBUG      
    Serial.printf("root id = %lu\r\n",himpellive.rootid);
#endif  

  }
  else                              
    rootconnect_flag = 0;
#ifdef MESH_DEBUG    
  Serial.printf("--> Start: New Connection, %s\n", mesh.subConnectionJson(true).c_str());
#endif  

  
  String nodetree = mesh.subConnectionJson(true);
  JSONVar myObject = JSON.parse(nodetree.c_str());             
  myObject = myObject["subs"];
#ifdef MESH_DEBUG    
  Serial.printf("sub count = %d\r\n",myObject.length());
#endif  


}

void changedConnectionCallback() {
  himpellive.rootid = mesh.getRootId(mesh.asNodeTree());
  if(himpellive.rootid)
  {
    rootconnect_flag = 1;
#ifdef MESH_DEBUG      
    Serial.printf("root id = %lu\r\n",himpellive.rootid);
#endif  

  }
  else                              
    rootconnect_flag = 0;
#ifdef MESH_DEBUG      
  Serial.printf("Change Connection, %s\n", mesh.subConnectionJson(true).c_str());
#endif  

  String nodetree = mesh.subConnectionJson(true);
  JSONVar myObject = JSON.parse(nodetree.c_str());             
  myObject = myObject["subs"];
#ifdef MESH_DEBUG    
  Serial.printf("sub count = %d\r\n",myObject.length());
#endif  

}

void nodeTimeAdjustedCallback(int32_t offset) {
#ifdef MESH_DEBUG  
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
#endif  

}

#define BUTTON_PIN_BITMASK 0x00000200 // 2^33 in hex
//-----------------------------------------------------------------------------
void waterlevel_init(void)
{

  Serial.begin(115200);
  // himpelSerial.begin(1200,SERIAL_8N1, U1_RX, U1_TX, true);

  himpelSerial.begin(1200, SWSERIAL_8N1, U1_RX, U1_TX, true);
  rempteSerial.begin(1200, SWSERIAL_8N1, SOFT_RX, SOFT_TX, true);

  delay(100);
  pinMode(STLED,OUTPUT);
  pinMode(DIPSW2_PIN,INPUT);
  pinMode(DIPSW1_PIN,INPUT);

  uh.s.stx = 0xAA;
  uh.s.etx = 0xEE;

#ifdef UART_DEBUG  
  Serial.printf("Total heap: %d", ESP.getHeapSize());
  Serial.printf("Free heap: %d", ESP.getFreeHeap());
  Serial.printf("Total PSRAM: %d", ESP.getPsramSize());
  Serial.printf("Free PSRAM: %d", ESP.getFreePsram());
#endif  

  uslavereq.s.header = 0xAAFFAAFF;
  uslavereq.s.stx = 0x02;
  uslavereq.s.lng = 0x05;
  uslavereq.s.etx = 0x03;
  himpellive.stringid = 12;

  EEPROM.begin(100); //EEPROM?�� 메모�?? 주소 �?? 100까�?? ?��?��

  for(int i = 0; i < 32; i ++)
  {
    uslaveset._b[i] = EEPROM.readByte(i);
  }

  if(uslaveset.s.maginum != MAGIC_NUM)
  {
    uslaveset.s.maginum = MAGIC_NUM;
    uslaveset.s.plantnum = 1234;
    uslaveset.s.masternum = 1;
    write_eeprom();
  }

  // Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) + " Seconds");


  // mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  // mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

  // mesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT, WIFI_AP_STA, TC_CH);

  delay(2000);
  for (int i = 0; i < 10; i ++)
  {
    read_key();
    delay(2);
  }
  Serial.printf(" m =%d s=%d\r\n", himpellive.masterid, himpellive.slaveid);

  mesh.init( MESH_PREFIX + String(himpellive.masterid), MESH_PASSWORD, MESH_PORT, WIFI_AP_STA );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);  
  //userScheduler.addTask( taskSendMessage );
  // This node and all other nodes should ideally know the mesh contains a root,
  // so call this on all nodes
  mesh.setContainsRoot(true);

  esp_task_wdt_init(WDT_TIMEOUT, true); //enable panic so ESP32 restarts
  esp_task_wdt_add(NULL); //add current thread to WDT watch  


}
//------------------------------------------------------------------------------
uint8_t calculator_crc(uint8_t lng,uint8_t *buf)
{
  #define CHKSUM_HEAD   0xAA
  uint8_t crc = CHKSUM_HEAD;

  for(int i = 0; i < lng; i ++)
    crc += buf[i];

  return crc;
}

//-----------------------------------------------------------------------------
void scan_SUR26A(void)    //150msec  FF AA FF AA 02 05 01 1F 03 D4 
{


}
//-----------------------------------------------------------------------------
void control_report(void)   //100msec
{
  static uint32_t oldepoch;
  static  uint16_t  wcnt;
  String hexstring = "";
  SLAVERECORD sla;
  USLAVERECORD usla;
  if(rootconnect_flag)
  {
    if(himpellive.f_recordpos != himpellive.r_recordpos)
    {
      if(!msgsended_flag)
      {

        msgsended_flag = 1;
        himpellive.resendcount = 0;

        Serial.printf("er = %d f = %d r = %d\r\n",usla.s.epoch,himpellive.f_recordpos,himpellive.r_recordpos);

        sendMessage(usla);
      }    
    }
    if(himpellive.slaveid < 1) return;

    if ((rtc.getEpoch() - oldepoch) < 3) return;
    oldepoch = rtc.getEpoch();
    if(himpellive.scan_mode == 2)
    {
      // if(himpellive.rcvcount)
      {
        himpellive.rcvcount = 0;

        Serial.printf("ef = %d f = %d r = %d\r\n",oldepoch,himpellive.f_recordpos,himpellive.r_recordpos);

      }
    }
      
    // Serial.printf("epoch = %d \r\n", rtc.getEpoch());
  }
}
//-----------------------------------------------------------------------------
void read_key(void)   //2msec
{
  static  uint8_t s1oncount,s1offcount,s2oncount,s2offcount;
  

  if(!DIPSW1_IN)
  {
    if(++ s1oncount > ONCOUNT)
    {
      s1oncount --;
      s1offcount = 0;
      dip1sw_flag = 1;
    }
  }
  else if(++ s1offcount > OFFCOUNT)
  {
    s1offcount --;
    s1oncount = 0;
    dip1sw_flag = 0;
  }
  if(!DIPSW2_IN)
  {
    if(++ s2oncount > ONCOUNT)
    {
      s2oncount --;
      s2offcount = 0;
      dip2sw_flag = 1;
    }
  }
  else if(++ s2offcount > OFFCOUNT)
  {
    s2offcount --;
    s2oncount = 0;
    dip2sw_flag = 0;
  }  
 

  if(dip1sw_flag)   himpellive.masterid = 1;
  else              himpellive.masterid = 0;
  if(dip2sw_flag)   himpellive.masterid += 2;

  himpellive.masterid += 100;


}

//-----------------------------------------------------------------------------
uint8_t calculator_crc(uint16_t lng, uint8_t *buf)
{
  uint8_t crc = 0xAA;

  for (int i = 0; i < lng; i++)
      crc += buf[i];

  return crc;
}
// Check whether write to EEPROM was successful or not with the EEPROM.commit() function.
void eeprom_commit() {
  if (EEPROM.commit()) {
    Serial.println("EEPROM successfully committed!");
  } else {
    Serial.println("ERROR! EEPROM commit failed!");
  }
}
//-----------------------------------------------------------------------------
void write_eeprom(void)
{


  uslaveset.s.maginum = MAGIC_NUM;
  for(int i = 0; i < 32; i ++)
    EEPROM.write(i,uslaveset._b[i]);
  eeprom_commit(); //기록?�� ?��?��?�� ????��

  for(int i = 0; i < 32; i ++)
  {
    uslaveset._b[i] = EEPROM.readByte(i);
  }
  // Serial.println(__func__);

  //EEPROM.end();  
}
//-----------------------------------------------------------------------------
void analysys_data(uint8_t *rxdata)
{
  // TX2023COM *tst;

  // tst = (TX2023COM *)rxdata;

  // switch (tst->cmd)
  // {
  //   case 'W':
  //       utc2023set.s.idnum = tst->devid;
  //       write_eeprom();
  //     break;
    
  //   case 'R':
  //       tst->cmd = 'R';
  //       tst->devid = utc2023set.s.idnum;
  //       tst->stx = 0x02;
  //       tst->etx = 0x03;
  //       tst->lng = 28;
  //       tst->ver = VER_INFO;
  //       // tst->crc = calculator_crc(31,&tst->stx);
  //       Serial.write(&tst->stx,32);

  //     break;    
  //   default:
  //     break;
  // }
}

//-----------------------------------------------------------------------------
void check_serial(void)
{
  static  uint8_t lng,rlng,rxdata[256];


  mesh.update();
  if(Serial.available())
  {
    char c = Serial.read();
    if (c == '1') himpellive.s_mode = 1;
    else if (c == '2') himpellive.s_mode = 2;
    else if (c == ' ') himpellive.s_mode = 0;

    StaticJsonDocument<1024> doc;    
    String str = Serial.readStringUntil('\n');      

    deserializeJson(doc, str);    
    if(doc["OPCode"] == 112)
    {
      uslaveset.s.plantnum = doc["plantnum"];
      uslaveset.s.masternum = doc["masternum"];
      write_eeprom();
      delay(500);
      esp_restart();

    }
    else if(doc["OPCode"] == 115)
    {
      String  msg;
      StaticJsonDocument<1024> doc;
      doc["OPCode"] = 115;
      doc["masternum"] = uslaveset.s.masternum;
      doc["plantnum"] = uslaveset.s.plantnum;
      doc["ver"] = VER_INFO;

      serializeJson(doc, msg);
      Serial.println(msg);
    }    
  }    

  if(himpelSerial.available())
  {
    char c = himpelSerial.read();
    rempteSerial.write(c);
    Serial.write(c);
  }
  if(rempteSerial.available())
  {
    char c = rempteSerial.read();
    himpelSerial.write(c);
    Serial.write(c);
  }

}
//-----------------------------------------------------------------------------
void misslogger_write(void)
{

}
//-----------------------------------------------------------------------------
void logger_write(void)
{
}
//-----------------------------------------------------------------------------
void print_wakeup_reason(void)
{
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}

//-----------------------------------------------------------------------------
void get_axisinfo(void)   //500msec
{

}
//-----------------------------------------------------------------------------
float take_average(uint8_t lng,float *buf)
{
  return 0.0;
}
//-----------------------------------------------------------------------------
void abalisys_bledata(uint8_t *buf)
{
    
}
//-----------------------------------------------------------------------------
uint8_t make_bcc(uint8_t lng, uint8_t *buf) 
{
  uint8_t bcc = 0;
  for (int i = 0; i < lng; i ++) 
  {
    bcc ^= buf[i];
  }
  return bcc;
}
//-----------------------------------------------------------------------------
const uint8_t himpelinfo[15]    = { 0xAA,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0xAA,0xEE};
const uint8_t himpeloff[15]     = { 0xAA,0x01,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0xAB,0xEE};
const uint8_t himpelon[15]      = { 0xAA,0x01,0x00,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0xAA,0xEE};
const uint8_t himpelauto[15]    = { 0xAA,0x01,0x01,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0xAA,0xEE};
const uint8_t himpelbypass[15]  = { 0xAA,0x01,0x02,0x01,0x03,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0xAA,0xEE};
//-----------------------------------------------------------------------------
void himpel_off(void) 
{ 
  uh.s.buf[0] = 1; 
  uh.s.buf[1] = 0; 
  uh.s.buf[2] = 0; 
  uh.s.buf[3] = 1; 
}
//-----------------------------------------------------------------------------
void himpel_on(void) 
{ 
  uh.s.buf[0] = 1; 
  uh.s.buf[1] = 0; 
  uh.s.buf[2] = 1; 
  uh.s.buf[3] = 1; 
}
//-----------------------------------------------------------------------------
void send_himpercommand(void) //500msec
{
  static uint8_t wcnt;
  if (++wcnt < 2) return;
  wcnt = 0;

  switch(himpellive.s_mode)
  {
    case  0:
      himpelSerial.write(himpelinfo, 15);
      break;


    case  1:    //ON CMD
      himpelSerial.write(himpelon, 15);
      himpellive.s_mode = 0;
      break;

    case  2:    //OFF CMD
      himpelSerial.write(himpeloff, 15);
      himpellive.s_mode = 0;
      break;

  }
}