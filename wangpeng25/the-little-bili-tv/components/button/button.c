#include "button.h"
#include "esp_log.h"

#define GPIO_INPUT_IO_0     0
#define GPIO_INPUT_IO_1     35
#define GPIO_INPUT_IO_2     33
#define GPIO_INPUT_IO_3     26
#define GPIO_INPUT_IO_4     14
#define GPIO_INPUT_PIN_SEL  ((1ULL<<GPIO_INPUT_IO_0) | (1ULL<<GPIO_INPUT_IO_1)| (1ULL<<GPIO_INPUT_IO_2)| (1ULL<<GPIO_INPUT_IO_3)| (1ULL<<GPIO_INPUT_IO_4))
#define ESP_INTR_FLAG_DEFAULT 0
void button_init()
{
    gpio_config_t io_conf;
    //interrupt of rising edge
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE ;
    //bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    //set as input mode    
    io_conf.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
}