#ifndef __PROC_H__
#define __PROC_H__

//#define	VER_INFO						1					//   Duration average
//#define	VER_INFO						2//   memset(&slaveset.s[i], 0x00, sizeof(STRINGSET));
//#define	VER_INFO						  2//      for (int i = 0; i < 12; i ++)        slaveset.s[oid].STARTVOLTAGE = 0;
#define	VER_INFO						  3     //psramdisk

#define	MAGIC_NUM						0x5763


#define	ROOTWAITTIME				20			//5				//root ?ï¿½ï¿½ï¿?? ???ï¿?? ?ï¿½ï¿½ï¿?? (?ï¿½ï¿½?ï¿½ï¿½ : ï¿??)

#define	ONCOUNT							2
#define	OFFCOUNT						100

#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  (60 * 60 * 6)        /* Time ESP32 will go to sleep (in seconds) */

#define 	WDT_TIMEOUT 			120

#define 	MSET_FLAG					0xAA



#define   MESH_PREFIX     "SWOOMAIR"
#define   MESH_PASSWORD   "swoomair5761"
#define   MESH_PORT       5761

#define SLAVEBUFFSIZE     2048
#define SLAVEBUFFHALF     (SLAVEBUFFSIZE / 2)
#define SLAVEBUFFMASK     (SLAVEBUFFSIZE - 1)
#define RECORDSIZE        sizeof(SLAVERECORD)
#define SwapTwoBytes(data) \
( (((data) >> 8) & 0x00FF) | (((data) << 8) & 0xFF00) ) 


#pragma pack(push)  /* push current alignment to stack */
#pragma pack(1)


typedef struct{
	uint16_t  maginum;
  uint16_t  masternum;
	uint16_t  plantnum;
	uint16_t  duration;
  float     refheight;
	float     offsetheight;
	uint8_t		dumy[16];
}MASTERSET;

typedef union{
	MASTERSET	s;
	uint8_t		_b[32];
} UMASTERSET;

typedef struct{
	uint8_t stx;
	uint8_t buf[12];
	uint8_t bcc;
  uint8_t etx;
}HIMPERINFO;

typedef union{
  HIMPERINFO   s;
	uint8_t		    _b[15];
} UHIMPERINFO;


typedef struct{
	uint8_t stx;
	uint8_t buf[12];
	uint8_t bcc;
  uint8_t etx;
}STRINGINFO;

typedef struct{
	uint32_t			epoch;  
	STRINGINFO		s[2];
  uint8_t		 slaveid;
  uint8_t		 ver;
}SLAVERECORD;

typedef union{
  SLAVERECORD   s;
	uint8_t		    _b[RECORDSIZE];
}USLAVERECORD;


typedef struct
{
  uint32_t sync;
  uint8_t stx;
  uint8_t lng;
  uint8_t address;
  uint8_t cmd;
  uint16_t STARTVOLTAGE;
  uint16_t STOPVOLTAGE;
  uint16_t OVEROUTVOLTAGE;
  uint16_t VOLTAGECORRECTRATIOCH1;
  uint16_t OVERCURRENT;
  uint16_t LIMITCURRENT;
  uint16_t BYPASSCURRENT;
  uint16_t CURRENTCORRECTRATIOCH1;
  uint16_t ZEROCURRENTLEVEL;
  uint16_t TEMPZONE;
  uint16_t INITDELAYTIME;
  uint16_t SWUNITPULSETIME;
  uint16_t SINGLEBLOCKTIME;
  uint16_t SINGLEUNITDUTY;
  uint16_t STARTDURATION;
  uint16_t MPPTCMDSTATUS;
  uint16_t SINGLERUNTIME;
  uint16_t SMPPTUNITDUTY;
  uint16_t SMPPTDUTYINCTIME;
  uint16_t SMPPTPOWERMARGIN;
  uint16_t VOLTAGECORRECTRATIOCH2;
  uint16_t CURRENTCORRECTRATIOCH2;
  uint16_t WORKINGCURRENTLIMIT;
  uint16_t CH1DACRATIO;
  uint16_t CURRHIGHMPPT;
  uint16_t GFDVOLTAGE;
  uint16_t DUTYLIMIT;
  short TEMPLIMIT; //????
  uint16_t CH2DACRATIO;
  uint8_t SINGLEMULTI;
  uint16_t UPCURRENT;
  uint16_t DOWNCURRENT; 
  uint16_t ZEROCURRENTLED;
  uint16_t SCANTIME;
  uint16_t SMPPTPOWERCOMPTIME;
  uint16_t SMPPTDELAYTIME;
  uint8_t etx;
  uint8_t crc;
}ST_PARA;
typedef union{
  ST_PARA s;
  uint8_t _b[81];
} UST_PARA;

