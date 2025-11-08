// graphics_framebuffer.c - Framebuffer graphics driver for ARM64/RISC-V64

#include "graphics.h"
#include "../printf.h"
#include <stddef.h>

#if defined(__aarch64__) || defined(__riscv64__)

// Generic framebuffer device
static graphics_device_t fb_device;

// Common framebuffer drawing functions
static void fb_clear(graphics_device_t *dev, uint32_t color) {
    if (dev->framebuffer == NULL) return;

    switch (dev->bpp) {
        case 32: {
            uint32_t *fb = (uint32_t *)dev->framebuffer;
            uint32_t pixels = dev->width * dev->height;
            for (uint32_t i = 0; i < pixels; i++) {
                fb[i] = color;
            }
            break;
        }
        case 16: {
            uint16_t *fb = (uint16_t *)dev->framebuffer;
            uint32_t pixels = dev->width * dev->height;
            uint16_t color16 = ((color >> 8) & 0xF800) | ((color >> 5) & 0x07E0) | ((color >> 3) & 0x001F);
            for (uint32_t i = 0; i < pixels; i++) {
                fb[i] = color16;
            }
            break;
        }
    }
}

static void fb_putpixel(graphics_device_t *dev, int32_t x, int32_t y, uint32_t color) {
    if (dev->framebuffer == NULL) return;
    if (x < 0 || y < 0 || (uint32_t)x >= dev->width || (uint32_t)y >= dev->height) return;

    uint32_t offset = y * dev->width + x;

    switch (dev->bpp) {
        case 32: {
            uint32_t *fb = (uint32_t *)dev->framebuffer;
            fb[offset] = color;
            break;
        }
        case 16: {
            uint16_t *fb = (uint16_t *)dev->framebuffer;
            uint16_t color16 = ((color >> 8) & 0xF800) | ((color >> 5) & 0x07E0) | ((color >> 3) & 0x001F);
            fb[offset] = color16;
            break;
        }
    }
}

static uint32_t fb_getpixel(graphics_device_t *dev, int32_t x, int32_t y) {
    if (dev->framebuffer == NULL) return 0;
    if (x < 0 || y < 0 || (uint32_t)x >= dev->width || (uint32_t)y >= dev->height) return 0;

    uint32_t offset = y * dev->width + x;

    switch (dev->bpp) {
        case 32: {
            uint32_t *fb = (uint32_t *)dev->framebuffer;
            return fb[offset];
        }
        case 16: {
            uint16_t *fb = (uint16_t *)dev->framebuffer;
            uint16_t color16 = fb[offset];
            // Convert 16-bit RGB565 to 32-bit RGB888
            return ((color16 & 0xF800) << 8) | ((color16 & 0x07E0) << 5) | ((color16 & 0x001F) << 3);
        }
    }
    return 0;
}

static void fb_fillrect(graphics_device_t *dev, graphics_rect_t rect, uint32_t color) {
    if (dev->framebuffer == NULL) return;

    // Clip rectangle to screen bounds
    int32_t x1 = rect.x;
    int32_t y1 = rect.y;
    int32_t x2 = rect.x + rect.width;
    int32_t y2 = rect.y + rect.height;

    if (x1 < 0) x1 = 0;
    if (y1 < 0) y1 = 0;
    if (x2 > (int32_t)dev->width) x2 = dev->width;
    if (y2 > (int32_t)dev->height) y2 = dev->height;

    for (int32_t y = y1; y < y2; y++) {
        for (int32_t x = x1; x < x2; x++) {
            fb_putpixel(dev, x, y, color);
        }
    }
}

static void fb_drawrect(graphics_device_t *dev, graphics_rect_t rect, uint32_t color) {
    if (dev->framebuffer == NULL) return;

    // Draw top and bottom edges
    for (uint32_t x = rect.x; x < rect.x + rect.width && x < dev->width; x++) {
        if (rect.y < dev->height) fb_putpixel(dev, x, rect.y, color);
        if (rect.y + rect.height - 1 < dev->height) {
            fb_putpixel(dev, x, rect.y + rect.height - 1, color);
        }
    }

    // Draw left and right edges
    for (uint32_t y = rect.y; y < rect.y + rect.height && y < dev->height; y++) {
        if (rect.x < dev->width) fb_putpixel(dev, rect.x, y, color);
        if (rect.x + rect.width - 1 < dev->width) {
            fb_putpixel(dev, rect.x + rect.width - 1, y, color);
        }
    }
}

static void fb_drawline(graphics_device_t *dev, graphics_point_t p1, graphics_point_t p2, uint32_t color) {
    if (dev->framebuffer == NULL) return;

    // Bresenham line algorithm
    int32_t x0 = p1.x, y0 = p1.y;
    int32_t x1 = p2.x, y1 = p2.y;

    int32_t dx = (x1 > x0) ? (x1 - x0) : (x0 - x1);
    int32_t dy = (y1 > y0) ? (y1 - y0) : (y0 - y1);
    int32_t sx = (x0 < x1) ? 1 : -1;
    int32_t sy = (y0 < y1) ? 1 : -1;
    int32_t err = (dx > dy) ? (dx / 2) : (-dy / 2);

    while (1) {
        fb_putpixel(dev, x0, y0, color);

        if (x0 == x1 && y0 == y1) break;

        int32_t e2 = err;
        if (e2 > -dx) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dy) {
            err += dx;
            y0 += sy;
        }
    }
}

static void fb_flush(graphics_device_t *dev) {
    // For framebuffer, flushing typically means cache operations
    (void)dev;
}

static bool fb_init(graphics_device_t *dev) {
    // Try to detect framebuffer via device tree or QEMU
    // For QEMU ARM64 virt machine, framebuffer is typically at a known address

    // Default: QEMU virt machine settings
    dev->width = 1024;
    dev->height = 768;
    dev->bpp = 32;
    dev->pitch = dev->width * 4;
    dev->framebuffer_size = dev->width * dev->height * 4;
    dev->mode = GRAPHICS_MODE_FB_32;

#ifdef __aarch64__
    // ARM64: Try to use QEMU framebuffer
    // In real hardware, this would be discovered via Device Tree
    dev->framebuffer = (void *)0x3eff0000;  // QEMU virt machine framebuffer
    printf("[FRAMEBUFFER] ARM64: Attempting to use QEMU framebuffer at 0x%lx\n", (uint64_t)dev->framebuffer);
#endif

#ifdef __riscv64__
    // RISC-V64: Similar approach
    dev->framebuffer = (void *)0x3eff0000;  // QEMU RISC-V framebuffer
    printf("[FRAMEBUFFER] RISC-V64: Attempting to use QEMU framebuffer at 0x%lx\n", (uint64_t)dev->framebuffer);
#endif

    return true;
}

static void fb_cleanup(graphics_device_t *dev) {
    (void)dev;
}

graphics_device_t *graphics_init_framebuffer(void) {
    fb_device.init = fb_init;
    fb_device.cleanup = fb_cleanup;
    fb_device.clear = fb_clear;
    fb_device.putpixel = fb_putpixel;
    fb_device.getpixel = fb_getpixel;
    fb_device.fillrect = fb_fillrect;
    fb_device.drawrect = fb_drawrect;
    fb_device.drawline = fb_drawline;
    fb_device.flush = fb_flush;

    if (fb_device.init(&fb_device)) {
        return &fb_device;
    }
    return NULL;
}

#endif // __aarch64__ || __riscv64__
