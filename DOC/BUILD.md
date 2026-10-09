# Compilacion y programacion

## Requisitos Debian/Ubuntu

```sh
sudo apt update
sudo apt install make gcc-avr avr-libc binutils-avr avrdude
```

## Compilar

```sh
cd C0DE
make clean
make
make size
```

El `Makefile` compila `src/*.c`, utiliza `inc/`, genera dependencias `.d` y coloca los artefactos ELF, HEX, BIN, MAP y LSS en `out/`.

La rama incluye una accion de CI para compilar tanto a 8 MHz como a 1 MHz.

## Configuracion

| Variable | Valor predeterminado | Significado |
| --- | --- | --- |
| `MCU` | `atmega328p` | Objetivo AVR-GCC |
| `F_CPU` | `8000000UL` | Frecuencia de CPU despues de `Clock_init()` |
| `PROGRAMMER` | `usbasp` | Cambiar segun programador ISP |
| `AVRDUDE_MCU` | `m328p` | Identificador para AVRDUDE |
| `PORT` | vacio | Solo si el programador requiere puerto |

`Clock_init()` fija el divisor de `CLKPR` a 1 (8 MHz nominal) o a 8 (1 MHz nominal), segun el valor de `F_CPU`. El codigo supone que se selecciono como fuente el **oscilador RC interno de 8 MHz**. No cambia los fusibles.

Compilacion alternativa a 1 MHz:

```sh
make clean
make F_CPU=1000000UL
```

**Nota:** usar `make clean` despues de cambiar `F_CPU`, porque Make no recompila objetos solo por cambios de variables de linea de comandos.

## Programacion (solo con hardware verificado)

```sh
make read-fuses PROGRAMMER=usbasp
make flash PROGRAMMER=usbasp
```

La primera orden solo lee los fusibles; la segunda programa FLASH. No se incluye ningun objetivo que escriba fusibles. Antes de programar, comprobar alimentacion, pines ISP, puente H, reloj real y modelo del programador.

## Limitaciones

- CI valida compilacion y enlazado, no comportamiento fisico.
- Calibracion QTR y polaridad de la linea requieren pruebas.
- La etapa 3 implementara periodo fijo para el PID y un protocolo de comandos mas completo.

Ver [DRIVERS.md](DRIVERS.md) y [HARDWARE.md](HARDWARE.md).
