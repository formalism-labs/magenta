
define CC_PP_DEFS.common
	GTEST_HAS_TR1_TUPLE=0
endef

CC_PP_DEFS += $(CC_PP_DEFS.common) $(CC_PP_DEFS.$(TARGET_OS_TYPE)) $(CC_PP_DEFS.$(TARGET_OS))

#----------------------------------------------------------------------------------------------

define CC_INCLUDE_DIRS.common
	$(GOOGLETEST_ROOT)
endef

CC_INCLUDE_DIRS += $(CC_INCLUDE_DIRS.common) $(CC_INCLUDE_DIRS.$(TARGET_OS_TYPE))

#----------------------------------------------------------------------------------------------

CC_SRC_DIR=$(GOOGLETEST_ROOT)/src

define CC_SRC_FILES.common
	gtest-all.cc
	gtest_main.cc
#	gtest-rv.cc
endef

CC_SRC_FILES += $(CC_SRC_FILES.common) $(CC_SRC_FILES.$(TARGET_OS_TYPE))
