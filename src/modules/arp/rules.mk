M:=\
    $(DIR_SRC)/modules/arp/arp.ko \

N:=\
    $(DIR_SRC)/modules/arp/arp.ko.map \

O:=\
    $(DIR_SRC)/modules/arp/main.o \

L:=\
    $(DIR_LIB)/fudge/fudge.a \
    $(DIR_LIB)/net/net.a \

include $(DIR_MK)/kmod.mk
