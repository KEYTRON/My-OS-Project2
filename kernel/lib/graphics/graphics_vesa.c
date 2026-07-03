// graphics_vesa.c - линейный framebuffer, полученный от GRUB (multiboot)
//
// Ядро запрашивает видеорежим в multiboot-заголовке (entry.S). GRUB
// устанавливает VBE-режим и передаёт адрес/геометрию framebuffer'а в
// multiboot info. kmain вызывает graphics_set_framebuffer_info() до
// graphics_init(); если информации нет — драйвер честно возвращает NULL
// и ядро остаётся в текстовом режиме.

#include "graphics.h"
#include "../printf.h"
#include <stddef.h>

#ifdef __x86_64__

static graphics_device_t vesa_device;

// Параметры framebuffer'а из multiboot info (0 = не задано)
static uint64_t mb_fb_addr = 0;
static uint32_t mb_fb_pitch = 0;
static uint32_t mb_fb_width = 0;
static uint32_t mb_fb_height = 0;
static uint8_t  mb_fb_bpp = 0;

void graphics_set_framebuffer_info(uint64_t addr, uint32_t pitch,
                                   uint32_t width, uint32_t height,
                                   uint8_t bpp) {
    mb_fb_addr = addr;
    mb_fb_pitch = pitch;
    mb_fb_width = width;
    mb_fb_height = height;
    mb_fb_bpp = bpp;
}

// Строка пикселей с учётом pitch (может отличаться от width*4)
static inline uint32_t *fb_row(graphics_device_t *dev, int32_t y) {
    return (uint32_t *)((uint8_t *)dev->framebuffer + (uint32_t)y * dev->pitch);
}

static void vesa_clear(graphics_device_t *dev, uint32_t color) {
    if (dev->framebuffer == NULL) return;
    for (uint32_t y = 0; y < dev->height; y++) {
        uint32_t *row = fb_row(dev, y);
        for (uint32_t x = 0; x < dev->width; x++) {
            row[x] = color;
        }
    }
}

static void vesa_putpixel(graphics_device_t *dev, int32_t x, int32_t y, uint32_t color) {
    if (dev->framebuffer == NULL) return;
    if (x < 0 || y < 0 || (uint32_t)x >= dev->width || (uint32_t)y >= dev->height) return;
    fb_row(dev, y)[x] = color;
}

static uint32_t vesa_getpixel(graphics_device_t *dev, int32_t x, int32_t y) {
    if (dev->framebuffer == NULL) return 0;
    if (x < 0 || y < 0 || (uint32_t)x >= dev->width || (uint32_t)y >= dev->height) return 0;
    return fb_row(dev, y)[x];
}

static void vesa_fillrect(graphics_device_t *dev, graphics_rect_t rect, uint32_t color) {
    if (dev->framebuffer == NULL) return;

    int32_t x1 = rect.x;
    int32_t y1 = rect.y;
    int32_t x2 = rect.x + rect.width;
    int32_t y2 = rect.y + rect.height;

    if (x1 < 0) x1 = 0;
    if (y1 < 0) y1 = 0;
    if (x2 > (int32_t)dev->width) x2 = dev->width;
    if (y2 > (int32_t)dev->height) y2 = dev->height;

    for (int32_t y = y1; y < y2; y++) {
        uint32_t *row = fb_row(dev, y);
        for (int32_t x = x1; x < x2; x++) {
            row[x] = color;
        }
    }
}

static void vesa_drawrect(graphics_device_t *dev, graphics_rect_t rect, uint32_t color) {
    if (dev->framebuffer == NULL) return;

    // Горизонтальные линии
    for (int32_t x = rect.x; x < rect.x + (int32_t)rect.width; x++) {
        if (x < 0 || (uint32_t)x >= dev->width) continue;
        if (rect.y >= 0 && (uint32_t)rect.y < dev->height)
            fb_row(dev, rect.y)[x] = color;
        int32_t yb = rect.y + rect.height - 1;
        if (yb >= 0 && (uint32_t)yb < dev->height)
            fb_row(dev, yb)[x] = color;
    }

    // Вертикальные линии
    for (int32_t y = rect.y; y < rect.y + (int32_t)rect.height; y++) {
        if (y < 0 || (uint32_t)y >= dev->height) continue;
        if (rect.x >= 0 && (uint32_t)rect.x < dev->width)
            fb_row(dev, y)[rect.x] = color;
        int32_t xr = rect.x + rect.width - 1;
        if (xr >= 0 && (uint32_t)xr < dev->width)
            fb_row(dev, y)[xr] = color;
    }
}

static void vesa_drawline(graphics_device_t *dev, graphics_point_t p1, graphics_point_t p2, uint32_t color) {
    if (dev->framebuffer == NULL) return;

    // Алгоритм Брезенхэма
    int32_t x0 = p1.x, y0 = p1.y;
    int32_t x1 = p2.x, y1 = p2.y;

    int32_t dx = (x1 > x0) ? (x1 - x0) : (x0 - x1);
    int32_t dy = (y1 > y0) ? (y1 - y0) : (y0 - y1);
    int32_t sx = (x0 < x1) ? 1 : -1;
    int32_t sy = (y0 < y1) ? 1 : -1;
    int32_t err = (dx > dy) ? (dx / 2) : (-dy / 2);

    while (1) {
        vesa_putpixel(dev, x0, y0, color);

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
    (void)dev; // рисуем сразу в linear framebuffer
}

static bool vesa_init(graphics_device_t *dev) {
    if (mb_fb_addr == 0 || mb_fb_bpp != 32) {
        // GRUB не передал 32-битный framebuffer — работаем в текстовом режиме
        return false;
    }

    dev->width = mb_fb_width;
    dev->height = mb_fb_height;
    dev->bpp = mb_fb_bpp;
    dev->pitch = mb_fb_pitch;
    dev->framebuffer = (void *)mb_fb_addr;
    dev->framebuffer_size = (uint64_t)mb_fb_pitch * mb_fb_height;
    dev->mode = GRAPHICS_MODE_VESA;

    printf("[VESA] Framebuffer from multiboot: %ux%u@%u pitch=%u at 0x%lx\n",
           dev->width, dev->height, dev->bpp, dev->pitch, mb_fb_addr);

    return true;
}

static void vesa_cleanup(graphics_device_t *dev) {
    (void)dev;
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
