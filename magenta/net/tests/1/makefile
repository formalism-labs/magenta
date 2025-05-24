
define CC_PP_DEFS.common
endef

define CC_PP_DEFS.windows
endef

CC_PP_DEFS += $(CC_PP_DEFS.common) $(CC_PP_DEFS.$(TARGET_OS_TYPE))

#----------------------------------------------------------------------------------------------

define CC_INCLUDE_DIRS.common
	../../../..
	$(VROOT)/googletest/googletest/include
endef

define CC_INCLUDE_DIRS.windows
endef

CC_INCLUDE_DIRS += $(CC_INCLUDE_DIRS.common) $(CC_INCLUDE_DIRS.$(TARGET_OS_TYPE))

#----------------------------------------------------------------------------------------------

define CC_SRC_FILES.common
	test1.cpp
	test2.cpp
	test3.cpp
endef

define CC_SRC_FILES.windows
endef

CC_SRC_FILES += $(CC_SRC_FILES.common) $(CC_SRC_FILES.$(TARGET_OS_TYPE))
