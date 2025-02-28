#
# Rules for building application
#

# ----------------------------------------------------------------------------
# project local config
# ----------------------------------------------------------------------------
include localconfig.mk

# ----------------------------------------------------------------------------
# common rules
# ----------------------------------------------------------------------------
ROOT_PATH := ../../..

include $(ROOT_PATH)/gcc.mk

# ----------------------------------------------------------------------------
# project name and project root path
# ----------------------------------------------------------------------------
PROJECT := $(notdir $(shell cd .. && pwd))
PRJ_ROOT_PATH := $(ROOT_PATH)/project/$(PROJECT)

# ----------------------------------------------------------------------------
# board path, including board_config.c and board_config.h
# ----------------------------------------------------------------------------
ifeq ($(__CONFIG_CHIP_TYPE), xr872)
PRJ_BOARD := $(PRJ_ROOT_PATH)/xr872_epotato
else ifeq ($(__CONFIG_CHIP_TYPE), xr808)
PRJ_BOARD := $(ROOT_PATH)/project/common/board/xr808_evb_io
else
PRJ_BOARD := null
endif

# ----------------------------------------------------------------------------
# objects
# ----------------------------------------------------------------------------
INCLUDE_EASYAIOT_ALL := $(shell find $(PRJ_ROOT_PATH)/easyAIoT -type d)
INCLUDE_EASYAIOT_IGNORE := $(PRJ_ROOT_PATH)/easyAIoT/Audio/WIN
INCLUDE_EASYAIOT_IGNORE += $(PRJ_ROOT_PATH)/easyAIoT/Audio/Linux%
DIR_INCLUDE_EASYAIOT := $(filter-out $(INCLUDE_EASYAIOT_IGNORE),$(INCLUDE_EASYAIOT_ALL))

INCLUDE_PATHS += -I$(PRJ_ROOT_PATH)
INCLUDE_PATHS += $(addprefix -I, $(DIR_INCLUDE_EASYAIOT))

DIRS_IGNORE := $(PRJ_ROOT_PATH)/gcc%
DIRS_IGNORE += $(PRJ_ROOT_PATH)/image%
DIRS_IGNORE += $(ROOT_PATH)/project/common/board/%
DIRS_IGNORE += $(PRJ_BOARD)
DIRS_IGNORE += $(PRJ_ROOT_PATH)/easyAIoT/Audio/WIN
DIRS_IGNORE += $(PRJ_ROOT_PATH)/easyAIoT/Audio/Linux%

DIRS_ALL := $(shell find $(PRJ_ROOT_PATH) $(ROOT_PATH)/project/common -type d)
DIRS := $(filter-out $(DIRS_IGNORE),$(DIRS_ALL))
DIRS += $(PRJ_BOARD)

SRCS := $(basename $(foreach dir,$(DIRS),$(wildcard $(dir)/*.[csS])))
SRCS += $(basename $(foreach dir,$(DIRS),$(wildcard $(dir)/*.cpp)))

OBJS := $(addsuffix .o,$(SRCS))

# extra libraries searching path
# PRJ_EXTRA_LIBS_PATH :=

# extra libraries
# PRJ_EXTRA_LIBS :=

# extra header files searching path
# PRJ_EXTRA_INC_PATH := 

# extra symbols (macros)
# PRJ_EXTRA_SYMBOLS :=

# ----------------------------------------------------------------------------
# override project variables
# ----------------------------------------------------------------------------
# linker script file
#   - relative to "./"
#   - define your own "LINKER_SCRIPT" to override the default one
LINKER_SCRIPT := ./appos.ld

# image config file
#   - relative to "../image/$(__CONFIG_CHIP_TYPE)/", eg. "../image/xr872/"
#   - define your own "IMAGE_CFG" to override the default one
IMAGE_CFG := ./image_auto_cal.cfg

# image name, default to xr_system
# IMAGE_NAME :=

# project make rules
include $(PRJ_MAKE_RULES)
