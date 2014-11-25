all: demos/nasmacs.ihx demos/hw.ihx demos/sieve.ihx demos/j.ihx

%.ihx: %.c crt0_nascom.rel
	@echo SDCC $<
	@/opt/sdcc/bin/sdcc --std-c99 -mz80 --code-loc 0x1004 --data-loc 0x4000 --no-std-crt0 crt0_nascom.rel $<

crt0_nascom.rel: crt0_nascom.s
	@echo SDASZ80 $<
	@/opt/sdcc/bin/sdasz80 -plosgff -o crt0_nascom.s
