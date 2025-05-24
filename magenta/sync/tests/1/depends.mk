
define MODULE_DEPENDS.common
	(rvfc.v2,$(VROOT)/magenta)
	(swInfra-InfraStruct.2,$(VROOT)/swInfra/InfraStruct/2)
	(freemasonBuild-gtest-1.4.0,$(VROOT)/freemasonBuild/gtest/1.4.0)
endef

define PREBUILT_MODULES.global
#	swInfra-InfraStruct.2
#	rvfc
#	loggerInfra
endef
