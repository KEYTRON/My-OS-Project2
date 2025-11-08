// graphics_primitives.c - Advanced graphics primitives implementation

#include "graphics_primitives.h"
#include "graphics.h"
#include "graphics_font.h"
#include <stdint.h>
#include <stddef.h>
#include <limits.h>

// Simple integer square root for freestanding environment
static uint32_t isqrt(uint32_t n) {
    if (n == 0) return 0;
    uint32_t x = n;
    uint32_t y = (x + 1) >> 1;
    while (y < x) {
        x = y;
        y = (x + n / x) >> 1;
    }
    return x;
}

// ========================================
// Advanced Drawing Implementation
// ========================================

// Filled circle using scanline algorithm
void graphics_fillcircle_adv(graphics_point_t center, uint32_t radius, uint32_t color) {
    if (g_graphics_device == NULL || g_graphics_device->putpixel == NULL) return;

    int32_t r = (int32_t)radius;
    int32_t cx = center.x;
    int32_t cy = center.y;

    // Use scanline approach for better fill
    for (int32_t y = -r; y <= r; y++) {
        uint32_t r_sq = (uint32_t)(r * r);
        uint32_t y_sq = (uint32_t)(y * y);
        if (y_sq >= r_sq) continue;
        uint32_t x_width = isqrt(r_sq - y_sq);
        for (int32_t x = -(int32_t)x_width; x <= (int32_t)x_width; x++) {
            graphics_putpixel(cx + x, cy + y, color);
        }
    }
}

// Draw filled triangle using barycentric coordinates
static bool point_in_triangle(graphics_point_t p, graphics_triangle_t tri) {
    int32_t x = p.x, y = p.y;
    int32_t x1 = tri.p1.x, y1 = tri.p1.y;
    int32_t x2 = tri.p2.x, y2 = tri.p2.y;
    int32_t x3 = tri.p3.x, y3 = tri.p3.y;

    int32_t denom = (y2 - y3) * (x1 - x3) + (x3 - x2) * (y1 - y3);
    if (denom == 0) return false;

    int32_t a = (y2 - y3) * (x - x3) + (x3 - x2) * (y - y3);
    int32_t b = (y3 - y1) * (x - x3) + (x1 - x3) * (y - y3);
    int32_t c = denom - a - b;

    if (denom > 0) {
        return a >= 0 && b >= 0 && c >= 0;
    } else {
        return a <= 0 && b <= 0 && c <= 0;
    }
}

void graphics_filltriangle(graphics_triangle_t tri, uint32_t color) {
    if (g_graphics_device == NULL) return;

    // Find bounding box
    int32_t min_x = tri.p1.x;
    int32_t max_x = tri.p1.x;
    int32_t min_y = tri.p1.y;
    int32_t max_y = tri.p1.y;

    if (tri.p2.x < min_x) min_x = tri.p2.x;
    if (tri.p2.x > max_x) max_x = tri.p2.x;
    if (tri.p2.y < min_y) min_y = tri.p2.y;
    if (tri.p2.y > max_y) max_y = tri.p2.y;

    if (tri.p3.x < min_x) min_x = tri.p3.x;
    if (tri.p3.x > max_x) max_x = tri.p3.x;
    if (tri.p3.y < min_y) min_y = tri.p3.y;
    if (tri.p3.y > max_y) max_y = tri.p3.y;

    // Clamp to screen bounds
    if (min_x < 0) min_x = 0;
    if (max_x >= (int32_t)g_graphics_device->width) max_x = g_graphics_device->width - 1;
    if (min_y < 0) min_y = 0;
    if (max_y >= (int32_t)g_graphics_device->height) max_y = g_graphics_device->height - 1;

    // Rasterize triangle
    for (int32_t y = min_y; y <= max_y; y++) {
        for (int32_t x = min_x; x <= max_x; x++) {
            graphics_point_t p = {.x = x, .y = y};
            if (point_in_triangle(p, tri)) {
                graphics_putpixel(x, y, color);
            }
        }
    }
}

// Draw triangle outline
void graphics_drawtriangle(graphics_triangle_t tri, uint32_t color) {
    graphics_drawline(tri.p1, tri.p2, color);
    graphics_drawline(tri.p2, tri.p3, color);
    graphics_drawline(tri.p3, tri.p1, color);
}

// Draw polygon outline
void graphics_drawpolygon(graphics_polygon_t *poly, uint32_t color) {
    if (poly == NULL || poly->points == NULL || poly->num_points < 2) return;

    for (uint32_t i = 0; i < poly->num_points; i++) {
        uint32_t next = (i + 1) % poly->num_points;
        graphics_drawline(poly->points[i], poly->points[next], color);
    }
}

