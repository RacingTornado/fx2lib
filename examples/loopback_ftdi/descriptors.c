/*
 * The descriptors below setup an FTDI device with a VID
 * of 0x0403, and PID of 0x6010.In this particular case
 * endpoint 1 is used to transfer data in and out of the device
 * since it can be easily accessed by the CPU and is not used
 * for other purposes(Typically used for transferring small amounts
 * of data which is what we need)
 * lsusb -v will show:
 * For an explanation of fields look at
 * http://www.keil.com/pack/doc/mw/USB/html/_u_s_b__endpoint__descriptor.html
 * vbDevice Descriptor:
 * bLength                18
 * bDescriptorType         1
 * bcdUSB               2.00
 * bDeviceClass          255 Vendor Specific Class
 * bDeviceSubClass       255 Vendor Specific Subclass
 * bDeviceProtocol       255 Vendor Specific Protocol
 * bMaxPacketSize0        64
 * idVendor           0x0403 Future Technology Devices International, Ltd
 * idProduct          0x6010 FT2232C Dual USB-UART/FIFO IC
 * bcdDevice            0.01
 * iManufacturer           1
 * iProduct                2
 * iSerial                 0
 * bNumConfigurations      1
 * Configuration Descriptor:
 *  bLength                 9
 *  bDescriptorType         2
 *  wTotalLength           32
 *  bNumInterfaces          1
 *  bConfigurationValue     1
 *  iConfiguration          0
 *  bmAttributes         0x80
 *    (Bus Powered)
 *  MaxPower              100mA
 *  Interface Descriptor:
 *    bLength                 9
 *    bDescriptorType         4
 *    bInterfaceNumber        0
 *    bAlternateSetting       0
 *    bNumEndpoints           2
 *    bInterfaceClass       255 Vendor Specific Class
 *    bInterfaceSubClass    255 Vendor Specific Subclass
 *    bInterfaceProtocol    255 Vendor Specific Protocol
 *    iInterface              3
 *    Endpoint Descriptor:
 *      bLength                 7
 *      bDescriptorType         5
 *      bEndpointAddress     0x01  EP 1 OUT
 *      bmAttributes            2
 *        Transfer Type            Bulk
 *        Synch Type               None
 *        Usage Type               Data
 *      wMaxPacketSize     0x0200  1x 512 bytes
 *      bInterval               0
 *    Endpoint Descriptor:
 *      bLength                 7
 *      bDescriptorType         5
 *      bEndpointAddress     0x81  EP 1 IN
 *      bmAttributes            2
 *        Transfer Type            Bulk
 *        Synch Type               None
 *        Usage Type               Data
 *      wMaxPacketSize     0x0200  1x 512 bytes
 *      bInterval               0
 */
#include "descriptors.h"

