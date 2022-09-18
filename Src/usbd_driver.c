#include "usbd_driver.h"

#include "data_typedefs.h"
#include "usb_standards.h"

static USB_OTG_GlobalTypeDef *usb_otg_global =
    ((USB_OTG_GlobalTypeDef *)(USB_OTG_FS_PERIPH_BASE + USB_OTG_GLOBAL_BASE));
static USB_OTG_DeviceTypeDef *usb_otg_device =
    ((USB_OTG_DeviceTypeDef *)(USB_OTG_FS_PERIPH_BASE + USB_OTG_DEVICE_BASE));

// Power and clock gating control register
__IO u32 *USB_OTG_FS_PCGCCTL =
    ((__IO u32 *)(USB_OTG_FS_PERIPH_BASE + USB_OTG_PCGCCTL_BASE));

static const u8 ENDPOINT_COUNT = 4;

//=================================================================================================
// Private Functions Prototypes
//=================================================================================================

static inline USB_OTG_INEndpointTypeDef *in_endpoint(const u8 endpoint_size);
static inline USB_OTG_OUTEndpointTypeDef *out_endpoint(const u8 endpoint_size);
static void usbrst_handler();
static void configure_endpoint0(const u8 endpoint_size);
static void configure_in_endpoint(const u8 endpoint_number,
                                  const UsbEndpointType endpoint_type,
                                  const u16 endpoint_size);

//=================================================================================================
// Public Functions
//=================================================================================================

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

void init_core() {
    // Enable usb core clock.
    SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_OTGFSEN);

    // Force usb to be in device mode and set turn around time.
    MODIFY_REG(usb_otg_global->GUSBCFG,
               USB_OTG_GUSBCFG_FDMOD | USB_OTG_GUSBCFG_PHYSEL | USB_OTG_GUSBCFG_TRDT,
               USB_OTG_GUSBCFG_FDMOD | USB_OTG_GUSBCFG_PHYSEL |
                   _VAL2FLD(USB_OTG_GUSBCFG_TRDT, 0x09));

    // Configure the device to run in full speed mode.
    MODIFY_REG(usb_otg_device->DCFG, USB_OTG_DCFG_DSPD, _VAL2FLD(USB_OTG_DCFG_DSPD, 0x3));

    // Enable device vbus sensing.
    SET_BIT(usb_otg_global->GCCFG, USB_OTG_GCCFG_VBUSBSEN);

    // Unmasks the main usb core interrupts.
    SET_BIT(usb_otg_global->GINTMSK,
            USB_OTG_GINTMSK_USBRST | USB_OTG_GINTMSK_ENUMDNEM | USB_OTG_GINTMSK_SOFM |
                USB_OTG_GINTMSK_USBSUSPM | USB_OTG_GINTMSK_WUIM | USB_OTG_GINTMSK_IEPINT |
                USB_OTG_GINTMSK_OEPINT | USB_OTG_GINTMSK_RXFLVLM);

    // Clear all pending interrupts.
    WRITE_REG(usb_otg_global->GINTSTS, ~(0UL));

    // Unmasks usb global interrupts.
    SET_BIT(usb_otg_global->GAHBCFG, USB_OTG_GAHBCFG_GINT);
}

void connect() {
    // Set power down disable bit. This will disable the power down.
    SET_BIT(usb_otg_global->GCCFG, USB_OTG_GCCFG_PWRDWN);

    // Clear soft disconnet. This will connect the usb device.
    CLEAR_BIT(usb_otg_device->DCTL, USB_OTG_DCTL_SDIS);
}

void disconnect() {
    // Set soft disconnect to disconnect the usb device.
    SET_BIT(usb_otg_device->DCTL, USB_OTG_DCTL_SDIS);

    // Clear the power down disable bit to enable power down and save power.
    CLEAR_BIT(usb_otg_global->GCCFG, USB_OTG_GCCFG_PWRDWN);
}

void gintsts_handler() {
    __IO u32 gintsts = usb_otg_global->GINTSTS;

    if (gintsts & USB_OTG_GINTSTS_USBRST) {
        usbrst_handler();
        SET_BIT(usb_otg_global->GINTSTS, USB_OTG_GINTSTS_USBRST);
    } else if (gintsts & USB_OTG_GINTSTS_ENUMDNE) {
    } else if (gintsts & USB_OTG_GINTSTS_IEPINT) {
    } else if (gintsts & USB_OTG_GINTSTS_OEPINT) {
    } else if (gintsts & USB_OTG_GINTSTS_RXFLVL) {
    }
}

//=================================================================================================
// Private Functions
//=================================================================================================

static inline USB_OTG_INEndpointTypeDef *in_endpoint(const u8 endpoint_size) {
    return ((USB_OTG_INEndpointTypeDef *)(USB_OTG_FS_PERIPH_BASE +
                                          USB_OTG_IN_ENDPOINT_BASE +
                                          (0x20 * endpoint_size)));
}

static inline USB_OTG_OUTEndpointTypeDef *out_endpoint(const u8 endpoint_size) {
    return ((USB_OTG_OUTEndpointTypeDef *)(USB_OTG_FS_PERIPH_BASE +
                                           USB_OTG_OUT_ENDPOINT_BASE +
                                           (0x20 * endpoint_size)));
}

static void usbrst_handler() {
    for (u8 i = 0; i < ENDPOINT_COUNT; ++i) {
    }
}

static void configure_endpoint0(const u8 endpoint_size) {
    // Unmask all interrupt of IN and OUT of endpoint0.
    SET_BIT(usb_otg_device->DAINTMSK, 1 << 0 | 1 << 16);

    // Configures maximum packet size, activates the endpoint, set nack
    MODIFY_REG(in_endpoint(0)->DIEPCTL, USB_OTG_DIEPCTL_MPSIZ,
               USB_OTG_DIEPCTL_USBAEP | _VAL2FLD(USB_OTG_DIEPCTL_MPSIZ, endpoint_size) |
                   USB_OTG_DIEPCTL_SNAK);

    // Enables endpoint data transmission by clearing the nack.
    SET_BIT(out_endpoint(0)->DOEPCTL, USB_OTG_DOEPCTL_EPENA | USB_OTG_DOEPCTL_CNAK);
}

static void configure_in_endpoint(const u8 endpoint_number,
                                  const UsbEndpointType endpoint_type,
                                  const u16 endpoint_size) {
    // Unmask all interrupts of endpoint passed.
    SET_BIT(usb_otg_device->DAINTMSK, 1 << endpoint_number);

    MODIFY_REG(in_endpoint(endpoint_number)->DIEPCTL,
               USB_OTG_DIEPCTL_MPSIZ | USB_OTG_DIEPCTL_EPTYP,
               USB_OTG_DIEPCTL_USBAEP | _VAL2FLD(USB_OTG_DIEPCTL_MPSIZ, endpoint_size) |
                   _VAL2FLD(USB_OTG_DIEPCTL_EPTYP, endpoint_type) | USB_OTG_DIEPCTL_SNAK |
                   USB_OTG_DIEPCTL_SD0PID_SEVNFRM);
}
