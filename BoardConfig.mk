#
# Copyright (C) 2012 The CyanogenMod Project
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

# This variable is set first, so it can be overridden
# by BoardConfigVendor.mk

include device/samsung/smdk4412-common/BoardCommonConfig.mk

# Wifi
WIFI_DRIVER_MODULE_PATH :=

# Bionic
MALLOC_SVELTE := true
TARGET_NEEDS_PLATFORM_TEXT_RELOCATIONS := true
LIBART_IMG_BASE := 0x30000000

# Graphics
TARGET_REQUIRES_SYNCHRONOUS_SETSURFACE := true

# Bluetooth
BOARD_BLUETOOTH_BDROID_BUILDCFG_INCLUDE_DIR := device/samsung/n7100/bluetooth

# Kernel
TARGET_KERNEL_SOURCE := kernel/samsung/smdk4412
TARGET_KERNEL_CONFIG := lineageos_n7100_defconfig

TARGET_SPECIFIC_HEADER_PATH += device/samsung/n7100/include

BOARD_USERDATAIMAGE_PARTITION_SIZE := 11408506864

# Cache
BOARD_CACHEIMAGE_PARTITION_SIZE := 104857600
BOARD_CACHEIMAGE_FILE_SYSTEM_TYPE := ext4

# Recovery
TARGET_RECOVERY_FSTAB := device/samsung/n7100/rootdir/fstab.smdk4x12
TARGET_RECOVERY_DENSITY := mdpi
TARGET_USERIMAGES_USE_F2FS := true
RECOVERY_FSTAB_VERSION := 2

# assert
TARGET_OTA_ASSERT_DEVICE := t03g,n7100,GT-N7100

# Init
ifneq ($(WITH_TWRP), true)
TARGET_INIT_VENDOR_LIB := libinit_n7100
endif

TARGET_LD_SHIM_LIBS += \
	/vendor/bin/gpsd|libsamsung_symbols.so \
	/vendor/bin/gpsd|libshims_sensorlistener.so \
	/vendor/bin/gpsd|libdmitry.so

# SELinux
BOARD_VENDOR_SEPOLICY_DIRS += device/samsung/n7100/selinux/vendor
BOARD_PLAT_PUBLIC_SEPOLICY_DIR += device/samsung/n7100/selinux/public
BOARD_PLAT_PRIVATE_SEPOLICY_DIR += device/samsung/n7100/selinux/private