#ifndef _CODEBASE64_H_
#define _CODEBASE64_H_

#ifdef __cplusplus
extern "C"
{
#endif

    char *encodeBase64(const void *src, size_t size);
    size_t decodeBase64(const char *src, void **data);

#ifdef __cplusplus
}
#endif

#endif
