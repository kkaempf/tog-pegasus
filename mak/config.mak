################################################################################
##
## Get external environment variables. Note that all external environment
## variables begin with "PEGASUS_".
##
################################################################################

ifdef PEGASUS_OS_TYPE
  OS_TYPE = $(PEGASUS_OS_TYPE)
else
  OS_TYPE = windows
endif

ifdef PEGASUS_BUILD
  BUILD_DIR = $(subst \,/,$(PEGASUS_BUILD))
else
  BUILD_DIR = $(ROOT)/build
endif

ifdef PEGASUS_ACE_ROOT
  ACE_ROOT = $(subst \,/,$(PEGASUS_ACE_ROOT))
else
  ERROR = PEGASUS_ACE_ROOT_UNDEFINED
PEGASUS_ACE_ROOT_UNDEFINED:
	@ echo PEGASUS_ACE_ROOT environment variable undefined
	@ exit 1
endif


################################################################################

OBJ_DIR = $(BUILD_DIR)/obj/$(DIR)
BIN_DIR = $(BUILD_DIR)/bin
LIB_DIR = $(BUILD_DIR)/lib
REPOSITORY_ROOT = $(BUILD_DIR)

ifeq ($(OS_TYPE),windows)
include $(ROOT)/mak/config-windows.mak
else
include $(ROOT)/mak/config-unix.mak
endif
