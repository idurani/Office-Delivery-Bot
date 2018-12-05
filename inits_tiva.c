/**
 * @file inits_tiva.c
 * @brief this file contains functions for initialization
 * @author Jared Rickard
 * @date 12/05/2018
 */

#include <inits_tiva.h>

/**
 * initialize the ping sensor
 * @author Jared Rickard
 * @date 12/05/2018
 */
void ping_init(void)
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

/**
 * initialize the IR sensor
 * @author Jared Rickard
 * @date 12/05/2018
 */
void adc_init(void){
    //GPIO initialization
	//enable clock for port b
    SYSCTL_RCGCGPIO_R |= 0b00000010;
    //disable SS0 sample sequencer to configure it
    SYSCTL_RCGCADC_R |= 0b00000001;
    //enable pin 4 to use an alternate function
    GPIO_PORTB_AFSEL_R |= 0b00010000;
    //make sure pin 4 is an input
    GPIO_PORTB_DIR_R &= 0b11101111;
    //disable the digital functionality of pin 4
    GPIO_PORTB_DEN_R &= 0b11101111;
    //enable the analog functionality of pin 4
    GPIO_PORTB_AMSEL_R |= 0b00010000;
    GPIO_PORTB_ADCCTL_R &= 0b11101111;
    //ADC initialization
    //initialize the ADC trigger source as processor (default)
    ADC0_ACTSS_R &= 0b0000000;
    //set 1st sample to use the AIN10 ADC pin
    ADC0_EMUX_R &= 0xFFFFFF0;
    //enable raw interrupt status
    ADC0_SSMUX0_R |= 0x000000A;
    //enable oversampling to average
    ADC0_SSCTL0_R |= 0x00000006;
    ADC0_SAC_R |= ADC_SAC_AVG_64X;
    //re-enable ADC0 SS0
    ADC0_ACTSS_R |= 0x00000001;
}

/**
 * initialize the PWM signal to the turret servo
 * @author Jared Rickard
 * @date 12/05/2018
 */
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

/**
 * load the song into the OI
 * @author
 * @date 12/05/2018
 */
void loadMary(){
	///array of notes
    unsigned char notes[] = {64, 62, 60, 62, 64, 64, 64};
    ///array of note durations
    unsigned char duration[] = {32, 32, 32, 32, 32, 32, 32};
    ///load the notes and their durations into the OI
    oi_loadSong(1, 7, notes, duration);
}
