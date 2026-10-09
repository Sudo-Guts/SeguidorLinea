# SeguidorLinea

Firmware en C para robot seguidor de linea con seis sensores QTR analogicos, control PID, dos motores por Timer1/PWM y comunicacion Bluetooth HC-05.

## Estructura

- `C0DE/`: firmware, cabeceras y Makefile.
- `DOC/`: documentacion de toolchain, pines y drivers.
- `PCB/`: disenio de PCB (pendiente).
- `SIM/`: simulaciones (pendientes).

## Compilacion en Linux

Requiere `make`, `avr-gcc`, `avr-libc` y `binutils-avr`:

```sh
cd C0DE
make clean
make
make size
```

Los artefactos ELF, HEX, BIN, MAP y LSS se generan en `C0DE/out/`.

## Reloj del microcontrolador

Configuracion de esta rama: **ATmega328P con oscilador RC interno nominal de 8 MHz**. El `Makefile` utiliza `F_CPU=8000000UL` por defecto y `Clock_init()` configura `CLKPR` para divisor 1 al arrancar, aun si `CKDIV8` inicializa el chip con divisor 8.

**No se escriben fusibles.** Es indispensable verificar que el AVR realmente tiene seleccionado el RC interno como fuente de reloj. No basta con cambiar `F_CPU` si el hardware usa otra fuente.

Para compilar la configuracion alternativa con divisor 8 (1 MHz nominal):

```sh
make clean
make F_CPU=1000000UL
```

## Estado de desarrollo

- Etapa 1: toolchain, Makefile, compilacion y documentacion.
- Etapa 2: Timer1 PWM con asignacion de canales fisicos correcta, ADC-QTR a 125 kHz y UART con RX/TX por interrupciones.
- Etapa 3 (pendiente): PID con muestreo fijo, estados del robot y manejo avanzado de perdida de linea.

**Importante:** los drivers compilan en CI para 8 MHz y 1 MHz, pero todavia se requiere verificar en banco el cableado, el puente H, el contraste de los QTR y la temporizacion efectiva. El inicio se realiza con los motores en standby.

Consulta [compilacion](DOC/BUILD.md), [hardware](DOC/HARDWARE.md) y [detalles de drivers](DOC/DRIVERS.md).
