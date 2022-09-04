#include "usbd_driver.h"

void init_gpio_pins() {
    // Enable the clock for GPIOA.
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN);

    // Set alternate function 10 for PA11 (data minus) PA12 (data plus).
    MODIFY_REG(GPIOA->AFR[1], GPIO_AFRH_AFSEL11 | GPIO_AFRH_AFSEL12,
               _VAL2FLD(GPIO_AFRH_AFSEL11, 0xa) | _VAL2FLD(GPIO_AFRH_AFSEL12, 0xa));

    // Configure pins to work in above set alternate function mode.
    MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODER11 | GPIO_MODER_MODER12,
               _VAL2FLD(GPIO_MODER_MODER11, 0x2) | _VAL2FLD(GPIO_MODER_MODER12, 0x2));
}
