#ifndef __PROC_H__
#define __PROC_H__

//#define	VER_INFO						1					//   Duration average
//#define	VER_INFO						2//   memset(&slaveset.s[i], 0x00, sizeof(STRINGSET));
//#define	VER_INFO						  2//      for (int i = 0; i < 12; i ++)        slaveset.s[oid].STARTVOLTAGE = 0;
#define	VER_INFO						  3     //psramdisk

#define	ESP32ALLETC_ID        1

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
#define RECORDSIZE        sizeof(HIMPERINFO)
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
	uint8_t setflag;
	uint8_t autobypass;
	uint8_t volume;
	uint8_t const1;
	uint8_t dumy1[5];
	uint8_t const2;
	uint8_t dumy2[2];
	uint8_t bcc;
  uint8_t etx;
}HIMPERCMD;

typedef union{
  HIMPERCMD   s;
	uint8_t		    _b[15];
} UHIMPERCMD;

typedef struct{
  uint16_t onoff;
	uint16_t flowmode;
	uint16_t flowlevel;
	uint16_t buf[13];
}HIMPERINFO;

typedef union{
  HIMPERINFO   s;
	uint8_t		    _b[32];
} UHIMPERINFO;


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
	uint8_t     rcbuf[15];
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
#define rccmd_flag           		ctrl_flag._b.bit20

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
void send_meshstatus(void);
#endif