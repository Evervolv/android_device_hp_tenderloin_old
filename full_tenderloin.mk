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

#TODO ---- What do we need to do to not use a static init & init.rc?
PRODUCT_COPY_FILES += \
    device/hp/tenderloin/init:root/init \
    device/hp/tenderloin/init.rc:root/init.rc \
    device/hp/tenderloin/ueventd.rc:root/ueventd.rc \
    device/hp/tenderloin/init.tenderloin.rc:root/init.tenderloin.rc \
    device/hp/tenderloin/ueventd.tenderloin.rc:root/ueventd.tenderloin.rc

#Add toushcreen config file
PRODUCT_COPY_FILES += \
    device/hp/tenderloin/cy8c-touchscreen.idc:system/usr/idc/cy8c-touchscreen.idc

## (2) Also get non-open-source GSM-specific aspects if available
$(call inherit-product-if-exists, vendor/hp/tenderloin/tenderloin-vendor.mk)

DEVICE_PACKAGE_OVERLAYS += device/hp/tenderloin/overlay

PRODUCT_COPY_FILES += \
    frameworks/base/data/etc/android.hardware.telephony.cdma.xml:system/etc/permissions/android.hardware.telephony.cdma.xml \
    frameworks/base/data/etc/handheld_core_hardware.xml:system/etc/permissions/handheld_core_hardware.xml \
    frameworks/base/data/etc/android.hardware.wifi.xml:system/etc/permissions/android.hardware.wifi.xml \
    frameworks/base/data/etc/android.hardware.sensor.proximity.xml:system/etc/permissions/android.hardware.sensor.proximity.xml \
    frameworks/base/data/etc/android.hardware.sensor.light.xml:system/etc/permissions/android.hardware.sensor.light.xml \
    frameworks/base/data/etc/android.hardware.touchscreen.multitouch.distinct.xml:system/etc/permissions/android.hardware.touchscreen.multitouch.distinct.xml \
    frameworks/base/data/etc/android.hardware.usb.accessory.xml:system/etc/permissions/android.hardware.usb.accessory.xml \
    frameworks/base/data/etc/android.software.sip.voip.xml:system/etc/permissions/android.software.sip.voip.xml

PRODUCT_PACKAGES += \
    librs_jni \
    libOmxVenc \
    libOmxVdec \
    com.android.future.usb.accessory
#    librs_jni \
#    gralloc.msm8660 \
#    copybit.msm8660 \
#    overlay.default \
#    libOmxCore \
#    libaudio \


# Keylayouts
#PRODUCT_COPY_FILES += \

# Kernel modules
#PRODUCT_COPY_FILES += \

# we have enough storage space to hold precise GC data
PRODUCT_TAGS += dalvik.gc.type-precise

##Disable HWAccel for now & set ldc_density
ADDITIONAL_BUILD_PROPERTIES += \
    ro.config.disable_hw_accel=true \
    ro.sf.lcd_density=160

PRODUCT_AAPT_CONFIG := normal mdpi
PRODUCT_AAPT_PREF_CONFIG := mdpi
PRODUCT_CHARACTERISTICS := tablet

PRODUCT_LOCALES += en

PRODUCT_COPY_FILES += \
    device/hp/tenderloin/vold.fstab:system/etc/vold.fstab

$(call inherit-product, $(SRC_TARGET_DIR)/product/languages_full.mk)

DEVICE_PACKAGE_OVERLAYS += device/hp/tenderloin/overlay

ifeq ($(TARGET_PREBUILT_KERNEL),)
	LOCAL_KERNEL := device/hp/tenderloin/kernel
else
	LOCAL_KERNEL := $(TARGET_PREBUILT_KERNEL)
endif

PRODUCT_COPY_FILES += \
    $(LOCAL_KERNEL):kernel

$(call inherit-product, build/target/product/full_base.mk)

PRODUCT_NAME := full_tenderloin
PRODUCT_DEVICE := tenderloin
PRODUCT_MODEL := HP Touchpad
PRODUCT_MANUFACTURER := HP
