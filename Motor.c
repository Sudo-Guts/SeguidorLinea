#include "Motor.h"
#include <avr/io.h>

void Motor(void) {
    // Pines de dirección como salidas
    MOTOR_A_IN1_DDR |= (1 << MOTOR_A_IN1_PIN);
    MOTOR_A_IN2_DDR |= (1 << MOTOR_A_IN2_PIN);
    MOTOR_B_IN1_DDR |= (1 << MOTOR_B_IN1_PIN);
    MOTOR_B_IN2_DDR |= (1 << MOTOR_B_IN2_PIN);

    // Pines PWM (OC1A/OC1B) como salidas
    MOTOR_A_PWM_DDR |= (1 << MOTOR_A_PWM_PIN);
    MOTOR_B_PWM_DDR |= (1 << MOTOR_B_PWM_PIN);

    // Standby como salida y activado
    MOTOR_STBY_DDR	|= (1 << MOTOR_STBY_PIN);
    MOTOR_STBY_PORT |= (1 << MOTOR_STBY_PIN);

    // Timer 1: Modo 14 (Fast PWM, ICR1 como TOP)
    TCCR1A = (1 << WGM11) | (1 << COM1A1) | (1 << COM1B1);
    TCCR1B = (1 << WGM13) | (1 << WGM12);

    ICR1 = PWM_TOP;
    OCR1A = 0;
    OCR1B = 0;

    // Preescaler 1 → iniciar timer
    TCCR1B |= (1 << CS10);
}

static void Motor_a_set_dir(motor_dir_t dir) {
    switch (dir) {
        case MOTOR_FORWARD:
            MOTOR_A_IN1_PORT |=  (1 << MOTOR_A_IN1_PIN);   // IN1 = 1
            MOTOR_A_IN2_PORT &= ~(1 << MOTOR_A_IN2_PIN);   // IN2 = 0
            break;
        case MOTOR_BRAKE:
            MOTOR_A_IN1_PORT |=  (1 << MOTOR_A_IN1_PIN);   // IN1 = 1
            MOTOR_A_IN2_PORT |=  (1 << MOTOR_A_IN2_PIN);   // IN2 = 1
            break;
        case MOTOR_COAST:
        default:
            MOTOR_A_IN1_PORT &= ~(1 << MOTOR_A_IN1_PIN);   // IN1 = 0
            MOTOR_A_IN2_PORT &= ~(1 << MOTOR_A_IN2_PIN);   // IN2 = 0
            break;
    }
}

static void Motor_b_set_dir(motor_dir_t dir) {
    switch (dir) {
        case MOTOR_FORWARD:
            MOTOR_B_IN1_PORT |=  (1 << MOTOR_B_IN1_PIN);
            MOTOR_B_IN2_PORT &= ~(1 << MOTOR_B_IN2_PIN);
            break;
        case MOTOR_BRAKE:
            MOTOR_B_IN1_PORT |=  (1 << MOTOR_B_IN1_PIN);
            MOTOR_B_IN2_PORT |=  (1 << MOTOR_B_IN2_PIN);
            break;
        case MOTOR_COAST:
        default:
            MOTOR_B_IN1_PORT &= ~(1 << MOTOR_B_IN1_PIN);
            MOTOR_B_IN2_PORT &= ~(1 << MOTOR_B_IN2_PIN);
            break;
    }
}

void Motor_a_set(uint16_t speed) {
    if (speed > 0) {
        Motor_a_set_dir(MOTOR_FORWARD);
        if (speed > PWM_MAX) speed = PWM_MAX;
        OCR1A = speed;
    } else {
        Motor_a_set_dir(MOTOR_COAST);
        OCR1A = 0;
    }
}

void Motor_b_set(uint16_t speed) {
    if (speed > 0) {
        Motor_b_set_dir(MOTOR_FORWARD);
        if (speed > PWM_MAX) speed = PWM_MAX;
        OCR1B = speed;
    } else {
        Motor_b_set_dir(MOTOR_COAST);
        OCR1B = 0;
    }
}

void Motor_a_dir_speed(motor_dir_t dir, uint16_t speed) {
    Motor_a_set_dir(dir);
    if (speed > PWM_MAX) speed = PWM_MAX;
    OCR1A = speed;
}


void Motor_b_dir_speed(motor_dir_t dir, uint16_t speed) {
    Motor_b_set_dir(dir);
    if (speed > PWM_MAX) speed = PWM_MAX;
    OCR1B = speed;
}

void Motor_stop(void) {
    Motor_a_set_dir(MOTOR_COAST);
    Motor_b_set_dir(MOTOR_COAST);
    OCR1A = 0;
    OCR1B = 0;
}

void Motor_brake(void) {
    Motor_a_set_dir(MOTOR_BRAKE);
    Motor_b_set_dir(MOTOR_BRAKE);
    OCR1A = 0;
    OCR1B = 0;
}

void Motor_standby(void) {
    MOTOR_STBY_PORT &= ~(1 << MOTOR_STBY_PIN);
}

void Motor_wake(void) {
    MOTOR_STBY_PORT |= (1 << MOTOR_STBY_PIN);
}

void Motor_clock_disable(void) {
    TCCR1B &= ~((1 << CS12) | (1 << CS11) | (1 << CS10));
}

void Motor_clock_enable(void) {
    TCCR1B = (TCCR1B & 0xF8) | (1 << CS10);
}