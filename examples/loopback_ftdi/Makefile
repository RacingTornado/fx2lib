FX2LIBDIR=../../
BASENAME = ftdi_main
SOURCES=ftdi_main.c ftdi_conf.c
PID=0x1004
CODE_SIZE = --code-size 0x3000
XRAM_LOC = --xram-loc 0x3200 
XRAM_SIZE = --xram-size 0x700
INT2JT =-Wl"-b INT2JT=0x3100"
include $(FX2LIBDIR)lib/fx2.mk
include $(FX2LIBDIR)lib/fx2-cdesc.mk
fx2_download:
	../download.sh build/$(BASENAME).ihx
