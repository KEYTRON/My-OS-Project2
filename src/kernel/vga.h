#ifndef VGA_H
#define VGA_H
// Очистка текстового режима VGA
void clear_screen();
void put_char(char c);
void print_string(const char *str);

// Графический режим 320x200x256
void put_pixel(int x, int y, unsigned char color);
void clear_graphics(unsigned char color);

#endif
