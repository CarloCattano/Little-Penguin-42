### Step 1: Modify Your Code to Support USB Keyboard Detection

- integrate USB device detection logic:

```c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/usb.h>

// USB device table for keyboards 
static struct usb_device_id usb_keyboard_table[] = {
    { USB_INTERFACE_INFO(USB_INTERFACE_CLASS_HID, USB_INTERFACE_SUBCLASS_BOOT, USB_INTERFACE_PROTOCOL_KEYBOARD) },
    {}  // Terminating entry
};
MODULE_DEVICE_TABLE(usb, usb_keyboard_table);

static int usb_keyboard_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
    printk(KERN_INFO "Hello, World! USB keyboard connected.\n");
    return 0;
}

static void usb_keyboard_disconnect(struct usb_interface *interface)
{
    printk(KERN_INFO "Goodbye, USB keyboard disconnected.\n");
}

// Registering the USB driver
static struct usb_driver usb_keyboard_driver = {
    .name = "usb_hellokernel_driver",
    .id_table = usb_keyboard_table,
    .probe = usb_keyboard_probe,
    .disconnect = usb_keyboard_disconnect,
};

module_usb_driver(usb_keyboard_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Carlo Cattano");
MODULE_DESCRIPTION("A USB keyboard detection module that prints messages when connected or disconnected.");
```

### Step 2: Create a Udev Rule to Auto-Load the Module

 - Create a udev rule file:

```bash
sudo nano /etc/udev/rules.d/99-usb-keyboard.rules
```
```bash

    ACTION=="add", SUBSYSTEM=="usb", ENV{ID_USB_INTERFACE_NUM}=="01", RUN+="/sbin/modprobe usb_hellokernel_driver"
```
Adjust ID_USB_INTERFACE_NUM or ATTR{idVendor} and ATTR{idProduct} if needed (check via ```lsusb```).

### Step 3: Test Your Setup
    - Reload udev rules:

```bash
sudo udevadm control --reload-rules
sudo udevadm trigger
```
    - Connect a USB keyboard to your system.
    - Check Kernel Logs:
    dmesg | grep "USB keyboard"

Turn In:
    Modified Code (usb_hellokernel_driver.c).
    Udev Rule File (99-usb-keyboard.rules).
    Proof of Module Functionality (log output or a short video).
