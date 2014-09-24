#ifndef STDLIB_PRINTF_H
#define STDLIB_PRINTF_H
/**
 * Defines prototypes for various printf-like functions, which will produce the
 * output string into the given string buffer. This is used by the kernel to
 * format strings of all sorts.
 */

#include <stdarg.h>
#include <types.h>

int snprintf(char* str, size_t size, const char* format, ...);
int sprintf(char* str, const char* format, ...);
int vsprintf(char* str, const char* format, va_list ap);
int vsnprintf(char* str, size_t size, const char* format, va_list ap);

#endif