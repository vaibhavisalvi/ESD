#include <lpc17xx.h>
#define FIRST_SEG 	0x00087000
#define SECOND_SEG 	0x0008F000
#define THIRD_SEG 	0x00097000
#define FOURTH_SEG 	0x0009F000
#define DISABLE_ALL 0x000A7000

#define	Ref_Vtg		9.50
#define	Full_Scale	0xFFF

unsigned int dig1=0x0,dig2=0x80,dig3=0x0,dig4=0x09;
float in_vtg;
unsigned int dig_count=0x00;
void Display(void);
void pwm_init(void);
unsigned long int i,j,k,temp1,temp2;
unsigned long int adc_temp;
float temp,float_var1;
unsigned char array_dec[10]={0x3F,0X06,0X5B,0X4F,0X66,0X6D,0X7D,0X07,0X7F,0X6F};


int main(void)
{
  SystemInit();
  SystemCoreClockUpdate();

	LPC_PINCON->PINSEL0 &= 0xFF0000FF; 	//data lines for 7-segment
	LPC_PINCON->PINSEL1 &= 0xFFFFFF00; 	//P0.15-P0.18 for decoder

	LPC_GPIO0->FIODIR |= 0x00000FF0;  	//P0.4-P0.11 as output
	LPC_GPIO0->FIODIR |= 0x00078000;	//P0.15-P0.18 as output

	LPC_SC->PCONP |= (1<<15);	//Power For GPIO Block

	LPC_PINCON->PINSEL3 |= 0XF0000000;	//P1.31 as AD0.5(fn 3)

	LPC_SC->PCONP |= (1<<12);	//enable the peripheral ADC
	pwm_init();
	while(1)
	{
		LPC_ADC->ADCR=1<<5|1<<21|1<<24;	//Selecting channel 5

    while((LPC_ADC->ADGDR & 0x80000000)==0);	//Polling until done bit is set
    adc_temp=LPC_ADC->ADDR5;	//Storing digital data
    adc_temp>>=4;
    adc_temp&=0xFFF;

    in_vtg = (((float)adc_temp * (float)Ref_Vtg))/((float)Full_Scale) + 90.0;	//calculating input analog voltage
		float_var1 = in_vtg;
		dig1 = (int)(in_vtg*100)%10; //Extracting 4th digit
		dig2 = (int)(in_vtg*10)%10;  //Extracting 3rd digit
		dig3 = ((int)in_vtg)%10;  //Extracting 2nd digit
		dig4 = ((int)in_vtg/10)%10; //Extracting 1st digit

		for(dig_count = 1; dig_count<=4; dig_count++) {

					Display();
    }



			if((float_var1>=92.6 && float_var1<92.7)||(float_var1>=92.8 && float_var1<92.9)||(float_var1>=93.40 && float_var1<93.50)||(float_var1>=93.6 && float_var1<93.7)||(float_var1>=98.2 && float_var1<98.3)||(float_var1>=98.4 && float_var1<98.5)||(float_var1>=94.2 && float_var1<94.3)||(float_var1>=94.4 && float_var1<94.5))
    {//Range specification to alter the intensity of the LED based on MR4 value
        LPC_PWM1->MR4=15000;
        LPC_PWM1->LER=0xFF;
    }
      else if((float_var1>=92.4 && float_var1<92.6)||(float_var1>=93.2 && float_var1<93.4)||(float_var1>=98.0 && float_var1<98.2)||(float_var1>=92.6 && float_var1<92.8)||(float_var1>=93.7 && float_var1<93.8)||(float_var1>=94.2 && float_var1<94.4))
    {
        LPC_PWM1->MR4=3000;
        LPC_PWM1->LER=0xFF;
    }
        else if((float_var1>=92.70 && float_var1<92.8)||(float_var1>=93.50 && float_var1<93.60)||(float_var1>=98.30 && float_var1<98.4)||(float_var1>=94.3 && float_var1<94.4))
    {//Match radio station
        LPC_PWM1->MR4=30000;
        LPC_PWM1->LER=0xFF;
    }
    else
    {
        LPC_PWM1->MR4=100;
        LPC_PWM1->LER=0xFF;//transfer to shadow
    }
	 }
}



void pwm_init()
{
    LPC_SC->PCONP |= (1<<6);  //PWM1 is powered
    LPC_PINCON->PINSEL3|=2<<14; //PWM1.4 is selected for P1.23
    LPC_PWM1->TCR=0x02;	//Reset counter and prescaler
    LPC_PWM1->PR=0; //Count frequency:Fpclk
    LPC_PWM1->PCR = 0X00001000;	//Single edge
    LPC_PWM1->MCR=0x02; // Reset On Match of PWM1MR0
    LPC_PWM1->MR0=30000;//trial and error
    LPC_PWM1->MR4=100;
    LPC_PWM1->LER=0xFF;	//enable shadow copy register
    LPC_PWM1->TCR=0x09;//Time start(Enable at pin 3)
    return;

}

void Display(void)
{
	if(dig_count==0x01)
	{
		temp1=dig1;
		LPC_GPIO0->FIOPIN=FIRST_SEG;
	}
	else if(dig_count==0x02)
	{
		temp1=dig2;
		LPC_GPIO0->FIOPIN=SECOND_SEG;
	}
	else if(dig_count==0x03)
	{
		temp1=dig3;
		LPC_GPIO0->FIOPIN=THIRD_SEG;
	}
	else if(dig_count==0x04)
	{
		temp1=dig4;
		LPC_GPIO0->FIOPIN=FOURTH_SEG;
	}
	temp1 &= 0x0F;
	temp2=array_dec[temp1];
	if(dig_count == 3)
		temp2 |= 0x80;		//for displaying decimal point for dig3.
	temp2=temp2<<4;
	LPC_GPIO0->FIOPIN|=temp2;
	for(i=0;i<200;i++);
	LPC_GPIO0->FIOPIN=DISABLE_ALL;
	LPC_GPIO0->FIOCLR=0x00000FF0;
}
