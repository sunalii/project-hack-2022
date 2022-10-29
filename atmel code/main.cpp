#define F_CPU 8000000
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/eeprom.h>
#include "LCD.h"
#include "I2C.h"

const uint8_t kaypad[4][4]={{7,8,9,10},{4,5,6,11},{1,2,3,12},{13,0,14,15}};
uint8_t getkeyNum();
char lcddata[20];
uint8_t keyout;
uint8_t user=0;

bool enterPassword();
void additem();
uint8_t hx711H=0; //Load Scale High Bits
uint16_t hx711L=0;//Load Scale Low Bits
volatile uint16_t TimerCal=0;// variable for collect echo data
float loadCellRead();
#define Loadcell_data 5
#define Loadcell_clk 4

const uint8_t password[5][6]={{7,8,9,1,2,3},{4,5,6,1,8,9},{1,2,3,2,9,8},{3,0,4,5,6,8},{9,8,7,5,3,1}};
void stepper(uint16_t rev,bool dir);

int main(void)
{
	
	TWIInit();
	DDRD|=0xF0;
	LcdInit();
	LcdSetCursor(4,0,"Welcome");
	_delay_ms(2000);
	
	for(uint8_t i=0;i<10;i++){
		//eeprom_write_byte((uint8_t*)i,0);
	}
	
	
	
	while(1){
	if (enterPassword())
	{break;
	}
	}
	
	additem();
	
	
	while (1)
	{
		
	}
}

uint8_t getkeyNum(){
	DDRA=0b00001111;
	PORTA=0b11110000;
	uint8_t getx;
	uint8_t gety;
	while(1){
		
		uint8_t pin=(~(PINA|0x0F));
		if (pin)
		{
			
			
			switch(pin){
				
				
				case (1<<4) :getx=0; break;
				case (1<<5) :getx=1;break;
				case (1<<6) :getx=2;break;
				case (1<<7) :getx=3;break;
				
				
			} //switch
			DDRA=0b0;
			PORTA=0b0;
			
			DDRA=0b11110000;
			PORTA=0b00001111;
			
			pin=~(PINA|0xF0);
			
			switch(pin){
				case (1<<0) :gety=0;break;
				case (1<<1) :gety=1;break;
				case (1<<2) :gety=2;break;
				case (1<<3) :gety=3;break;
				
			} //switch
			
			DDRA=0b00001111;
			PORTA=0b11110000;
			
			
			return kaypad[gety][getx];
			
		}//if
		
		
		
	}//while 1
}


bool enterPassword(){
	LcdCommand(LCD_CLEARDISPLAY);
	LcdSetCursor(0,0,"Enter password");
	
	_delay_ms(500);
	uint8_t pos=0;
	uint8_t numbers[6]={0,0,0,0,0,0};
	sprintf(lcddata,"%u%u%u%u%u%u",numbers[0],numbers[1],numbers[2],numbers[3],numbers[4],numbers[5]);
	LcdSetCursor(0,1,lcddata);
	LcdSetCursor(pos,1,"");
	LcdCommand(LCD_DISPLAYCONTROL|LCD_DISPLAYON|LCD_BLINKON);
	
	while(1){
		keyout=getkeyNum();
		if (keyout<10)
		{
			numbers[pos]=keyout;
			sprintf(lcddata,"%u%u%u%u%u%u",numbers[0],numbers[1],numbers[2],numbers[3],numbers[4],numbers[5]);
			LcdSetCursor(0,1,lcddata);
			pos++;
			if (pos>5)
			{pos=0;
			}
			LcdSetCursor(pos,1,"");
			_delay_ms(200);
		}
		
		else if (keyout==14)
		{LcdCommand(LCD_CLEARDISPLAY);
			LcdCommand(LCD_DISPLAYCONTROL|LCD_DISPLAYON|LCD_BLINKOFF);
			break;
		}
	}
	
	uint8_t outval=0;
	
	for(uint8_t i=0;i<5;i++){
		
		outval=0;
		for(uint8_t j=0;j<6;j++){
			if (password[i][j]==numbers[j])
			{outval++;
			}
		}
		
		if (outval>=6)
		{	user=i;
			break;
		}
		
		
	}
	
	LcdCommand(LCD_CLEARDISPLAY);
	
	if (outval>=6)
	{LcdSetCursor(0,0,"Password ok");
		_delay_ms(500);
		LcdCommand(LCD_CLEARDISPLAY);
		return 1;
	} 
	else
	{LcdSetCursor(0,0,"Password error");
		_delay_ms(500);
		LcdCommand(LCD_CLEARDISPLAY);
		return 0;
	}
	
	

}

