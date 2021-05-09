/**
 * Library for UART communication between LaFortuna devices
 * 
 * COMP2215 
 * wdk1g19 + saw1g19 2021
 * 
 * 
 * made with <3 for kpz
 */

#include "os.h"

void USART_Init(unsigned int baud);
void USART_Transmit(unsigned int data);
unsigned char USART_Receive(void);
unsigned int USART_Wait_And_Receive(void);
