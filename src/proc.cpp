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

// #define SERIAL_DEBUG


UBIT ctrl_flag;
UBIT ctrl_flag1;
ESP32Time rtc(0);  // offset in seconds GMT+1
long epochTime;

UHIMPERCMD uhc;
UHIMPERCMD uhs;
UHIMPERINFO uhi;

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

constexpr char hexmap[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

String hexChar(unsigned char srcData)
{
  String destBuf = "";
  destBuf = String(hexmap[(srcData & 0xF0) >> 4]);
  destBuf += String(hexmap[srcData & 0x0F]); 
  return destBuf;
}
//-----------------------------------------------------------------------------
void strToBin(byte bin[], char const str[]) 
{
  for (size_t i = 0; str[i] and str[i + 1]; i += 2) {
    char slice[] = {0, 0, 0};
    strncpy(slice, str + i, 2);
    bin[i / 2] = strtol(slice, nullptr, 16);
  }
}
//-----------------------------------------------------------------------------
void sendMessage(UHIMPERINFO uhi) 
{
  String hexstring = "";
  JSONVar jsonReadings;
  jsonReadings["id"] = ESP32ALLETC_ID;
  //jsonReadings["epoch"] = (int)slaverecord[slavelive.r_recordpos].epoch;
  for (int i = 0; i < RECORDSIZE;i ++)
    hexstring += String(hexChar(uhi._b[i]));
  jsonReadings["hex"] = hexstring;
  hexstring = JSON.stringify(jsonReadings);
  
  mesh.sendSingle(himpellive.rootid,hexstring);
#ifdef MESH_DEBUG  
  Serial.println(hexstring);
#endif  
}


// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  static UHIMPERINFO olduhi;
  int oid;
  String hexstr;
#ifdef MESH_DEBUG    
  Serial.printf("Received from %u msg=%s\n", from, msg.c_str());
#endif  

  JSONVar myObject = JSON.parse(msg.c_str());
  oid = myObject["id"];
  if (oid != ESP32ALLETC_ID) return;
  hexstr = (const char *)myObject["hex"];

  strToBin(uhi._b, hexstr.c_str());

  for (int i = 0; i < sizeof(HIMPERCMD);i ++)
  {
    if(olduhi._b[i] != uhi._b[i])
    {
      memcpy(olduhi._b, uhi._b, sizeof(HIMPERCMD));
      if (uhi.s.onoff == 0) uhi.s.flowlevel = 0;
      himpellive.s_mode = 1;  // ON
      break;
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

  uhc.s.stx = 0xAA;
  uhc.s.etx = 0xEE;
  uhc.s.const1 = 1;
  uhc.s.const2 = 1;

#ifdef UART_DEBUG  
  Serial.printf("Total heap: %d", ESP.getHeapSize());
  Serial.printf("Free heap: %d", ESP.getFreeHeap());
  Serial.printf("Total PSRAM: %d", ESP.getPsramSize());
  Serial.printf("Free PSRAM: %d", ESP.getFreePsram());
#endif  


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

  delay(100);
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
  static uint8_t lng,rlng,rxdata[256];
  static uint8_t h_mode,hbcc,hbuf[32],hrlng;
  static uint8_t rc_mode,rcbcc,rcbuf[32],rcrlng;

  mesh.update();
  if(Serial.available())
  {
    char c = Serial.read();
    if (c == '1') himpellive.s_mode = 1;
    else if (c == '2') himpellive.s_mode = 2;
    else if (c == '3') himpellive.s_mode = 3;    
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
#ifdef SERIAL_DEBUG
    Serial.printf("%02X ",c);
#endif    
    hbcc ^= c;
    switch(h_mode)
    {
      case 0:
        if (c == 0xAA) 
        {
#ifdef SERIAL_DEBUG          
          Serial.printf("[HP] ");
#endif          
          h_mode++;
          hbcc = c;
          hrlng = 0;
          hbuf[hrlng++] = c;
        }
        break;

      case  1:
        hbuf[hrlng++] = c;
        if (hrlng >= 13) h_mode++;
        break;

      case  2:
        if (hbcc == 0) h_mode++;
        else
          h_mode = 0;
        break;

      case 3:
        h_mode = 0;
        if(c == 0xEE)
        {
#ifdef SERIAL_DEBUG
          Serial.println("OK");          
#endif          
          for (int i = 0; i < 15; i++) 
            uhs._b[i] = hbuf[i];

          digitalWrite(STLED,!digitalRead(STLED));        
        }
        break;

      default:
        h_mode = 0;
        break;
    }
  }
  if(rempteSerial.available())
  {
    char c = rempteSerial.read();
#ifdef SERIAL_DEBUG
    Serial.printf("%02X ",c);
#endif    
    rcbcc ^= c;
    switch(rc_mode)
    {
      case 0:
        if (c == 0xAA) 
        {
#ifdef SERIAL_DEBUG          
          Serial.printf("[RC] ");
#endif          
          rc_mode++;
          rcbcc = c;
          rcrlng = 0;
          himpellive.rcbuf[rcrlng++] = c;
        }
        break;

      case  1:
        himpellive.rcbuf[rcrlng++] = c;
        if (rcrlng >= 13) rc_mode++;
        break;

      case  2:
        himpellive.rcbuf[rcrlng++] = c;
        if (rcbcc == 0) rc_mode++;
        else
          rc_mode = 0;
        break;

      case 3:
        rc_mode = 0;
        himpellive.rcbuf[rcrlng++] = c;
        if(c == 0xEE)
        {
#ifdef SERIAL_DEBUG
          Serial.println();          
#endif
        if(dip2sw_flag)
          himpelSerial.write(himpellive.rcbuf, rcrlng);
        }
        break;

      default:
        rc_mode = 0;
        break;
    }
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
void sethimpelcommand(void)
{
  uhc.s.setflag = 1;  
  uhc.s.autobypass = uhi.s.flowmode;
  if(uhi.s.flowmode == 2) uhc.s.volume = 3;
  else if(uhi.s.onoff == 1)
    uhc.s.volume = uhi.s.flowlevel;
  else
    uhc.s.volume = 0;
  uhc.s.bcc = make_bcc(13, uhc._b);

#ifdef SERIAL_DEBUG
  Serial.print("[S] ");
  for (int i = 0; i < 15; i ++) 
  {
    Serial.printf("%02X ", uhc._b[i]);
  }
  Serial.println();
#endif  
}
//-----------------------------------------------------------------------------
void send_himpercommand(void) //1000msec
{
  // return;

  if (dip2sw_flag) return;

  switch(himpellive.s_mode)
  {
    case  0:
      himpelSerial.write(himpelinfo, 15);
      break;


    case  1:    //CMD
      sethimpelcommand();
      himpelSerial.write(uhc._b, 15);
      himpellive.s_mode = 0;
      break;

    case  2:    
      himpelSerial.write(himpeloff, 15);
      himpellive.s_mode = 0;
      break;

    // case  3:    //AUTO CMD
    //   himpelSerial.write(himpelauto, 15);
    //   himpellive.s_mode = 0;
    //   break;

  }
}
//-----------------------------------------------------------------------------
void send_meshstatus(void)  //1000msec
{

  if(rootconnect_flag)
  {

    if (uhs.s.volume) uhi.s.onoff = 1;
    else              uhi.s.onoff = 0;    
    uhi.s.flowmode = uhs.s.autobypass;
    // if(uhi.s.onoff)
    //   uhi.s.flowlevel = uhs.s.volume + 1;
    // else
    
    uhi.s.flowlevel = uhs.s.volume;

    if (uhs.s.autobypass) uhi.s.onoff = 1;
    sendMessage(uhi);
  }
}