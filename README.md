# SeguidorLinea

Firmware en C para un robot seguidor de linea con seis sensores QTR analogicos, control PID, dos motores con Timer1/PWM y comunicacion UART con el modulo HC-05.

## Estructura

- `C0DE/`: fuentes, cabeceras y Makefile.
- `DOC/`: documentacion tecnica y configuracion de hardware.
- `PCB/`: archivos futuros de KiCad.
- `SIM/`: archivos futuros de simulacion.

## Compilacion en Linux

Requiere `make`, `avr-gcc`, `avr-libc` y `binutils-avr`:

```sh
cd C0DE
make
make size
```

Artefactos generados en `C0DE/out/`: `.elf`, `.hex`, `.bin`, `.map` y `.lss`.

Consultar [DOC/BUILD.md](DOC/BUILD.md) para el toolchain, las variables de compilacion, AVRDUDE y los fusibles.

## Estado del hardware

Los valores de referencia son **ATmega328P a 1 MHz**, inferidos del firmware existente; deben contrastarse con el microcontrolador, la fuente de reloj y los fusibles instalados.

> Esta etapa prepara el sistema de compilacion, no corrige aun el controlador PID ni los drivers. Hay discrepancias conocidas de pines PWM y puntos pendientes en el HC-05: ver [DOC/HARDWARE.md](DOC/HARDWARE.md). No grabar el firmware sin verificar el circuito.
