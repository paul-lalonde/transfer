# V000.2

firm_src = ${filter-out cust.c tune.c test.c,${wildcard *.c}}
root = ./

#back of mac
MODEM=/dev/tty.usbmodemfa141

IPATH = .
VPATH = .

CCFLAGS=-Wall -gdwarf-2 -Os -std=gnu99 \
			-mmcu=$$part \
			-DF_CPU=$(F_CPU) \
			-ffunction-sections -fdata-sections \
			-Wl,--relax,--gc-sections \
			-Wl,--undefined=_mmcu,--section-start=.mmcu=0x910000 \

F_CPU=1000000

all: cust.bin test.bin tune.bin

debug: CCFLAGS += -DDEBUG=1
debug: all

include ${root}/Makefile.common

cust.hex: cust.axf
test.hex: test.axf
tune.hex: tune.axf

cust.axf: ${firm_src} cust.c ${wildcard *.h}
	part=${<} ; part=$${part/_*}; part=attiny44a; \
	${AVR}gcc ${CCFLAGS} ${firm_src} cust.c -DCUSTOMER_BUILD=1 -o ${@}
	@ls -l ${@}
	@file ${@}
	@${AVR}size ${@}

test.axf: ${firm_src} test.c ${wildcard *.h}
	part=${<} ; part=$${part/_*}; part=attiny44a; \
	${AVR}gcc ${CCFLAGS} ${firm_src}  test.c -DTEST_BUILD=1 -o ${@}
	@${AVR}size ${@}|sed '1d'

tune.axf: ${firm_src} tune.c ${wildcard *.h}
	part=${<} ; part=$${part/_*}; part=attiny44a; \
	${AVR}gcc ${CCFLAGS} ${firm_src} tune.c  -DTUNE_BUILD=1 -o ${@}
	@${AVR}size ${@}|sed '1d'

%.hex : %.axf
	${AVR}objcopy -j .text -j .data -O ihex ${<} ${@}

%.bin : %.axf
	${AVR}objcopy -j .text -j .data -O binary ${<} ${@}

clean: clean-${OBJ}
	rm -rf *.a *.bin *.axf ${target} *.vcd *.hex program programmer prog.c