float loadCellRead(){
	hx711H=0;hx711L=0;  //clear variables
	for(uint8_t i=0;i<8;i++){  // Load cell data high 8 bits
		PORTC|=(1<<Loadcell_clk); //Clock pin high
		_delay_us(10);
		if ((PINC&(1<<Loadcell_data))>>Loadcell_data)  //read data pin
		{hx711H|=(1<<(7-i));//set hx 711 varible
		}
		else
		{hx711H&=~(1<<(7-i));
		}
		PORTC&=~(1<<Loadcell_clk); //Clock pin low
		_delay_us(5);
	}
	
	
	for(uint8_t i=0;i<16;i++){ // Load cell data low 16 bits
		PORTC|=(1<<Loadcell_clk); //Clock pin high
		_delay_us(10);
		if ((PINC&(1<<Loadcell_data))>>Loadcell_data) //read data pin
		{hx711L|=(1<<(15-i));
		}
		else
		{hx711L&=~(1<<(15-i));
		}
		PORTC&=~(1<<Loadcell_clk); //Clock pin low
		_delay_us(5);
	}
	
	hx711L=hx711L>>1; //shift bits
	
	if (hx711H&1)  //bit setup
	{hx711L|=(1<<15);
	}
	else
	{hx711L&=~(1<<15);
	}
	hx711H=hx711H>>1;
	
	return (hx711H*(65536/18029.6))+hx711L/18029.6; //load cell calibration
}

