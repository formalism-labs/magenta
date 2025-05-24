
MODULE=contrib-minIni
MODULE_DIR=$(VROOT)/magenta/contrib/minIni

include $(MK)/module/start

MODULE_PRODUCT=lib

MININI_ROOT=$(VROOT)/github.compuphase.minIni/dev
MININI_INCLUDES=$(MININI_ROOT)

CC_SUB_INCLUDE_DIRS += $(MININI_INCLUDES)

include $(MK)/module/end
