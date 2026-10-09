#ifndef CLOCK_H
#define CLOCK_H

/* Selecciona el prescaler CPU para el oscilador RC interno de 8 MHz.
 * Debe llamarse ANTES de inicializar UART, ADC, PWM o delays.
 * No escribe fusibles.
 */
void Clock_init(void);

#endif
