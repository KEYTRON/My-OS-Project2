// printf.h — упрощённый printf (без float, без сложных спецификаторов)
#ifndef PRINTF_H
#define PRINTF_H

#include <stdarg.h>
#include <stddef.h>

void printf(const char* format, ...);

#endif // PRINTF_H