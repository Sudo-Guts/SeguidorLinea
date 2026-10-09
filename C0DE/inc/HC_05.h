#ifndef HC05_H
#define HC05_H

#include <stdint.h>

/* UART 8N1 a baud variable.
 * RX (32 B) y TX (64 B) usan buffers circulares por interrupciones.
 * Activar interrupciones globales con sei() despues de inicializar.
 */
void hc05_init(uint32_t baud);
void hc05_enable_rx_interrupt(void);
uint8_t hc05_try_getc(uint8_t *out);
uint8_t hc05_try_putc(uint8_t data);
uint8_t hc05_try_puti(int16_t num); /* Linea ASCII terminada en CR LF. */

void hc05_putc(char data);          /* Bloqueante: requiere IRQ habilitadas. */
void hc05_puts(const char *str);
void hc05_puti(int16_t num);
char hc05_getc(void);               /* Bloqueante: lee del buffer RX. */
uint8_t hc05_gets(char *buffer, uint8_t max_len);

extern volatile char hc05_last_rx;
extern volatile uint8_t hc05_rx_dropped;

#endif
