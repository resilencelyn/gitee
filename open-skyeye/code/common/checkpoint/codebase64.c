#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

const static char Code[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const static uint32_t DCode[] = {
    0, 0, 0, 0, 0, 0, 0, 0,             // 00 - 07
    0, 0, 0, 0, 0, 0, 0, 0,             // 08 - 15
    0, 0, 0, 0, 0, 0, 0, 0,             // 16 - 23
    0, 0, 0, 0, 0, 0, 0, 0,             // 24 - 31
    0, 0, 0, 0, 0, 0, 0, 0,             // 32 - 39
    0, 0, 0, 62, 0, 0, 0, 63,           // 40 - 47
    52, 53, 54, 55, 56, 57, 58, 59,     // 48 - 55
    60, 61, 0, 0, 0, 65, 0, 0,          // 56 - 63
    0, 0, 1, 2, 3, 4, 5, 6,             // 64 - 71
    7, 8, 9, 10, 11, 12, 13, 14,        // 72 - 79
    15, 16, 17, 18, 19, 20, 21, 22,     // 80 - 87
    23, 24, 25, 0, 0, 0, 0, 0,          // 88 - 95
    0, 26, 27, 28, 29, 30, 31, 32,      // 96 - 103
    33, 34, 35, 36, 37, 38, 39, 40,     // 104 - 111
    41, 42, 43, 44, 45, 46, 47, 48,     // 112 - 119
    49, 50, 51, 0, 0, 0, 0, 0,          // 120 - 127
};

char *encodeBase64(const void *src, size_t size)
{
    uint8_t *data = (uint8_t *) src;

    size_t needsize = (size / 3) * 4 + 4;
    char *buf = malloc(needsize * sizeof (char));

    size_t j, leftsize = size;

    for (j = 0; leftsize > 2; leftsize -= 3, data += 3)
    {
        buf[j++] = Code[data[0] >> 2];
        buf[j++] = Code[((data[0] << 4) & 0x30) | (data[1] >> 4)];
        buf[j++] = Code[((data[1] << 2) & 0x3c) | (data[2] >> 6)];
        buf[j++] = Code[data[2] & 0x3f];
    }
    switch (leftsize)
    {
        case 1:
            buf[j++] = Code[data[0] >> 2];
            buf[j++] = Code[(data[0] << 4) & 0x30];
            buf[j++] = '=';
            break;
        case 2:
            buf[j++] = Code[data[0] >> 2];
            buf[j++] = Code[((data[0] << 4) & 0x30) | (data[1] >> 4)];
            buf[j++] = Code[(data[1] << 2) & 0x3c];
            buf[j++] = '=';
            buf[j++] = '=';
            break;
        default:
            break;
    }
    buf[j] = '\0';
    return buf;
}

size_t decodeBase64(const char *src, void **data)
{
    uint8_t *srcb = (uint8_t *) src;

    size_t size = strlen(src);
    char *buf = malloc(sizeof (char) * (size / 4 * 3 + (size % 4) / 2));

    /* calculate '=' number */
    size_t leftcharnum;

    for (leftcharnum = 0; src[size - 1 - leftcharnum] == '='; leftcharnum++)
    {
        /* do nothing */ ;
    }

    size_t i, leftsize = size - leftcharnum;
    uint32_t word;

    for (i = 0; leftsize > 3; leftsize -= 4, srcb += 4)
    {
        word = 0;
        word = (word | DCode[srcb[0]]) << 6;
        word = (word | DCode[srcb[1]]) << 6;
        word = (word | DCode[srcb[2]]) << 6;
        word = (word | DCode[srcb[3]]);
        buf[i++] = (word & 0xff0000) >> 16;
        buf[i++] = (word & 0xff00) >> 8;
        buf[i++] = word & 0xff;
    }
    switch (leftsize)
    {
        case 3:
            word = 0;
            word = (word | DCode[srcb[0]]) << 6;
            word = (word | DCode[srcb[1]]) << 6;
            word = (word | DCode[srcb[2]]) << 6;
            buf[i++] = (word & 0xff0000) >> 16;
            buf[i++] = (word & 0xff00) >> 8;
            if (leftcharnum != 2)
            {
                fprintf(stderr, "base64 source length is error (left 3 char but not have 2 '='\n");
            }
            break;
        case 2:
            word = 0;
            word = (word | DCode[srcb[0]]) << 6;
            word = (word | DCode[srcb[1]]) << 6;
            word = word << 6;
            buf[i++] = (word & 0xff0000) >> 16;
            if (leftcharnum != 1)
            {
                fprintf(stderr, "base64 source length is error (left 2 char but not have 1 '='\n");
            }
            break;
        case 1:
            fprintf(stderr, "base64 source length is error!\n");
            break;
        default:
            break;
    }
    *data = (void *) buf;
    return i;
}
