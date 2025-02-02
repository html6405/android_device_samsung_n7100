# Release name
PRODUCT_RELEASE_NAME := n7100

# Inherit device configuration
$(call inherit-product, device/samsung/n7100/n7100.mk)
$(call inherit-product, device/samsung/smdk4412-common/common.mk)

# Inherit from the common Open Source product configuration
$(call inherit-product, $(SRC_TARGET_DIR)/product/aosp_base_telephony.mk)

# Boot animation
TARGET_SCREEN_HEIGHT := 1280
TARGET_SCREEN_WIDTH := 720

# Inherit some common stuff.
$(call inherit-product, vendor/lineage/config/common_full_phone.mk)

# Device identifier. This must come after all inclusions
PRODUCT_DEVICE := n7100
PRODUCT_NAME := lineage_n7100
PRODUCT_BRAND := samsung
PRODUCT_MODEL := GT-N7100
PRODUCT_MANUFACTURER := samsung

# Set build fingerprint / ID / Product Name ect.
PRODUCT_BUILD_PROP_OVERRIDES += \
        PRODUCT_NAME=t03gxx \
        TARGET_DEVICE=t03g \
        PRIVATE_BUILD_DESC="t03gxx-user 4.3 JSS15J N7100XXUEMK4 release-keys" 

BUILD_FINGERPRINT := samsung/t03gxx/t03g:4.3/JSS15J/N7100XXUEMK4:user/release-keys 
