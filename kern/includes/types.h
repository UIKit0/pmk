#ifdef __cplusplus
extern "C" {
#endif

#ifndef TYPES_H
#define TYPES_H

#if !defined(__cplusplus)
#include <stdbool.h>
#endif

// Built-ins
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

#include <limits.h>

// Standard libraries
#include "includes/stdlib.h"

// Object types
#include "types/hashmap.h"
#include "types/list.h"
#include "types/ordered_array.h"

// These tell gcc how to optimise branches since it's stupid
#define likely(x)    __builtin_expect(!!(x), 1)
#define unlikely(x)  __builtin_expect(!!(x), 0)

// Logging
#include "pexpert/logging.h"
#define KDEBUG(...) pexpert_log(kLogLevelDebug, __VA_ARGS__)
#define KINFO(...) pexpert_log(kLogLevelInfo, __VA_ARGS__)
#define KSUCCESS(...) pexpert_log(kLogLevelSuccess, __VA_ARGS__)
#define KWARNING(...) pexpert_log(kLogLevelWarning, __VA_ARGS__)
#define KERROR(...) pexpert_log(kLogLevelError, __VA_ARGS__)
#define KCRITICAL(...) pexpert_log(kLogLevelCritical, __VA_ARGS__)

// Attributes for functions
#define __used	__attribute__((__used__))

extern void pexpert_panic(const char *file, const int line, const char *message);

//#define PANIC(msg) panic(msg, __FILE__, __LINE__);
#define ASSERT(b) ((b) ? (void)0 : pexpert_panic(__FILE__, __LINE__, #b))

// This can be used to swap words and longwords
#define ENDIAN_DWORD_SWAP(x) ((x >> 24) & 0xFF) | ((x << 8) & 0xFF0000) | ((x >> 8) & 0xFF00) | ((x << 24) & 0xFF000000)
#define ENDIAN_WORD_SWAP(x) ((x & 0xFF) << 0x08) | ((x & 0xFF00) >> 0x08)


// Macros for defining linkage of functions
#ifdef __cplusplus
#define C_FUNCTION extern "C"
#else
#define C_FUNCTION
#endif

#endif

#ifdef __cplusplus
}
#endif