#include<lpc17xx.h>
#include<stdio.h>
#include<math.h>

void display(float); //Seven Seg Display function declaration

unsigned int timeval=0;
float result=0.00;
int i = 0;

unsigned long SevenSeg[] = {0x3F0, 0x060, 0x5B0, 0x4F0, 0x660, 0x6D0, 0x7D0, 0x070, 0x7F0, 0x6F0};    //0,1,2,3,4,5,6,7,8,9 Display on seven seg

void timer_init1()
{
	LPC_TIM0->TCR=0x02;     //TC and PC Reset
	LPC_TIM0->PR=0x02;
	LPC_TIM0->MR0=0x49;
	LPC_TIM0->CTCR=0x00;    //Timer Mode
	LPC_TIM0->EMR=0x20;     //Set match bit upon match
	LPC_TIM0->MCR=0x04;     //Stop on MR0. TC and PC stopped and TCR0[0] = 0 if MR0 = TC
	LPC_TIM0->TCR=0x01;     //Enabled for Counting
}

void timer_init2()
{
	LPC_TIM1->TCR=0x02;     //TC and PC Reset
	LPC_TIM1->PR=0x02;
	LPC_TIM1->MR0=0x00;
	LPC_TIM1->CTCR=0x00;    //Timer Mode
	LPC_TIM1->EMR=0x10;
	LPC_TIM1->MCR=0x00;
	LPC_TIM1->TCR=0x01;     //Enable for Counting
}

int main()
{
	//P0.15=trig, P0.16=echo
	SystemInit();
	SystemCoreClockUpdate();
	LPC_PINCON->PINSEL0 &= 0x3F0000FF;     //P0.4 to P0.11 for 7 segment, P0.15 for Trigger HC-SR04
	LPC_PINCON->PINSEL1 &= 0xFFFFFFFC;     //P0.16 for Echo HC-SR04
	LPC_PINCON->PINSEL3 &= 0XFFC03FFF;     //P1.23 to P1.26 for 7 segment
	LPC_GPIO0->FIODIR = 0x00008FF0;        //P0.4 to P0.11 Output for 7 segment, P0.15 Output for Trigger, P0.16 Input for Echo
	LPC_GPIO1->FIODIR = 0x07800000;        //P1.23 to P1.26 Output for 7 Segment

	while(1)
	{
		LPC_GPIO0->FIOSET =(1<<15);          //Set P0.15
		timer_init1();                       //Trigger
		
		while(!(LPC_TIM0->EMR & (0x01)));    //10 microsecond Delay

		LPC_TIM0->TCR=0x02;                  //Stop Timer
		LPC_GPIO0->FIOCLR =(1<<15);          // clear P0.15 

		
		while(!(LPC_GPIO0->FIOPIN&(1<<16)));
		
		timer_init2();                       //Counting till we get Echo

		while((LPC_GPIO0->FIOPIN&(1<<16)));

		LPC_TIM1->TCR=0x00;                  //Stop Counter

		timeval= LPC_TIM1->TC;               //Time between
		result=timeval/58.31;                //Result in cm. result = (T(microseconds) * 0.0343 (cm / microseconds)) / 2
		//Distance D = Dist / 2, Dist = Velocity * Time, Dist is twice the gap between the sensor and object since it travels from sensor to object and comes back
		display(result);                     //Seven Segment Display
	}
}

void display(float f)
{
	int a, b,c,d,j;
	float dec1;

	a = f/10;                            //a is the digit at tens place
	b = fmod(f,10);                      //b is the digit at unit place
	dec1=fmod(f,1);
	dec1 = dec1 *100;
	c = dec1/10;                         //c is the digit at tenth place
	d = fmod(dec1,10);                   //d is the digit at hundredth place
	
	LPC_GPIO1->FIOPINH = 0x0000;
	LPC_GPIO0->FIOPINL = SevenSeg[d];    //setting the 4th display
	for(j=0;j<100;j++);

	LPC_GPIO1->FIOPINH = 0x0080;
	LPC_GPIO0->FIOPINL = SevenSeg[c];    //setting the 3rd display
	for(j=0;j<100;j++);

	LPC_GPIO1->FIOPINH = 0x0100;
	LPC_GPIO0->FIOPINL = SevenSeg[b];    //setting the 2nd display
	for(j=0;j<100;j++);

	LPC_GPIO1->FIOPINH = 0x0180;
	LPC_GPIO0->FIOPINL = SevenSeg[a];    //setting the 1st display
	for(j=0;j<100;j++);

	for(j=0;j<1000;j++); //delay
}
