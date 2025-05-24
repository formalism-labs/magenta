
SRC_ROOT=../../../../../..
include $(SRC_ROOT)/freemasonBuild/freemason/4/main

include $(MK)/defs

#----------------------------------------------------------------------------------------------

define CC_PP_DEFS.common
	USE_SSL
endef

define CC_PP_DEFS.windows
endef

CC_PP_DEFS += $(CC_PP_DEFS.common) $(CC_PP_DEFS.$(TARGET_OS_TYPE))

#----------------------------------------------------------------------------------------------

define CC_INCLUDE_DIRS.common
endef

define CC_INCLUDE_DIRS.windows
	$(VROOT)/nbu.contrib/open-ssl/$(OPENSSL_VER)/windows
endef

CC_INCLUDE_DIRS += $(CC_INCLUDE_DIRS.common) $(CC_INCLUDE_DIRS.$(TARGET_OS_TYPE))

#----------------------------------------------------------------------------------------------

define CC_SRC_FILES.common
#	htpasswd.cpp
	match.cpp
	mini_httpd.cpp
	tdate_parse.cpp
endef

CC_SRC_FILES += $(CC_SRC_FILES.common) $(CC_SRC_FILES.$(TARGET_OS_TYPE))

#----------------------------------------------------------------------------------------------

include $(MK)/rules
