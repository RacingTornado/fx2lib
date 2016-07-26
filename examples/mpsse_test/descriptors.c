/*
The descriptors below setup an FTDI device with a VID
of 0x0403, and PID of 0x6010.In this particular case
endpoint 1 is used to transfer data in and out of the device
since it can be easily accessed by the CPU and is not used
for other purposes(Typically used for transferring small aamount
of data which is what we need)
Bus 003 Device 039: ID 0403:6010
Future Technology Devices International, Ltd FT2232C Dual USB-UART/FIFO IC
vbDevice Descriptor:
  bLength                18
  bDescriptorType         1
  bcdUSB               2.00
  bDeviceClass            0 (Defined at Interface level)
  bDeviceSubClass         0
  bDeviceProtocol         0
  bMaxPacketSize0        64
  idVendor           0x0403 Future Technology Devices International, Ltd
  idProduct          0x6010 FT2232C Dual USB-UART/FIFO IC
  bcdDevice            7.00
  iManufacturer           1
  iProduct                2
  iSerial                 3
  bNumConfigurations      1
  Configuration Descriptor:
    bLength                 9
    bDescriptorType         2
    wTotalLength           55
    bNumInterfaces          2
    bConfigurationValue     1
    iConfiguration          0
    bmAttributes         0x80
      (Bus Powered)
    MaxPower              100mA
    Interface Descriptor:
      bLength                 9
      bDescriptorType         4
      bInterfaceNumber        0
      bAlternateSetting       0
      bNumEndpoints           2
      bInterfaceClass       255 Vendor Specific Class
      bInterfaceSubClass    255 Vendor Specific Subclass
      bInterfaceProtocol    255 Vendor Specific Protocol
      iInterface              2
      Endpoint Descriptor:
        bLength                 7
        bDescriptorType         5
        bEndpointAddress     0x81  EP 1 IN
        bmAttributes            2
          Transfer Type            Bulk
          Synch Type               None
          Usage Type               Data
        wMaxPacketSize     0x0200  1x 512 bytes
        bInterval               0
      Endpoint Descriptor:
        bLength                 7
        bDescriptorType         5
        bEndpointAddress     0x02  EP 2 OUT
        bmAttributes            2
          Transfer Type            Bulk
          Synch Type               None
          Usage Type               Data
        wMaxPacketSize     0x0200  1x 512 bytes
        bInterval               0
    Interface Descriptor:
      bLength                 9
      bDescriptorType         4
      bInterfaceNumber        1
      bAlternateSetting       0
      bNumEndpoints           2
      bInterfaceClass       255 Vendor Specific Class
      bInterfaceSubClass    255 Vendor Specific Subclass
      bInterfaceProtocol    255 Vendor Specific Protocol
      iInterface              2
      Endpoint Descriptor:
        bLength                 7
        bDescriptorType         5
        bEndpointAddress     0x83  EP 3 IN
        bmAttributes            2
          Transfer Type            Bulk
          Synch Type               None
          Usage Type               Data
        wMaxPacketSize     0x0200  1x 512 bytes
        bInterval               0
      Endpoint Descriptor:
        bLength                 7
        bDescriptorType         5
        bEndpointAddress     0x04  EP 4 OUT
        bmAttributes            2
          Transfer Type            Bulk
          Synch Type               None
          Usage Type               Data
        wMaxPacketSize     0x0200  1x 512 bytes
        bInterval               0






*/
#include "descriptors.h"