typedef struct{
  uint16_t STARTVOLTAGE;
  uint16_t STOPVOLTAGE;
  uint16_t OVEROUTVOLTAGE;
  uint16_t VOLTAGECORRECTRATIOCH1;
  uint16_t OVERCURRENT;
  uint16_t LIMITCURRENT;
  uint16_t BYPASSCURRENT;
  uint16_t CURRENTCORRECTRATIOCH1;
  uint16_t ZEROCURRENTLEVEL;
  uint16_t TEMPZONE;
  uint16_t INITDELAYTIME;
  uint16_t SWUNITPULSETIME;
  uint16_t SINGLEBLOCKTIME;
  uint16_t SINGLEUNITDUTY;
  uint16_t STARTDURATION;
  uint16_t MPPTCMDSTATUS;
  uint16_t SINGLERUNTIME;
  uint16_t SMPPTUNITDUTY;
  uint16_t SMPPTDUTYINCTIME;
  uint16_t SMPPTPOWERMARGIN;
  uint16_t VOLTAGECORRECTRATIOCH2;
  uint16_t CURRENTCORRECTRATIOCH2;
  uint16_t WORKINGCURRENTLIMIT;
  uint16_t CH1DACRATIO;
  uint16_t CURRHIGHMPPT;
  uint16_t GFDVOLTAGE;
  uint16_t DUTYLIMIT;
  short TEMPLIMIT;
  uint16_t CH2DACRATIO;
  uint8_t SINGLEMULTI;
  uint16_t UPCURRENT;
  uint16_t DOWNCURRENT;
  uint16_t CURRENTLEDOFFLEVEL;  
  uint16_t SCANTIME;  
  uint16_t SMPPTPOWERCOMPTIME;  
  uint16_t SMPPTDELAYTIME;
}STRINGSET;

typedef struct{
	STRINGSET		s[12];
	uint8_t				slaveid;
}SLAVERECORDSET;


#define   SUSETCOM_SIZE        77
#define   SUSETCRC_SIZE        (SUSETCOM_SIZE - 1)
typedef struct{
  uint8_t stx;
  uint8_t address;
  uint8_t lng;
  uint8_t cmd;
  uint16_t STARTVOLTAGE;
  uint16_t STOPVOLTAGE;
  uint16_t OVEROUTVOLTAGE;
  uint16_t VOLTAGECORRECTRATIOCH1;
  uint16_t OVERCURRENT;
  uint16_t LIMITCURRENT;
  uint16_t BYPASSCURRENT;
  uint16_t CURRENTCORRECTRATIOCH1;
  uint16_t ZEROCURRENTLEVEL;
  uint16_t TEMPZONE;
  uint16_t INITDELAYTIME;
  uint16_t SWUNITPULSETIME;
  uint16_t SINGLEBLOCKTIME;
  uint16_t SINGLEUNITDUTY;
  uint16_t STARTDURATION;
  uint16_t MPPTCMDSTATUS;
  uint16_t SINGLERUNTIME;
  uint16_t SMPPTUNITDUTY;
  uint16_t SMPPTDUTYINCTIME;
  uint16_t SMPPTPOWERMARGIN;
  uint16_t VOLTAGECORRECTRATIOCH2;
  uint16_t CURRENTCORRECTRATIOCH2;
  uint16_t WORKINGCURRENTLIMIT;
  uint16_t CH1DACRATIO;
  uint16_t CURRHIGHMPPT;
  uint16_t GFDVOLTAGE;
  uint16_t DUTYLIMIT;
  short TEMPLIMIT;
  uint16_t CH2DACRATIO;
  uint8_t SINGLEMULTI;
  uint16_t UPCURRENT;
  uint16_t DOWNCURRENT;
  uint16_t CURRENTLEDOFFLEVEL;  
  uint16_t SCANTIME;  
  uint16_t SMPPTPOWERCOMPTIME;  
  uint16_t SMPPTDELAYTIME;
  uint8_t etx;
  uint8_t crc;
}SUMCUSET;

