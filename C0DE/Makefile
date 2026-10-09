# SeguidorLinea - AVR-GCC / GNU Make
# Ejecutar desde C0DE/: make

.DEFAULT_GOAL := all

TARGET       := SeguidorLinea
MCU          ?= atmega328p
F_CPU        ?= 1000000UL
PROGRAMMER   ?= usbasp
AVRDUDE_MCU  ?= m328p
PORT         ?=
AVRDUDE_FLAGS ?=

CC       := avr-gcc
OBJCOPY  := avr-objcopy
OBJDUMP  := avr-objdump
SIZE     := avr-size
AVRDUDE  := avrdude

OUT      := out
SRCS     := $(wildcard src/*.c)
OBJS     := $(patsubst src/%.c,$(OUT)/%.o,$(SRCS))
DEPS     := $(OBJS:.o=.d)
ELF      := $(OUT)/$(TARGET).elf
HEX      := $(OUT)/$(TARGET).hex
BIN      := $(OUT)/$(TARGET).bin
MAP      := $(OUT)/$(TARGET).map
LST      := $(OUT)/$(TARGET).lss

CFLAGS   := -mmcu=$(MCU) -DF_CPU=$(F_CPU) -std=gnu11 -Os -g3 \
            -Wall -Wextra -ffunction-sections -fdata-sections -Iinc
LDFLAGS  := -mmcu=$(MCU) -Wl,-Map,$(MAP),--gc-sections
LDLIBS   := -lm

.PHONY: all clean size flash read-fuses help

all: $(HEX) $(BIN) $(LST) size

$(OUT):
	mkdir -p $@

$(OUT)/%.o: src/%.c | $(OUT)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(ELF): $(OBJS)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) $(LDLIBS) -o $@

$(HEX): $(ELF)
	$(OBJCOPY) -O ihex -R .eeprom $< $@

$(BIN): $(ELF)
	$(OBJCOPY) -O binary -R .eeprom $< $@

$(LST): $(ELF)
	$(OBJDUMP) -h -S $< > $@

size: $(ELF)
	$(SIZE) -C --mcu=$(MCU) $<

flash: $(HEX)
	$(AVRDUDE) -p $(AVRDUDE_MCU) -c $(PROGRAMMER) $(if $(strip $(PORT)),-P $(PORT)) $(AVRDUDE_FLAGS) -U flash:w:$<:i

# Lectura exclusivamente; NO modifica los fusibles.
read-fuses:
	$(AVRDUDE) -p $(AVRDUDE_MCU) -c $(PROGRAMMER) $(if $(strip $(PORT)),-P $(PORT)) $(AVRDUDE_FLAGS) \
	  -U lfuse:r:-:h -U hfuse:r:-:h -U efuse:r:-:h

clean:
	$(RM) $(OBJS) $(DEPS) $(ELF) $(HEX) $(BIN) $(MAP) $(LST)

help:
	@printf 'Objetivos: all, size, flash, read-fuses, clean, help\n'
	@printf 'Variables: MCU=%s F_CPU=%s PROGRAMMER=%s PORT=%s\n' '$(MCU)' '$(F_CPU)' '$(PROGRAMMER)' '$(PORT)'

-include $(DEPS)
