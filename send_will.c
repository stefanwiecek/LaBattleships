// #include <stdio.h>
// #include <stdlib.h>
// #include <util/delay.h>
// #include <avr/io.h>
// #include "lcd.h"


// void USART_Init( unsigned int baud );
// void USART_Transmit( unsigned char data );

// unsigned char USART_Receive( void );

// int main(void) {

//     // 10x10 board, one for each player
//     /**
//      * - - - x - - - - - -
//      * x - - x - - - - - - 
//      * x - - - - - - - - -
//      * x - x x - - - - - - 
//      * - - - - - - - - - -
//      * - - - - - - - - - -
//      * - - - - - - - - - -
//      * - - - - - - - - - - 
//      * - - - - - - - - - -
//      * - - - - - - - - - -
//      *
//      * 0 - empty box
//      * 1 - ship tile, not hit
//      * 2 - ship tile, hit
//      * 3 - checked tile
//      */

//     // uint8_t player1[] = {0, 0, 0, 1, 0, 0,0,0,0,0,
//     //                      1, 0, 0, 1, 0, 0,0,0,0,0,
//     //                      1, 0, 0, 0, 0, 0,0,0,0,0,
//     //                      1, 0, 0, 1, 1, 0,0,0,0,0,
//     //                      0, 0, 0, 0, 0, 0,0,0,0,0,
//     //                      0, 0, 0, 0, 0, 0,0,0,0,0,
//     //                      0, 0, 0, 0, 0, 0,0,0,0,0,
//     //                      0, 0, 0, 0, 0, 0,0,0,0,0,
//     //                      0, 0, 0, 0, 0, 0,0,0,0,0,
//     //                      0, 0, 0, 0, 0, 0,0,0,0,0};

//     // uint8_t player2[] = {0, 0, 0, 1, 0, 0,0,0,0,0,
//     //                      1, 0, 0, 1, 0, 0,0,0,0,0,
//     //                      1, 0, 0, 0, 0, 0,0,0,0,0,
//     //                      1, 0, 0, 1, 1, 0,0,0,0,0,
//     //                      0, 0, 0, 0, 0, 0,0,0,0,0,
//     //                      0, 0, 0, 0, 0, 0,0,0,0,0,
//     //                      0, 0, 0, 0, 0, 0,0,0,0,0,
//     //                      0, 0, 0, 0, 0, 0,0,0,0,0,
//     //                      0, 0, 0, 0, 0, 0,0,0,0,0,
//     //                      0, 0, 0, 0, 0, 0,0,0,0,0};



//     // while (!isGameFinished(&player1, &player2)) {
//     //     // get user input 

//     // }
//     USART_Init(9600);
//     init_lcd();
//     display_string("READY for S!");
//     for(;;){
//         USART_Transmit('W');
//         display_char(USART_Receive());
//         _delay_ms(1000);   
//     }
// }



// // int isGameFinished(uint8_t *p1, uint8_t *p2) {
// //     // iterate through board; if either board does not have any ship tiles left (i.e. value 1), return true
// //     for (int i = 0, len = sizeof p1/sizeof p1[0]; i < len; i++) {
// //         if (p1[i] == 1 || p2[i] == 1) {
// //             return 0;
// //         }
// //     }
// //     // no ship tiles left, return 1
// //     return 1;
// // }

// void USART_Init( unsigned int baud ){
//     /* Set baud rate */
//     //UBRRH = (unsigned char)(baud>>8);
//     UBRR1 = (unsigned char)baud;
//     /* Enable receiver and transmitter */
//     UCSR1B = (1<<RXEN1)|(1<<TXEN1);
//     /* Set frame format: 8data, 2stop bit */
//     UCSR1C = (1<<USBS1)|(3<<UCSZ10);
//     }

// void USART_Transmit( unsigned char data )
// {
//     /* Wait for empty transmit buffer */
//     while ( !( UCSR1A & (1<<UDRE1)) );
//     /* Put data into buffer, sends the data */
//     UDR1 = data;
// }

// unsigned char USART_Receive( void )
// {
//     /* Wait for data to be received */
//     while ( !(UCSR1A & (1<<RXC1)) );
//     /* Get and return received data from buffer */
//     return UDR1;
// }

