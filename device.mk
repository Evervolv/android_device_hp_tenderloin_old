#
# Copyright (C) 2011 The Evervolv Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# Proprietary stuff.
$(call inherit-product-if-exists, vendor/hp/tenderloin/tenderloin-vendor.mk)

# This is the hardware-specific overlay, which points to the location
# of hardware-specific resource overrides, typically the frameworks and
# application settings that are stored in resourced.
DEVICE_PACKAGE_OVERLAYS += device/hp/tenderloin/overlay

#TODO ---- What do we need to do to not use a static init & init.rc?
PRODUCT_COPY_FILES += \
    device/hp/tenderloin/init:root/init \
    device/hp/tenderloin/init.rc:root/init.rc \
    device/hp/tenderloin/ueventd.rc:root/ueventd.rc \
    device/hp/tenderloin/init.tenderloin.rc:root/init.tenderloin.rc \
    device/hp/tenderloin/ueventd.tenderloin.rc:root/ueventd.tenderloin.rc

# Add touchcreen configuration file
PRODUCT_COPY_FILES += \
    device/hp/tenderloin/cy8c-touchscreen.idc:system/usr/idc/cy8c-touchscreen.idc

# Vold configuration
PRODUCT_COPY_FILES += \
    device/hp/tenderloin/vold.fstab:system/etc/vold.fstab

# Telephony stuff, not sure if we need all of these.
PRODUCT_COPY_FILES += \
    frameworks/base/data/etc/android.hardware.telephony.cdma.xml:system/etc/permissions/android.hardware.telephony.cdma.xml \
    frameworks/base/data/etc/handheld_core_hardware.xml:system/etc/permissions/handheld_core_hardware.xml \
    frameworks/base/data/etc/android.hardware.wifi.xml:system/etc/permissions/android.hardware.wifi.xml \
    frameworks/base/data/etc/android.hardware.sensor.proximity.xml:system/etc/permissions/android.hardware.sensor.proximity.xml \
    frameworks/base/data/etc/android.hardware.sensor.light.xml:system/etc/permissions/android.hardware.sensor.light.xml \
    frameworks/base/data/etc/android.hardware.touchscreen.multitouch.distinct.xml:system/etc/permissions/android.hardware.touchscreen.multitouch.distinct.xml \
    frameworks/base/data/etc/android.hardware.usb.accessory.xml:system/etc/permissions/android.hardware.usb.accessory.xml \
    frameworks/base/data/etc/android.software.sip.voip.xml:system/etc/permissions/android.software.sip.voip.xml

# Keylayouts
PRODUCT_COPY_FILES += \
    device/hp/tenderloin/keylayout/gpio-keys.kl:system/usr/keylayout/gpio-keys.kl \
    device/hp/tenderloin/keylayout/pmic8058_pwrkey.kl:system/usr/keylayout/pmic8058_pwrkey.kl

# Kernel modules
PRODUCT_COPY_FILES += \
    device/hp/tenderloin/modules/ansi_cprng.ko:system/lib/modules/ansi_cprng.ko \
    device/hp/tenderloin/modules/ar6000.ko:system/lib/modules/ar6000.ko \
    device/hp/tenderloin/modules/cifs.ko:system/lib/modules/cifs.ko \
    device/hp/tenderloin/modules/cpaccess.ko:system/lib/modules/cpaccess.ko \
    device/hp/tenderloin/modules/dma_test.ko:system/lib/modules/dma_test.ko \
    device/hp/tenderloin/modules/gspca_main.ko:system/lib/modules/gspca_main.ko \
    device/hp/tenderloin/modules/lcd.ko:system/lib/modules/lcd.ko \
    device/hp/tenderloin/modules/librasdioif.ko:system/lib/modules/librasdioif.ko \
    device/hp/tenderloin/modules/nls_utf8.ko:system/lib/modules/nls_utf8.ko \
    device/hp/tenderloin/modules/ntfs.ko:system/lib/modules/ntfs.ko \
    device/hp/tenderloin/modules/oprofile.ko:system/lib/modules/oprofile.ko \
    device/hp/tenderloin/modules/scsi_wait_scan.ko:system/lib/modules/scsi_wait_scan.ko \
    device/hp/tenderloin/modules/tcp_bic.ko:system/lib/modules/tcp_bic.ko \
    device/hp/tenderloin/modules/tun.ko:system/lib/modules/tun.ko

# We have enough storage space to hold precise GC data
PRODUCT_TAGS += dalvik.gc.type-precise

# Additional packages.
PRODUCT_PACKAGES += \
    librs_jni \
    libOmxVenc \
    libOmxVdec \
    hwcomposer.default \
    com.android.future.usb.accessory \
    ts_srv \
#    librs_jni \
#    gralloc.msm8660 \
#    copybit.msm8660 \
#    overlay.default \
#    libOmxCore \
#    libaudio 

# This is a high density device.
PRODUCT_LOCALES += hdpi
PRODUCT_AAPT_CONFIG := normal hdpi

# Prebuilt kernel.
ifeq ($(TARGET_PREBUILT_KERNEL),)
	LOCAL_KERNEL := device/hp/tenderloin/kernel
else
	LOCAL_KERNEL := $(TARGET_PREBUILT_KERNEL)
endif

# Copy the kernel.
PRODUCT_COPY_FILES += \
    $(LOCAL_KERNEL):kernel

# This is a tablet.
PRODUCT_CHARACTERISTICS := tablet
