/*
 * SeguidorLinea: integracion provisional de drivers (etapa 2).
 * La temporizacion fija del PID corresponde a la etapa 3.
 */
#include <avr/interrupt.h>
#include <stdint.h>
#include "Clock.h"
#include "QTR_Sensors.h"
#include "Motor.h"
#include "HC_05.h"

#define VBASE             750
#define INTEGRAL_MAX      2000
#define INTEGRAL_MIN     -2000
#define TELEMETRY_PERIOD  10

#define KP 2
#define KI 0.5
#define KD 3

static uint16_t calibrated[QTR_NUM_SENSORS];
static uint16_t position;
static int16_t error = 0, last_error = 0;
static int32_t integral = 0;
static int16_t derivativo = 0, correction = 0;
static int16_t vel_izq, vel_der;
static uint8_t flag_stop = 1;

static void procesar_comando_simple(uint8_t c) {
    switch (c) {
        case 1:                         /* STOP */
            flag_stop = 1;
            Motor_stop();
            Motor_standby();
            integral = 0;
            break;
        case 2:                         /* START */
            integral = 0;
            last_error = error;
            Motor_stop();
            Motor_wake();
            flag_stop = 0;
            break;
        case 3:                         /* Reset integral */
            integral = 0;
            break;
        default:
            /* Comandos 4..6 reservados para protocolo de ajuste PID. */
            break;
    }
}

int main(void) {
    Clock_init();                      /* Prescaler CPU antes de perifericos. */
    Motor();                           /* STBY bajo por seguridad. */
    hc05_init(9600);
    QTR_init_and_calibrate();          /* 50 muestras; mover sobre fondo/linea. */
    hc05_enable_rx_interrupt();
    sei();

    uint8_t telemetry_counter = 0;

    while (1) {
        uint8_t command;
        while (hc05_try_getc(&command)) {
            procesar_comando_simple(command);
        }

        QTR_read_calibrated(calibrated);
        position = QTR_read_line_white(calibrated);
        error = (int16_t)position - 2500;
        derivativo = error - last_error;

        if (flag_stop) {
            integral = 0;
        } else {
            integral += error;
            if (integral > INTEGRAL_MAX) integral = INTEGRAL_MAX;
            if (integral < INTEGRAL_MIN) integral = INTEGRAL_MIN;
        }

        /* PID original: se sustituira por un lazo de periodo fijo. */
        correction = (int16_t)(((KP * error) + (KI * integral) / 10
                              + (KD * derivativo) / 10) / 10);
        vel_izq = VBASE + correction;
        vel_der = VBASE - correction;

        if (!flag_stop) {
            Motor_a_set(vel_izq);
            Motor_b_set(vel_der);
        }

        if (++telemetry_counter >= TELEMETRY_PERIOD) {
            telemetry_counter = 0;
            (void)hc05_try_puti(error); /* No bloquear el lazo si TX esta lleno. */
        }

        last_error = error;
    }
}
