#ifndef __MYTYPE_DEF__
#define __MYTYPE_DEF__

struct BITS
{
	uint8_t	bit0:1;
	uint8_t	bit1:1;
	uint8_t	bit2:1;
	uint8_t	bit3:1;
	uint8_t	bit4:1;
	uint8_t	bit5:1;
	uint8_t	bit6:1;
	uint8_t	bit7:1;
	uint8_t	bit8:1;
	uint8_t	bit9:1;
	uint8_t	bit10:1;
	uint8_t	bit11:1;
	uint8_t	bit12:1;
	uint8_t	bit13:1;
	uint8_t	bit14:1;
	uint8_t	bit15:1;
	uint8_t	bit16:1;
	uint8_t	bit17:1;
	uint8_t	bit18:1;
	uint8_t	bit19:1;
	uint8_t	bit20:1;
	uint8_t	bit21:1;
	uint8_t	bit22:1;
	uint8_t	bit23:1;
	uint8_t	bit24:1;
	uint8_t	bit25:1;
	uint8_t	bit26:1;
	uint8_t	bit27:1;
	uint8_t	bit28:1;
	uint8_t	bit29:1;
	uint8_t	bit30:1;
	uint8_t	bit31:1;						
};

struct WITS
{
	uint8_t	bit0:2;
	uint8_t	bit1:2;
	uint8_t	bit2:2;
	uint8_t	bit3:2;
	uint8_t	bit4:2;
	uint8_t	bit5:2;
	uint8_t	bit6:2;
	uint8_t	bit7:2;
};

struct DWITS
{
	uint8_t	bit0:3;
	uint8_t	bit1:3;
	uint8_t	bit2:2;
	uint8_t	bit3:1;
	uint8_t	bit4:3;
	uint8_t	bit5:3;
	uint8_t	bit6:1;
	uint8_t	bit7:2;
	uint8_t	bit8:3;
	uint8_t	bit9:3;
	uint8_t bit10:3;
	uint8_t	bit11:3;
	uint8_t bit12:2;
};

union UBIT
{
	struct BITS			_b;
	uint32_t   	    _dwb;
	uint16_t   	    _wb[2];
	uint8_t 				_bb[4];
};

#endif