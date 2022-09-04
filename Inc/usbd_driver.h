#pragma once

#include "stm32f4xx.h"

#define USB_OTG_HS_GLOBAL \
    ((USB_OTG_GlobalTypeDef *)(USB_OTG_HS_PERIPH_BASE + USB_OTG_GLOBAL_BASE))
#define USB_OTG_HS_DEVICE \
    ((USB_OTG_DeviceTypeDef *)(USB_OTG_HS_PERIPH_BASE + USB_OTG_DEVICE_BASE))

// Power and clock gating control register
#define USB_OTG_HS_PCGCCTL \
    ((__IO uint32_t *)(USB_OTG_HS_PERIPH_BASE + USB_OTG_PCGCCTL_BASE))
