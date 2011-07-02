#include <lib/call.h>
#include <lib/memory.h>
#include <lib/string.h>
#include <kernel/vfs.h>
#include <kernel/modules.h>
#include <arch/x86/modules/io/io.h>
#include <arch/x86/modules/vga/vga.h>

static struct vga_device vgaDevice;
static struct modules_device vgaFramebufferDevice;
static struct modules_device vgaFramebufferColorDevice;
static struct modules_device vgaFramebufferCursorDevice;
static unsigned char vgaFbColor;

static unsigned int vga_read_framebuffer(char *buffer, unsigned int count, unsigned int offset)
{

    unsigned int i;
    unsigned int j = 0;

    for (i = offset; i < offset + count; i++, j++)
    {

        if (i == VGA_FB_SIZE)
            return j;

        memory_copy(buffer + j, (void *)(VGA_FB_ADDRESS + i * 2), 1);

    }

    return j;

}

static unsigned int vga_fb_node_read(struct vfs_node *node, unsigned int offset, unsigned int count, void *buffer)
{

    return vgaDevice.read_framebuffer(buffer, count, offset);

}

static unsigned int vga_write_framebuffer(char *buffer, unsigned int count, unsigned int offset)
{

    unsigned int i;
    unsigned int j = 0;

    for (i = offset; i < offset + count; i++, j++)
    {

        if (i == VGA_FB_SIZE)
            return j;

        memory_copy((void *)(VGA_FB_ADDRESS + i * 2), buffer + j, 1);
        memory_set((void *)(VGA_FB_ADDRESS + i * 2 + 1), vgaFbColor, 1);

    }

    return j;

}

static unsigned int vga_fb_node_write(struct vfs_node *node, unsigned int offset, unsigned int count, void *buffer)
{

    return vgaDevice.write_framebuffer(buffer, count, offset);

}

static unsigned int vga_fb_color_node_read(struct vfs_node *node, unsigned int offset, unsigned int count, void *buffer)
{

    if (count != 1)
        return 0;

    ((char *)buffer)[0] = vgaFbColor;

    return 1;

}

static unsigned int vga_fb_color_node_write(struct vfs_node *node, unsigned int offset, unsigned int count, void *buffer)
{

    if (count != 1)
        return 0;

    vgaFbColor = ((char *)buffer)[0];

    return 1;

}

static void vga_set_cursor_offset(unsigned short offset)
{

    io_outb(0x3D4, 14);
    io_outb(0x3D5, offset >> 8);
    io_outb(0x3D4, 15);
    io_outb(0x3D5, offset);

}

static unsigned int vga_fb_cursor_node_write(struct vfs_node *node, unsigned int offset, unsigned int count, void *buffer)
{

    if (count != 1)
        return 0;

    short position = ((short *)buffer)[0];

    vgaDevice.set_cursor_offset(position);

    return 1;

}

void vga_init()
{

    string_copy(vgaDevice.base.name, "vga");
    vgaDevice.read_framebuffer = vga_read_framebuffer;
    vgaDevice.write_framebuffer = vga_write_framebuffer;
    vgaDevice.set_cursor_offset = vga_set_cursor_offset;

    string_copy(vgaFramebufferDevice.name, "vga_fb");
    string_copy(vgaFramebufferDevice.node.name, "vga_fb"); //TODO:REMOVE
    vgaFramebufferDevice.node.length = VGA_FB_SIZE;
    vgaFramebufferDevice.node.operations.read = vga_fb_node_read;
    vgaFramebufferDevice.node.operations.write = vga_fb_node_write;
    modules_register_device(MODULES_DEVICE_TYPE_VGA, &vgaFramebufferDevice);

    string_copy(vgaFramebufferColorDevice.name, "vga_fb_color");
    string_copy(vgaFramebufferColorDevice.node.name, "vga_fb_color"); //TODO:REMOVE
    vgaFramebufferColorDevice.node.length = 1;
    vgaFramebufferColorDevice.node.operations.read = vga_fb_color_node_read;
    vgaFramebufferColorDevice.node.operations.write = vga_fb_color_node_write;
    modules_register_device(MODULES_DEVICE_TYPE_VGA, &vgaFramebufferColorDevice);

    string_copy(vgaFramebufferCursorDevice.name, "vga_fb_cursor");
    string_copy(vgaFramebufferCursorDevice.node.name, "vga_fb_cursor"); //TODO:REMOVE
    vgaFramebufferCursorDevice.node.length = 1;
    vgaFramebufferCursorDevice.node.operations.write = vga_fb_cursor_node_write;
    modules_register_device(MODULES_DEVICE_TYPE_VGA, &vgaFramebufferCursorDevice);

    //TODO:REMOVE ALL BELOW
    struct vfs_node *devNode = vfs_find_root("/dev");
    devNode->operations.write(devNode, devNode->length, 1, &vgaFramebufferDevice.node);
    devNode->operations.write(devNode, devNode->length, 1, &vgaFramebufferColorDevice.node);
    devNode->operations.write(devNode, devNode->length, 1, &vgaFramebufferCursorDevice.node);


}

