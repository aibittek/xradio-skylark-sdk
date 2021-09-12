#
# project local config options, override the global config options
#

# ----------------------------------------------------------------------------
# override global config options
# ----------------------------------------------------------------------------
# enable/disable XIP, default to y
export __CONFIG_XIP := y

# enable/disalbe PSRAM for XR872AT
# export __CONFIG_PSRAM := y

# enable/disalbe WPS mode
# export __CONFIG_WLAN_STA_WPS := y

# enable/disalbe AP mode
# export __CONFIG_WLAN_AP := y

# enable/disalbe monitor mode
# export __CONFIG_WLAN_MONITOR := y

# enable/disalbe OTA, default to n
export __CONFIG_OTA := y

# enable/disalbe play/ctrl
export __CONFIG_XPLAYER := y

# enable/disalbe secure boot
# export __CONFIG_SECURE_BOOT := y

# enable/disalbe cache setup
# export _CONFIG_CACHE_POLICY := y

# enable/disalbe heap/stack debug, note if enable more 8KB memory will used
# export __CONFIG_MALLOC_TRACE := y
