#include "HC_05.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#define HC05_RX_SIZE 32U
#define HC05_TX_SIZE 64U
#define RX_MASK (HC05_RX_SIZE - 1U)
#define TX_MASK (HC05_TX_SIZE - 1U)

static volatile uint8_t rx_buffer[HC05_RX_SIZE];
static volatile uint8_t tx_buffer[HC05_TX_SIZE];
static volatile uint8_t rx_head, rx_tail;
static volatile uint8_t tx_head, tx_tail;

volatile char hc05_last_rx = 0;
volatile uint8_t hc05_rx_dropped = 0;

static uint32_t abs_difference(uint32_t a, uint32_t b) {
    return a >= b ? a - b : b - a;
}

void hc05_init(uint32_t baud) {
    if (baud == 0) return;

    /* Escoge el divisor con menor error nominal; en empate prefiere 16x. */
    uint32_t norm_div = (F_CPU + (8UL * baud)) / (16UL * baud);
    uint32_t fast_div = (F_CPU + (4UL * baud)) / (8UL * baud);
    if (norm_div == 0) norm_div = 1;
    if (fast_div == 0) fast_div = 1;
    if (norm_div > 4096UL) norm_div = 4096UL;
    if (fast_div > 4096UL) fast_div = 4096UL;

    uint32_t norm_rate = F_CPU / (16UL * norm_div);
    uint32_t fast_rate = F_CPU / (8UL * fast_div);
    uint8_t double_speed = (abs_difference(fast_rate, baud) <
                            abs_difference(norm_rate, baud));
    uint16_t ubrr = (uint16_t)((double_speed ? fast_div : norm_div) - 1UL);

    UCSR0B = 0;
    UCSR0A = double_speed ? (1 << U2X0) : 0;
    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)ubrr;
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); /* 8N1 */
    rx_head = rx_tail = tx_head = tx_tail = 0;
    hc05_rx_dropped = 0;
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
}

void hc05_enable_rx_interrupt(void) {
    UCSR0B |= (1 << RXCIE0);
}

ISR(USART_RX_vect) {
    uint8_t status = UCSR0A;
    uint8_t data = UDR0;
    hc05_last_rx = (char)data;

    if (status & ((1 << FE0) | (1 << DOR0) | (1 << UPE0))) {
        if (hc05_rx_dropped != 255) ++hc05_rx_dropped;
        return;
    }

    uint8_t next = (rx_head + 1U) & RX_MASK;
    if (next == rx_tail) {
        if (hc05_rx_dropped != 255) ++hc05_rx_dropped;
        return;
    }
    rx_buffer[rx_head] = data;
    rx_head = next;
}

ISR(USART_UDRE_vect) {
    if (tx_head == tx_tail) {
        UCSR0B &= ~(1 << UDRIE0);
    } else {
        UDR0 = tx_buffer[tx_tail];
        tx_tail = (tx_tail + 1U) & TX_MASK;
    }
}

uint8_t hc05_try_getc(uint8_t *out) {
    uint8_t ok = 0;
    if (out == 0) return 0;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        if (rx_head != rx_tail) {
            *out = rx_buffer[rx_tail];
            rx_tail = (rx_tail + 1U) & RX_MASK;
            ok = 1;
        }
    }
    return ok;
}

uint8_t hc05_try_putc(uint8_t data) {
    uint8_t ok = 0;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        uint8_t next = (tx_head + 1U) & TX_MASK;
        if (next != tx_tail) {
            tx_buffer[tx_head] = data;
            tx_head = next;
            UCSR0B |= (1 << UDRIE0);
            ok = 1;
        }
    }
    return ok;
}

void hc05_putc(char data) {
    while (!hc05_try_putc((uint8_t)data)) {}
}

void hc05_puts(const char *str) {
    while (*str) hc05_putc(*str++);
}

uint8_t hc05_try_puti(int16_t num) {
    char reverse[5];
    char message[9];
    uint8_t count = 0, len = 0, ok = 0;
    uint16_t value = num < 0 ? (uint16_t)(-(int32_t)num) : (uint16_t)num;

    do {
        reverse[count++] = (char)('0' + (value % 10U));
        value /= 10U;
    } while (value != 0);

    if (num < 0) message[len++] = '-';
    while (count != 0) message[len++] = reverse[--count];
    message[len++] = '\r';
    message[len++] = '\n';

    /* Encola mensaje completo o ninguno: evita lineas truncadas. */
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        uint8_t used = (tx_head - tx_tail) & TX_MASK;
        if ((HC05_TX_SIZE - 1U - used) >= len) {
            for (uint8_t i = 0; i < len; ++i) {
                tx_buffer[tx_head] = (uint8_t)message[i];
                tx_head = (tx_head + 1U) & TX_MASK;
            }
            UCSR0B |= (1 << UDRIE0);
            ok = 1;
        }
    }
    return ok;
}

void hc05_puti(int16_t num) {
    while (!hc05_try_puti(num)) {}
}

char hc05_getc(void) {
    uint8_t data;
    while (!hc05_try_getc(&data)) {}
    return (char)data;
}

uint8_t hc05_gets(char *buffer, uint8_t max_len) {
    uint8_t count = 0;
    if (buffer == 0 || max_len == 0) return 0;
    while (count < (uint8_t)(max_len - 1U)) {
        char c = hc05_getc();
        if (c == '\n' || c == '\r') break;
        buffer[count++] = c;
    }
    buffer[count] = '\0';
    return count;
}
