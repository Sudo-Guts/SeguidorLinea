#include "QTR_Sensors.h"
#include <avr/io.h>
#include <util/delay.h>

static uint16_t qtr_min[QTR_NUM_SENSORS];
static uint16_t qtr_max[QTR_NUM_SENSORS];

static uint16_t _last_position = 2500;

void QTR(void) {
    QTR_IR_LED_DDR |= (1 << QTR_IR_LED_PIN);
    QTR_IR_LED_PORT &= ~(1 << QTR_IR_LED_PIN);

    ADMUX = (1 << REFS0);
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

void QTR_init_and_calibrate(void) {
    QTR();
    QTR_calibrate();
}

void QTR_set_ir_leds(uint8_t on) {
    if (on)	QTR_IR_LED_PORT |= (1 << QTR_IR_LED_PIN);
    else	QTR_IR_LED_PORT &= ~(1 << QTR_IR_LED_PIN);
}


static uint16_t QTR_read_adc_channel(uint8_t channel) {
    channel &= 0b00000111;
    ADMUX = (ADMUX & 0xF0) | channel;
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
    uint8_t low  = ADCL;
    uint8_t high = ADCH;
    return ((uint16_t)high << 8) | low;
}

void QTR_read_raw(uint16_t *values) {
    QTR_set_ir_leds(1);
    for (uint8_t i = 0; i < QTR_NUM_SENSORS; i++) {
        values[i] = QTR_read_adc_channel(i);
    }
    QTR_set_ir_leds(0);
}

void QTR_calibrate(void) {
    uint16_t sample[QTR_NUM_SENSORS];

    for (uint8_t i = 0; i < QTR_NUM_SENSORS; i++) {
        qtr_min[i] = 1023;
        qtr_max[i] = 0;
    }

    for (uint16_t s = 0; s < QTR_CAL_SAMPLES; s++) {
        QTR_read_raw(sample);
        for (uint8_t i = 0; i < QTR_NUM_SENSORS; i++) {
            if (sample[i] < qtr_min[i]) qtr_min[i] = sample[i];
            if (sample[i] > qtr_max[i]) qtr_max[i] = sample[i];
        }
        _delay_ms(100);
    }
}

void QTR_read_calibrated(uint16_t *values) {
    uint16_t raw[QTR_NUM_SENSORS];
    QTR_read_raw(raw);

    for (uint8_t i = 0; i < QTR_NUM_SENSORS; i++) {
        if (raw[i] <= qtr_min[i]) {
            values[i] = 0;
        } else if (raw[i] >= qtr_max[i]) {
            values[i] = 1000;
        } else {
            uint32_t range = qtr_max[i] - qtr_min[i];
            values[i] = (uint16_t)(((uint32_t)(raw[i] - qtr_min[i]) * 1000) / range);
        }
    }
}

static uint16_t QTR_compute_line(uint16_t *values) {
    uint32_t avg = 0;
    uint16_t sum = 0;
    uint8_t on_line = 0;

    for (uint8_t i = 0; i < QTR_NUM_SENSORS; i++) {
        if (values[i] > QTR_NOISE_THRESHOLD) {
            on_line = 1;
            avg += (uint32_t)values[i] * (i * 1000);
            sum += values[i];
        }
    }

    if (!on_line) {
        return _last_position;
    }

    _last_position = avg / sum;
    return _last_position;
}

uint16_t QTR_read_line_black(uint16_t *values) {
    return QTR_compute_line(values);
}

uint16_t QTR_read_line_white(uint16_t *values) {
    uint16_t inverted[QTR_NUM_SENSORS];
    for (uint8_t i = 0; i < QTR_NUM_SENSORS; i++) {
        inverted[i] = 1000 - values[i];
    }
    return QTR_compute_line(inverted);
}