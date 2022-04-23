#include <stdarg.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

int fprintf(FILE *f,const char *format,...) {
  int n;
  va_list arg_ptr;
  va_start(arg_ptr,format);
  n=vfprintf(f,format,arg_ptr);
  va_end(arg_ptr);
  return n;
}
