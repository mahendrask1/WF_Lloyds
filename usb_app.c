/*
 * Stubs for removed USB device (CDC) stack. Parameter_Upload / menu paths still
 * call these symbols; all operations are no-ops.
 */
#include <stdint.h>
#include "usb_app.h"

int32_t USBD_Initialize(uint8_t instance)
{
  (void)instance;
  return 0;
}

int32_t USBD_Connect(uint8_t instance)
{
  (void)instance;
  return 0;
}

int32_t USBD_CDC_ACM_WriteData(uint8_t instance, const char *buf, uint32_t len)
{
  (void)instance;
  (void)buf;
  (void)len;
  return 0;
}