// Simple convex polygon fill (using scanline algorithm)
void graphics_fillpolygon(graphics_polygon_t *poly, uint32_t color) {
    if (poly == NULL || poly->points == NULL || poly->num_points < 3) return;

    // For now, use simple approach: draw horizontal scanlines
    // Find bounding box
    int32_t min_y = poly->points[0].y;
    int32_t max_y = poly->points[0].y;

    for (uint32_t i = 1; i < poly->num_points; i++) {
        if (poly->points[i].y < min_y) min_y = poly->points[i].y;
        if (poly->points[i].y > max_y) max_y = poly->points[i].y;
    }

    if (min_y < 0) min_y = 0;
    if (max_y >= (int32_t)g_graphics_device->height) max_y = g_graphics_device->height - 1;

    // For each scanline, find intersections and fill
    for (int32_t y = min_y; y <= max_y; y++) {
        graphics_point_t scanline = {.x = 0, .y = y};

        // Simple fill: draw horizontal line across polygon at this y
        // This is a simplified algorithm - full implementation would be more complex
        int32_t min_x = INT32_MAX;
        int32_t max_x = INT32_MIN;

        // Find min/max x at this scanline by checking all edges
        for (uint32_t i = 0; i < poly->num_points; i++) {
            uint32_t next = (i + 1) % poly->num_points;
            int32_t y1 = poly->points[i].y;
            int32_t y2 = poly->points[next].y;
            int32_t x1 = poly->points[i].x;
            int32_t x2 = poly->points[next].x;

            // Check if edge crosses scanline
            if ((y <= y1 && y >= y2) || (y <= y2 && y >= y1)) {
                // Linear interpolation to find x at this y
                if (y1 != y2) {
                    int32_t x_intersect = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
                    if (x_intersect < min_x) min_x = x_intersect;
                    if (x_intersect > max_x) max_x = x_intersect;
                }
            }
        }

        // Draw horizontal line if we found intersections
        if (min_x != INT32_MAX && max_x != INT32_MIN && min_x <= max_x) {
            for (int32_t x = min_x; x <= max_x; x++) {
                if (x >= 0 && x < (int32_t)g_graphics_device->width) {
                    graphics_putpixel(x, y, color);
                }
            }
        }
    }
}

// Gradient rectangle
void graphics_gradient_rect(graphics_rect_t rect, uint32_t color1, uint32_t color2,
                            graphics_gradient_direction_t direction) {
    if (g_graphics_device == NULL) return;

    uint8_t r1, g1, b1, r2, g2, b2;
    graphics_color_to_rgb(color1, &r1, &g1, &b1);
    graphics_color_to_rgb(color2, &r2, &g2, &b2);

    if (direction == GRADIENT_HORIZONTAL) {
        for (int32_t x = rect.x; x < rect.x + (int32_t)rect.width; x++) {
            // Interpolate color
            float t = (float)(x - rect.x) / (float)rect.width;
            uint8_t r = (uint8_t)(r1 + (r2 - r1) * t);
            uint8_t g = (uint8_t)(g1 + (g2 - g1) * t);
            uint8_t b = (uint8_t)(b1 + (b2 - b1) * t);
            uint32_t grad_color = graphics_rgb_to_color(r, g, b);

            graphics_rect_t col = {.x = x, .y = rect.y, .width = 1, .height = rect.height};
            graphics_fillrect(col, grad_color);
        }
    } else if (direction == GRADIENT_VERTICAL) {
        for (int32_t y = rect.y; y < rect.y + (int32_t)rect.height; y++) {
            // Interpolate color
            float t = (float)(y - rect.y) / (float)rect.height;
            uint8_t r = (uint8_t)(r1 + (r2 - r1) * t);
            uint8_t g = (uint8_t)(g1 + (g2 - g1) * t);
            uint8_t b = (uint8_t)(b1 + (b2 - b1) * t);
            uint32_t grad_color = graphics_rgb_to_color(r, g, b);

            graphics_rect_t row = {.x = rect.x, .y = y, .width = rect.width, .height = 1};
            graphics_fillrect(row, grad_color);
        }
    }
}

// Rounded rectangle
void graphics_rounded_rect(graphics_rect_t rect, uint32_t radius, uint32_t color, bool filled) {
    if (g_graphics_device == NULL) return;

    uint32_t r = (radius < rect.width / 2) ? radius : rect.width / 2;
    r = (r < rect.height / 2) ? r : rect.height / 2;

    // Draw straight edges
    if (filled) {
        // Top and bottom edges
        graphics_rect_t top = {
            .x = rect.x + (int32_t)r, .y = rect.y,
            .width = rect.width - 2 * r, .height = rect.height
        };
        graphics_fillrect(top, color);

        // Left and right edges
        graphics_rect_t left = {
            .x = rect.x, .y = rect.y + (int32_t)r,
            .width = r, .height = rect.height - 2 * r
        };
        graphics_fillrect(left, color);

        graphics_rect_t right = {
            .x = rect.x + (int32_t)rect.width - (int32_t)r, .y = rect.y + (int32_t)r,
            .width = r, .height = rect.height - 2 * r
        };
        graphics_fillrect(right, color);

        // Corner circles
        graphics_point_t tl = {.x = rect.x + (int32_t)r, .y = rect.y + (int32_t)r};
        graphics_fillcircle(tl, r, color);
        graphics_point_t tr = {.x = rect.x + (int32_t)rect.width - (int32_t)r, .y = rect.y + (int32_t)r};
        graphics_fillcircle(tr, r, color);
        graphics_point_t bl = {.x = rect.x + (int32_t)r, .y = rect.y + (int32_t)rect.height - (int32_t)r};
        graphics_fillcircle(bl, r, color);
        graphics_point_t br = {.x = rect.x + (int32_t)rect.width - (int32_t)r, .y = rect.y + (int32_t)rect.height - (int32_t)r};
        graphics_fillcircle(br, r, color);
    } else {
        // Draw outline version
        graphics_drawrect(rect, color);
        // TODO: Draw rounded corners as arcs
    }
}

