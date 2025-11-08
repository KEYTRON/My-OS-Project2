// graphics_primitives.h - Advanced graphics primitives
//
// Provides enhanced drawing functions for complex shapes and effects
// Built on top of the basic graphics API

#ifndef GRAPHICS_PRIMITIVES_H
#define GRAPHICS_PRIMITIVES_H

#include <stdint.h>
#include <stdbool.h>
#include "graphics.h"

// Triangle structure
typedef struct {
    graphics_point_t p1;
    graphics_point_t p2;
    graphics_point_t p3;
} graphics_triangle_t;

// Polygon structure (simple convex polygon)
typedef struct {
    graphics_point_t *points;
    uint32_t num_points;
} graphics_polygon_t;

// Gradient directions
typedef enum {
    GRADIENT_HORIZONTAL,
    GRADIENT_VERTICAL,
    GRADIENT_DIAGONAL,
} graphics_gradient_direction_t;

// ========================================
// Advanced Drawing Functions
// ========================================

// Draw filled circle (using Midpoint Circle Algorithm)
void graphics_fillcircle_adv(graphics_point_t center, uint32_t radius, uint32_t color);

// Draw triangle (filled)
void graphics_filltriangle(graphics_triangle_t tri, uint32_t color);

// Draw triangle outline
void graphics_drawtriangle(graphics_triangle_t tri, uint32_t color);

// Draw polygon outline
void graphics_drawpolygon(graphics_polygon_t *poly, uint32_t color);

// Draw filled polygon (simple convex)
void graphics_fillpolygon(graphics_polygon_t *poly, uint32_t color);

// Draw gradient rectangle (horizontal or vertical)
void graphics_gradient_rect(graphics_rect_t rect, uint32_t color1, uint32_t color2,
                            graphics_gradient_direction_t direction);

// Draw rounded rectangle
void graphics_rounded_rect(graphics_rect_t rect, uint32_t radius, uint32_t color, bool filled);

// Draw anti-aliased line (Xiaolin Wu algorithm)
void graphics_line_aa(graphics_point_t p1, graphics_point_t p2, uint32_t color);

// ========================================
// Text Rendering Extensions
// ========================================

// Draw text with centered alignment
void graphics_draw_text_centered(uint32_t x, uint32_t y, const char *text,
                                 uint32_t fg_color, uint32_t bg_color);

// Draw text with right alignment
void graphics_draw_text_right(uint32_t x, uint32_t y, const char *text,
                              uint32_t fg_color, uint32_t bg_color);

// Draw text with shadow effect
void graphics_draw_text_shadow(uint32_t x, uint32_t y, const char *text,
                               uint32_t fg_color, uint32_t shadow_color);

// ========================================
// Drawing Utilities
// ========================================

// Interpolate between two colors
uint32_t graphics_interpolate_color(uint32_t color1, uint32_t color2, float factor);

// Get brightness of color (0.0 - 1.0)
float graphics_get_color_brightness(uint32_t color);

// Create color from HSV
uint32_t graphics_hsv_to_rgb(float h, float s, float v);

#endif // GRAPHICS_PRIMITIVES_H
