#ifndef INSANE_ASPRINTF_H
#define INSANE_ASPRINTF_H
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#ifndef __cplusplus
#include <stdarg.h>
#else
#include <cstdarg>
extern "C"
{
#endif

#define insane_free(ptr) { free(ptr); ptr = NULL; }

extern int asprintf  (char **strp, const char *fmt, /*args*/ ...);
extern int vasprintf (char **strp, const char *fmt, va_list ap);
extern int asnprintf (char **strp, size_t str_m, const char *fmt, /*args*/ ...);
extern int vasnprintf(char **strp, size_t str_m, const char *fmt, va_list ap);

#ifdef __cplusplus
}
#endif

#endif
