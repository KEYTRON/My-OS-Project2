// graphics.h - Cross-platform graphics abstraction layer
// Provides a unified graphics API for all supported architectures

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>
#include <stdbool.h>

// Graphics modes and color definitions
#define GRAPHICS_RGB(r, g, b)   (((uint32_t)(r) << 16) | ((uint32_t)(g) << 8) | (uint32_t)(b))
#define GRAPHICS_RGBA(r, g, b, a) (((uint32_t)(a) << 24) | ((uint32_t)(r) << 16) | ((uint32_t)(g) << 8) | (uint32_t)(b))

// Common colors
#define GRAPHICS_COLOR_BLACK    GRAPHICS_RGB(0, 0, 0)
#define GRAPHICS_COLOR_WHITE    GRAPHICS_RGB(255, 255, 255)
#define GRAPHICS_COLOR_RED      GRAPHICS_RGB(255, 0, 0)
#define GRAPHICS_COLOR_GREEN    GRAPHICS_RGB(0, 255, 0)
#define GRAPHICS_COLOR_BLUE     GRAPHICS_RGB(0, 0, 255)
#define GRAPHICS_COLOR_CYAN     GRAPHICS_RGB(0, 255, 255)
#define GRAPHICS_COLOR_MAGENTA  GRAPHICS_RGB(255, 0, 255)
#define GRAPHICS_COLOR_YELLOW   GRAPHICS_RGB(255, 255, 0)
#define GRAPHICS_COLOR_GRAY     GRAPHICS_RGB(128, 128, 128)

// Graphics driver modes
typedef enum {
    GRAPHICS_MODE_TEXT,      // Text-only (80x25)
    GRAPHICS_MODE_FB_32,     // Framebuffer 32-bit
    GRAPHICS_MODE_FB_16,     // Framebuffer 16-bit
    GRAPHICS_MODE_VESA,      // VESA VBE
    GRAPHICS_MODE_NONE       // No graphics
} graphics_mode_t;

// Point structure
typedef struct {
    int32_t x;
    int32_t y;
} graphics_point_t;

// Rectangle structure
typedef struct {
    int32_t x;
    int32_t y;
    uint32_t width;
    uint32_t height;
} graphics_rect_t;

// Graphics device structure
typedef struct graphics_device {
    // Display properties
    uint32_t width;           // Width in pixels (or columns for text)
    uint32_t height;          // Height in pixels (or rows for text)
    uint32_t bpp;             // Bits per pixel
    graphics_mode_t mode;     // Current graphics mode

    // Framebuffer info
    void *framebuffer;        // Pointer to framebuffer memory
    uint32_t framebuffer_size; // Total framebuffer size in bytes
    uint32_t pitch;           // Bytes per scanline

    // Driver functions
    bool (*init)(struct graphics_device *dev);
    void (*cleanup)(struct graphics_device *dev);
    void (*clear)(struct graphics_device *dev, uint32_t color);
    void (*putpixel)(struct graphics_device *dev, int32_t x, int32_t y, uint32_t color);
    uint32_t (*getpixel)(struct graphics_device *dev, int32_t x, int32_t y);
    void (*fillrect)(struct graphics_device *dev, graphics_rect_t rect, uint32_t color);
    void (*drawrect)(struct graphics_device *dev, graphics_rect_t rect, uint32_t color);
    void (*drawline)(struct graphics_device *dev, graphics_point_t p1, graphics_point_t p2, uint32_t color);
    void (*flush)(struct graphics_device *dev);
} graphics_device_t;

// Global graphics device
extern graphics_device_t *g_graphics_device;

// Core graphics API
graphics_device_t *graphics_init(void);
void graphics_cleanup(void);

// Drawing functions
void graphics_clear(uint32_t color);
void graphics_putpixel(int32_t x, int32_t y, uint32_t color);
uint32_t graphics_getpixel(int32_t x, int32_t y);
void graphics_fillrect(graphics_rect_t rect, uint32_t color);
void graphics_drawrect(graphics_rect_t rect, uint32_t color);
void graphics_drawline(graphics_point_t p1, graphics_point_t p2, uint32_t color);
void graphics_drawcircle(graphics_point_t center, uint32_t radius, uint32_t color);
void graphics_fillcircle(graphics_point_t center, uint32_t radius, uint32_t color);

// Text rendering (for text mode)
void graphics_putchar(int32_t x, int32_t y, char c, uint32_t fg, uint32_t bg);
void graphics_putstring(int32_t x, int32_t y, const char *str, uint32_t fg, uint32_t bg);

// Utility functions
void graphics_flush(void);
graphics_mode_t graphics_get_mode(void);
uint32_t graphics_get_width(void);
uint32_t graphics_get_height(void);
uint32_t graphics_get_bpp(void);

// Color utilities
uint32_t graphics_rgb_to_color(uint8_t r, uint8_t g, uint8_t b);
void graphics_color_to_rgb(uint32_t color, uint8_t *r, uint8_t *g, uint8_t *b);

#endif // GRAPHICS_H
