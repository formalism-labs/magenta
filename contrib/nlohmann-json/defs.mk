
MODULE=contrib-nlohmann-json

include $(MK)/module/start

MODULE_PRODUCT=none

NLOHMANN_JSON_ROOT=$(VROOT)/github.nlohmann.json
NLOHMANN_JSON_INCUDES=$(NLOHMANN_JSON_ROOT)/src

CC_SUB_INCLUDE_DIRS += $(NLOHMANN_JSON_INCUDES)

include $(MK)/module/end
