FX2LIBDIR=../../
BASENAME = i2c_main
SOURCES=i2c_main.c
CODE_SIZE = --code-size 0x3000
XRAM_LOC = --xram-loc 0x3200 
XRAM_SIZE = --xram-size 0x700
DSCR_AREA=
INT2JT=
include $(FX2LIBDIR)/lib/fx2.mk
fx2_download:
	../download.sh build/$(BASENAME).ihx
