#include "seven_seg.h"
#include "hal_general.h"
#include "hal_seven_seg.h"
#include <stdint.h>

//static variables to hold current display outputs
//used for UpdateDisplay() only
static uint8_t disp0 = 0;
static uint8_t disp1 = 0;
static uint8_t disp2 = 0;
static uint8_t disp3 = 0;
static uint8_t decimal = 0;

void SevenSeg_Init(void)
{
	hal_seven_seg_Init();
}

void SevenSeg_DisplayHex(uint16_t value)
{
	uint8_t ss0, ss1, ss2, ss3;
	ss3 = value & 0x000F;
	ss2 = (value >> 4) & 0x000F;
	ss1 = (value >> 8) & 0x000F;
	ss0 = (value >> 12) & 0x000F;
	
	hal_Display(ss0, ss1, ss2, ss3, 0);

	//updates static variables
	disp0 = ss0;
	disp1 = ss1;
	disp2 = ss2;
	disp3 = ss3;
	decimal = 0;
}

void SevenSeg_DisplayRaw(uint8_t seg0, uint8_t seg1, uint8_t seg2, uint8_t seg3)
{
	hal_Display(seg0, seg1, seg2, seg3, 0);

	//updates static variables
	disp0 = seg0;
	disp1 = seg1;
	disp2 = seg2;
	disp3 = seg3;
	decimal = 0;
}

void SevenSeg_DisplayFloat(float value)
{
	uint16_t val_int = 0;
	uint8_t dec = 0;
	uint8_t ss0, ss1, ss2, ss3;
	uint16_t value_BCD;
	
	if(value >= 0 && value < 1)
	{
		dec = 4;
		val_int = value * 1000;
		value_BCD = SevenSeg_BCD(val_int);
		ss3 = value_BCD & 0x000F;
		ss2 = (value_BCD >> 4) & 0x000F;
		ss1 = (value_BCD >> 8) & 0x000F;
		ss0 = (value_BCD >> 12) & 0x000F;
		hal_Display(ss3, ss2, ss1, ss0, dec);
	}
	else if(value >= 1 && value < 10)
	{
		dec = 4;
		val_int = value * 1000;
		value_BCD = SevenSeg_BCD(val_int);
		ss3 = value_BCD & 0x000F;
		ss2 = (value_BCD >> 4) & 0x000F;
		ss1 = (value_BCD >> 8) & 0x000F;
		ss0 = (value_BCD >> 12) & 0x000F;
		hal_Display(ss3, ss2, ss1, ss0, dec);
	}
	else if(value >= 10 && value < 100)
	{
		dec = 3;
		val_int = value * 100;
		value_BCD = SevenSeg_BCD(val_int);
		ss3 = value_BCD & 0x000F;
		ss2 = (value_BCD >> 4) & 0x000F;
		ss1 = (value_BCD >> 8) & 0x000F;
		ss0 = (value_BCD >> 12) & 0x000F;
		hal_Display(ss3, ss2, ss1, ss0, dec);
	}
	else if(value >= 100 && value < 1000)
	{
		dec = 2;
		val_int = value * 10;
		value_BCD = SevenSeg_BCD(val_int);
		ss3 = value_BCD & 0x000F;
		ss2 = (value_BCD >> 4) & 0x000F;
		ss1 = (value_BCD >> 8) & 0x000F;
		ss0 = (value_BCD >> 12) & 0x000F;
		hal_Display(ss3, ss2, ss1, ss0, dec);
	}
	else if(value >= 1000 && value < 10000)
	{
		dec = 1;
		val_int = value;
		value_BCD = SevenSeg_BCD(val_int);
		ss3 = value_BCD & 0x000F;
		ss2 = (value_BCD >> 4) & 0x000F;
		ss1 = (value_BCD >> 8) & 0x000F;
		ss0 = (value_BCD >> 12) & 0x000F;
		hal_Display(ss3, ss2, ss1, ss0, dec);
	}
	else 
	{
		hal_Display(0, 0, 0, 0, 0);
	}

	//updates static variables
	decimal = dec;
	disp0 = ss0;
	disp1 = ss1;
	disp2 = ss2;
	disp3 = ss3;

}

uint16_t SevenSeg_BCD(uint16_t value)
{
	uint16_t BCD_Result = 0;
	int shift = 0;
	
	while(value > 0)
	{
		BCD_Result |= (value % 10) << (shift++ << 2);
		value /= 10;
	}
	
	return BCD_Result;
}

void UpdateDisplay(void)
{
    hal_Display(disp3, disp2, disp1, disp0, decimal);
}