typedef union{
  SUMCUSET    s;
  uint8_t   _b[SUSETCOM_SIZE];
}USUMCUSET;


#define   SUDATACOM_SIZE        56
#define   SUDATACRC_SIZE        (SUDATACOM_SIZE - 1)
typedef struct{
  uint8_t   stx;
  uint8_t   lng;
  uint8_t   address;
  uint8_t   cmd;
  uint16_t  VI1_VALUE;
  uint16_t  VO1_VALUE;
  uint16_t  IO1_VALUE;
  uint16_t  PWO1_OUTVALUE;
  uint16_t  PWM1_VALUE;
  char      TEMP_1VALUE;
  uint16_t  VI2_VALUE;
  uint16_t  VO2_VALUE; 
  uint16_t  IO2_VALUE;  
  uint16_t  PWO2_OUTVALUE;
  uint16_t  PWM2_VALUE;
  char      TEMP_2VALUE;
  uint32_t  CURRENT_HIGH;
  uint32_t  PW_TOTALVALUE;
  uint16_t  CH2_MODE;
  uint16_t  CH02_GFD_PINVALUE;
  uint16_t  CH1_MODE;
  uint16_t  CH01_GFD_PINVALUE;
  uint16_t  error_flag1;
  uint8_t   SETSTATUS;
  uint16_t  GFD[2][2];    //CH1 P,M Ch2 P,M		
  uint8_t   dumy;
  uint8_t   etx;
  uint8_t   crc;
}SUDATAMCUCOM;
  
typedef union{
  SUDATAMCUCOM    s;
  uint8_t   _b[SUDATACOM_SIZE];
}USUDATAMCUCOM;

typedef struct{
  uint32_t    header;  
  uint8_t    	stx;
  uint8_t    	lng;
	uint8_t    	id;
	uint8_t    	cmd;
	uint8_t    	etx;
	uint8_t    	bcc;
}SLAVEREQ;

typedef union {
	SLAVEREQ		s;
	uint8_t			_b[10];
}USLAVEREQ;


typedef struct
{
  uint32_t sync;
  uint8_t stx;
  uint8_t lng;
  uint8_t address;
  uint8_t cmd;
  uint8_t paracode;
  uint32_t value;
  uint8_t etx;
  uint8_t crc;
}SU_SET;
typedef union
{
  SU_SET  s;
  uint8_t _b[15];   //FF AA FF AA 02 0A 03 A1 00 00 00 00 00 03 5D 
}USU_SET;


typedef struct{
  uint8_t scan_mode;
  uint32_t rootid;
  uint8_t stringid;
  uint8_t masterid;
  uint8_t slaveid;
  uint16_t f_recordpos;
  uint16_t r_recordpos;
  uint16_t wakeupgap;
  uint16_t    seqnum;
  uint8_t     adpos;
	uint8_t     retrycnt;
	uint8_t     s_mode;
	uint8_t     a_mode;
	uint8_t     resendcount;
	uint8_t			reqid;
	uint16_t		interval;
	String      blemsg;
	String      readmsg;
  uint8_t     rcvcount;
	unsigned long    epoch;
}HIMPELLIVE;

#define BUF_SIZE    8
#define MAX_SIZE		(BUF_SIZE - 1)
typedef struct{
  float       temp[BUF_SIZE];
  float       angleAcX[BUF_SIZE];
  float       angleAcY[BUF_SIZE];
  float       distance[BUF_SIZE];
}TC2023BUF;

typedef struct{
	uint8_t		stx;
	uint8_t		lng;
	uint8_t		cmd;
	uint16_t	devid;
	uint8_t		dumy[24];
	uint8_t		ver;
	uint8_t		etx;
	uint8_t		crc;
}TX2023COM;

