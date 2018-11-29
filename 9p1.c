#include "lcd.h"
#include "timer.h"
#include "uart.h"
#include "math.h"

#define tpd 153
#define zeroPos 7500
#define power -1.086
#define coe 65430

int adc = 1;
float cm = 0;

float sweepData[2][91];

void ping_init(void) //make sure the timer is disabled first
{
	//enable GPIO clock
	SYSCTL_RCGCGPIO_R |= 0b00000010;
	//enable Digital function of pin 3
	GPIO_PORTB_DEN_R |= 0b00001000;
	//enable timer function of pin 3
	GPIO_PORTB_PCTL_R |= 0x00007000;

	//start Timer clock
	SYSCTL_RCGCTIMER_R |= 0x00000008;
	//disable timer 3b
	TIMER3_CTL_R = 0x00000000;
	//setup timer
	TIMER3_CFG_R = 0x00000004;
	//put timer in capture mode
	TIMER3_TBMR_R |= 0x00000007;
	//set upper limit of timer
	TIMER3_TBILR_R |= 0x0000FFFF;
	//re-enable timer
	TIMER3_CTL_R |= 0x00000D00;
}

void sendPulse(void)
{
	//disable alternate function
	GPIO_PORTB_AFSEL_R &= ~0b00001000;
	//set as output
	GPIO_PORTB_DIR_R |= 0b00001000;
	//set pin high
	GPIO_PORTB_DATA_R |= 0b00001000;
	//wait 5 us
	timer_waitMicros(5);
	//set pin low
	GPIO_PORTB_DATA_R &= 0b11110111;
	//set as input
	GPIO_PORTB_DIR_R &= 0b11110111;
	//re-enable alternate function
	GPIO_PORTB_AFSEL_R |= 0b00001000;
}

float read_ping(){
	//send a pulse out and set GPIO in
	unsigned int startTime = 0;
	unsigned int endTime = 0;
	signed int delta = 0;
	sendPulse();
	timer_waitMicros(115);
	TIMER3_ICR_R = 0xFFFFFFFF;
	//wait for an event from the timer
	while ((TIMER3_RIS_R & TIMER_RIS_CBERIS) == 0)
	{}
	//record the start time of delta
	startTime = TIMER3_TBPS_R & 0x000000FF;
	startTime = startTime << 16;
	startTime += (TIMER3_TBR_R & 0x0000FFFF);
	//clear the interrupt indicating start of delta
	TIMER3_ICR_R |= 0x00000400;
	//wait for an event from the timer
	while ((TIMER3_RIS_R & TIMER_RIS_CBERIS) == 0)
	{}
	//record the end time of delta
	endTime = TIMER3_TBPS_R & 0x000000FF;
	endTime = endTime << 16;
	endTime += (TIMER3_TBR_R & 0x0000FFFF);
	//clear the interrupt indicating end of delta and overflow of the counter
	TIMER3_ICR_R |= 0x00000400;
	//change the delta calculation depending on if there is overflow
	delta = startTime - endTime;
	if(delta < 0)
	{
		delta = startTime + (0x0000FFFF - endTime);
	}
	return 0.0010625 * delta;
}

void adc_init(void){
	//GPIO initialization
	SYSCTL_RCGCGPIO_R |= 0b00000010;	//enable clock for port b
	SYSCTL_RCGCADC_R |= 0b00000001;		//disable SS0 sample sequencer to configure it
	GPIO_PORTB_AFSEL_R |= 0b00010000;	//enable pin 4 to use an alternate function
	GPIO_PORTB_DIR_R &= 0b11101111;		//make sure pin 4 is an input
	GPIO_PORTB_DEN_R &= 0b11101111;		//disable the digital functionality of pin 4
	GPIO_PORTB_AMSEL_R |= 0b00010000;	//enable the analog functionality of pin 4
	GPIO_PORTB_ADCCTL_R &= 0b11101111;
	//ADC initialization
	ADC0_ACTSS_R &= 0b0000000;			//initialize the ADC trigger source as processor (default)
	ADC0_EMUX_R &= 0xFFFFFF0;			//set 1st sample to use the AIN10 ADC pin
	ADC0_SSMUX0_R |= 0x000000A;			//enable raw interrupt status
	ADC0_SSCTL0_R |= 0x00000006;		//enable oversampling to average
	ADC0_SAC_R |= ADC_SAC_AVG_64X;
	ADC0_ACTSS_R |= 0x00000001;			//re-enable ADC0 SS0
}

void read_ir(){
	ADC0_PSSI_R=ADC_PSSI_SS0; 					// initialize sampling
	while((ADC0_RIS_R & ADC_RIS_INR0) == 0){	//wait for a sample to finish
		}
	adc = ADC0_SSFIFO0_R & 0x00000FFF;			//take sample data into main function
	ADC0_ISC_R = 0x0000001;						//clear the interrupt
	timer_waitMillis(50);						//wait 0.02s
	cm = coe * pow((float) adc, power);		//i hate powers 60188, 1.081
}

int get_ir_raw(){
	return adc;
}

float get_dis_ir(){
	return cm;
}

void PWM_init()
{
	//enable GPIO port B clock
	SYSCTL_RCGCGPIO_R |= 0b00000010;
	//set pin 5 as output
	GPIO_PORTB_DIR_R |= 0b00100000;
	//enable digital functionality of pin 5
	GPIO_PORTB_DEN_R |= 0b00100000;
	//enable alternate function of pin 5
	GPIO_PORTB_AFSEL_R |= 0b00100000;
	//set which alternate function of pin 5
	GPIO_PORTB_PCTL_R |= 0x00700000;

	//enable Timer clock
	SYSCTL_RCGCTIMER_R |= 0b00000010;
	//disable timer 1b
	TIMER1_CTL_R = 0x00000000;
	//set to a 16 bit timer
	TIMER1_CFG_R |= 0x0004;
	//set the timer mode to PWM, periodic timer
	TIMER1_TBMR_R |= 0x000A;

	//load upper limit of 20 ms period
	//20 ms = 320000 ticks which can be loaded into ILR, PR doesn't need anything
	TIMER1_TBILR_R = 0xE200;
	TIMER1_TBPR_R = 0x0004;
	//set initial match to 90 degrees
	TIMER1_TBMATCHR_R = 0x91EC;
	TIMER1_TBPMR_R = 0x0004;
	//re-enable timer
	TIMER1_CTL_R |= 0x0100;
}

void move_servo(float deg)
{
	//calculate the position
	float posChangeFloat = 320000 - (zeroPos + (tpd * deg));
	//cast to int so it can be written to registers
	int posChange = (int) posChangeFloat;
	//write to match registers
	TIMER1_TBMATCHR_R = posChange;
	TIMER1_TBPMR_R = posChange >> 16;
}


int main(void)
{
	PWM_init();
	ping_init();
	adc_init();
	lcd_init();
	uart_init();
	move_servo(0);
	timer_waitMillis(1000);
	char str[50];
	int i;

	sprintf(str, "Deg\tIR Distance(cm)\tSonarDistance(cm)\n\r");
	uart_sendStr(str);

	for(i = 0; i < 91; i += 1){
		move_servo(i*2);
		timer_waitMillis(20);
		read_ir();
		sweepData[0][i] = get_dis_ir();
		sweepData[1][i] = read_ping();
		sprintf(str, "%d\t%.1f\t\t%.1f\n\r", i*2, sweepData[0][i], sweepData[1][i] );
		uart_sendStr(str);
	}


}
