include $(ROOT)/mak/config-unix.mak

OS = aix

ARCHITECTURE = rs

COMPILER = xlc

PLATFORM_VERSION_SUPPORTED = yes

SYS_INCLUDES = -I/usr/vacpp/include

DEFINES = -DPEGASUS_PLATFORM_$(PEGASUS_PLATFORM)

DEPEND_INCLUDES =

DEPEND_DEFINES = -D__IBMCPP__=500

ifdef PEGASUS_DEBUG
FLAGS = -g
else
FLAGS = -O2
endif

FLAGS += -qrtti=dyna 

SYS_LIBS = -ldl 

# SSL support
PEGASUS_HAS_SSL = yes

ifdef PEGASUS_HAS_SSL
OPENSSL_HOME = /usr/linux
endif

CXX = xlC_r

SH = sh

YACC = bison

COPY = cp

MOVE = mv

LIB_SUFFIX = .so

AIX_LIB_PRIORITY = 0
AIX_LD_HALT = 8

ifndef PEGASUS_USE_MU_DEPEND
PEGASUS_HAS_MAKEDEPEND = yes
endif

PEGASUS_SUPPORTS_DYNLIB = yes
