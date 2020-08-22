obj-m := hid-m708.o
KVERSION := $(shell uname -r)
KDIR := /lib/modules/$(KVERSION)/build
PWD := $(shell pwd)
DESTDIR =
UDEV_RULES = $(DESTDIR)/lib/udev/rules.d/90-m708.rules
DEPMOD_CONF = $(DESTDIR)/etc/depmod.d/m708.conf
HID_REBIND = $(DESTDIR)/lib/udev/hid-rebind
XORG_CONF := $(DESTDIR)/usr/share/X11/xorg.conf.d/50-m708.conf
PACKAGE_NAME = m708-kernel-driver
PACKAGE_VERSION = 1
PACKAGE = $(PACKAGE_NAME)-$(PACKAGE_VERSION)
DKMS_MODULES_NAME = m708
DKMS_MODULES = $(DKMS_MODULES_NAME)/$(PACKAGE_VERSION)
DKMS_SOURCE_DIR = $(DESTDIR)/usr/src/$(DKMS_MODULES_NAME)-$(PACKAGE_VERSION)
modules modules_install clean:
	$(MAKE) -C $(KDIR) M=$(PWD) $@

depmod_conf_install:
	install -D -m 0644 depmod.conf $(DEPMOD_CONF)
	depmod -a

depmod_conf_uninstall:
	rm -vf $(DEPMOD_CONF)
	depmod -a
xorg_conf_install:
	install -D -m 0644 xorg.conf $(XORG_CONF)

xorg_conf_uninstall:
	rm -vf $(XORG_CONF)

udev_rules_install_files:
	install -D -m 0755 hid-rebind $(HID_REBIND)
	install -D -m 0644 udev.rules $(UDEV_RULES)

udev_rules_install: udev_rules_install_files
	udevadm control --reload

udev_rules_uninstall_files:
	rm -vf $(UDEV_RULES) $(HID_REBIND)

udev_rules_uninstall: udev_rules_uninstall_files
	udevadm control --reload

modules_uninstall:
	rm -vf /lib/modules/*/extra/hid-m708.ko

install: modules modules_install depmod_conf_install udev_rules_install xorg_conf_install

uninstall: xorg_conf_uninstall udev_rules_uninstall depmod_conf_uninstall modules_uninstall

dkms_check:
	@if ! which dkms >/dev/null; then \
	    echo "DKMS not found, aborting." >&2; \
	    echo "Make sure DKMS is installed,"; \
	    echo "and \"make\" is running under sudo, or as root."; \
	    exit 1; \
	fi

dkms_source_install:
	install -m 0755 -d $(DKMS_SOURCE_DIR)
	install -m 0644 Makefile *.[hc] $(DKMS_SOURCE_DIR)
	install -m 0755 -d $(DKMS_SOURCE_DIR)/usbhid
	install -m 0644 usbhid/*.[hc] $(DKMS_SOURCE_DIR)/usbhid

dkms_modules_install: dkms_check
	@if dkms status $(DKMS_MODULES_NAME) | grep . >/dev/null; then \
	    echo "DKMS has module added already, aborting." >&2; \
	    echo "Run \"make dkms_uninstall\" first" >&2; \
	    exit 1; \
	fi
	dkms add .
	dkms build $(DKMS_MODULES)
	dkms install $(DKMS_MODULES)

dkms_modules_uninstall: dkms_check
	set -e -x; \
	dkms status $(DKMS_MODULES_NAME) | \
	    while IFS=':' read -r modules status; do \
	        echo "$$modules" | { \
	            IFS=', ' read -r modules_name modules_version \
	                             kernel_version kernel_arch ignore; \
	            if [ -z "$$kernel_version" ]; then \
	                dkms remove \
	                            "$$modules_name/$$modules_version" \
	                            --all; \
	            else \
	                dkms remove \
	                            "$$modules_name/$$modules_version" \
	                            -k "$$kernel_version/$$kernel_arch"; \
	            fi; \
	        } \
	    done

dkms_install: dkms_modules_install depmod_conf_install udev_rules_install xorg_conf_install

dkms_uninstall: xorg_conf_uninstall udev_rules_uninstall depmod_conf_uninstall dkms_modules_uninstall
