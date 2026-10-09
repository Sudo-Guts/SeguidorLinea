#ifndef MOTOR_H
#define MOTOR_H

#include <avr/io.h>
#include <stdint.h>

/* Cableado heredado: Motor A -> PB2 (OC1B); Motor B -> PB1 (OC1A).
 * Los pines se conservan. Lo que cambia son los registros OCR usados.
 */
#define MOTOR_A_PWM_DDR    DDRB
#define MOTOR_A_PWM_PIN    PB2

#define MOTOR_B_PWM_DDR    DDRB
#define MOTOR_B_PWM_PIN    PB1

#define MOTOR_A_IN1_DDR    DDRB
#define MOTOR_A_IN1_PORT   PORTB
#define MOTOR_A_IN1_PIN    PB7
#define MOTOR_A_IN2_DDR    DDRB
#define MOTOR_A_IN2_PORT   PORTB
#define MOTOR_A_IN2_PIN    PB0

#define MOTOR_B_IN1_DDR    DDRB
#define MOTOR_B_IN1_PORT   PORTB
#define MOTOR_B_IN1_PIN    PB4
#define MOTOR_B_IN2_DDR    DDRB
#define MOTOR_B_IN2_PORT   PORTB
#define MOTOR_B_IN2_PIN    PB3

#define MOTOR_STBY_DDR     DDRB
#define MOTOR_STBY_PORT    PORTB
#define MOTOR_STBY_PIN     PB5

/* Fast PWM, TOP=999, sin prescaler: 8 kHz a F_CPU=8 MHz. */
#define PWM_TOP            999U
#define PWM_MAX            PWM_TOP

typedef enum {
    MOTOR_FORWARD,
    MOTOR_BRAKE,
    MOTOR_COAST
} motor_dir_t;

void Motor(void);
void Motor_a_set(int16_t speed);
void Motor_b_set(int16_t speed);
void Motor_a_dir_speed(motor_dir_t dir, uint16_t speed);
void Motor_b_dir_speed(motor_dir_t dir, uint16_t speed);
void Motor_stop(void);
void Motor_brake(void);
void Motor_standby(void);
void Motor_wake(void);
void Motor_clock_disable(void);
void Motor_clock_enable(void);

#endif
