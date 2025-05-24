
MODULE=contrib-boost

include $(MK)/module/start

MODULE_PRODUCT=none

BOOST_ROOT=$(VROOT)/boost-1.64.0
BOOST_INCLUDES=$(BOOST_ROOT)

CC_SUB_INCLUDE_DIRS += $(BOOST_INCLUDES)

include $(MK)/module/end
