#include<lpc21xx.h>
#define sw1 14
#define sw2 15
#define sw3 16
#define lcd 0XF<<20
#define RS 1<<17 
#define RW 1<<18
#define E  1<<19
typedef unsigned int u32;
int c=0;
int d=0;
struct can
{
	u32 id;
	u32 rtr;
	u32 dlc;
	u32 byteA;
	u32 byteB;
};
void delay_ms(int s){
 	T0PR=60000-1;
 	T0TCR=0X01;
 	while(T0TC<s);
 	T0TCR=0X03;
 	T0TCR=0;
}
void COMMAND(unsigned char data)
{
 	IOCLR1=lcd;
	IOSET1=(data&0XF0)<<16;
	IOCLR1=RS;
	IOCLR1=RW;
	IOSET1=E;
	delay_ms(2);
	IOCLR1=E;

	IOCLR1=lcd;
	IOSET1=(data&0X0F)<<20;
	IOCLR1=RS;
	IOCLR1=RW;
	IOSET1=E;
	delay_ms(2);
	IOCLR1=E;

}
void INIT(void)
{
	IODIR1=lcd|RS|E|RW;
	COMMAND(0X01);//TO CLEAR THE DISPLAY
	COMMAND(0X02);//TO RETURN CURSOR TO HOME
	COMMAND(0X0C);//DISPLAY ON CURSOR OFF
	COMMAND(0X28);//4-BIT INTERFACING
	COMMAND(0X80);//TO SELECT DDRAM
}
void DATA(unsigned char data)
{
 	IOCLR1=lcd;
	IOSET1=(data&0XF0)<<16;
	IOSET1=RS;
	IOCLR1=RW;
	IOSET1=E;
	delay_ms(2);
	IOCLR1=E;

	IOCLR1=lcd;
	IOSET1=(data&0X0F)<<20;
	IOSET1=RS;
	IOCLR1=RW;
	IOSET1=E;
	delay_ms(2);
	IOCLR1=E;

}

void STR(unsigned char* p)
{
	while(*p){
	 	DATA(*p++);
	}
}
void LCD_INTEGER(int n)
{
	unsigned char a[5];
	signed char i=0;
	if(n==0)
		DATA('0');
	else
		if(n<0)
		{
			DATA('-');
			n=-n;
		}
		while(n>0)
		{
			a[i++]=n%10;
			n=n/10;
		}
		for(--i;i>=0;i--)
		DATA(a[i]+48);
}
void can_init()
{
 	PINSEL1|=0X00014000;//P0.23 P0.24
 	VPBDIV=1;//60MHZ
 	C2MOD=0x1;//reset mode for writting 
 	AFMR=0X02;
 	C2BTR=0X001C001D;
 	C2MOD=0x0;//normal mode
}
void can_tx(struct can m1)
{
 	C2TID2=m1.id;
	C2TFI2=m1.dlc<<16;
	if(m1.rtr==0)
	{
	 	C2TFI2&=~(1<<30);//RTR=0
		C2TDA2=m1.byteA;
		C2TDB2=m1.byteB;
	}
	else
	{
		C2TFI2|=(1<<30);//RTR=1
	}
	C2CMR=(1<<0)|(1<<6);
	while(C2GSR&(1<<3)==0);
}
void can_rx(struct can *m2)
{
 	while(C2GSR&(0X01)==0);
	m2->id=C2RID;
	m2->dlc=C2RFS>>16&0XF;
	m2->rtr=C2RFS>>30&1;
	if(m2->rtr==0){
		m2->byteA=C2RDA;
		m2->byteB=C2RDB;
		}
		C2CMR=1<<2;
}
void START(void)
{
		unsigned char *s="VEHICLE DASHBOARD USING CAN COMMUNICATION";
		char i=0;
		while(i<44)
		{
			COMMAND(0X80);
			STR(s+i);
			delay_ms(200);
			COMMAND(0X01);
			i++;
		}
}


int main()
	{
 	struct can m1;
	struct can m2;
	struct can m3;
	struct can m4;
	INIT();
	can_init();

	m1.rtr=0;
 	m1.dlc=4;
 	m1.byteA=0xffffffff;
 	m1.byteB=0;

	m3.id=0x5;
	m3.rtr=0;
 	m3.dlc=4;
 	//m3.byteA=0xffffffff;
 	m3.byteB=0;

	//m4.id=0x3;
	m4.rtr=0;
 	m4.dlc=4;
 	m4.byteA=0xffffaaaa;
 	m4.byteB=0;


 	START();
 	while (1)
	{
        if(((IOPIN0 >> sw1)&1) == 0)
		{  
            m1.id = 0x01;    
            can_tx(m1);

			can_rx(&m2);

			//COMMAND(0xcb);
			//LCD_INTEGER(m2.byteA);
			if (m2.byteA <=360) 
			{
       			 COMMAND(0xc0); 
       			 STR("LOW FUEL..!       ");
			} 
			if (m2.byteA >361 && m2.byteA<=700) 
			{
        		COMMAND(0xc0);
       			STR("MEDIUM FUEL..        ");
	  		}
			if (m2.byteA >700) 
			{
       		 COMMAND(0xc0); 
       		 STR("HIGH FUEL...        ");
			}        
        }
       
        if(((IOPIN0 >> sw2)&1) == 0)
		{  
            delay_ms(200);
			d++;
			switch(d)
			{
				case 1:
				{
					m4.id=0x3;
					can_tx(m4);
					COMMAND(0XC0);
			        STR("Left Indicator on...       "); 
					break;
				}
				default:
				{
					m4.id=0x4;
					can_tx(m4);
					COMMAND(0XC0);
			        STR("Left Indicator off...       "); 
					
				}  
				if(d==2);
				d=0;
				}
		}
		
		if(((IOPIN0 >> sw3)&1) == 0) 
		{  
	     	delay_ms(200);
			c++;
			switch(c)
			{
				case 1:
				{
					//m3.id=0x5;
					m3.byteA=9000;
					can_tx(m3);
					COMMAND(0XC0);
			        STR("Low Speed...      "); 
					break;
				}
				case 2:
				{
					//m3.id=0x5;
					m3.byteA=12000;
					can_tx(m3);
					COMMAND(0XC0);
			        STR("Medium Speed...      "); 
					break;
				}
				case 3:
				{
					//m3.id=0x5;
					m3.byteA=15000;
					can_tx(m3);
					COMMAND(0XC0);
			        STR("High Speed...          ");
					break;
				}
				default:
				{
					//m3.id=0x5;
					m3.byteA=0;
					can_tx(m3);
					COMMAND(0XC0);
			        STR("Wiper off...         "); 
				}
			}
			if(c==4)
			c=0;
			//COMMAND(0XC0);
			//STR("wiper...");          
        }  
    }	
}



	   /*
		can_rx(&m2);
		COMMAND(0xc0);
		LCD_INTEGER(m2.byteA);
		if (m2.byteA <=360) 
		{
        COMMAND(0x80); 
        STR("LOW FUEL..!");
		} 
		if (m2.byteA >361 && m2.byteA<=700) 
		{
        COMMAND(0x80);
        STR("MEDIUM FUEL");
	    }
		if (m2.byteA >700) 
		{
        COMMAND(0x80); 
        STR("HIGH FUEL...");
		}  */
