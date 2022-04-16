#include "asnprintf.h"

int asprintf(char **strp, const char *fmt, ...)
{
    int r;
    va_list ap;

    va_start(ap, fmt);
    r = vasprintf(strp, fmt, ap);
    va_end(ap);
    return (r);
}

int asnprintf(char **strp, size_t str_m, const char *fmt, ...)
{
    int r;
    va_list ap;

    va_start(ap, fmt);
    r = vasnprintf(strp, str_m, fmt, ap);
    va_end(ap);
    return (r);
}

int vasprintf(char **strp, const char *fmt, va_list ap)
{
    int r = -1, size;

    va_list ap2;

    va_copy(ap2, ap);

    *strp = NULL;
    size = vsnprintf(0, 0, fmt, ap2);

    if ((size >= 0) && (size < INT_MAX))
    {
        *strp = (char *) malloc(size + 1);      //+1 for null
        if (*strp)
        {
            r = vsnprintf(*strp, size + 1, fmt, ap);    //+1 for null
            if ((r < 0) || (r > size))
            {
                insane_free(*strp);
                r = -1;
            }
        }
    } else
    {
        *strp = 0;
    }

    va_end(ap2);

    return (r);
}

int vasnprintf(char **strp, size_t str_m, const char *fmt, va_list ap)
{
    int r = -1, size;

    va_list ap2;

    va_copy(ap2, ap);

    *strp = NULL;
    size = vsnprintf(0, 0, fmt, ap2);

    if ((size_t) size + 1 >= str_m)
        size = str_m;
    if ((size >= 0) && (size < INT_MAX))
    {
        *strp = (char *) malloc(size + 1);      //+1 for null
        if (*strp)
        {
            r = vsnprintf(*strp, size + 1, fmt, ap);    //+1 for null
            if ((r < 0) || (r > size))
            {
                insane_free(*strp);
                r = -1;
            }
        }
    } else
    {
        *strp = 0;
    }

    va_end(ap2);

    return (r);
}
