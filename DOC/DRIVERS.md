# Etapa 2: drivers y periféricos

## CPU y reloj

- Objetivo: ATmega328P con **RC interno nominal de 8 MHz**.
- `Clock_init()` escribe `CLKPR` para seleccionar divisor 1 al arrancar.
- El fusible `CKDIV8` puede permanecer programado: configura el divisor inicial, que el firmware cambia al arrancar.
- El nuevo `F_CPU=8000000UL` describe la frecuencia **después** de `Clock_init()`.
- No se modifican los fusibles. Si el chip usa reloj externo o una fuente distinta, esta configuración **no** garantiza 8 MHz.
- Para hacer una compilación de 1 MHz con divisor 8: `make clean && make F_CPU=1000000UL`. Es necesario limpiar porque Make no detecta por si solo cambios en `F_CPU`.

## Motor / Timer1

- Se respetan los pines físicos heredados:
  - Motor A PWM: PB2 = OC1B = OCR1B.
  - Motor B PWM: PB1 = OC1A = OCR1A.
- Modo 14: Fast PWM, ICR1=999, prescaler=1. Frecuencia nominal: 8 kHz a 8 MHz.
- Velocidades firmadas negativas se limitan a cero (sin inversión de marcha); valores mayores a 999 se limitan a 999.
- En arranque, `STBY=0`. STOP deja PWM a cero y STBY inactivo; START despierta el driver.
- **Sin verificar** el modelo del puente H y el pinout real, los modos `BRAKE`/`COAST` deben comprobarse contra la hoja de datos del driver concreto.

## ADC / seis QTR analógicos

- ADC0..ADC5; referencia AVcc y buffers digitales deshabilitados.
- Prescaler ADC: 64 con 8 MHz y 8 con 1 MHz. Ambos producen 125 kHz nominales.
- Emisores IR encendidos durante todas las conversiones, con 200 us de asentamiento; apagados al terminar.
- La ISR UART **ya no toca los LEDs**.
- La calibración mantiene 50 muestras separadas por 100 ms: durante ~5 segundos, desplazar **todos** los sensores sobre línea y fondo.
- `QTR_read_line_white()` conserva la elección de línea blanca del firmware original. Comprobar experimentalmente polaridad y contraste: según el módulo analógico, podría corresponder `QTR_read_line_black()`.
- La pérdida de línea sigue retornando la última posición. Se abordará en el controlador de la etapa 3.

## UART HC-05

- 9600 baudios, 8N1, cálculo de divisor normal o doble velocidad según menor error nominal.
- RX: cola circular de 32 bytes y `USART_RX_vect`; TX: cola de 64 bytes y `USART_UDRE_vect`.
- Comandos binarios de un byte heredados:
  - `0x01`: STOP; `0x02`: START; `0x03`: poner integral a cero.
  - `0x04` a `0x06` están **reservados**, sin parser de valores implementado. Se ignoran y **no** bloquean comandos posteriores.
- Telemetría: líneas ASCII de error con signo terminadas en `\r\n`, p. ej. `-123\r\n`.
- `hc05_try_puti()` coloca la línea completa o la descarta si no cabe; no bloquea el control. Los `hc05_put*` y `hc05_get*` son bloqueantes y requieren interrupciones habilitadas.
- `hc05_rx_dropped` cuenta hasta 255 bytes descartados por errores de UART o llenado de cola.
- Advertencia: el control PID **todavía no** se ejecuta a periodo fijo; la telemetría sigue contándose por iteraciones.

## Pruebas en banco sugeridas

1. Comprobar que la placa posee el oscilador RC interno seleccionado y que PB6/PB7 están libres de cristal externo.
2. Ejecutar `make clean && make && make size` desde `C0DE/`.
3. Sin motores energizados, verificar con instrumento que PB2 (Motor A) y PB1 (Motor B) producen PWM de aproximadamente 8 kHz al enviar START.
4. Verificar salidas PWM a cero y STBY bajo al iniciar y después de `0x01`.
5. Mover la barra QTR de un extremo a otro durante la calibración, comprobar valores y polaridad.
6. Con HC-05 a 9600 8N1, enviar `0x02`, `0x01`, `0x04`, `0x01`; confirmar que STOP sigue operativo después del comando reservado.
7. Comprobar telemetría ASCII con delimitador CRLF y ausencia de bloqueos al desconectar el receptor.

Estas verificaciones no sustituyen una prueba de banco. No modificar fusibles ni asumir pines del puente H sin corroborar el circuito.
