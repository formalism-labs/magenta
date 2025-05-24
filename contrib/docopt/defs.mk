
MODULE=contrib-docopt
# MODULE_DIR=$(VROOT)/magenta/contrib/docopt

include $(MK)/module/start

MODULE_PRODUCT=lib

DOCOPT_ROOT=$(VROOT)/github.docopt.docopt.cpp
DOCOPT_INCLUDES=$(DOCOPT_ROOT)

CC_SUB_INCLUDE_DIRS += $(DOCOPT_INCLUDES)

include $(MK)/module/end
