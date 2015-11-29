/*
 * main.c
 *
 *  Created on: Sep 8, 2012
 *      Author: root
 */

#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define BAUDRATE 1200
#define BAUD_PRESCALLER (((F_CPU / (BAUDRATE * 16UL))) - 1)

/*
 #define FORWARD 0x11
 #define BACKWARD 0x22
 #define LEFT 0x33
 #define RIGHT 0x44
 #define STOPM 0x41
 */

//revision 1
#define FORWARD 'w'
#define BACKWARD 'q'
#define LEFT 'a'
#define RIGHT 's'
#define STOPM 'x'

uint8_t current;
//char last = 0x00;

char packet[5];

int state = 0;

int packetn = 0;

void initIO(void);
void callAction();
uint8_t receive(void);
void USART_init(void);

void USART_sendchar(unsigned char data);

int main(void)
{
	initIO();
	USART_init();
	sei();

	while(1)
	{
		//callAction();
		/*if(current != last)
		 {
		 USART_sendchar(current);
		 USART_sendchar('\n');

		 last = current;
		 }*/
		//USART_sendchar('\c');
		//USART_sendchar(state);
		while(packetn == 0)
			;

		USART_sendchar(packet[2]);
		current = packet[2];
		callAction();

		packetn = 0;
	}

	return 0;
}

void initIO(void)
{
	DDRD |= 1 << 4;
	DDRC = 0xFF;
	DDRB = 0x00;

	PORTC = 0x00;
	PORTD = 0x10;

	return;
}

void USART_init(void)
{
	UBRR0H = (unsigned char) (((F_CPU / (1200 * 16UL)) - 1) >> 8);
	UBRR0L = (unsigned char) ((F_CPU / (1200 * 16UL)) - 1);

	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = (1 << USBS0) | (3 << UCSZ00);

	UCSR0B |= (1 << RXCIE0);

	return;
}

uint8_t receive(void)
{
	while(!(UCSR0A & (1 << RXC0)))
		;

	return UDR0;
}

ISR(USART_RX_vect)
{
	//USART_sendchar('G');
	static char chksum;
	static char buff;

	buff = UDR0;

	/*if(state == 2)
	 {
	 USART_sendchar(state);
	 USART_sendchar(buff);
	 }
	 if(state == 1)
	 {
	 USART_sendchar(state);
	 USART_sendchar(buff);
	 }*/

	switch (state)
	{
	case 0:
		if(UDR0 == 0x44)
			state = 1;
		break;
		/*case 1:
		 if(UDR0 == 0x44)
		 state = 2;
		 break;*/
	case 1:
		if(UDR0 == 0x44)
			break;
		packet[2] = UDR0;
		state = 2;
		//USART_sendchar(packet[2]);
		break;
	case 2:
		packet[3] = UDR0;
		chksum = ~packet[2];
		if((packet[3] + packet[2]) == 0xFF)
		{
			if(packet[2] != current)
				packetn = 1;
			//USART_sendchar(packet[2]);
		}
		//USART_sendchar(packet[2]);
		state = 0;
		break;
	}
}

/*ISR(USART_RX_vect)
 {
 //PORTD = 0x00;

 uint8_t addr, data, chk;

 addr = receive();

 /*if(!(addr == 0xAA))
 {
 data = receive();
 chk = receive();

 if(addr == 0x44)
 {
 if(chk == data + addr)
 {
 current = data;
 }
 }
 }*/

/*
 * TRANSMIT PACKET CODE
 *
 * for (int i = 0; i < 5; i++)
 {
 USART_Transmit(0xAA);
 _delay_ms(10);
 }
 USART_Transmit(devID);
 _delay_ms(10);
 USART_Transmit(devID);
 _delay_ms(10);
 USART_Transmit(snd);
 _delay_ms(10);
 //USART_Transmit(devID + snd);
 USART_Transmit(~snd);
 _delay_ms(10);
 USART_Transmit(~devID);
 *
 *
 *



 if(addr == 0x44)
 {
 USART_sendchar('1');
 /*data = receive();

 chk = receive();

 if(chk == ~data)
 {
 current = data;
 }*

 }
 if(addr)

 current = data;

 //PORTD = 0x10;			TOOOOOOOOOOOO LONG
 } */

void USART_sendchar(unsigned char data)
{
	while(!(UCSR0A & (1 << UDRE0)))
		;
	UDR0 = data;

	return;
}

void callAction()
{
	switch (current)
	{
	case FORWARD:
		PORTC = 0b10100;
		break;
	case BACKWARD:
		PORTC = 0b01010;
		break;
	case LEFT:
		PORTC = 0b10010;
		break;
	case RIGHT:
		PORTC = 0b01100;
		break;
	case STOPM:
		PORTC = 0x00;
		break;
	case 'p':
		PORTD = 0x00;
		break;
	case 'o':
		PORTD = 0x10;
		break;
		/*case 'w':
		 PORTD = 0x00;
		 break;
		 case 'x':
		 PORTD = 0x10;
		 break;*/
	}

	return;
}