typedef union{
	TX2023COM		s;
	uint8_t			_b[32];
}UTX2023COM;




typedef struct{
	uint8_t 		node;
	uint16_t		ch1involtage;
	uint16_t		ch1outvoltage;
	uint16_t		ch1outcurrent;
	uint16_t		ch1power;
	uint16_t		ch1pwm;
	uint8_t		  ch1mode;
	uint8_t		  ch1temp;
	uint16_t		ch2involtage;
	uint16_t		ch2outvoltage;
	uint16_t		ch2outcurrent;
	uint16_t		ch2power;
	uint16_t		ch2pwm;
	uint8_t		  ch2mode;
	uint8_t		  ch2temp;
	uint16_t		curr_high;
	uint8_t     statusbit;
}NODEINFO;


typedef struct{
	// uint16_t		masterid;
	// uint32_t		epoch;
	// uint16_t		radiation;
	// uint16_t		pvtemp;
	// uint16_t		humidity;
	// uint8_t		  sensor;
	// uint8_t		  slaveid;
	NODEINFO		d[12];
}MASTERCOM;

typedef union{
	MASTERCOM		s;
	uint8_t			_b[12*28];
}UMASTERCOM;




#define	INIT_MODE							0
#define	DETECT_MODE						10
#define	READANGLE_MODE				20
#define	READALTRA_MODE				30
#define	SEND_MODE							40
#define	REUPLOAD_MODE					50
#define	LOGSEND_MODE					60
#define	MISSSEND_MODE					70
#define	MISLOGSEND_MODE				80
#define	GETTIME_MODE					90
#define	GETTIMEACK_MODE				100
#define	BLINK_MODE						110
#define	WAIT_MODE							200

extern	HIMPELLIVE	  himpellive;
extern	uint8_t blebuf[256],bf_ptr,br_ptr;


extern  UBIT ctrl_flag; 
#define rootconnect_flag       	ctrl_flag._b.bit5
#define msgsended_flag      	  ctrl_flag._b.bit6
#define reupload_flag      			ctrl_flag._b.bit7
#define bleread_flag      			ctrl_flag._b.bit8
#define bleinit_flag      			ctrl_flag._b.bit9
#define shutdown_flag      			ctrl_flag._b.bit10
#define dobleinitflag      			ctrl_flag._b.bit11
#define ackreceived_flag      	ctrl_flag._b.bit12
#define paraset_flag      	    ctrl_flag._b.bit13
#define received_flag      	    ctrl_flag._b.bit14
#define swpwmset_flag      	    ctrl_flag._b.bit15
#define cmdreset_flag      	    ctrl_flag._b.bit16
#define fselsave_flag      	    ctrl_flag._b.bit17
#define rselsave_flag      	    ctrl_flag._b.bit18
#define epochset_flag           ctrl_flag._b.bit19

extern  UBIT ctrl_flag1; 

#define resend_flag             ctrl_flag1._b.bit0
#define dip1sw_flag            	ctrl_flag1._b.bit1
#define dip2sw_flag            	ctrl_flag1._b.bit2
#define dip3sw_flag            	ctrl_flag1._b.bit3
#define dip4sw_flag            	ctrl_flag1._b.bit4
#define dip5sw_flag            	ctrl_flag1._b.bit5
#define dip6sw_flag            	ctrl_flag1._b.bit6
#define dip7sw_flag            	ctrl_flag1._b.bit7
#define dip8sw_flag            	ctrl_flag1._b.bit8



void waterlevel_init(void);
void scan_SUR26A(void);
void check_serial(void);
uint8_t calculator_crc(uint8_t lng,uint8_t *buf);
void get_axisinfo(void);
void print_wakeup_reason(void);
float take_average(uint8_t lng,float *buf);
void write_eeprom(void);
void read_key(void);
void control_tc2023(void);
void check_blebuf(void);
void misslogger_write(void);
uint32_t  gotRootId();
void reqservertime(void);
void broad_message(void);
uint16_t calc_wakeup_time(void);
void control_report(void);
void send_himpercommand(void);
#endif