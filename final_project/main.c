#include <msp430.h>
#include <math.h>




float digital_value = 0;
float count_limit = 100;
int count = 0;
volatile int timer_enable = 0;

/**
 * main.c
 */
int main(void)
{

    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    TACTL &= ~TACLR;
    TACTL |= TASSEL_1;
    TACTL |= MC_1;
    TACTL |= ID_1;

    TACCR0 = 10;
    TACCTL0 |= CCIE;
    TACCTL0 &= ~CCIFG;

    TACCR1 = 5;
    TACCTL1 |= CCIE;
    TACCTL1 &= ~CCIFG;






    ADC10CTL0 &= ~ENC; //disable ADC and enable bit setting

    //setting port 1 pin 4 to be a analog input
    P1SEL |= BIT4;
    ADC10AE0 |= BIT4;

    P1DIR |= BIT0;
    P1DIR |= BIT1;




    //settings for the control register 0 for ADC+
    ADC10CTL0 |= REFON; //enable reference voltage
    ADC10CTL0 |= SREF_0;//turns reference voltage at 0v and 3.3v
    ADC10CTL0 |= ADC10SHT_2; // we want the conversion form analog to digital to take 16 clock cycles
    ADC10CTL0 |= ADC10IE; // We enable interrupt for ADC
    __enable_interrupt();

    //settings for the control register for
    ADC10CTL1 |= INCH_4; // sets ADC to read channel 4 or port 1 pin 4
    ADC10CTL1 |= SHS_0;
    ADC10CTL1 |= ADC10SSEL_3; // we choose the SMCLK ADC clock
    ADC10CTL0 |= ADC10ON;//turn on the ADC



    while(1){
        //we turn on, enable, and then wait for ADC to give value
        ADC10CTL0 |= ENC;


        while( (abs(count_limit - count)) < 5){
        ADC10CTL0 |= ADC10SC;
        while( ADC10BUSY & ADC10CTL1 );
        }


            if((abs(count_limit - count)) != 0){
            timer_enable = 1;
            }




        ADC10CTL0 &= ~ENC;
    }


    return 0;
}




#pragma vector = ADC10_VECTOR
__interrupt void ADC10_ISR(void){

        digital_value = ADC10MEM;
        count_limit = digital_value * (0.17);
        ADC10CTL0 &= ~ADC10IFG;
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void TIMER_PERIOD_ISR(void){
               P1OUT &= ~BIT0;
               TACCTL0 &= ~CCIFG;
}

#pragma vector = TIMER0_A1_VECTOR
__interrupt void TIMER_FE_ISR(void){

               if(timer_enable == 1){
               P1OUT |= BIT0;
               timer_enable = 0;

                           if(count_limit > count){
                           P1OUT &= ~BIT1; //direction
                           count++;
                           }

                           if(count_limit < count){
                           P1OUT |= BIT1; //direction
                           count--;
                           }

               }


               TACCTL1 &= ~CCIFG;
}





