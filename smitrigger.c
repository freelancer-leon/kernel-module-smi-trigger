#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <asm/io.h>

#define DEVICE_NAME "smitrigger"
#define CLASS_NAME "smitriggerClass"

static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

static struct file_operations fops = {.read = device_read,
                                      .write = device_write,
                                      .open = device_open,
                                      .release = device_release};

static int majorNumber;
static struct class *smitriggerClass = NULL;
static struct device *smitriggerDevice = NULL;


static int __init smitrigger_init(void) {
  printk(KERN_INFO "Hello from smitrigger\n");

  majorNumber = register_chrdev(0, DEVICE_NAME, &fops);

  if (majorNumber < 0) {
    printk(KERN_ALERT "Registering char device failed with %d\n", majorNumber);
    return majorNumber;
  }
  printk(KERN_INFO "Smitrigger registered correctly with major number %d\n",
         majorNumber);

  smitriggerClass = class_create(CLASS_NAME);

  if (IS_ERR(smitriggerClass)) {
    unregister_chrdev(majorNumber, DEVICE_NAME);
    printk(KERN_ALERT "Smitrigger failed to register device class\n");
    return PTR_ERR(smitriggerClass);
  }
  printk(KERN_INFO "Smitrigger device class registered correctly\n");

  smitriggerDevice = device_create(smitriggerClass, NULL, MKDEV(majorNumber, 0),
                                   NULL, DEVICE_NAME);

  if (IS_ERR(smitriggerDevice)) {
    class_destroy(smitriggerClass);
    unregister_chrdev(majorNumber, DEVICE_NAME);
    printk(KERN_ALERT "Smitrigger failed to create the device\n");
    return PTR_ERR(smitriggerDevice);
  }
  printk(KERN_INFO "Smitrigger device class created correctly\n");
  return 0;
}

static void __exit smitrigger_exit(void) {
  device_destroy(smitriggerClass, MKDEV(majorNumber, 0));
  class_unregister(smitriggerClass);
  class_destroy(smitriggerClass);
  unregister_chrdev(majorNumber, DEVICE_NAME);

  printk(KERN_INFO "Goodbye from smitrigger\n");
}

static int device_open(struct inode *inode, struct file *file) {
  printk(KERN_INFO "Smitrigger open\n");
  return 0;
}


static int device_release(struct inode *inode, struct file *file) {
  printk(KERN_INFO "Smitrigger release\n");

  return 0;
}

static ssize_t device_read(struct file *filp,
                           char *buffer,
                           size_t length,
                           loff_t *offset) {
  printk(KERN_INFO "Smitrigger read\n");

  return 0;
}

static ssize_t device_write(struct file *filp, const char *buff, size_t len,
                            loff_t *off) {
  uint8_t smm_response = 0x0;
  uint32_t smi_count = 0x0;

  printk(KERN_ALERT "Smitrigger write. Software SMI gets triggered.\n");

  // check smi count
  rdmsrl(MSR_SMI_COUNT, smi_count);
  printk(KERN_INFO "SMI counter earlier is 0x%x.\n", smi_count);

  //check the initial value recieve from 0xB2 port
  smm_response = inb(0xb2);
  printk(KERN_INFO "Data recieved from port 0xB2 is 0x%x.\n", smm_response);

  //write to 0xB2 port to cause SMI
  outb(0x80, 0xb2);

  // Check the respose
  smm_response = inb(0xb2);
  printk(KERN_INFO "Data recieved after is 0x%x.\n", smm_response);

  rdmsrl(MSR_SMI_COUNT, smi_count);
  printk(KERN_INFO "SMI counter later is 0x%x.\n", smi_count);

  return len;
}

module_init(smitrigger_init);
module_exit(smitrigger_exit);

MODULE_LICENSE("GPL");