__code __at(0x3e00) struct usb_descriptors code_descriptors =
{
   .device = {
      .bLength		= USB_DT_DEVICE_SIZE,
      .bDescriptorType	= USB_DT_DEVICE,
      .bcdUSB			= USB_BCD_V20,
      .bDeviceClass 		= USB_CLASS_VENDOR_SPEC,
      .bDeviceSubClass	= USB_SUBCLASS_VENDOR_SPEC,
      .bDeviceProtocol	= 0xff,
      .bMaxPacketSize0	= 64,
      .idVendor		= 0x0403,
      .idProduct		= 0x6010,
      .bcdDevice		= 0x0700,
      .iManufacturer		= USB_STRING_INDEX(0),
      .iProduct		= USB_STRING_INDEX(1),
      .iSerialNumber		= USB_STRING_INDEX_NONE,
      .bNumConfigurations	= 1,
   },
   .qualifier = {
      .bLength		= USB_DT_DEVICE_QUALIFIER_SIZE,
      .bDescriptorType	= USB_DT_DEVICE_QUALIFIER,
      .bcdUSB			= USB_BCD_V20,
      .bDeviceClass		= USB_CLASS_VENDOR_SPEC,
      .bDeviceSubClass	= USB_SUBCLASS_VENDOR_SPEC,
      .bDeviceProtocol	= 0xff,
      .bMaxPacketSize0	= 64,
      .bNumConfigurations	= 1,
      .bRESERVED		= 0,
   },
   .highspeed = {
      .config = {
         .bLength		= USB_DT_CONFIG_SIZE,
         .bDescriptorType	= USB_DT_CONFIG,
         .wTotalLength		= sizeof(descriptors.highspeed),
         .bNumInterfaces		= 1,
         .bConfigurationValue	= 1,
         .iConfiguration		= 0,
         .bmAttributes		= USB_CONFIG_ATT_ONE,
         .bMaxPower		= 0x32, // FIXME: ???
      },
      .interface = {
         .bLength		= USB_DT_INTERFACE_SIZE,
         .bDescriptorType	= USB_DT_INTERFACE,
         .bInterfaceNumber	= 0,
         .bAlternateSetting	= 0,
         .bNumEndpoints		= 2,
         .bInterfaceClass	= USB_CLASS_VENDOR_SPEC,
         .bInterfaceSubClass	= USB_SUBCLASS_VENDOR_SPEC,
         .bInterfaceProtocol	= USB_PROTOCOL_VENDOR_SPEC,
         .iInterface		= USB_STRING_INDEX(2),
      },
      .endpoints = {
         {
            .bLength		= USB_DT_ENDPOINT_SIZE,
            .bDescriptorType 	= USB_DT_ENDPOINT,
            .bEndpointAddress	= USB_ENDPOINT_NUMBER(0x2) | USB_DIR_OUT,
            .bmAttributes		= USB_ENDPOINT_XFER_BULK,
            .wMaxPacketSize		= 512,
            .bInterval		= 0,
         },
         {
            .bLength		= USB_DT_ENDPOINT_SIZE,
            .bDescriptorType 	= USB_DT_ENDPOINT,
            .bEndpointAddress	= USB_ENDPOINT_NUMBER(0x1) | USB_DIR_IN,
            .bmAttributes		= USB_ENDPOINT_XFER_BULK,
            .wMaxPacketSize		= 512,
            .bInterval		= 0,
         },
      },
   },
   .fullspeed = {
      .config = {
         .bLength		= USB_DT_CONFIG_SIZE,
         .bDescriptorType	= USB_DT_CONFIG,
         .wTotalLength		= sizeof(descriptors.fullspeed),
         .bNumInterfaces		= 1,
         .bConfigurationValue	= 1,
         .iConfiguration		= 0,
         .bmAttributes		= USB_CONFIG_ATT_ONE,
         .bMaxPower		= 0x32, // FIXME: ???
      },
      .interface = {
         .bLength		= USB_DT_INTERFACE_SIZE,
         .bDescriptorType	= USB_DT_INTERFACE,
         .bInterfaceNumber	= 0,
         .bAlternateSetting	= 0,
         .bNumEndpoints		= 2,
         .bInterfaceClass	= USB_CLASS_VENDOR_SPEC,
         .bInterfaceSubClass	= USB_SUBCLASS_VENDOR_SPEC,
         .bInterfaceProtocol	= 0xff,
         .iInterface		= 3,
      },
      .endpoints = {
         {
            .bLength		= USB_DT_ENDPOINT_SIZE,
            .bDescriptorType 	= USB_DT_ENDPOINT,
            .bEndpointAddress	= USB_ENDPOINT_NUMBER(0x2) | USB_DIR_OUT,
            .bmAttributes		= USB_ENDPOINT_XFER_BULK,
            .wMaxPacketSize		= 64,
            .bInterval		= 0,
         },
         {
            .bLength		= USB_DT_ENDPOINT_SIZE,
            .bDescriptorType 	= USB_DT_ENDPOINT,
            .bEndpointAddress	= USB_ENDPOINT_NUMBER(0x6) | USB_DIR_IN,
            .bmAttributes		= USB_ENDPOINT_XFER_BULK,
            .wMaxPacketSize		= 64,
            .bInterval		= 0,
         },
      },
   },
#include "build/descriptors_stringtable.inc"
};
