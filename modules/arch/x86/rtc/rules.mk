MOD:=$(MODULES_ARCH_PATH)/rtc/rtc.ko
OBJ:=$(MODULES_ARCH_PATH)/rtc/main.o

$(MOD): $(OBJ) $(LIBFUDGE)
	$(LD) $(LDFLAGS) -o $@ $^

MODULES+=$(MOD)
MODULES_OBJECTS+=$(OBJ)
