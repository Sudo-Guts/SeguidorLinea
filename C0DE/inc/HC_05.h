#ifndef HC05_H
#define HC05_H

#include <stdint.h>

// ======================== CONFIGURACIÓN ========================
// F_CPU se define desde C0DE/Makefile para todos los modulos.

// ======================== FUNCIONES ========================

/**
 * @brief Inicializa la UART para comunicación con el HC-05.
 *        Configura 8 bits de datos, 1 bit de parada, sin paridad.
 * @param baud Tasa de baudios (ej. 9600, 38400).
 */
void hc05_init(uint32_t baud);

/**
 * @brief Transmite un carácter por la UART (bloqueante).
 * @param data Carácter a enviar.
 */
void hc05_putc(char data);

/**
 * @brief Transmite una cadena de texto terminada en nulo.
 * @param str Puntero a la cadena.
 */
void hc05_puts(const char *str);

/**
 * @brief Transmite un número entero con signo como texto.
 * @param num Número a enviar.
 */
void hc05_puti(int16_t num);

/**
 * @brief Recibe un carácter por la UART (bloqueante).
 * @return Carácter recibido.
 */
char hc05_getc(void);





/**
 * @brief Recibe una línea (hasta encontrar '\n' o '\r').
 * @param buffer Puntero donde almacenar la cadena.
 * @param max_len Longitud máxima del buffer (incluye terminador nulo).
 * @return Número de caracteres leídos.
 */
uint8_t hc05_gets(char *buffer, uint8_t max_len);

/**
 * @brief Habilita la interrupción de recepción USART.
 *        Cuando llegue un dato, se activará la ISR USART_RX_vect.
 */
void hc05_enable_rx_interrupt(void);

/**
 * @brief Último carácter recibido por interrupción.
 *        Se actualiza en la ISR (se define en src/HC_05.c).
 */
extern volatile char hc05_last_rx;

#endif // HC05_H