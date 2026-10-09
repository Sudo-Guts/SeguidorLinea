#ifndef QTR_SENSORS_H
#define QTR_SENSORS_H

#include <avr/io.h>
#include <stdint.h>

#define QTR_NUM_SENSORS      6
#define QTR_IR_LED_PORT      PORTB
#define QTR_IR_LED_DDR       DDRB
#define QTR_IR_LED_PIN       PB6

#define QTR_NOISE_THRESHOLD  50U
#define QTR_CAL_SAMPLES      50U
#define QTR_EMITTER_SETTLE_US 200

void QTR(void);
void QTR_init_and_calibrate(void);
void QTR_set_ir_leds(uint8_t on);
void QTR_read_raw(uint16_t *values);
void QTR_calibrate(void);
void QTR_read_calibrated(uint16_t *values);
uint16_t QTR_read_line_black(uint16_t *values);
uint16_t QTR_read_line_white(uint16_t *values);

#endif
