
define MODULE_DEPENDS.common
	(rvfc.v2-net-tests-2-client,$(VROOT)/rvfc/v2/net/tests/2/client)
	(rvfc.v2-net-tests-2-server,$(VROOT)/rvfc/v2/net/tests/2/server)
endef

define PREBUILT_MODULES.global
	nbu.contrib-openssl
endef
