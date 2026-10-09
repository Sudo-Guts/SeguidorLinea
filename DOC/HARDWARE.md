# Hardware: inventario preliminar

**Configuracion presunta:** ATmega328P con `F_CPU=1 MHz`. No se ha comprobado la placa, el encapsulado, el reloj real ni los fusibles. Esta tabla refleja las definiciones existentes y NO es un esquema electrico validado.

| Funcion | Pin AVR indicado en el codigo | Archivo |
| --- | --- | --- |
| QTR ADC0..ADC5 (6 sensores) | PC0..PC5 | `QTR_Sensors.c` |
| Habilitacion emisores IR | PB6 | `QTR_Sensors.h` |
| Motor A IN1 / IN2 | PB7 / PB0 | `Motor.h` |
| Motor B IN1 / IN2 | PB4 / PB3 | `Motor.h` |
| Motor A PWM (declarado OC1A) | PB2 | `Motor.h` |
| Motor B PWM (declarado OC1B) | PB1 | `Motor.h` |
| Driver STBY | PB5 | `Motor.h` |
| UART RX0 / TX0 | PD0 / PD1 | `HC_05.c` |

## Verificaciones obligatorias antes de la etapa de drivers

1. **PWM:** en ATmega328P, OC1A corresponde a PB1 y OC1B a PB2. Las macros actuales parecen invertidas. No se modificaron sin confirmar el cableado.
2. **Reloj:** PB6 y PB7 tambien son XTAL1/XTAL2. Si se utiliza un oscilador externo en estos pines, no pueden operar simultaneamente como las E/S indicadas.
3. **ISP:** PB3, PB4 y PB5 comparten funciones con MOSI, MISO y SCK; verificar que la etapa de potencia permanezca deshabilitada al programar.
4. **Frecuencia/fusibles:** verificar hardware real antes de cambiar `F_CPU` o programar el microcontrolador.
5. **Driver de motores:** identificar el modelo exacto y comprobar su conexion al AVR.

Esta etapa no modifica las asignaciones fisicas ni los fusibles.
