#include "bsp_usb.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"

void usb_init(void)
{
    MX_USB_DEVICE_Init();
    //CDC_Transmit_FS((uint8_t*)"hello\r\n", 7);
}

void usb_process(void)
{
    //CDC_Transmit_FS((uint8_t*)"hello\r\n", 7);
    //HAL_Delay(1000);
}
