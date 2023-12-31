// SPDX-License-Identifier: GPL-2.0
/*
 * u_uac1.h - Utility definitions for UAC1 function
 *
 * Copyright (C) 2016 Ruslan Bilovol <ruslan.bilovol@gmail.com>
 */

#ifndef __U_UAC1_H
#define __U_UAC1_H

#include <linux/usb/composite.h>

#define UAC1_OUT_EP_MAX_PACKET_SIZE	200
#define UAC1_DEF_CCHMASK	0x3
#define UAC1_DEF_CSRATE		48000
#define UAC1_DEF_CSSIZE		2
#define UAC1_DEF_PCHMASK	0x3
#define UAC1_DEF_PSRATE		48000
#define UAC1_DEF_PSSIZE		2
#define UAC1_DEF_REQ_NUM	32


struct f_uac1_opts {
	struct usb_function_instance	func_inst;
	int				c_chmask;
	int				c_srate;
	int				c_ssize;
	int				c_status;
	int				p_chmask;
	int				p_srate;
	int				p_ssize;
	int				p_status;
	int				req_number;
	unsigned int            ep_maxp_size;
	unsigned			bound:1;

	struct mutex			lock;
	int				refcnt;
	struct f_uac1                   *uac1;
	struct device			*device;
#ifdef CONFIG_USB_CONFIGFS_UEVENT
	struct work_struct		work;
#endif
};

extern struct device *create_function_device(char *name);
#endif /* __U_UAC1_H */
