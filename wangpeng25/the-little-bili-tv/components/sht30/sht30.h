#ifndef _SHT30_
#define _SHT30_
typedef struct _sht30
{
    uint8_t temp;
    uint8_t humi;
}sht30_t;
int sht30_get_value();
void sht30_init(void);
#endif