// graphics_vesa.c - VESA VBE 3.0 graphics driver for x86_64

#include "graphics.h"
#include "../printf.h"
#include <stddef.h>

#ifdef __x86_64__

// Simple VESA framebuffer detection for QEMU
// In a real implementation, this would use INT 0x10 BIOS calls

static graphics_device_t vesa_device;

// Simple framebuffer-based drawing functions
static void vesa_clear(graphics_device_t *dev, uint32_t color) {
    if (dev->framebuffer == NULL) return;

    uint32_t *fb = (uint32_t *)dev->framebuffer;
    uint32_t pixels = dev->width * dev->height;

    for (uint32_t i = 0; i < pixels; i++) {
        fb[i] = color;
    }
}

static void vesa_putpixel(graphics_device_t *dev, int32_t x, int32_t y, uint32_t color) {
    if (dev->framebuffer == NULL) return;
    if (x < 0 || y < 0 || (uint32_t)x >= dev->width || (uint32_t)y >= dev->height) return;

    uint32_t *fb = (uint32_t *)dev->framebuffer;
    fb[y * dev->width + x] = color;
}

static uint32_t vesa_getpixel(graphics_device_t *dev, int32_t x, int32_t y) {
    if (dev->framebuffer == NULL) return 0;
    if (x < 0 || y < 0 || (uint32_t)x >= dev->width || (uint32_t)y >= dev->height) return 0;

    uint32_t *fb = (uint32_t *)dev->framebuffer;
    return fb[y * dev->width + x];
}

static void vesa_fillrect(graphics_device_t *dev, graphics_rect_t rect, uint32_t color) {
    if (dev->framebuffer == NULL) return;

    uint32_t *fb = (uint32_t *)dev->framebuffer;

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
            fb[y * dev->width + x] = color;
        }
    }
}

static void vesa_drawrect(graphics_device_t *dev, graphics_rect_t rect, uint32_t color) {
    if (dev->framebuffer == NULL) return;

    uint32_t *fb = (uint32_t *)dev->framebuffer;

    // Draw top and bottom edges
    for (uint32_t x = rect.x; x < rect.x + rect.width && x < dev->width; x++) {
        if (rect.y < dev->height) fb[rect.y * dev->width + x] = color;
        if (rect.y + rect.height - 1 < dev->height) {
            fb[(rect.y + rect.height - 1) * dev->width + x] = color;
        }
    }

    // Draw left and right edges
    for (uint32_t y = rect.y; y < rect.y + rect.height && y < dev->height; y++) {
        if (rect.x < dev->width) fb[y * dev->width + rect.x] = color;
        if (rect.x + rect.width - 1 < dev->width) {
            fb[y * dev->width + (rect.x + rect.width - 1)] = color;
        }
    }
}

static void vesa_drawline(graphics_device_t *dev, graphics_point_t p1, graphics_point_t p2, uint32_t color) {
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
        if (x0 >= 0 && x0 < (int32_t)dev->width && y0 >= 0 && y0 < (int32_t)dev->height) {
            uint32_t *fb = (uint32_t *)dev->framebuffer;
            fb[y0 * dev->width + x0] = color;
        }

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

static void vesa_flush(graphics_device_t *dev) {
    // For a real VESA implementation, this would flush to the actual VESA framebuffer
    // For QEMU with linear framebuffer, this is typically a no-op
    (void)dev;
}

static bool vesa_init(graphics_device_t *dev) {
    // In QEMU, we can detect the framebuffer via multiboot
    // For now, we'll use a hardcoded address that works on many systems
    // Real implementation would use BIOS INT 0x10 calls

    // Try to detect framebuffer from QEMU/Multiboot
    // Default to 1024x768@32bpp at 0xE0000000 (QEMU standard)

    dev->width = 1024;
    dev->height = 768;
    dev->bpp = 32;
    dev->pitch = dev->width * 4;  // 32-bit = 4 bytes per pixel
    dev->framebuffer_size = dev->width * dev->height * 4;
    dev->mode = GRAPHICS_MODE_VESA;

    // Try common framebuffer addresses
    uint64_t fb_addresses[] = {
        0xe0000000,  // QEMU default
        0xfd000000,  // Alternative
        0xf0000000,  // Another common address
        0,           // End marker
    };

    // For now, use a simple approach - allocate from kernel space
    // Real implementation would map these addresses properly
    printf("[VESA] Attempting to initialize at 1024x768@32bpp\n");

    // Try to map framebuffer - for now, just indicate attempt
    // Real implementation would handle memory mapping
    dev->framebuffer = (void *)fb_addresses[0];

    printf("[VESA] Framebuffer at 0x%lx\n", (uint64_t)dev->framebuffer);

    return true;
}

static void vesa_cleanup(graphics_device_t *dev) {
    (void)dev;
    // Cleanup code here if needed
}

graphics_device_t *graphics_init_vesa(void) {
    vesa_device.init = vesa_init;
    vesa_device.cleanup = vesa_cleanup;
    vesa_device.clear = vesa_clear;
    vesa_device.putpixel = vesa_putpixel;
    vesa_device.getpixel = vesa_getpixel;
    vesa_device.fillrect = vesa_fillrect;
    vesa_device.drawrect = vesa_drawrect;
    vesa_device.drawline = vesa_drawline;
    vesa_device.flush = vesa_flush;

    if (vesa_device.init(&vesa_device)) {
        return &vesa_device;
    }
    return NULL;
}

#endif // __x86_64__
