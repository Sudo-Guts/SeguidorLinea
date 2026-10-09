# Hardware: inventario de pines y supuestos

**Microcontrolador objetivo:** ATmega328P. El firmware supone oscilador RC interno nominal de 8 MHz y configura `CLKPR` a divisor 1 antes de inicializar perifericos.

Esta tabla refleja el cableado supuesto por las macros, no sustituye el esquematico electrico.

| Funcion | Pin AVR | Canal o periferico |
| --- | --- | --- |
| QTR 0..5 | PC0..PC5 | ADC0..ADC5 |
| Emisores infrarrojos QTR | PB6 | GPIO, control activo-alto supuesto |
| Motor A IN1 / IN2 | PB7 / PB0 | GPIO |
| Motor B IN1 / IN2 | PB4 / PB3 | GPIO |
| Motor A PWM | PB2 | **OC1B / OCR1B** |
| Motor B PWM | PB1 | **OC1A / OCR1A** |
| Driver STBY | PB5 | GPIO |
| HC-05 RX0 / TX0 | PD0 / PD1 | USART0 |

## Antes de energizar los motores

1. Verificar fisicamente el cableado de PB2 (Motor A) y PB1 (Motor B). Se corrigio el **registro OCR de cada motor**, no el pin fisico.
2. Confirmar la fuente de reloj por fusibles: PB6 y PB7 solo pueden actuar como GPIO si estan libres de cristal/oscilador externo.
3. Confirmar el modelo del puente H y su tabla de verdad: `STBY`, `COAST` y `BRAKE` pueden variar segun el circuito.
4. Confirmar tension de alimentacion y niveles logicos, especialmente en RX del HC-05.
5. PB3, PB4 y PB5 se comparten con las seniales de programacion ISP; asegurar que el circuito de potencia no interfiera con el programador.
6. Verificar que sensores QTR sean analogicos y que el control de emisores sea compatible con el pin PB6.
7. Iniciar pruebas con ruedas elevadas y posibilidad de desconectar la alimentacion de motores.

No se alteran fusibles ni conexiones fisicas desde el firmware. Para pruebas individuales, ver [DRIVERS.md](DRIVERS.md).
