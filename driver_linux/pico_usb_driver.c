#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#define USB_PICO_VENDOR_ID  0xCAFE
#define USB_PICO_PRODUCT_ID 0x4001

#define DEVICE_NAME "pico_usb"

static int major_number;
static struct pico_usb_dev *pico_dev;

/* ---------------- USB ID TABLE ---------------- */

static const struct usb_device_id pico_table[] = {
    { USB_DEVICE(USB_PICO_VENDOR_ID, USB_PICO_PRODUCT_ID) },
    {}
};
MODULE_DEVICE_TABLE(usb, pico_table);

struct pico_usb_dev {
    struct usb_device *udev;
    struct usb_interface *interface;
    __u8 ep_in;
    __u8 ep_out;
};

/* ---------------- FILE OPERATIONS ---------------- */

static ssize_t pico_read(struct file *file, char __user *buffer,
                         size_t len, loff_t *offset)
{
    int retval;
    int actual;
    char *kbuf;

    if (!pico_dev)
        return -ENODEV;

    kbuf = kmalloc(len, GFP_KERNEL);
    if (!kbuf)
        return -ENOMEM;

    retval = usb_bulk_msg(
        pico_dev->udev,
        usb_rcvbulkpipe(pico_dev->udev, pico_dev->ep_in),
        kbuf,
        len,
        &actual,
        1000
    );

    if (retval) {
        kfree(kbuf);
        return retval;
    }

    if (copy_to_user(buffer, kbuf, actual)) {
        kfree(kbuf);
        return -EFAULT;
    }

    kfree(kbuf);
    *offset += actual;
    return actual;
}

static ssize_t pico_write(struct file *file, const char __user *buffer,
                          size_t len, loff_t *offset)
{
    int retval;
    int actual;
    char *kbuf;

    if (!pico_dev) {
        printk(KERN_ERR "pico_usb: dispositivo não disponível\n");
        return -ENODEV;
    }

    printk(KERN_INFO "pico_usb: write chamado (%zu bytes)\n", len);

    kbuf = kmalloc(len, GFP_KERNEL);
    if (!kbuf) {
        printk(KERN_ERR "pico_usb: falha ao alocar memória\n");
        return -ENOMEM;
    }

    if (copy_from_user(kbuf, buffer, len)) {
        printk(KERN_ERR "pico_usb: erro em copy_from_user\n");
        kfree(kbuf);
        return -EFAULT;
    }

    /* Logs do protocolo */
    printk(KERN_INFO "pico_usb: comando recebido: 0x%02X\n", kbuf[0]);

    if (len > 1)
        printk(KERN_INFO "pico_usb: param1: 0x%02X\n", kbuf[1]);

    if (len > 2)
        printk(KERN_INFO "pico_usb: param2: 0x%02X\n", kbuf[2]);

    retval = usb_bulk_msg(
        pico_dev->udev,
        usb_sndbulkpipe(pico_dev->udev, pico_dev->ep_out),
        kbuf,
        len,
        &actual,
        1000
    );

    kfree(kbuf);

    if (retval) {
        printk(KERN_ERR "pico_usb: erro ao enviar USB (%d)\n", retval);
        return retval;
    }

    printk(KERN_INFO "pico_usb: %d bytes enviados ao dispositivo\n", actual);

    *offset += actual;
    return actual;
}


static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read  = pico_read,
    .write = pico_write,
};

/* ---------------- USB CALLBACKS ---------------- */

static int pico_probe(struct usb_interface *interface,
                      const struct usb_device_id *id)
{
    struct usb_host_interface *iface_desc;
    struct usb_endpoint_descriptor *endpoint;
    int i;

    pr_info("pico_usb: RP2040 conectado (VID=0x%04X, PID=0x%04X)\n",
            id->idVendor, id->idProduct);

    pico_dev = kzalloc(sizeof(struct pico_usb_dev), GFP_KERNEL);
    if (!pico_dev)
        return -ENOMEM;

    pico_dev->udev = usb_get_dev(interface_to_usbdev(interface));
    pico_dev->interface = interface;

    iface_desc = interface->cur_altsetting;

    for (i = 0; i < iface_desc->desc.bNumEndpoints; i++) {
        endpoint = &iface_desc->endpoint[i].desc;

        if (usb_endpoint_is_bulk_in(endpoint))
            pico_dev->ep_in = endpoint->bEndpointAddress;

        if (usb_endpoint_is_bulk_out(endpoint))
            pico_dev->ep_out = endpoint->bEndpointAddress;
    }

    usb_set_intfdata(interface, pico_dev);

    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0) {
        pr_err("pico_usb: erro ao registrar char device\n");
        usb_put_dev(pico_dev->udev);
        kfree(pico_dev);
        usb_set_intfdata(interface, NULL);
        return major_number;
    }

    pr_info("pico_usb: /dev/%s criado com major %d\n",
            DEVICE_NAME, major_number);

    return 0;
}

static void pico_disconnect(struct usb_interface *interface)
{
    struct pico_usb_dev *dev = usb_get_intfdata(interface);

    pr_info("pico_usb: RP2040 desconectado\n");

    if (dev) {
        if (major_number > 0)
            unregister_chrdev(major_number, DEVICE_NAME);
        usb_set_intfdata(interface, NULL);
        usb_put_dev(dev->udev);
        kfree(dev);
        pico_dev = NULL;
        major_number = 0;
    }
}

/* ---------------- USB DRIVER STRUCT ---------------- */

static struct usb_driver pico_driver = {
    .name       = "pico_usb_driver",
    .id_table   = pico_table,
    .probe      = pico_probe,
    .disconnect = pico_disconnect,
};

/* ---------------- MODULE INIT / EXIT ---------------- */

static int __init pico_init(void)
{
    return usb_register(&pico_driver);
}

static void __exit pico_exit(void)
{
    usb_deregister(&pico_driver);
}

module_init(pico_init);
module_exit(pico_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ana Alicy & Ana Beatriz");
MODULE_DESCRIPTION("Driver USB para RP2040 Pico");
MODULE_VERSION("1.0");