#ifndef __SKYEYE_MM_H__
#define __SKYEYE_MM_H__
#include <stdlib.h>
#include <stdio.h>
#ifdef __cplusplus
 extern "C" {
#endif

//void *skyeye_mm(size_t sz);
void *skyeye_mm_orig(size_t sz, const char *file, const char *func, int line);
#define skyeye_mm(sz) skyeye_mm_orig((sz), (__FILE__), (__func__), (__LINE__))

//void *skyeye_mm_zero(size_t sz);
void *skyeye_mm_zero_orig(size_t sz, const char *file, const char *func, int line);
#define skyeye_mm_zero(sz) skyeye_mm_zero_orig((sz), (__FILE__), (__func__), (__LINE__))

//void *skyeye_realloc(void *ptr, size_t size);
void *skyeye_realloc_orig(void *ptr, size_t size, const char *file, const char *func, int line);
#define skyeye_realloc(ptr,sz) skyeye_realloc_orig((ptr), (sz), (__FILE__), (__func__), (__LINE__))

//char *skyeye_strdup(const char *s);
char *skyeye_strdup_orig(const char *s, const char *file, const char *func, int line);
#define skyeye_strdup(s) skyeye_strdup_orig((s), (__FILE__), (__func__), (__LINE__))

//void skyeye_free(void *p);
void skyeye_free_orig(void *p, const char *file, const char *func, int line);
#define skyeye_free(p) skyeye_free_orig((p), (__FILE__), (__func__), (__LINE__))

//void *skyeye_mm_align(size_t align, size_t sz);
void *skyeye_mm_align_orig(size_t align, size_t sz, const char *file, const char *func, int line);
#define skyeye_mm_align(align, sz) skyeye_mm_align_orig((align), (sz), (__FILE__), (__func__), (__LINE__))

//void skyeye_align_free(void *p);
void skyeye_align_free_orig(void *p, const char *file, const char *func, int line);
#define skyeye_align_free(p) skyeye_align_free_orig((p), (__FILE__), (__func__), (__LINE__))


/**
* IS_MM_TEST
* MM_TEST_LOG_INFO Apply memory and release function of logs.
* MM_TEST_LOG_RESULT The calculation results.
*/
#define IS_MM_TEST

#ifdef IS_MM_TEST
#define MM_TEST_LOG_INFO    "mm_info.log"
#define MM_TEST_LOG_RESULT  "mm_result.log"

typedef struct mm_info {
    size_t id;
    char *type;       //call function type.
    char *file;       //call function filename.
    char *func;       //call funciont name.
    size_t line;      //code line.
    unsigned long mm; //memory address.
    size_t sz;        //data size.
    struct mm_info *next;
}mm_info_t;

void mm_info_insert(const char *type, const char *file, const char *func, int line, unsigned long mm, size_t sz);
int  mm_info_search(mm_info_t *d);
size_t get_mm_size(void *p);
void mm_info_print(char *args);
void skyeye_save_mm_result();
void skyeye_save_mm_info();
void set_mm_save_log(int k);
#endif
#ifdef __cplusplus
}
#endif

#endif
