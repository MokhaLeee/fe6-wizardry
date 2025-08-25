#pragma once

#include "prelude.h"
#include <string.h>
#include <stdarg.h>

#ifndef __PRINTFLIKE
#define __PRINTFLIKE(__fmt,__varargs) __attribute__((__format__ (__printf__, __fmt, __varargs)))
#endif

/**
 * stdio
 */
struct io_handle;

typedef struct FILE {
	const struct io_handle *io;
} FILE;

struct io_handle {
	size_t (*write)(FILE *fp, const char *buf, size_t len);
	size_t (*read)(FILE *fp, char *buf, size_t len);
};

extern FILE __stdio_FILEs[3];

#define stdin  (&__stdio_FILEs[0])
#define stdout (&__stdio_FILEs[1])
#define stderr (&__stdio_FILEs[2])

void io_init(void);

/**
 * console
 */
int vsnprintf(char *str, size_t len, const char *fmt, va_list ap);
int vsprintf(char *str, const char *fmt, va_list ap);
int vfprintf(FILE *fp, const char *fmt, va_list ap);

int sprintf(char *str, const char *fmt, ...) __PRINTFLIKE(2, 3);
int snprintf(char *str, size_t len, const char *fmt, ...) __PRINTFLIKE(3, 4);
int fprintf(FILE *fp, const char *fmt, ...) __PRINTFLIKE(2, 3);
int printf(const char *fmt, ...) __PRINTFLIKE(1, 2);

/**
 * utils
 */
#define MGBA_LOG_FATAL 0
#define MGBA_LOG_ERROR 1
#define MGBA_LOG_WARN 2
#define MGBA_LOG_INFO 3
#define MGBA_LOG_DEBUG 4

extern int mgba_print_level;

void mgba_print(const char *buf);
bool mgba_open(void);
void mgba_close(void);

void nocashgba_print(const char *buf);

void kprintf_test(void);

/**
 * Always print to STDOUT regardless on debug config
 */
#define fmt_printf(format, ...) printf("(%s): "format, __func__, ##__VA_ARGS__)
