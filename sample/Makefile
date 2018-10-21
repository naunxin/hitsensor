EXE				= test

LIBPATH 		= $(shell cd ../output; pwd)
INCLUDEPATH 	= $(LIBPATH)/include
#MIPS_GCC		?= cc
MIPS_GCC		?=arm-hisiv500-linux-gcc
export ARCH=arm

.PHONY: all
all:
	make $(EXE)

$(EXE): $(EXE).c
	$(MIPS_GCC) -o $@ $^ -I$(INCLUDEPATH) 

.PHONY: clean
clean:
ifneq ("$(wildcard $(EXE))", "")
	rm -r $(EXE)
endif

