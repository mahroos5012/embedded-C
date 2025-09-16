#include<lpc21xx.h>
#include<string.h>
#include"4bit.h"
#include"eeprom.h"
#define sw1 1<<14
#define sw2 1<<15
#define LED 1<<17
void display(void);
int main()
{		
    int incount=0;
	int outcount=0;
	int totalcount=0;						   
    display();
	init_i2c();
	
   i2c_eeprom_write(0x50,0x00,'0');
   i2c_eeprom_write(0x50,0x00,incount);
   //incount=i2c_eeprom_read(0x50,0x00);
   delay_ms(200);
	IODIR0|=LED;
	while(1)
	{

   if((IOPIN0&(1<<sw1))==0)
	{
	   delay_ms(200);
	   incount++;
	   IOCLR0=LED;
	  LCD_COMMAND(0xc0);
	  LCD_DATA((incount/10)+48);
	  LCD_DATA((incount%10)+48);
	   i2c_eeprom_write(0x50,0x00,incount);	
	   while((IOPIN0&(1<<sw1))==0);
	}
if((IOPIN0&(1<<sw2))==0)
	{
	  delay_ms(200);

	  if(incount!=outcount)
	  	  outcount++;

	  LCD_COMMAND(0xc4);
	  LCD_DATA((outcount/10)+48);
	  LCD_DATA((outcount%10)+48);
	  while((IOPIN0&(1<<sw2))==0);
	 }
	 if(incount>outcount)
	 {
	 IOCLR0=LED;
	 }
	 else
	 {
	  IOSET0=LED;
	  }
	  totalcount=incount-outcount;
	  LCD_COMMAND(0xc9);
	  LCD_DATA((totalcount/10)+48);
	  LCD_DATA((totalcount%10)+48);
	  

}
}

void display()
{
   LCD_INIT();
   LCD_COMMAND(0xc0);
   LCD_STRING("V24CE2K4"); 
   LCD_COMMAND(0x80);
   LCD_SCROLLING("BIDIRECTIONAL VISITOR COUNTER                 ");
   
   LCD_COMMAND(0x01);
   LCD_COMMAND(0x80);
   LCD_STRING("IN");
   LCD_COMMAND(0x84);
   LCD_STRING("OUT");
   LCD_COMMAND(0x89);
   LCD_STRING("TOTAL");
 }
