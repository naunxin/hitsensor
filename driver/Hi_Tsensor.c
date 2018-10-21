#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/fcntl.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/workqueue.h>
#include <asm/uaccess.h>
#include <mach/io.h>
#include <asm/io.h>
#include <linux/fs.h>
#include "Hi_Tsensor.h"

unsigned int overdown = 0;
unsigned int overup = 0;

//module_param(overdown,ulong,S_IRUGO);
//module_param(overup,ulong,S_IRUGO);

#define TSENSORBASE 			IO_ADDRESS(0x120E0000)
#define  PERI_PMC68   			 (TSENSORBASE+0x110)
#define  PERI_PMC69  			 (TSENSORBASE+0x114)
#define  PERI_PMC70   			 (TSENSORBASE+0x118)
#define  PERI_PMC71  			 (TSENSORBASE+0x11C)
#define  PERI_PMC72   			 (TSENSORBASE+0x120)
#define  PERI_PMC73  			 (TSENSORBASE+0x124)
#define  PERI_PMC74   			 (TSENSORBASE+0x128)



static int HiTsensor_open(struct inode *inode, struct file *filp)
{
	return 0;		
}

static int HiTsensor_release(struct inode *inode, struct file *filp)
{	
	writel(0x0, (volatile void __iomem *)PERI_PMC68);
	return 0;	
}

static long HiTsensor_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	Hi_Tsensor_info hitsensor_info;
	unsigned int tmp = 0;
	u32 reg_data;
	//printk("cmd = %04x\n",cmd);
	switch(cmd)
	{
		case SET_MODE:
			{				
				copy_from_user(&hitsensor_info, (Hi_Tsensor_info*)arg, sizeof(Hi_Tsensor_info));				
				reg_data = readl((volatile void __iomem *)PERI_PMC68);
				if(hitsensor_info.value == 1)
				{
					reg_data |= 1 << 29;
				}	
				else
				{
					reg_data &= ~(1 << 29);
				}
				writel(reg_data, (volatile void __iomem *)PERI_PMC68);				
			}
			break;

		case SET_PERIOD:
			{		
				copy_from_user(&hitsensor_info, (Hi_Tsensor_info*)arg, sizeof(Hi_Tsensor_info));
				tmp = (hitsensor_info.value >> 1) | 0x1;
				tmp = tmp & 0xFF;
				reg_data = readl((volatile void __iomem *)PERI_PMC68);				
				reg_data &= 0xFF00FFFF;
				reg_data |= tmp << 16;							
				writel(reg_data, (volatile void __iomem *)PERI_PMC68);
			}
			break;
		
		case SET_OVERUP:
			{
				copy_from_user(&hitsensor_info, (Hi_Tsensor_info*)arg, sizeof(Hi_Tsensor_info));
				reg_data = (hitsensor_info.value&0x3FF) << 16;
				writel(reg_data, (volatile void __iomem *)PERI_PMC69);
			}
			break;
			
		case SET_OVERDN:
			{		
				copy_from_user(&hitsensor_info, (Hi_Tsensor_info*)arg, sizeof(Hi_Tsensor_info));
				reg_data = readl((volatile void __iomem *)PERI_PMC69);
				reg_data |= hitsensor_info.value&0x3FF;
				writel(reg_data, (volatile void __iomem *)PERI_PMC69);
			}
			break;
		
		case SET_START:
			{	
				copy_from_user(&hitsensor_info, (Hi_Tsensor_info*)arg, sizeof(Hi_Tsensor_info));				
				reg_data = readl((volatile void __iomem *)PERI_PMC68);
				if(hitsensor_info.value == 1)
				{
					reg_data |= 1 << 30;
				}	
				else
				{
					reg_data &= ~(1 << 30);
				}
				writel(reg_data, (volatile void __iomem *)PERI_PMC68);
			}
			break;
			
		
		case GET_TEMPER:
			{
				copy_from_user(&hitsensor_info, (Hi_Tsensor_info*)arg, sizeof(Hi_Tsensor_info));
				tmp = readl((volatile void __iomem *)PERI_PMC70);		
				hitsensor_info.value = tmp & 0x3FF;			
				copy_to_user((void __user *)arg, &hitsensor_info, sizeof(Hi_Tsensor_info));
			}
			break;
			
		case GET_OVERUP:
			{
				copy_from_user(&hitsensor_info, (Hi_Tsensor_info*)arg, sizeof(Hi_Tsensor_info));		
				tmp = readl((volatile void __iomem *)PERI_PMC74);				
				hitsensor_info.value = tmp & 0x1;
				copy_to_user((void __user *)arg, &hitsensor_info, sizeof(Hi_Tsensor_info));
			}
			break;
			
		case GET_OVERDN:
			{
				copy_from_user(&hitsensor_info, (Hi_Tsensor_info*)arg, sizeof(Hi_Tsensor_info));					
				tmp = readl((volatile void __iomem *)PERI_PMC74);
				hitsensor_info.value = (tmp >> 1) & 0x1;
				copy_to_user((void __user *)arg, &hitsensor_info, sizeof(Hi_Tsensor_info));
			}
			break;
			
		default:
			{
				printk(KERN_ERR "HiTsensor error Unkown parameter\n");
				return -1;
			}
			break;
	}

	return 0;
}


static struct file_operations hitsensor_fops = {
owner:THIS_MODULE,
      open:HiTsensor_open,
      unlocked_ioctl:HiTsensor_ioctl,
      release:HiTsensor_release,
};

static struct miscdevice hitsensor_dev = {
	MISC_DYNAMIC_MINOR,
	"hi_tsensor",
	&hitsensor_fops,
};


static int __init hi_tsensor_init(void)
{
	signed int ret = 0;

	ret = misc_register(&hitsensor_dev);
	if (ret)
	{
		printk(KERN_ERR "register misc dev for /dev/hi_tsensor fail!\n");
		return ret;
	}
	//printk("HiTsensor overup = %d overdown = %d \n",overup,overdown);
	return 0;         
}

static void __exit hi_tsensor_exit(void)
{
	writel(0x0, (volatile void __iomem *)PERI_PMC68);
	misc_deregister(&hitsensor_dev);
}

module_init(hi_tsensor_init);
module_exit(hi_tsensor_exit);

MODULE_AUTHOR("LISHUANGLIANG");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Hi_Tsensor interface for HI3536");
