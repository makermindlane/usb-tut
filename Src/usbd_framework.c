#include "usbd_framework.h"

void usbd_init() {
    init_gpio_pins();
    init_core();
    connect();
}
