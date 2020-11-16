BIN = kristhoefer

SOURCES = \
		  main.c \
		  i2c.c \
		  adc.c \
		  timer.c \
		  serial.c \
		  pca9685.c \
		  button.c
PORT =/dev/ttyS5
MCU=atmega328p

CC =avr-gcc
LD=avr-ld
OBJCOPY=avr-objcopy
OBJDUMP=avr-objdump
AVRSIZE=avr-size

CFLAGS = -g -Wall -pedantic -O2 -DF_CPU=16000000UL -mmcu=$(MCU) -c# Compile flags
LDFLAGS = -mmcu=$(MCU)# Länkarﬂaggor

OBJS = $(SOURCES:.c=.o)
OUT=${BIN}.elf ${BIN}.hex ${BIN}.lss

flash: all # Flasha/programmera arduinon – bygger vid behov
	avrdude -F -V -c arduino -p $(MCU) -P $(PORT) -b 115200 -U flash:w:$(BIN).hex


all: $(OUT) # Bygg rubbet, men ﬂasha inte till target
    		# *.map Minneskonﬁguration = metadata
    		# *.elf Exekverbar binär, kan tryckas in i en debugger/analysverktyg = maskinkod + data + debugsymboler
    		# *.hex ELF strippad och konverterad till ihex – ﬂashas till arduinon = maskinkod + data
    		# *.lss ELF’en översatt till assembler (!) + minnessegment + minnesinnehåll = assembler + data + metadata

$(OBJS): makefile

%.o:%.c
	$(CC) $(CFLAGS) -MD $< -o $@
	@cp $*.d $*.P; \
		sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
		-e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P; \
		rm -f $*.d

%.lss: %.elf
	$(OBJDUMP) -h -S -s $< > $@

%.elf: $(OBJS) # Länka
	$(CC) -Wl,-Map=$(@:.elf=.map) $(LDFLAGS) -o $@ $^
	$(AVRSIZE) $@

%.hex: %.elf
	$(OBJCOPY) -O ihex -R .fuse -R .lock -R .user_signatures -R .comment $< $@

clean: # Rensa upp byggkatalogen
	@echo " Cleaning...";
	rm -f $(OUT) $(OBJS) *.map *.P *.out 

screen: # öppna en serie terminal
	screen $(PORT) 38400
