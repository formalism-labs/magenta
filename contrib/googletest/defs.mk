
MODULE=contrib-googletest
# MODULE_DIR=$(VROOT)/magenta/contrib/googletest

include $(MK)/module/start

MODULE_PRODUCT=lib

GOOGLETEST_ROOT=$(VROOT)/github.google.googletest/googletest
GOOGLETEST_INCUDES=$(GOOGLETEST_ROOT)/include

CC_SUB_INCLUDE_DIRS += $(GOOGLETEST_INCUDES)

include $(MK)/module/end
