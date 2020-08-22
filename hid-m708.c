// SPDX-License-Identifier: GPL-2.0+
/*
 *  HID driver for Ugee M708
 *
 */

/*
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 */

#include <linux/device.h>
#include <linux/hid.h>
#include <linux/module.h>
#include <linux/usb.h>

#include "hid-ids.h"

/* Size of the original descriptor of M708 */
#define M708_RDESC_ORIG_SIZE	36

/* Fixed report descriptor of M708 */
static __u8 m708_rdesc_fixed[] = {
    0x06, 0x0A, 0xFF,  // Usage Page (Vendor Defined 0xFF0A)
    0x09, 0x01,        // Usage (0x01)
    0xA1, 0x01,        // Collection (Application)
    0x85, 0x02,        //   Report ID (2)
    0x09, 0x02,        //   Usage (0x02)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x09,        //   Report Count (9)
    0x15, 0x00,        //   Logical Minimum (0)
    0x26, 0xFF, 0x00,  //   Logical Maximum (255)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x03,        //   Usage (0x03)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x09,        //   Report Count (9)
    0x15, 0x00,        //   Logical Minimum (0)
    0x26, 0xFF, 0x00,  //   Logical Maximum (255)
    0x91, 0x02,        //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,              // End Collection
    0x05, 0x0D,        // Usage Page (Digitizer)
    0x09, 0x02,        // Usage (Pen)
    0xA1, 0x01,        // Collection (Application)
    0x85, 0x07,        //   Report ID (7)
    0x09, 0x20,        //   Usage (Stylus)
    0xA1, 0x00,        //   Collection (Physical)
    0x09, 0x42,        //     Usage (Tip Switch)
    0x09, 0x44,        //     Usage (Barrel Switch)
	0x09, 0x46,        //     Usage (Tablet Pick)
    0x15, 0x00,        //     Logical Minimum (0)
    0x25, 0x01,        //     Logical Maximum (1)
    0x75, 0x01,        //     Report Size (1)
    0x95, 0x03,        //     Report Count (3)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x02,        //     Report Count (2)
    0x81, 0x03,        //     Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x32,        //     Usage (In Range)
    0x95, 0x01,        //     Report Count (1)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x02,        //     Report Count (2)
    0x81, 0x03,        //     Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x75, 0x10,        //     Report Size (16)
    0x95, 0x01,        //     Report Count (1)
    0x35, 0x00,        //     Physical Minimum (0)
    0xA4,              //     Push
    0x05, 0x01,        //       Usage Page (Generic Desktop Ctrls)
    0x09, 0x30,        //       Usage (X)
    0x65, 0x33,        //       Unit (System: English Linear, Length: Inch)
    0x55, 0x0D,        //       Unit Exponent (-3)
    0x46, 0x0F, 0x27,  //       Physical Maximum (9999)
    0x26, 0x6A, 0xC6,  //       Logical Maximum (50794)
    0x81, 0x02,        //       Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x31,        //       Usage (Y)
    0x46, 0x6F, 0x17,  //       Physical Maximum (5999)
    0x26, 0x0A, 0x77,  //       Logical Maximum (30474)
    0x81, 0x02,        //       Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xB4,              //     Pop
    0x09, 0x30,        //     Usage (Tip Pressure)
    0x45, 0x00,        //     Physical Maximum (0)
    0x26, 0xFF, 0x1F,  //     Logical Maximum (8191)
    0x81, 0x42,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,Null State)
    0x09, 0x3D,        //     Usage (X Tilt)
    0x15, 0x81,        //     Logical Minimum (-127)
    0x25, 0x7F,        //     Logical Maximum (127)
    0x75, 0x08,        //     Report Size (8)
    0x95, 0x01,        //     Report Count (1)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x3E,        //     Usage (Y Tilt)
    0x15, 0x81,        //     Logical Minimum (-127)
    0x25, 0x7F,        //     Logical Maximum (127)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              //   End Collection
    0xC0,              // End Collection
    0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
    0x09, 0x07,        // Usage (Keypad)
    0xA1, 0x01,        // Collection (Application)
    0x85, 0x06,        //   Report ID (6)
    0x05, 0x0D,        //   Usage Page (Digitizer)
    0x09, 0x39,        //   Usage (Tablet Function Keys)
    0xA0,              //   Collection
    0x75, 0x08,        //     Report Size (8)
    0x95, 0x01,        //     Report Count (1)
    0x81, 0x03,        //     Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x05, 0x09,        //     Usage Page (Button)
    0x75, 0x01,        //     Report Size (1)
    0x19, 0x01,        //     Usage Minimum (0x01)
    0x29, 0x08,        //     Usage Maximum (0x08)
    0x15, 0x00,        //     Logical Minimum (0)
    0x25, 0x01,        //     Logical Maximum (1)
    0x95, 0x08,        //     Report Count (8)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x75, 0x08,        //     Report Size (8)
    0x95, 0x07,        //     Report Count (7)
    0x81, 0x03,        //     Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              //   End Collection
    0xC0,              // End Collection
};

