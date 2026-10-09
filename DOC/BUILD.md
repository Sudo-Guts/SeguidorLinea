# Compilacion y programacion

## Requisitos (Debian/Ubuntu)

```sh
sudo apt update
sudo apt install make gcc-avr avr-libc binutils-avr avrdude
```

Comprueba el toolchain:

```sh
avr-gcc --version
avr-objcopy --version
avrdude -v
```

## Compilar

Todos los comandos Make se ejecutan desde `C0DE/`:

```sh
cd C0DE
make
make size
make clean
```

El `Makefile` compila todos los `src/*.c`, utiliza las cabeceras de `inc/`, genera archivos de dependencia `.d` y almacena los resultados en `out/`.

Artefactos: `SeguidorLinea.elf`, `SeguidorLinea.hex`, `SeguidorLinea.bin`, `SeguidorLinea.map` y `SeguidorLinea.lss`.

## Configuracion

Valores predeterminados del proyecto:

| Variable | Valor inicial | Observaciones |
| --- | --- | --- |
| `MCU` | `atmega328p` | Debe corresponder al AVR real |
| `F_CPU` | `1000000UL` | Hertz; NO configura los fusibles |
| `PROGRAMMER` | `usbasp` | Solo ejemplo, verificar programador |
| `AVRDUDE_MCU` | `m328p` | Identificador de AVRDUDE |
| `PORT` | vacio | Solo si el programador requiere puerto |

Por ejemplo, para compilar con una frecuencia realmente configurada de 8 MHz:

```sh
make clean
make F_CPU=8000000UL
```

**Importante:** `F_CPU` solo informa al compilador de la frecuencia real para los calculos de retardos, UART, etc. No cambia el reloj fisico. Verifica la fuente de reloj y los fusibles antes de modificar el valor.

## Programacion ISP (no automatica)

Solo cuando el programador, la alimentacion, los pines y el microcontrolador esten verificados:

```sh
make flash PROGRAMMER=usbasp
```

Si corresponde a tu programador, puedes ajustar `PROGRAMMER`, `PORT` y `AVRDUDE_FLAGS`. Ejemplo de lectura de fusibles, sin escritura:

```sh
make read-fuses PROGRAMMER=usbasp
```

No existe un objetivo `make fuses` deliberadamente. Nunca se debe deducir el valor de los fusibles solo a partir de `F_CPU`.

## Limitaciones conocidas

- La compilacion no valida el cableado ni los fusibles.
- El firmware existente aun requiere correcciones en las asignaciones OC1A/OC1B y el protocolo HC-05.
- Las pruebas en hardware y las mejoras de control digital corresponden a las siguientes etapas.
