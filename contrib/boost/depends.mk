
define BOOST_LIB_NAMES.mingw-w64
	atomic
	chrono
	container
	context
	coroutine
	date_time
	exception
	fiber
	filesystem
	graph
	iostreams
	locale
	log
	log_setup
	math_c99
	math_c99f
	math_c99l
	math_tr1
	math_tr1f
	math_tr1l
	prg_exec_monitor
	program_options
	random
	regex
	serialization
	signals
	system
	test_exec_monitor
	thread
	timer
	type_erasure
	unit_test_framework
	wave
	wserialization
endef

define BOOST_LIB_NAMES.win32
	atomic
	chrono
	container
	context
	coroutine
	date_time
	exception
	filesystem
	graph
	iostreams
	locale
	log_setup
	log
	math_c99f
	math_c99l
	math_c99
	math_tr1f
	math_tr1l
	math_tr1
	prg_exec_monitor
	program_options
	random
	regex
	serialization
	signals
	system
	test_exec_monitor
	thread
	timer
	type_erasure
	unit_test_framework
	wave
	wserialization
endef

BOOST_LIB_NAMES=$(BOOST_LIB_NAMES.$(TARGET_OS))

BOOST_PATH=$(REPOSITORY_LIBS_PATH)/boost-1.64.0/$(BOOST_TARGET_PLATFORM)

define MODULE_BIN_DEPENDS.mingw-x64-gcc7.1-debug
	((lib :abs) ($(foreach L,$(BOOST_LIB_NAMES),$(BOOST_PATH)/libboost_$(L)-mgw71-mt-d-1_64.a)))
endef

define MODULE_BIN_DEPENDS.mingw-x64-gcc7.1-opt
	((lib :abs) ($(foreach L,$(BOOST_LIB_NAMES),$(BOOST_PATH)/libboost_$(L)-mgw71-mt-1_64.a)))
endef

define MODULE_BIN_DEPENDS.win-x64-msc19.11-debug
	((lib :abs) ($(foreach L,$(BOOST_LIB_NAMES),$(BOOST_PATH)/libboost_$(L)-vc-mt-sgd-1_64.lib)))
endef

define MODULE_BIN_DEPENDS.win-x64-msc19.11-opt
	((lib :abs) ($(foreach L,$(BOOST_LIB_NAMES),$(BOOST_PATH)/libboost_$(L)-vc-mt-s-1_64.lib)))
endef

MODULE_BIN_DEPENDS.common=$(MODULE_BIN_DEPENDS.$(BOOST_TARGET_PLATFORM)$(_VARIANT_FLAVOR))