static __u8 *m708_report_fixup(struct hid_device *hdev, __u8 *rdesc,
				    unsigned int *rsize)
{
    if (*rsize == M708_RDESC_ORIG_SIZE) {
        rdesc = m708_rdesc_fixed;
        *rsize = sizeof(m708_rdesc_fixed);
    }

	return rdesc;
}

/**
 * Enable alternative HID reports by setting a special feature report.
 *
 * @hdev:	HID device
 *
 * The specific report ID and data were discovered by sniffing the
 * Windows driver traffic.
 */
static int v3_tablet_enable(struct hid_device *hdev)
{
	struct list_head *list;
	struct list_head *head;
	struct hid_report *report;
	__s32 *value;
	
    list = &hdev->report_enum[HID_OUTPUT_REPORT].report_list;
	list_for_each(head, list) {
		report = list_entry(head, struct hid_report, list);
		if (report->id == 2)
			break;
	}

	if (head == list) {
		hid_err(hdev, "tablet-enabling feature report not found\n");
		return -ENODEV;
	}

	if (report->maxfield < 1 || report->field[0]->report_count < 7) {
		hid_err(hdev, "invalid tablet-enabling feature report\n");
		return -ENODEV;
	}

	value = report->field[0]->value;

	value[0] = 0xB0;
	value[1] = 0x04;
	value[2] = 0x00;
	value[3] = 0x00;
	value[4] = 0x00;
	value[5] = 0x00;
	value[6] = 0x00;
	hid_hw_request(hdev, report, HID_REQ_SET_REPORT);
    hid_info(hdev,"sent magic");
	return 0;
}

static int m708_probe(struct hid_device *hdev,
			   const struct hid_device_id *id)
{
	int rc;
	struct usb_interface *iface = to_usb_interface(hdev->dev.parent);
	__u8 bInterfaceNumber = iface->cur_altsetting->desc.bInterfaceNumber;
    if (bInterfaceNumber != 2){
        hid_info(hdev, "interface is invalid, ignoring\n");
        return -ENODEV;
    }
    hdev->quirks |= HID_QUIRK_MULTI_INPUT;
	rc = hid_parse(hdev);
	if (rc) {
		hid_err(hdev, "parse failed\n");
		return rc;
	}

	rc = hid_hw_start(hdev, HID_CONNECT_DEFAULT);
	if (rc) {
		hid_err(hdev, "hw start failed\n");
		return rc;
	}
    v3_tablet_enable(hdev);
	return 0;
}

static int m708_raw_event(struct hid_device *hdev,
				struct hid_report *report,
				u8 *data, int size)
{
	/* Do not handle anything but input reports */
	if (report->type != HID_INPUT_REPORT) {
		return 0;
	}
	
	if (data[1] == 0xB1) {
        // ignore control message
    } else if (data[1] == 0xF0) {
        report->id = data[0] = 6;
    } else {
        report->id = data[0] = 7;
    }

	return 0;
}

static const struct hid_device_id devices[] = {
	{ HID_USB_DEVICE(USB_VENDOR_ID_UGEE,
				USB_DEVICE_ID_UGEE_TABLET_M708_V2) },
	{ }
};
MODULE_DEVICE_TABLE(hid, devices);

static struct hid_driver m708_driver = {
	.name = "m708",
	.id_table = devices,
	.probe = m708_probe,
	.report_fixup = m708_report_fixup,
    .raw_event = m708_raw_event,
};
module_hid_driver(m708_driver);

MODULE_LICENSE("GPL");
MODULE_VERSION("1");
