/**
 * @file main.c
 * @author CCCPRabbit
 * @date 2026-01-29
 * @brief Main function
 */

#define F_CPU  16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include "usart_interrupt.h"

int main(){

    // Add your code here and press Ctrl + Shift + B to build
    InitUART(103);
    sei();
    
    while(1)
    {
        if(ReceiveByteAvailable()>=10)
        {
            for(unsigned char i=0;i<10;i++)
                TransmitByte(ReceiveByte());
            TransmitByte('\r');
        }
    }

    return 0;
}
