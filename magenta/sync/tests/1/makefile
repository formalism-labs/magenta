
define SPECS
	(PLATFORM=rv-win32 DEBUG=1)
	(PLATFORM=rv-win32 OPT=1)
#	(PLATFORM=rv-tamar OPT=1)
	(PLATFORM=rv-linux-x86 DEBUG=1)
	(PLATFORM=rv-linux-ppc OPT=1)
endef

#----------------------------------------------------------------------------------------------

define CC_PP_DEFS +=
endef

define CC_PP_DEFS.windows
endef

CC_PP_DEFS += $(CC_PP_DEFS.$(TARGET_OS_TYPE))

#----------------------------------------------------------------------------------------------

define CC_INCLUDE_DIRS +=
	$(VROOT)/freemasonBuild/gtest/1.4.0/include
endef

define CC_INCLUDE_DIRS.windows
endef

CC_INCLUDE_DIRS += $(CC_INCLUDE_DIRS.$(TARGET_OS_TYPE))

#----------------------------------------------------------------------------------------------

define CC_SRC_FILES +=
	test1.cpp
endef

define CC_SRC_FILES.windows
endef

CC_SRC_FILES += $(CC_SRC_FILES.$(TARGET_OS_TYPE))
