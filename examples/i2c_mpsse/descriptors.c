
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
      .bcdDevice		= 0x0001,
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
            .bEndpointAddress	= USB_ENDPOINT_NUMBER(0x1) | USB_DIR_OUT,
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
