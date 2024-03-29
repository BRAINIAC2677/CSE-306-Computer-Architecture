/*
 * Register Bank.cpp
 *
 * Created: 2/18/2023 2:40:15 PM
 * Author : Team GGWP
 */

#define F_CPU 1000000

#include <avr/io.h>

unsigned char arr[16];
unsigned char data, address1, address2;

void UART_init(void)
{
  // Normal speed, disable multi-proc
  UCSRA = 0b00000000;
  // Enable Rx (not Tx), disable interrupts
  UCSRB = 0b00010000;
  // Asynchronous mode, no parity, 1 stop bit, 8 data bits
  UCSRC = 0b10000110;
  // Baud rate 9600bps, assuming 1MHz clock
  UBRRL = 12;
  UBRRH = 0;
}

unsigned char UART_receive(void)
{
  // Wait until RXC flag is set to logic 1
  while ((UCSRA & (1 << RXC)) == 0x00)
    ;
  return UDR; // Read the received character from UDR
}

int main(void)
{
  DDRA = 0xFF;
  DDRC = 0xFF; // A and C will be for outputting two 8 bit results
  DDRB = 0x00; // will receive the data to be written to the specified register
  DDRD = 0b01011110;
  // D4 D3 D2 D1 for register index
  // PD0 RXD, TXD won't be enabled
  // D6 for cjmp output
  // D5 for write flag input

  // $zero, $t0, $t1, $t2, $t3, $t4, BP, SP
  arr[6] = arr[7] = 0xFF;
  MCUCSR = (1 << JTD);
  MCUCSR = (1 << JTD); // for enabling port C for I/O

  while (1)
  {
    data = UART_receive(); // both 4-bit addresses will be in concatenated form
                           // here
    address1 = data & (15 << 4);
    address2 = data & 15;

    unsigned char write_flag;
    write_flag = PIND & (1 << 5);

    if (write_flag)
    {
      unsigned char reg_address = 0;
      reg_address = PIND & (15 << 1);

      if (reg_address != 0)
      {
        // write nothing to zero register
        unsigned char write_data = PINB;
        arr[reg_address] = write_data;
      }
    }

    PORTA = arr[address1];
    PORTC = arr[address2];

    if (arr[address1] == arr[address2])
    {
      PORTD = PORTD | (1 << 6);
    }
    else
    {
      PORTD = PORTD & ~(1 << 6);
    }
  }
}