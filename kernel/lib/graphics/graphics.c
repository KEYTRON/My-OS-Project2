// graphics.c - Cross-platform graphics implementation

#include "graphics.h"
#include "../printf.h"

// Global graphics device
graphics_device_t *g_graphics_device = NULL;

// Forward declarations for architecture-specific implementations
#ifdef __x86_64__
graphics_device_t *graphics_init_vesa(void);      // x86_64 VESA VBE (graphics_vesa.c)
#else
static inline graphics_device_t *graphics_init_vesa(void) { return NULL; }
#endif

#if defined(__aarch64__) || defined(__riscv64__)
graphics_device_t *graphics_init_framebuffer(void); // ARM64/RISC-V framebuffer (graphics_framebuffer.c)
#else
static inline graphics_device_t *graphics_init_framebuffer(void) { return NULL; }
#endif

graphics_device_t *graphics_init_text(void);       // Text mode fallback (this file)

// Initialize graphics system
graphics_device_t *graphics_init(void) {
    if (g_graphics_device != NULL) {
        return g_graphics_device;
    }

    // Try to initialize based on architecture
#ifdef __x86_64__
    // Try VESA VBE first on x86_64
    g_graphics_device = graphics_init_vesa();
    if (g_graphics_device != NULL) {
        printf("[GRAPHICS] Initialized VESA VBE: %ux%u @%ubpp\n",
               g_graphics_device->width, g_graphics_device->height, g_graphics_device->bpp);
        return g_graphics_device;
    }
#endif

#if defined(__aarch64__) || defined(__riscv64__)
    // Try framebuffer on ARM64/RISC-V
    g_graphics_device = graphics_init_framebuffer();
    if (g_graphics_device != NULL) {
        printf("[GRAPHICS] Initialized Framebuffer: %ux%u @%ubpp\n",
               g_graphics_device->width, g_graphics_device->height, g_graphics_device->bpp);
        return g_graphics_device;
    }
#endif

    // Fallback to text mode
    printf("[GRAPHICS] Falling back to text mode\n");
    g_graphics_device = graphics_init_text();
    if (g_graphics_device != NULL) {
        printf("[GRAPHICS] Initialized Text mode: %ux%u\n",
               g_graphics_device->width, g_graphics_device->height);
        return g_graphics_device;
    }

    printf("[GRAPHICS] ERROR: Could not initialize any graphics mode!\n");
    return NULL;
}

// Cleanup graphics system
void graphics_cleanup(void) {
    if (g_graphics_device != NULL && g_graphics_device->cleanup != NULL) {
        g_graphics_device->cleanup(g_graphics_device);
    }
    g_graphics_device = NULL;
}

// Drawing functions
void graphics_clear(uint32_t color) {
    if (g_graphics_device == NULL || g_graphics_device->clear == NULL) return;
    g_graphics_device->clear(g_graphics_device, color);
}

void graphics_putpixel(int32_t x, int32_t y, uint32_t color) {
    if (g_graphics_device == NULL || g_graphics_device->putpixel == NULL) return;
    g_graphics_device->putpixel(g_graphics_device, x, y, color);
}

uint32_t graphics_getpixel(int32_t x, int32_t y) {
    if (g_graphics_device == NULL || g_graphics_device->getpixel == NULL) return 0;
    return g_graphics_device->getpixel(g_graphics_device, x, y);
}

void graphics_fillrect(graphics_rect_t rect, uint32_t color) {
    if (g_graphics_device == NULL || g_graphics_device->fillrect == NULL) return;
    g_graphics_device->fillrect(g_graphics_device, rect, color);
}

void graphics_drawrect(graphics_rect_t rect, uint32_t color) {
    if (g_graphics_device == NULL || g_graphics_device->drawrect == NULL) return;
    g_graphics_device->drawrect(g_graphics_device, rect, color);
}

void graphics_drawline(graphics_point_t p1, graphics_point_t p2, uint32_t color) {
    if (g_graphics_device == NULL || g_graphics_device->drawline == NULL) return;
    g_graphics_device->drawline(g_graphics_device, p1, p2, color);
}

// Simple circle drawing using Midpoint Circle Algorithm
void graphics_drawcircle(graphics_point_t center, uint32_t radius, uint32_t color) {
    if (g_graphics_device == NULL || g_graphics_device->putpixel == NULL) return;

    int32_t x = radius;
    int32_t y = 0;
    int32_t d = 3 - 2 * radius;

    while (x >= y) {
        // Draw 8 symmetrical points
        g_graphics_device->putpixel(g_graphics_device, center.x + x, center.y + y, color);
        g_graphics_device->putpixel(g_graphics_device, center.x - x, center.y + y, color);
        g_graphics_device->putpixel(g_graphics_device, center.x + x, center.y - y, color);
        g_graphics_device->putpixel(g_graphics_device, center.x - x, center.y - y, color);
        g_graphics_device->putpixel(g_graphics_device, center.x + y, center.y + x, color);
        g_graphics_device->putpixel(g_graphics_device, center.x - y, center.y + x, color);
        g_graphics_device->putpixel(g_graphics_device, center.x + y, center.y - x, color);
        g_graphics_device->putpixel(g_graphics_device, center.x - y, center.y - x, color);

        if (d < 0) {
            d = d + 4 * y + 6;
        } else {
            d = d + 4 * (y - x) + 10;
            x--;
        }
        y++;
    }
}

