#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/mt7620a
	NAME:=MT7620A
	PACKAGES:=\
		kmod-usb-core kmod-usb2 kmod-usb-ohci \
		kmod-ledtrig-usbdev
endef

define Profile/Default/Description
	Default package set compatible with mt7620a boards.
endef
$(eval $(call Profile,mt7620a))
