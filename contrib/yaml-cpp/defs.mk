
MODULE=contrib-yaml-cpp

include $(MK)/module/start

MODULE_PRODUCT=lib

YAML_CPP_ROOT=$(VROOT)/github.jbeder.yaml-cpp
YAML_CPP_INCLUDES=$(YAML_CPP_ROOT)/include

CC_SUB_INCLUDE_DIRS += $(YAML_CPP_INCLUDES)

include $(MK)/module/end
