#include "HC_05.h"
#include <avr/io.h>

volatile char hc05_last_rx = 0;

void hc05_init(uint32_t baud) {
    uint16_t ubrr = (F_CPU / (16 * baud)) - 1;

    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)ubrr;

    UCSR0B = (1 << RXEN0) | (1 << TXEN0);

    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void hc05_putc(char data) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = data;
	
}

void hc05_puts(const char *str) {
    while (*str) {
        hc05_putc(*str++);
    }
}

void hc05_puti( int16_t num ) {

	int8_t Hnum = (int8_t)num;
	
	Hnum = (-1)*Hnum;
	
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = Hnum;

	//while (!(UCSR0A & (1 << UDRE0)));
	//UDR0 = Lnum;
/*
    char buffer[7];
    uint8_t i = 0;
    uint8_t negativo = 0;

    if (num < 0) {
        negativo = 1;
        num = -num;
    }

    do {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    } while (num > 0);

    if (negativo) {
        buffer[i++] = '-';
    }

    while (i) {
        hc05_putc(buffer[--i]);
    }
*/
}


char hc05_getc(void) {
    while (!(UCSR0A & (1 << RXC0)));
    return UDR0;
}

uint8_t hc05_gets(char *buffer, uint8_t max_len) {
    uint8_t count = 0;
    char c;
    while (count < max_len - 1) {
        c = hc05_getc();
        if (c == '\n' || c == '\r') {
            break;
        }
        buffer[count++] = c;
    }
    buffer[count] = '\0';
    return count;
}

void hc05_enable_rx_interrupt(void) {
    UCSR0B |= (1 << RXCIE0);
}