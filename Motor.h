#ifndef MOTOR_H
#define MOTOR_H

#include <stdint.h>

// Pines PWM (Timer 1)
#define MOTOR_A_PWM_DDR   DDRB
#define MOTOR_A_PWM_PORT  PORTB
#define MOTOR_A_PWM_PIN   PB2   // OC1A

#define MOTOR_B_PWM_DDR   DDRB
#define MOTOR_B_PWM_PORT  PORTB
#define MOTOR_B_PWM_PIN   PB1   // OC1B

// Pines de dirección Motor A (AIN1, AIN2)
#define MOTOR_A_IN1_DDR   DDRB
#define MOTOR_A_IN1_PORT  PORTB
#define MOTOR_A_IN1_PIN   PB7

#define MOTOR_A_IN2_DDR   DDRB
#define MOTOR_A_IN2_PORT  PORTB
#define MOTOR_A_IN2_PIN   PB0

// Pines de dirección Motor B (BIN1, BIN2)
#define MOTOR_B_IN1_DDR   DDRB
#define MOTOR_B_IN1_PORT  PORTB
#define MOTOR_B_IN1_PIN   PB4

#define MOTOR_B_IN2_DDR   DDRB
#define MOTOR_B_IN2_PORT  PORTB
#define MOTOR_B_IN2_PIN   PB3

// Pin de Standby
#define MOTOR_STBY_DDR    DDRB
#define MOTOR_STBY_PORT   PORTB
#define MOTOR_STBY_PIN    PB5

// Parámetros del PWM
#define PWM_TOP           999   // ICR1 = 999 → 1 kHz, 1000 pasos
#define PWM_MAX           999

typedef enum {
    MOTOR_FORWARD,   // Adelante
    MOTOR_BRAKE,     // Freno corto
    MOTOR_COAST      // Libre (alta impedancia)
} motor_dir_t;


void Motor(void);

void Motor_a_set(uint16_t speed);
void Motor_b_set(uint16_t speed);

void Motor_a_dir_speed(motor_dir_t dir, uint16_t speed);
void Motor_b_dir_speed(motor_dir_t dir, uint16_t speed);

void Motor_stop(void);
void Motor_brake(void);
void Motor_standby(void);
void Motor_wake(void);

void Motor_clock_disable(void);
void Motor_clock_enable(void);

#endif