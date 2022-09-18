#pragma once

typedef enum UsbEndpointType {
    UsbEndpointType_Control,
    UsbEndpointType_Isochronous,
    UsbEndpointType_Bulk,
    UsbEndpointType_Interrupt
} UsbEndpointType;
