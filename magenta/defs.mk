
MODULE=magenta

# module magenta

include $(MK)/module/start

MODULE_PRODUCT=lib

# MODULE_TARGET_DIR_PREFIX.PREBUILT=../Release/bin

# cinclude ../Build/common.defs.mk

MAGENTA_ROOT=$(VROOT)/magenta
MAGENTA_INCLUDES=$(MAGENTA_ROOT)

CC_SUB_INCLUDE_DIRS += $(MAGENTA_INCLUDES)

include $(MK)/module/end
# endmodule