// Fill circle
void graphics_fillcircle(graphics_point_t center, uint32_t radius, uint32_t color) {
    if (g_graphics_device == NULL || g_graphics_device->putpixel == NULL) return;

    for (int32_t y = -radius; y <= radius; y++) {
        for (int32_t x = -radius; x <= radius; x++) {
            if (x * x + y * y <= radius * radius) {
                g_graphics_device->putpixel(g_graphics_device, center.x + x, center.y + y, color);
            }
        }
    }
}

// Text rendering
void graphics_putchar(int32_t x, int32_t y, char c, uint32_t fg, uint32_t bg) {
    // This will be implemented when we have font support
    (void)x; (void)y; (void)c; (void)fg; (void)bg;
}

void graphics_putstring(int32_t x, int32_t y, const char *str, uint32_t fg, uint32_t bg) {
    if (str == NULL) return;
    while (*str) {
        graphics_putchar(x, y, *str, fg, bg);
        x += 8;  // Assume 8-pixel character width
        str++;
    }
}

// Utility functions
void graphics_flush(void) {
    if (g_graphics_device != NULL && g_graphics_device->flush != NULL) {
        g_graphics_device->flush(g_graphics_device);
    }
}

graphics_mode_t graphics_get_mode(void) {
    if (g_graphics_device == NULL) return GRAPHICS_MODE_NONE;
    return g_graphics_device->mode;
}

uint32_t graphics_get_width(void) {
    if (g_graphics_device == NULL) return 0;
    return g_graphics_device->width;
}

uint32_t graphics_get_height(void) {
    if (g_graphics_device == NULL) return 0;
    return g_graphics_device->height;
}

uint32_t graphics_get_bpp(void) {
    if (g_graphics_device == NULL) return 0;
    return g_graphics_device->bpp;
}

// Color utilities
uint32_t graphics_rgb_to_color(uint8_t r, uint8_t g, uint8_t b) {
    return GRAPHICS_RGB(r, g, b);
}

void graphics_color_to_rgb(uint32_t color, uint8_t *r, uint8_t *g, uint8_t *b) {
    if (r != NULL) *r = (color >> 16) & 0xFF;
    if (g != NULL) *g = (color >> 8) & 0xFF;
    if (b != NULL) *b = color & 0xFF;
}

// Architecture-specific implementations are in separate files:
// - graphics_vesa.c for x86_64 VESA VBE
// - graphics_framebuffer.c for ARM64/RISC-V64 framebuffer

// Text mode implementation (fallback)
static graphics_device_t text_device = {
    .width = 80,
    .height = 25,
    .bpp = 8,
    .mode = GRAPHICS_MODE_TEXT,
    .framebuffer = NULL,
    .framebuffer_size = 0,
    .pitch = 0,
};

static bool text_mode_init(graphics_device_t *dev) {
    // Text mode uses existing VGA system
    return true;
}

static void text_mode_cleanup(graphics_device_t *dev) {
    (void)dev;
}

static void text_mode_clear(graphics_device_t *dev, uint32_t color) {
    (void)dev; (void)color;
    // Will be handled by VGA
}

static void text_mode_putpixel(graphics_device_t *dev, int32_t x, int32_t y, uint32_t color) {
    (void)dev; (void)x; (void)y; (void)color;
    // Text mode doesn't support pixel-level drawing
}

static uint32_t text_mode_getpixel(graphics_device_t *dev, int32_t x, int32_t y) {
    (void)dev; (void)x; (void)y;
    return 0;
}

static void text_mode_fillrect(graphics_device_t *dev, graphics_rect_t rect, uint32_t color) {
    (void)dev; (void)rect; (void)color;
}

static void text_mode_drawrect(graphics_device_t *dev, graphics_rect_t rect, uint32_t color) {
    (void)dev; (void)rect; (void)color;
}

static void text_mode_drawline(graphics_device_t *dev, graphics_point_t p1, graphics_point_t p2, uint32_t color) {
    (void)dev; (void)p1; (void)p2; (void)color;
}

static void text_mode_flush(graphics_device_t *dev) {
    (void)dev;
}

graphics_device_t *graphics_init_text(void) {
    text_device.init = text_mode_init;
    text_device.cleanup = text_mode_cleanup;
    text_device.clear = text_mode_clear;
    text_device.putpixel = text_mode_putpixel;
    text_device.getpixel = text_mode_getpixel;
    text_device.fillrect = text_mode_fillrect;
    text_device.drawrect = text_mode_drawrect;
    text_device.drawline = text_mode_drawline;
    text_device.flush = text_mode_flush;

    if (text_device.init(&text_device)) {
        return &text_device;
    }
    return NULL;
}