void additem(){
	DDRC|=(1<<Loadcell_clk); //Load cell clock pin
	PORTC&=~(1<<Loadcell_clk);//Clock pin low
	uint8_t itemkind=0;
	while(1){
	LcdSetCursor(0,0,"Enter disposing ");
	LcdSetCursor(4,1,"item");
	_delay_ms(500);
	while(1){
		if (!(PINC&(1<<2)))
		{break;
		}
	}
	LcdCommand(LCD_CLEARDISPLAY);
	LcdSetCursor(0,0,"Calculating . .");
	uint16_t timeval=0;
	while(1){
		
		timeval++;
		sprintf(lcddata,"%u",timeval);
		LcdSetCursor(0,1,lcddata);
		_delay_ms(500);		
		if (!(PINC&(1<<3)))
		{break;
		}
	}
	LcdCommand(LCD_CLEARDISPLAY);
	sprintf(lcddata,"length %u",timeval);
	LcdSetCursor(0,0,lcddata);
	float weight=loadCellRead();
	sprintf(lcddata,"weight %0.0fg",weight);
	LcdSetCursor(0,1,lcddata);
	_delay_ms(2000);
	LcdCommand(LCD_CLEARDISPLAY);
	if ((5<=weight)&&(weight<=10)&&(14<=timeval)&&(timeval<=16))//5-10g,15cm
	{	
		itemkind=1;
		eeprom_write_byte((uint8_t*)(user*2),(eeprom_read_byte((uint8_t*)(user*2)))+1);
		LcdSetCursor(0,0,"Pen Detected");
		TWIStart();
		TWIWriteAddrs(0x20);
		TWIWriteData(6);
		TWIStop();
	}
	else if ((12<=weight)&&(weight<=28)&&(11<=timeval)&&(timeval<=15))//12-28g,11-15cm
	{	
		itemkind=2;
		eeprom_write_byte((uint8_t*)((user*2)+1),(eeprom_read_byte((uint8_t*)((user*2)+1)))+1);
		LcdSetCursor(0,0,"Highlighter Det.");
		TWIStart();
		TWIWriteAddrs(0x20);
		TWIWriteData(7);
		TWIStop();
	}
	else{
		LcdSetCursor(0,0,"Try again");
		TWIStart();
		TWIWriteAddrs(0x20);
		TWIWriteData(8);
		TWIStop();
	}
	LcdSetCursor(0,1,"ReEnter-1 Exit-2");
	keyout=20;
	keyout=getkeyNum();
	if (keyout==1)
	{LcdCommand(LCD_CLEARDISPLAY);
	}
	if (keyout==2)
	{	LcdCommand(LCD_CLEARDISPLAY);
		break;
	}

	}
	
	if (itemkind==1)
	{
		if (eeprom_read_byte((uint8_t*)(user*2))>=3)
	{eeprom_write_byte(((uint8_t*)(user*2)),eeprom_read_byte((uint8_t*)(user*2))-3);
		LcdSetCursor(0,0,"Select pen");
		LcdSetCursor(0,1,"Blue-1 Black-2 Red-3");
		while(1){
			keyout=getkeyNum();
			if ((keyout==1)||(keyout==2)||(keyout==3))
			{	
				TWIStart();
				TWIWriteAddrs(0x20);
				TWIWriteData(keyout);
				TWIStop();
				break;
			}
		}
		
		while(1){
			if (!(PINC&(1<<6)))
			{
				TWIStart();
				TWIWriteAddrs(0x20);
				TWIWriteData(0);
				TWIStop();
				break;
			}
		}
	stepper(5,1);
	stepper(5,0);	
	}
	else{
	LcdSetCursor(0,0,"Not enough");
	LcdSetCursor(0,1,"Balance");	
	}
	
	}
	else if (itemkind==2)
	{
			if (eeprom_read_byte((uint8_t*)(user*2+1))>=2)
			{eeprom_write_byte(((uint8_t*)(user*2+1)),eeprom_read_byte((uint8_t*)(user*2+1))-2);
			LcdSetCursor(0,0,"Select type");	
			LcdSetCursor(0,1,"Green-1 Yellow-2");
			
			while(1){
				keyout=getkeyNum();
				if ((keyout==1)||(keyout==2))
				{
					TWIStart();
					TWIWriteAddrs(0x20);
					TWIWriteData(keyout+3);
					TWIStop();
					break;
				}
			}
			while(1){
				if (!(PINC&(1<<6)))
				{
					TWIStart();
					TWIWriteAddrs(0x20);
					TWIWriteData(0);
					TWIStop();
					break;
				}
			}
			stepper(5,1);
			stepper(5,0);
			}
			else{
			LcdSetCursor(0,0,"Not enough");		
			LcdSetCursor(0,1,"Balance");		
			}
			
	}
	
}

void stepper(uint16_t rev,bool dir){
	for(uint16_t j=0;j<rev;j++){
		//4,4+5,5,5+6,6,6+7,7,7+4
		if (dir)
		{for(uint8_t i=4;i<8;i++){
			PORTD=(PORTD&0x0F)|(1<<i);
			_delay_ms(100);
			if (i==7)
			{PORTD|=(1<<4);
			}
			else
			{PORTD|=(1<<i+1);
			}
			_delay_ms(100);
		}
		} 
		else
		{for(uint8_t i=7;i>3;i--){
			PORTD=(PORTD&0x0F)|(1<<i);
			_delay_ms(100);
			if (i==7)
			{PORTD|=(1<<7);
			}
			else
			{PORTD|=(1<<i-1);
			}
			_delay_ms(100);
		}
		}
		
		
		
	}
	
}