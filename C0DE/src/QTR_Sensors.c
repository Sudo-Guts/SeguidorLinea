#include "QTR_Sensors.h"
#include <util/delay.h>

/* 1 MHz / 8 = 125 kHz; 8 MHz / 64 = 125 kHz.
 * Ambas configuraciones mantienen el ADC dentro de 50..200 kHz.
 */
#if F_CPU == 8000000UL
#define QTR_ADC_PRESCALER ((1 << ADPS2) | (1 << ADPS1))
#elif F_CPU == 1000000UL
#define QTR_ADC_PRESCALER ((1 << ADPS1) | (1 << ADPS0))
#else
#error "Configurar el prescaler ADC para F_CPU"
#endif

static uint16_t qtr_min[QTR_NUM_SENSORS];
static uint16_t qtr_max[QTR_NUM_SENSORS];
static uint16_t last_position = 2500;

void QTR(void) {
    QTR_IR_LED_DDR |= (1 << QTR_IR_LED_PIN);
    QTR_set_ir_leds(0);

    DDRC &= (uint8_t)~0x3FU;
    PORTC &= (uint8_t)~0x3FU;
    DIDR0 |= 0x3FU;                    /* Deshabilita buffers digitales ADC0..5 */
    ADMUX = (1 << REFS0);              /* Referencia AVcc */
    ADCSRA = (1 << ADEN) | QTR_ADC_PRESCALER;
}

void QTR_init_and_calibrate(void) {
    QTR();
    QTR_calibrate();
}

void QTR_set_ir_leds(uint8_t on) {
    if (on) QTR_IR_LED_PORT |= (1 << QTR_IR_LED_PIN);
    else    QTR_IR_LED_PORT &= ~(1 << QTR_IR_LED_PIN);
}

static uint16_t qtr_adc_read(uint8_t channel) {
    ADMUX = (ADMUX & 0xF0U) | (channel & 0x07U);
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC)) {}
    return ADC;                        /* Lectura de 16 bits: ADCL, luego ADCH */
}

void QTR_read_raw(uint16_t *values) {
    QTR_set_ir_leds(1);
    _delay_us(QTR_EMITTER_SETTLE_US);
    for (uint8_t i = 0; i < QTR_NUM_SENSORS; ++i) {
        values[i] = qtr_adc_read(i);
    }
    QTR_set_ir_leds(0);
}

void QTR_calibrate(void) {
    uint16_t sample[QTR_NUM_SENSORS];
    for (uint8_t i = 0; i < QTR_NUM_SENSORS; ++i) {
        qtr_min[i] = 1023;
        qtr_max[i] = 0;
    }

    /* Mover manualmente TODOS los sensores sobre linea y fondo
     * durante estos ~5 s; de lo contrario el calibrado no es util.
     */
    for (uint16_t s = 0; s < QTR_CAL_SAMPLES; ++s) {
        QTR_read_raw(sample);
        for (uint8_t i = 0; i < QTR_NUM_SENSORS; ++i) {
            if (sample[i] < qtr_min[i]) qtr_min[i] = sample[i];
            if (sample[i] > qtr_max[i]) qtr_max[i] = sample[i];
        }
        _delay_ms(100);
    }
}

void QTR_read_calibrated(uint16_t *values) {
    uint16_t raw[QTR_NUM_SENSORS];
    QTR_read_raw(raw);

    for (uint8_t i = 0; i < QTR_NUM_SENSORS; ++i) {
        if (raw[i] <= qtr_min[i]) {
            values[i] = 0;
        } else if (raw[i] >= qtr_max[i]) {
            values[i] = 1000;
        } else {
            uint16_t range = qtr_max[i] - qtr_min[i];
            values[i] = (uint16_t)(((uint32_t)(raw[i] - qtr_min[i]) * 1000U) / range);
        }
    }
}

static uint16_t qtr_compute_line(const uint16_t *values) {
    uint32_t weighted = 0;
    uint16_t sum = 0;

    for (uint8_t i = 0; i < QTR_NUM_SENSORS; ++i) {
        if (values[i] > QTR_NOISE_THRESHOLD) {
            weighted += (uint32_t)values[i] * ((uint16_t)i * 1000U);
            sum += values[i];
        }
    }

    if (sum == 0) return last_position;
    last_position = (uint16_t)(weighted / sum);
    return last_position;
}

uint16_t QTR_read_line_black(uint16_t *values) {
    return qtr_compute_line(values);
}

uint16_t QTR_read_line_white(uint16_t *values) {
    uint16_t inverted[QTR_NUM_SENSORS];
    for (uint8_t i = 0; i < QTR_NUM_SENSORS; ++i) {
        inverted[i] = 1000U - values[i];
    }
    return qtr_compute_line(inverted);
}
