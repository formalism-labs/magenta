
SRC_ROOT=../../../../../..
include $(SRC_ROOT)/freemasonBuild/freemason/4/main

include $(MK)/defs

#----------------------------------------------------------------------------------------------

define CC_PP_DEFS.common
endef

define CC_PP_DEFS.windows
endef

CC_PP_DEFS += $(CC_PP_DEFS.common) $(CC_PP_DEFS.$(TARGET_OS_TYPE))

#----------------------------------------------------------------------------------------------

define CC_INCLUDE_DIRS.common
	..
	../common
endef

define CC_INCLUDE_DIRS.windows
	$(VROOT)/nbu.contrib/open-ssl/$(OPENSSL_VER)/windows
endef

CC_INCLUDE_DIRS += $(CC_INCLUDE_DIRS.common) $(CC_INCLUDE_DIRS.$(TARGET_OS_TYPE))

#----------------------------------------------------------------------------------------------

CC_SRC_DIR=..

define CC_SRC_FILES.common
	server/server.cpp
endef

define CC_SRC_FILES.windows
	common/getopt.c
endef

CC_SRC_FILES += $(CC_SRC_FILES.common) $(CC_SRC_FILES.$(TARGET_OS_TYPE))

#----------------------------------------------------------------------------------------------

include $(MK)/rules
