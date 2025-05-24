
define CC_PP_DEFS +=
	BOOST_ALL_NO_LIB
endef

CC_PP_DEFS += $(CC_PP_DEFS.$(TARGET_OS_TYPE)) $(CC_PP_DEFS.$(TARGET_OS))

#----------------------------------------------------------------------------------------------

define CC_INCLUDE_DIRS +=
	.
	..
endef

CC_INCLUDE_DIRS += $(CC_INCLUDE_DIRS.$(TARGET_OS_TYPE))

#----------------------------------------------------------------------------------------------

define CC_SRC_FILES.etc
	db/oracle/collection.cpp
	db/oracle/cursor.cpp
	db/oracle/database.cpp
	db/oracle/large-objects.cpp
	db/oracle/object.cpp
	db/oracle/sql/commands.cpp
	db/oracle/values.cpp

	matlab/mex/array.cpp
	matlab/mex/file.cpp
	matlab/mex/function.cpp
	matlab/mex/matrix.cpp
	matlab/mex/module.cpp
	matlab/mex/string.cpp
	matlab/mex/struct.cpp
	matlab/mex/vector.cpp
	matlab/simulink/lib-template.cpp
	matlab/simulink/sfunction.cpp
	matlab/simulink/template.cpp
endef

#----------------------------------------------------------------------------------------------

define CC_SRC_FILES +=
	exceptions/general.cpp

	files/directory.cpp
	files/file.cpp
	files/filename.cpp
	files/open-file.cpp
	files/path.cpp

	system/general.cpp
	system/env.cpp

	text/args.cpp
	text/text.cpp
	text/strings.cpp
	text/ini-file.cpp
	text/json.cpp
	text/yaml.cpp
	text/docopt.cpp
	
	types/block.cpp
endef

#----------------------------------------------------------------------------------------------

define CC_SRC_FILES.windows
#	db/registry/key.cpp
#	db/registry/value.cpp
	
#	geometry/matrix.cpp

#	graphics/bitmap-viewer.cpp
#	graphics/sgi-ifl/image.cpp
#	graphics/win32/bitmap.cpp
#	graphics/win32/color.cpp
#	graphics/win32/device-context.cpp
#	graphics/win32/graphic-objects.cpp
#	graphics/win32/shapes.cpp
	
#	inet/general.cpp
#	inet/internet.cpp
#	inet/network.cpp

	ipc/pipe.cpp

#	lang/perl/general.cpp

#	math/discrete/unordered-selector.cpp

#	media/avi-file.cpp

#	net/address.cpp
#	net/basics.cpp
#	net/host.cpp
#	net/interface.cpp
#	net/secure-socket.cpp
#	net/socket.cpp

#	security/context.cpp
#	security/general.cpp

#	sync/atomic.cpp
#	sync/event.cpp
#	sync/mutex.cpp
#	sync/thread.cpp

	system/event.cpp
	system/mutex.cpp
	system/process.cpp
	system/thread.cpp
#	system/uuid.cpp
	system/wait-object.cpp

	system/win32/instance.cpp
	system/win32/job.cpp
#	system/win32/localhost.cpp
	system/win32/module.cpp
	system/win32/resources.cpp

#	text/command-line.cpp
endef

#----------------------------------------------------------------------------------------------

define CC_SRC_FILES.linux
#	system/event.cpp
#	system/mutex.cpp
#	system/process.cpp
#	system/thread.cpp
#	system/wait-object.cpp
endef

#----------------------------------------------------------------------------------------------

CC_SRC_FILES += $(CC_SRC_FILES.$(TARGET_OS_TYPE))

#----------------------------------------------------------------------------------------------

define TESTS +=
endef
