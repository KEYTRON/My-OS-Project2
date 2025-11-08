// graphics_font.h - Simple bitmap font support for graphics system

#ifndef GRAPHICS_FONT_H
#define GRAPHICS_FONT_H

#include <stdint.h>
#include <stddef.h>

// Simple 8x8 bitmap font character
typedef struct {
    uint8_t bitmap[8];  // 8 bytes = 8 rows of 8 pixels
} graphics_font_char_t;

// Font structure
typedef struct {
    graphics_font_char_t chars[256];  // 256 ASCII characters
    uint8_t char_width;
    uint8_t char_height;
    const char *name;
} graphics_font_t;

// Get default 8x8 bitmap font
extern graphics_font_t graphics_default_font;

// Text rendering functions
void graphics_draw_char_at(int32_t x, int32_t y, char c, uint32_t fg_color, uint32_t bg_color);
void graphics_draw_string_at(int32_t x, int32_t y, const char *text, uint32_t fg_color, uint32_t bg_color);

// Get font dimensions
uint8_t graphics_get_font_width(void);
uint8_t graphics_get_font_height(void);

#endif // GRAPHICS_FONT_H