// Anti-aliased line (simplified)
void graphics_line_aa(graphics_point_t p1, graphics_point_t p2, uint32_t color) {
    // For now, use standard Bresenham
    // Full anti-aliased implementation would use Xiaolin Wu algorithm
    graphics_drawline(p1, p2, color);
}

// ========================================
// Text Rendering Extensions
// ========================================

void graphics_draw_text_centered(uint32_t x, uint32_t y, const char *text,
                                 uint32_t fg_color, uint32_t bg_color) {
    if (text == NULL) return;

    uint8_t char_width = graphics_get_font_width();
    uint32_t text_width = 0;
    const char *p = text;
    while (*p && *p != '\n') {
        text_width += char_width;
        p++;
    }

    // Adjust x to center the text
    uint32_t centered_x = (x > text_width / 2) ? (x - text_width / 2) : x;
    graphics_draw_string_at(centered_x, y, text, fg_color, bg_color);
}

void graphics_draw_text_right(uint32_t x, uint32_t y, const char *text,
                              uint32_t fg_color, uint32_t bg_color) {
    if (text == NULL) return;

    uint8_t char_width = graphics_get_font_width();
    uint32_t text_width = 0;
    const char *p = text;
    while (*p && *p != '\n') {
        text_width += char_width;
        p++;
    }

    // Adjust x to right-align the text
    uint32_t right_x = (x > text_width) ? (x - text_width) : x;
    graphics_draw_string_at(right_x, y, text, fg_color, bg_color);
}

void graphics_draw_text_shadow(uint32_t x, uint32_t y, const char *text,
                               uint32_t fg_color, uint32_t shadow_color) {
    // Draw shadow offset
    graphics_draw_string_at(x + 2, y + 2, text, shadow_color, 0x000000);
    // Draw main text
    graphics_draw_string_at(x, y, text, fg_color, 0x000000);
}

// ========================================
// Color Utilities
// ========================================

uint32_t graphics_interpolate_color(uint32_t color1, uint32_t color2, float factor) {
    uint8_t r1, g1, b1, r2, g2, b2;
    graphics_color_to_rgb(color1, &r1, &g1, &b1);
    graphics_color_to_rgb(color2, &r2, &g2, &b2);

    uint8_t r = (uint8_t)(r1 + (r2 - r1) * factor);
    uint8_t g = (uint8_t)(g1 + (g2 - g1) * factor);
    uint8_t b = (uint8_t)(b1 + (b2 - b1) * factor);

    return graphics_rgb_to_color(r, g, b);
}

float graphics_get_color_brightness(uint32_t color) {
    uint8_t r, g, b;
    graphics_color_to_rgb(color, &r, &g, &b);
    // Standard brightness formula
    return (0.299f * r + 0.587f * g + 0.114f * b) / 255.0f;
}

uint32_t graphics_hsv_to_rgb(float h, float s, float v) {
    // Ensure values are in proper ranges
    while (h < 0) h += 360;
    while (h >= 360) h -= 360;
    if (s < 0) s = 0;
    if (s > 1) s = 1;
    if (v < 0) v = 0;
    if (v > 1) v = 1;

    float c = v * s;

    // Manual fmod and fabs for freestanding environment
    float h_prime = h / 60.0f;
    int32_t h_prime_int = (int32_t)h_prime;
    float h_prime_frac = h_prime - h_prime_int;
    float h_mod = h_prime_frac;
    if (h_mod < 0) h_mod = -h_mod;
    if (h_mod > 1) h_mod = 2 - h_mod;

    float x = c * (1 - h_mod);
    float m = v - c;

    float r, g, b;

    if (h < 60) {
        r = c; g = x; b = 0;
    } else if (h < 120) {
        r = x; g = c; b = 0;
    } else if (h < 180) {
        r = 0; g = c; b = x;
    } else if (h < 240) {
        r = 0; g = x; b = c;
    } else if (h < 300) {
        r = x; g = 0; b = c;
    } else {
        r = c; g = 0; b = x;
    }

    uint8_t r_byte = (uint8_t)((r + m) * 255);
    uint8_t g_byte = (uint8_t)((g + m) * 255);
    uint8_t b_byte = (uint8_t)((b + m) * 255);

    return graphics_rgb_to_color(r_byte, g_byte, b_byte);
}
