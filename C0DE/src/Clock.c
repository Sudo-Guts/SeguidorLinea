#include "Clock.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#ifndef F_CPU
#error "F_CPU debe definirse desde el Makefile"
#endif

#if F_CPU == 8000000UL
#define CLOCK_PRESCALER_BITS 0
#elif F_CPU == 1000000UL
#define CLOCK_PRESCALER_BITS ((1 << CLKPS1) | (1 << CLKPS0))
#else
#error "Clock_init solo admite 8 MHz o 1 MHz desde RC interno de 8 MHz"
#endif

void Clock_init(void) {
    uint8_t old_sreg = SREG;
    cli();
    /* Secuencia protegida: completar segunda escritura en cuatro ciclos. */
    CLKPR = (1 << CLKPCE);
    CLKPR = CLOCK_PRESCALER_BITS;
    SREG = old_sreg;
}
