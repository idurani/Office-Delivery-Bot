#include "sensors.h"

//volatile oi_t* data;
//volatile int unforeseeableConsequences;
//
//void TIMER0A_Handler()
//{
//	TIMER0_ICR_R = 0xFFFFFFFFF;
//	unforeseeableConsequences = usefulShit(data)
//}
//void usefulShit_init()
//{
//	//disable timer
//	TIMER0_CTL_R &= 0xFFFFFFFE;
//	//set up timer in 32-bit mode
//	TIMER0_CFG_R &= 0xFFFFFFF8;
//	//set up timer in periodic mode
//	TIMER0_TAMR_R |= 0x00000002;
//	TIMER0_TAMR_R &= 0xFFFFFFFE;
//	//set up upper limit of count
//	TIMER0_TAILR_R = 0x0000FFFF;
//	//set up interrupt for timeout
//	TIMER0_IMR_R |= 0x00000001;
//	NVIC_EN1_R |= 0x00000008;
//	IntRegister(INT_TIMER0A, TIMER0A_Handler)
//	IntMasterEnaable();
//	//
//	TIMER0_CTL_R |= 0x00000001;
//
//}

int usefulShit(oi_t *data)
{
	int unforeseeableConsequences = 0;
	oi_update(data);
	int cliffLeft = data->cliffLeftSignal;
	if(cliffLeft > 2700)
	{
		unforeseeableConsequences = unforeseeableConsequences + 0b00000001;
	}
	int cliffFrontLeft = data->cliffFrontLeftSignal;
	if(cliffFrontLeft > 2700)
	{
		unforeseeableConsequences = unforeseeableConsequences + 0b00000010;
	}
	int cliffFrontRight = data->cliffFrontRightSignal;
	if(cliffFrontRight > 2700)
	{
		unforeseeableConsequences = unforeseeableConsequences + 0b00000100;
	}
	int cliffRight = data->cliffRightSignal;
	if(cliffRight > 2700)
	{
		unforeseeableConsequences = unforeseeableConsequences + 0b00001000;
	}
	if(data->cliffLeft)
	{
		unforeseeableConsequences = unforeseeableConsequences + 0b00010000;
	}
	if(data->cliffFrontLeft)
	{
		unforeseeableConsequences = unforeseeableConsequences + 0b00100000;
	}
	if(data->cliffFrontRight)
	{
		unforeseeableConsequences = unforeseeableConsequences + 0b01000000;
	}
	if(data->cliffRight)
	{
		unforeseeableConsequences = unforeseeableConsequences + 0b10000000;
	}
	if(data->bumpLeft)
	{
		unforeseeableConsequences = unforeseeableConsequences + 0b000100000000;
	}
	if(data->bumpRight)
	{
		unforeseeableConsequences = unforeseeableConsequences + 0b001000000000;
	}
	return unforeseeableConsequences;
}
