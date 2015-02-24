#include <fudge.h>
#include <kernel.h>
#include <base/base.h>
#include <system/system.h>
#include "platform.h"

enum platform_register
{

    PLATFORM_REGISTER_PIT               = 0x0040,
    PLATFORM_REGISTER_RTC               = 0x0070,
    PLATFORM_REGISTER_UART4             = 0x02E8,
    PLATFORM_REGISTER_UART2             = 0x02F8,
    PLATFORM_REGISTER_UART3             = 0x03E8,
    PLATFORM_REGISTER_UART1             = 0x03F8

};

enum platform_irq
{

    PLATFORM_IRQ_PIT                    = 0x00,
    PLATFORM_IRQ_UART2                  = 0x03,
    PLATFORM_IRQ_UART1                  = 0x04,
    PLATFORM_IRQ_RTC                    = 0x08

};

struct device
{

    unsigned short base;
    unsigned short irq;

};

static struct base_bus bus;

static struct device devices[] = {
    {0},
    {PLATFORM_REGISTER_PIT, PLATFORM_IRQ_PIT},
    {PLATFORM_REGISTER_RTC, PLATFORM_IRQ_RTC},
    {PLATFORM_REGISTER_UART1, PLATFORM_IRQ_UART1},
    {PLATFORM_REGISTER_UART2, PLATFORM_IRQ_UART2},
    {PLATFORM_REGISTER_UART3, PLATFORM_IRQ_UART1},
    {PLATFORM_REGISTER_UART4, PLATFORM_IRQ_UART2}
};

unsigned short platform_getbase(unsigned int id)
{

    return devices[id].base;

}

unsigned short platform_getirq(unsigned int id)
{

    return devices[id].irq;

}

static void bus_setup()
{

}

static unsigned int bus_next(unsigned int id)
{

    return (id < 6) ? id + 1 : 0;

}

void module_init()
{

    base_initbus(&bus, PLATFORM_BUS_TYPE, "platform", bus_setup, bus_next);

}

void module_register()
{

    base_registerbus(&bus);

}

void module_unregister()
{

    base_unregisterbus(&bus);

}

