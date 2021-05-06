/* COMP2215 Task 5---SKELETON */

#include "os.h"


int update_dial(int);
int collect_delta(int);
int check_switches(int);
void USART_Init( unsigned int baud );
void USART_Transmit( unsigned char data );
unsigned char USART_Receive( void );
int read_data(int state);


int position = 0;


void main(void) {
    USART_Init(9600);
    os_init();

    os_add_task( read_data, 500, 1);
    os_add_task( collect_delta,   500, 1);
    os_add_task( check_switches,  100, 1);

    sei();
    for(;;){}

}


int collect_delta(int state) {
	position += os_enc_delta();
	return state;
}

int read_data(int state){
    unsigned int data = USART_Receive();
    if (data != '@'){
        display_char(data);
    }
    return state;
}

int check_switches(int state) {

	if (get_switch_press(_BV(SWN))) {
		USART_Transmit('N');
	}

	if (get_switch_press(_BV(SWE))) {
		USART_Transmit('E');
	}

	if (get_switch_press(_BV(SWS))) {
			display_string("South\n");
	}

	if (get_switch_press(_BV(SWW))) {
			display_string("West\n");
	}



	if (get_switch_short(_BV(SWC))) {
			display_string("[S] Centre\n");
	}

	if (get_switch_rpt(_BV(SWN))) {
			display_string("[R] North\n");
	}

	if (get_switch_rpt(_BV(SWE))) {
			display_string("[R] East\n");
	}

	if (get_switch_rpt(_BV(SWS))) {
			display_string("[R] South\n");
	}

	if (get_switch_rpt(_BV(SWW))) {
			display_string("[R] West\n");
	}

	if (get_switch_rpt(SWN)) {
			display_string("[R] North\n");
	}

	return state;
}

void USART_Init( unsigned int baud ){
    /* Set baud rate */
    //UBRRH = (unsigned char)(baud>>8);
    UBRR1 = (unsigned char)baud;
    /* Enable receiver and transmitter */
    UCSR1B = (1<<RXEN1)|(1<<TXEN1);
    /* Set frame format: 8data, 2stop bit */
    UCSR1C = (1<<USBS1)|(3<<UCSZ10);
    }

void USART_Transmit( unsigned char data )
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