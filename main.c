/*
 * SeguidorLinea.c
 *
 * Creado: 11/05/2026 09:58:53 a. m.
 * Autor : Alumnos
 *
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "qtr_sensors.h"
#include "motor.h"
#include "HC_05.h"

#define VBASE         750
#define INTEGRAL_MAX  2000
#define INTEGRAL_MIN -2000
#define DT_MS         10
#define TELEMETRY_PERIOD 10

#define	KP	2	//2		=	 KP/10
#define KI	0.5	//0.5	=	 KI/100
#define KD	3	//3		=	 KD/100

/*
volatile int16_t KP = ;
volatile int16_t KI = 0;
volatile int16_t KD = 1;
*/
uint16_t calibrated[QTR_NUM_SENSORS];
uint16_t position;
int16_t  error = 0;
int16_t  last_error = 0;
int32_t  integral = 0;
int16_t  derivativo = 0;
int16_t  C = 0;
int16_t  vel_izq, vel_der;

volatile uint8_t flag_stop = 1;

volatile uint8_t cmd_pendiente = 0;
volatile char    num_buffer[6];
volatile uint8_t num_index = 0;

void procesar_comando_simple(uint8_t c);
void aplicar_valor(uint8_t cmd, int16_t valor);

ISR(USART_RX_vect) {
	QTR_set_ir_leds(1);
	uint8_t rx = UDR0;
	hc05_last_rx = rx;
	    if (cmd_pendiente == 0) {
		    if (rx == 1 || rx == 2 || rx == 3) {
			    procesar_comando_simple(rx);
			} else if (rx == 4 || rx == 5 || rx == 6) {
			    cmd_pendiente = rx;
		    }
		} else {
		    //aplicar_valor(cmd_pendiente, rx);
	    }
	    QTR_set_ir_leds(0);
}

void procesar_comando_simple(uint8_t c) {
    switch (c) {
        case 1:
            flag_stop = 1;
			Motor_clock_disable();
            Motor_stop();
            break;
        case 2:
			Motor_clock_enable();
            flag_stop = 0;
            break;
        case 3:
            integral = 0;
            break;
        default:
            break;
    }
}
/*
void aplicar_valor(uint8_t cmd, int16_t valor) {
    switch (cmd) {
        case 4:
            KP = valor;
            break;
        case 5:
            KI = valor;
            break;
        case 6:
            KD = valor;
            break;
        default:
            break;
    }
}
*/
int main(void) {
    hc05_init(9600);

    Motor();
    Motor_wake();

    QTR_init_and_calibrate();

    hc05_enable_rx_interrupt();
    sei();

    uint8_t telemetry_counter = 0;
	
    while (1) {
			QTR_read_calibrated(calibrated);

            position = QTR_read_line_white(calibrated);

            error = (int16_t)position - 2500;

            derivativo = error - last_error;

            if (flag_stop) {
                integral = 0;
            } else {
                integral += error;
                integral = (integral > INTEGRAL_MAX) ? INTEGRAL_MAX : integral;
                integral = (integral < INTEGRAL_MIN) ? INTEGRAL_MIN : integral;
            }

            C = (int16_t)(( (KP * error) + (KI * integral)/10 + (KD * derivativo)/10 ) / 10);
			
            vel_izq = VBASE + C;
            vel_der = VBASE - C;
			
			if (!flag_stop) {
				Motor_a_set(vel_izq);
				Motor_b_set(vel_der);
			}
			
            if (++telemetry_counter >= TELEMETRY_PERIOD) {
                telemetry_counter = 0;
                hc05_puti(error);
            }
			
            last_error = error;
        }
    return 0;
}