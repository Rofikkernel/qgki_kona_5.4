// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2021, The Linux Foundation. All rights reserved.
 * Copyright (c) 2022 Qualcomm Innovation Center, Inc. All rights reserved.
 *
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/mount.h>
#include <linux/genhd.h>
#include <linux/device.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/netdevice.h>
#include <linux/rtnetlink.h>
#define PATH_SIZE	32

static void rename_net_device_name(struct device_node *np)
{
	struct device_node *net_node = np;
	int index = 0, err = 0;
	const char *src, *dst;
	struct net_device *net_dev = NULL;

	if (net_node) {
		while (!of_property_read_string_index(net_node, "actual-dev", index,
			&src) && !of_property_read_string_index(net_node,
				"rename-dev", index, &dst)) {
			if (rtnl_trylock()) {
				net_dev = dev_get_by_name(&init_net, src);
				if (net_dev) {
					err = dev_change_name(net_dev, dst);
					if (err != 0)
						pr_err("rename-devices: Net rename err = %d\n",
								 err);
				}
				else
					pr_err("rename-devices: no net_dev\n");
				rtnl_unlock();

			}
			index++;
		}
	}
}

static void rename_blk_device_name(struct device_node *np)
{
	dev_t dev;
	int index = 0, partno;
	struct gendisk *disk;
	struct device_node *node = np;
	char dev_path[PATH_SIZE];
	const char *actual_name;
	char *modified_name;

	while (!of_property_read_string_index(node, "actual-dev", index,
						&actual_name)) {
		memset(dev_path, '\0', PATH_SIZE);
		snprintf(dev_path, PATH_SIZE, "/dev/%s", actual_name);
		dev = name_to_dev_t(dev_path);
		if (!dev) {
			pr_err("rename-devices: No device path : %s\n", dev_path);
			return;
		}
		disk = get_gendisk(dev, &partno);
		if (!disk) {
			pr_err("rename-devices: No device with dev path : %s\n", dev_path);
			return;
		}
		if (!of_property_read_string_index(node, "rename-dev", index,
					(const char **)&modified_name)) {
			device_rename(disk_to_dev(disk), modified_name);
		} else {
			pr_err("rename-devices: rename-dev for actual-dev = %s is missing\n",
							actual_name);
			return;
		}
		index++;
	}
}

static int __init rename_devices_init(void)
{
	struct device_node *node = NULL, *child = NULL;
	const char *device_type;

	node = of_find_compatible_node(NULL, NULL, "qcom,rename-devices");

	if (!node) {
		pr_err("rename-devices: qcom,rename-devices node is missing\n");
		goto out;
	}

	for_each_child_of_node(node, child) {
		if (!of_property_read_string(child, "device-type", &device_type)) {
			if (strcmp(device_type, "block") == 0)
				rename_blk_device_name(child);
			else if (strcmp(device_type, "net") == 0)
				rename_net_device_name(child);
			else
				pr_err("rename-devices: unsupported device\n");
		} else
			pr_err("rename-devices: device-type is missing\n");
	}

out:
	of_node_put(node);
	return  0;
}

late_initcall(rename_devices_init);
MODULE_DESCRIPTION("Rename devices");
MODULE_LICENSE("GPL v2");
