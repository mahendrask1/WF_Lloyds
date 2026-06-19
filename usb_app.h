/* usb_app.h - stubs for former Keil USB CDC (USB stack removed from build). */
#ifndef USB_APP_H
#define USB_APP_H

#include <stdint.h>

int32_t USBD_Initialize(uint8_t instance);
int32_t USBD_Connect(uint8_t instance);
int32_t USBD_CDC_ACM_WriteData(uint8_t instance, const char *buf, uint32_t len);

#endif
