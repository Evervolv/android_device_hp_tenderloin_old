USE_CAMERA_STUB := true

# inherit from the proprietary version
-include vendor/hp/tenderloin/BoardConfigVendor.mk

TARGET_NO_BOOTLOADER := true

TARGET_BOARD_PLATFORM := msm8660
TARGET_BOARD_PLATFORM_GPU := qcom-adreno200

TARGET_CPU_ABI := armeabi-v7a
TARGET_CPU_ABI2 := armeabi
TARGET_ARCH_VARIANT := armv7-a-neon
TARGET_CPU_SMP := true
ARCH_ARM_HAVE_TLS_REGISTER := true

TARGET_BOOTLOADER_BOARD_NAME := tenderloin
TARGET_USES_OVERLAY := true
TARGET_NO_RADIOIMAGE := true
TARGET_HAVE_TSLIB := false
TARGET_GLOBAL_CFLAGS += -mfpu=neon -mfloat-abi=softfp
TARGET_GLOBAL_CPPFLAGS += -mfpu=neon -mfloat-abi=softfp

# Wifi related defines
BOARD_WPA_SUPPLICANT_DRIVER      := WEXT
BOARD_WPA_SUPPLICANT_PRIVATE_LIB := lib_driver_cmd_wext
WPA_SUPPLICANT_VERSION           := VER_0_8_X
BOARD_WLAN_DEVICE                := bcm4329
WIFI_DRIVER_MODULE_PATH          := "/system/lib/modules/bcm4329.ko"
WIFI_DRIVER_FW_PATH_STA          := "/vendor/firmware/fw_bcm4329.bin"
WIFI_DRIVER_FW_PATH_AP           := "/vendor/firmware/fw_bcm4329_apsta.bin"
WIFI_DRIVER_MODULE_ARG           := "firmware_path=/vendor/firmware/fw_bcm4329.bin nvram_path=/proc/calibration"
WIFI_DRIVER_MODULE_NAME          := "bcm4329"

#Bluetooth
#BOARD_HAVE_BLUETOOTH := true
#BOARD_HAVE_BLUETOOTH_BCM := true

# Define egl.cfg location
BOARD_EGL_CFG := device/hp/tenderloin/egl.cfg
BOARD_USES_OVERLAY := true
#USE_OPENGL_RENDERER := true
#BOARD_USES_HGL := true

COMMON_GLOBAL_CFLAGS += \
    -DMISSING_EGL_EXTERNAL_IMAGE \
    -DMISSING_EGL_PIXEL_FORMAT_YV12 \
    -DMISSING_GRALLOC_BUFFERS

# Legacy touchscreen support
BOARD_USE_LEGACY_TOUCHSCREEN := true

BOARD_KERNEL_CMDLINE := no_console_suspend=1
BOARD_KERNEL_BASE := 0x40208000
BOARD_PAGE_SIZE := 1819634989

# Define Prebuilt kernel locations
TARGET_PREBUILT_KERNEL := device/hp/tenderloin/kernel

TARGET_USERIMAGES_USE_EXT4 := true
BOARD_BOOTIMAGE_PARTITION_SIZE := 16777216
BOARD_RECOVERYIMAGE_PARTITION_SIZE := 0x00480000
BOARD_SYSTEMIMAGE_PARTITION_SIZE := 838860800
BOARD_USERDATAIMAGE_PARTITION_SIZE := 20044333056
BOARD_FLASH_BLOCK_SIZE := 131072

TARGET_RELEASETOOLS_EXTENSIONS := device/htc/common

BOARD_USES_MMCUTILS := true
BOARD_HAS_NO_MISC_PARTITION := true
BOARD_HAS_NO_SELECT_BUTTON := true
BOARD_CUSTOM_GRAPHICS:= ../../../device/hp/tenderloin/graphics.c
BOARD_USES_RECOVERY_CHARGEMODE := true
