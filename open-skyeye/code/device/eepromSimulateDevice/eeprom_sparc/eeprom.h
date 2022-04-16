#ifndef __EEPROM_CAT24C64_
#define __EEPROM_CAT24C64_


typedef  struct EEPROM_STATE{
    conf_object_t *obj;
    memory_space_intf* image_space;
    conf_object_t* image;
    int KeyAddr1;
    int KeyAddr2;
    int KeyAddr3;
    int chip_num;
    int write_chip_flag;

    FILE* fp[3];
    char *filename1;
    char *filename2;
    char *filename3;
}eeprom_dev;

#define CHIP1_KEY_ADDR1 0x15554
#define CHIP1_KEY_ADDR2 0x2aaa8

#define CHIP2_KEY_ADDR1 0x95554
#define CHIP2_KEY_ADDR2 0xaaaa8

#define CHIP3_KEY_ADDR1 0x115554
#define CHIP3_KEY_ADDR2 0x12aaa8

#define KEY1_VALUE 0xaaaaaaaa
#define KEY2_VALUE 0x55555555
#define KEY3_VALUE 0xa0a0a0a0
#define EEPROM_CHIP_1 0
#define EEPROM_CHIP_2 1
#define EEPROM_CHIP_3 2
#endif