__code __at(0x3e00) struct usb_descriptors code_descriptors =
{
   .device                    =
   {
      /*Size of Descriptor in Bytes*/
      .bLength                = USB_DT_DEVICE_SIZE,
      /*Constant Device Descriptor (0x01)*/
      .bDescriptorType        = USB_DT_DEVICE,
      /*USB Specification Number which device complies too.*/
      .bcdUSB                 = USB_BCD_V20,
      /*Class Code (Assigned by USB Org)
       * If equal to Zero, each interface specifies it’s own class code
       * If equal to 0xFF, the class code is vendor specified.*/
      .bDeviceClass           = USB_CLASS_VENDOR_SPEC,
      /*Subclass Code (Assigned by USB Org)*/
      .bDeviceSubClass        = USB_SUBCLASS_VENDOR_SPEC,
      /*Protocol Code (Assigned by USB Org)*/
      .bDeviceProtocol        = 0xff,
      /*Maximum Packet Size for Zero Endpoint. Valid Sizes are 8, 16, 32, 64*/
      .bMaxPacketSize0        = 64,
      /*Vendor ID*/
      .idVendor               = 0x0403,
      /*Product ID*/
      .idProduct              = 0x6010,
      /*Device Release Number*/
      .bcdDevice              = 0x0001,
      /*Index of Manufacturer String Descriptor*/
      .iManufacturer          = USB_STRING_INDEX(0),
      /*Index of Product String Descriptor*/
      .iProduct               = USB_STRING_INDEX(1),
      /*Index of Serial Number String Descriptor*/
      .iSerialNumber          = USB_STRING_INDEX_NONE,
      /*Number of Possible Configurations*/
      .bNumConfigurations     = 1,
   },
   .qualifier                 =
   {
      /*Size of this descriptor in bytes. */
      .bLength                = USB_DT_DEVICE_QUALIFIER_SIZE,
      /*Device Qualifier Descriptor Type = 6. */
      .bDescriptorType        = USB_DT_DEVICE_QUALIFIER,
      /*USB Specification Release Number in Binary-Coded Decimal (i.e., 2.10 is 210h).
       * This field identifies the release of the USB Specification with which the device
       * and its descriptors are compliant.
       * At least V2.00 is required to use this descriptor.*/
      .bcdUSB                 = USB_BCD_V20,
      /*Class code (assigned by the USB-IF).
       * If this field is set to FFh, the device class is vendor specific.*/
      .bDeviceClass           = USB_CLASS_VENDOR_SPEC,
      /*Subclass code (assigned by the USB-IF).*/
      .bDeviceSubClass        = USB_SUBCLASS_VENDOR_SPEC,
      /*Protocol code (assigned by the USB-IF).Set to FFh,
       * the device uses a vendor specific protocol on a device basis.*/
      .bDeviceProtocol        = 0xff,
      /*Maximum packet size for other speed.*/
      .bMaxPacketSize0        = 64,
      /*Number of other-speed configurations.*/
      .bNumConfigurations     = 1,
      /*Reserved for future use, must be zero.*/
      .bRESERVED              = 0,
   },
   .highspeed                 =
   {
      .config                 =
      {
         /*Size of this descriptor in bytes.*/
         .bLength             = USB_DT_CONFIG_SIZE,
         /*Configuration Descriptor Type = 2.*/
         .bDescriptorType     = USB_DT_CONFIG,
         /*Total length of data returned for this configuration.
          * Includes the combined length of all descriptors
          *(configuration, interface, endpoint, and class or vendor specific)
          * returned for this configuration.*/
         .wTotalLength        = sizeof(descriptors.highspeed),
         /*Number of interfaces supported by this configuration.*/
         .bNumInterfaces      = 1,
         /*Value to select this configuration with SetConfiguration().*/
         .bConfigurationValue = 1,
         /*Index of string descriptor describing this configuration.*/
         .iConfiguration      = 0,
         /*Configuration characteristics
          * D7: Reserved (must be set to one for historical reasons)
          * D6: Self-powered
          * D5: Remote Wakeup
          * D4...0: Reserved (reset to zero)*/
         .bmAttributes        = USB_CONFIG_ATT_ONE,
         /*Maximum power consumption of the USB device from the
          * bus in this specific configuration when the device is
          * fully operational. Expressed in 2mA units (i.e., 50 = 100mA).*/
         .bMaxPower           = 0x32, // FIXME: ???
      },
      .interface              =
      {
         /*Size of this descriptor in bytes.*/
         .bLength             = USB_DT_INTERFACE_SIZE,
         /*Interface Descriptor Type = 4.*/
         .bDescriptorType     = USB_DT_INTERFACE,
         /*The number of this interface.*/
         .bInterfaceNumber    = 0,
         /*Value used to select an alternate setting for the
          * interface identified in the prior field.
          * Allows an interface to change the settings on the fly.*/
         .bAlternateSetting   = 0,
         /*Number of endpoints used by this interface (excluding endpoint zero).*/
         .bNumEndpoints       = 2,
         /*Class code (assigned by the USB-IF).*/
         .bInterfaceClass     = USB_CLASS_VENDOR_SPEC,
         /*Subclass code (assigned by the USB-IF).*/
         .bInterfaceSubClass  = USB_SUBCLASS_VENDOR_SPEC,
         /*Protocol code (assigned by the USB).*/
         .bInterfaceProtocol  = USB_PROTOCOL_VENDOR_SPEC,
         /*Index of string descriptor describing this interface.*/
         .iInterface          = USB_STRING_INDEX(2),
      },
      .endpoints              =
      {
         {
            /*Size of this descriptor in bytes.*/
            .bLength          = USB_DT_ENDPOINT_SIZE,
            /*Endpoint Descriptor Type = 5.*/
            .bDescriptorType  = USB_DT_ENDPOINT,
            /*The address of the endpoint on the USB device described by this descriptor. The address is encoded as follows:
             * Bit 3...0: The endpoint number
             * Bit 6...4: Reserved, reset to zero
             * Bit 7: Direction, ignored for control endpoints.
             *      0 = OUT endpoint
             *      1 = IN endpoint*/
            .bEndpointAddress = USB_ENDPOINT_NUMBER(0x1) | USB_DIR_OUT,
            /*The endpoint attribute when configured through bConfigurationValue.*/
            .bmAttributes     = USB_ENDPOINT_XFER_BULK,
            /*Is the maximum packet size of this endpoint.*/
            .wMaxPacketSize   = 512,
            /*Interval for polling endpoint for data transfers*/
            .bInterval        = 0,
         },
         {
            /*Size of this descriptor in bytes.*/
            .bLength          = USB_DT_ENDPOINT_SIZE,
            /*Endpoint Descriptor Type = 5.*/
            .bDescriptorType  = USB_DT_ENDPOINT,
            /*The address of the endpoint on the USB device described by this descriptor. The address is encoded as follows:
             * Bit 3...0: The endpoint number
             * Bit 6...4: Reserved, reset to zero
             * Bit 7: Direction, ignored for control endpoints.
             *      0 = OUT endpoint
             *      1 = IN endpoint*/
            .bEndpointAddress = USB_ENDPOINT_NUMBER(0x1) | USB_DIR_IN,
            /*The endpoint attribute when configured through bConfigurationValue.*/
            .bmAttributes     = USB_ENDPOINT_XFER_BULK,
            /*Is the maximum packet size of this endpoint.*/
            .wMaxPacketSize   = 512,
            /*Interval for polling endpoint for data transfers*/
            .bInterval        = 0,
         },
      },
   },
   .fullspeed                 =
   {
      /*Refer above comments for descriptor explanations.*/
      .config                 =
      {
         .bLength             = USB_DT_CONFIG_SIZE,
         .bDescriptorType     = USB_DT_CONFIG,
         .wTotalLength        = sizeof(descriptors.fullspeed),
         .bNumInterfaces      = 1,
         .bConfigurationValue = 1,
         .iConfiguration      = 0,
         .bmAttributes        = USB_CONFIG_ATT_ONE,
         .bMaxPower           = 0x32,// FIXME: ???
      },
      .interface              =
      {
         .bLength            = USB_DT_INTERFACE_SIZE,
         .bDescriptorType    = USB_DT_INTERFACE,
         .bInterfaceNumber   = 0,
         .bAlternateSetting  = 0,
         .bNumEndpoints      = 2,
         .bInterfaceClass    = USB_CLASS_VENDOR_SPEC,
         .bInterfaceSubClass = USB_SUBCLASS_VENDOR_SPEC,
         .bInterfaceProtocol = 0xff,
         .iInterface         = 3,
      },
      .endpoints              =
      {
         {
            .bLength          = USB_DT_ENDPOINT_SIZE,
            .bDescriptorType  = USB_DT_ENDPOINT,
            .bEndpointAddress = USB_ENDPOINT_NUMBER(0x2) | USB_DIR_OUT,
            .bmAttributes     = USB_ENDPOINT_XFER_BULK,
            .wMaxPacketSize   = 64,
            .bInterval        = 0,
         },
         {
            .bLength          = USB_DT_ENDPOINT_SIZE,
            .bDescriptorType  = USB_DT_ENDPOINT,
            .bEndpointAddress = USB_ENDPOINT_NUMBER(0x6) | USB_DIR_IN,
            .bmAttributes     = USB_ENDPOINT_XFER_BULK,
            .wMaxPacketSize   = 64,
            .bInterval        = 0,
         },
      },
   },
#include "build/descriptors_stringtable.inc"
};
