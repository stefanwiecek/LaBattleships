/**
 * Library for UART communication between LaFortuna devices
 * 
 * COMP2215 
 * wdk1g19 + saw1g19 2021
 * 
 * 
 * made with <3 for kpz
 */


void USART_Init( unsigned int baud ){
    /* Set baud rate */
    //UBRRH = (unsigned char)(baud>>8);
    UBRR1 = (unsigned char)baud;
    /* Enable receiver and transmitter */
    UCSR1B = (1<<RXEN1)|(1<<TXEN1);
    /* Set frame format: 8data, 2stop bit */
    UCSR1C = (1<<USBS1)|(3<<UCSZ10);
    }

void USART_Transmit( unsigned int data )
{
    /* Wait for empty transmit buffer */
    while ( !( UCSR1A & (1<<UDRE1)) );

    /* Put data into buffer, sends the data */
    UDR1 = data;
}

unsigned char USART_Receive( void )
{
    /* Wait for data to be received */
    // while ( !(UCSR1A & (1<<RXC1)) );
    if (UCSR1A & (1<<RXC1)){
        return UDR1;
    }

    /* Get and return received data from buffer */
    return '@';
}


unsigned int USART_Wait_And_Receive( void )
{
    /* Wait for data to be received */
    while ( !(UCSR1A & (1<<RXC1)) );

    /* Get and return received data from buffer */
    return UDR1;
}
