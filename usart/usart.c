#include "usart.h"

void USART_Init(unsigned int baud)
{
    /* Set baud rate */
    //UBRRH = (unsigned char)(baud>>8);
    // UBRR1 = (unsigned char)baud;

    UBRR1H = (F_CPU / (baud * 16L) - 1) >> 8;
    UBRR1L = (F_CPU / (baud * 16L) - 1);

    /* Enable receiver and transmitter */
    UCSR1B = (1 << RXEN1) | (1 << TXEN1);
    /* Set frame format: 8data, 2stop bit */
    UCSR1C = (1 << USBS1) | (3 << UCSZ10);

    // UDR1 = 0x00;
}

void USART_Transmit(unsigned int data)
{
    /* Wait for empty transmit buffer */
    while (!(UCSR1A & (1 << UDRE1)))
        ;

    /* Put data into buffer, sends the data */
    UDR1 = data;
}

unsigned char USART_Receive(void)
{
    /* Wait for data to be received */
    // while ( !(UCSR1A & (1<<RXC1)) );
    if (UCSR1A & (1 << RXC1))
    {
        return UDR1;
    }

    /* Get and return received data from buffer */
    return '@';
}

unsigned int USART_Wait_And_Receive(void)
{
    /* Wait for data to be received */
    while (!(UCSR1A & (1 << RXC1)))
        ;
    /* Get and return received data from buffer */
    return UDR1;
}