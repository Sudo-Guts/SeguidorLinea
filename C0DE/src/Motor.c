#include "Motor.h"

/* El registro de cada motor se escoge segun el PIN REAL, no su etiqueta. */
static uint16_t motor_clamp(int16_t speed) {
    if (speed <= 0) return 0;
    if (speed >= (int16_t)PWM_MAX) return PWM_MAX;
    return (uint16_t)speed;
}

static uint16_t motor_clamp_unsigned(uint16_t speed) {
    return speed > PWM_MAX ? PWM_MAX : speed;
}

static void motor_a_direction(motor_dir_t dir) {
    switch (dir) {
        case MOTOR_FORWARD:
            MOTOR_A_IN1_PORT |= (1 << MOTOR_A_IN1_PIN);
            MOTOR_A_IN2_PORT &= ~(1 << MOTOR_A_IN2_PIN);
            break;
        case MOTOR_BRAKE:
            MOTOR_A_IN1_PORT |= (1 << MOTOR_A_IN1_PIN);
            MOTOR_A_IN2_PORT |= (1 << MOTOR_A_IN2_PIN);
            break;
        case MOTOR_COAST:
        default:
            MOTOR_A_IN1_PORT &= ~(1 << MOTOR_A_IN1_PIN);
            MOTOR_A_IN2_PORT &= ~(1 << MOTOR_A_IN2_PIN);
            break;
    }
}

static void motor_b_direction(motor_dir_t dir) {
    switch (dir) {
        case MOTOR_FORWARD:
            MOTOR_B_IN1_PORT |= (1 << MOTOR_B_IN1_PIN);
            MOTOR_B_IN2_PORT &= ~(1 << MOTOR_B_IN2_PIN);
            break;
        case MOTOR_BRAKE:
            MOTOR_B_IN1_PORT |= (1 << MOTOR_B_IN1_PIN);
            MOTOR_B_IN2_PORT |= (1 << MOTOR_B_IN2_PIN);
            break;
        case MOTOR_COAST:
        default:
            MOTOR_B_IN1_PORT &= ~(1 << MOTOR_B_IN1_PIN);
            MOTOR_B_IN2_PORT &= ~(1 << MOTOR_B_IN2_PIN);
            break;
    }
}

void Motor(void) {
    /* Standby inactivo durante la inicializacion y calibracion. */
    MOTOR_STBY_PORT &= ~(1 << MOTOR_STBY_PIN);
    MOTOR_STBY_DDR |= (1 << MOTOR_STBY_PIN);

    MOTOR_A_IN1_DDR |= (1 << MOTOR_A_IN1_PIN);
    MOTOR_A_IN2_DDR |= (1 << MOTOR_A_IN2_PIN);
    MOTOR_B_IN1_DDR |= (1 << MOTOR_B_IN1_PIN);
    MOTOR_B_IN2_DDR |= (1 << MOTOR_B_IN2_PIN);

    motor_a_direction(MOTOR_COAST);
    motor_b_direction(MOTOR_COAST);

    MOTOR_A_PWM_DDR |= (1 << MOTOR_A_PWM_PIN);
    MOTOR_B_PWM_DDR |= (1 << MOTOR_B_PWM_PIN);

    /* Timer1 modo 14: Fast PWM, ICR1=TOP, prescaler=1. */
    TCCR1A = (1 << WGM11) | (1 << COM1A1) | (1 << COM1B1);
    TCCR1B = (1 << WGM13) | (1 << WGM12);
    TCNT1 = 0;
    ICR1 = PWM_TOP;
    OCR1A = 0;
    OCR1B = 0;
    TCCR1B |= (1 << CS10);
}

void Motor_a_set(int16_t speed) {
    uint16_t duty = motor_clamp(speed);
    OCR1B = duty;                       /* PB2 = OC1B: Motor A */
    motor_a_direction(duty ? MOTOR_FORWARD : MOTOR_COAST);
}

void Motor_b_set(int16_t speed) {
    uint16_t duty = motor_clamp(speed);
    OCR1A = duty;                       /* PB1 = OC1A: Motor B */
    motor_b_direction(duty ? MOTOR_FORWARD : MOTOR_COAST);
}

void Motor_a_dir_speed(motor_dir_t dir, uint16_t speed) {
    OCR1B = motor_clamp_unsigned(speed);
    motor_a_direction(dir);
}

void Motor_b_dir_speed(motor_dir_t dir, uint16_t speed) {
    OCR1A = motor_clamp_unsigned(speed);
    motor_b_direction(dir);
}

void Motor_stop(void) {
    OCR1A = 0;
    OCR1B = 0;
    motor_a_direction(MOTOR_COAST);
    motor_b_direction(MOTOR_COAST);
}

void Motor_brake(void) {
    OCR1A = 0;
    OCR1B = 0;
    motor_a_direction(MOTOR_BRAKE);
    motor_b_direction(MOTOR_BRAKE);
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
    TCCR1B = (TCCR1B & ~((1 << CS12) | (1 << CS11) | (1 << CS10)))
             | (1 << CS10);
}
