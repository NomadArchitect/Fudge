M:=\
    $(DIR_SRC)/modules/arch/x86/io/io.ko \

O:=\
    $(DIR_SRC)/modules/arch/x86/io/io.o \

L:=\
    $(DIR_SRC)/fudge/fudge.a \

include $(DIR_MK)/mod.mk
