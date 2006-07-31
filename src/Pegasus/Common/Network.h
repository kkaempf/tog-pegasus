//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//==============================================================================
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Network_h
#define Pegasus_Network_h

#include <Pegasus/Common/Config.h>

//==============================================================================
//
// Network.h
//
//     This file includes network-related system-header files. Please include
//     this file directly rather than including system headers directly. If
//     special inclusions are necessary for any platform, please add them to
//     this file rather than other files. The reason for this file is to limit 
//     platform-specific conditional compilation expressions to only a few 
//     well-known header files.
// 
//==============================================================================

//------------------------------------------------------------------------------
//
// PEGASUS_OS_TYPE_WINDOWS network system header files
//
//------------------------------------------------------------------------------

#ifdef PEGASUS_OS_TYPE_WINDOWS
#   ifdef FD_SETSIZE
#       error 
	    "<Pegasus/Common/Network.h>: FD_SETSIZE is already defined. This "
	    "file must be included prior to any header file that defines "
	    "FD_SETSIZE, such as <windows.h>, <winsock.h>, or winsock2.h>."
#   endif
#   define FD_SETSIZE 1024
#   include <windows.h>
#   ifndef _WINSOCKAPI_
#       include <winsock2.h>
#   endif
#   include <wincrypt.h>
#endif

//------------------------------------------------------------------------------
//
// PEGASUS_OS_TYPE_UNIX or PEGASUS_OS_VMS network system header files.
//
//------------------------------------------------------------------------------

#if defined(PEGASUS_OS_TYPE_UNIX) || defined (PEGASUS_OS_VMS)
#   ifdef PEGASUS_PLATFORM_ZOS_ZSERIES_IBM
#       define _OPEN_SYS_SOCK_IPV6
#   endif
#   include <errno.h>
#   include <sys/types.h>
#   include <fcntl.h>
#   include <netdb.h>
#   include <netinet/in.h>
#   include <arpa/inet.h>
#   include <sys/socket.h>
#   include <sys/time.h>
#   ifndef PEGASUS_DISABLE_LOCAL_DOMAIN_SOCKET
#       include <unistd.h>
#       include <sys/un.h>
#   endif
#   ifdef PEGASUS_OS_OS400
#       include <unistd.cleinc>
#   else
#       include <unistd.h>
#   endif
#   ifdef PEGASUS_PLATFORM_ZOS_ZSERIES_IBM
#       ifndef TCP_NODELAY
#           define TCP_NODELAY 1
#       endif
#   else
#       include <netinet/tcp.h>
#   endif
#endif

//------------------------------------------------------------------------------
//
// PEGASUS_SOCKET_ERROR
//
//------------------------------------------------------------------------------

#ifdef PEGASUS_OS_TYPE_WINDOWS
#   define PEGASUS_SOCKET_ERROR SOCKET_ERROR
#else
#   define PEGASUS_SOCKET_ERROR (-1)
#endif

////////////////////////////////////////////////////////////////////////////////
//
// getSocketError()
//
////////////////////////////////////////////////////////////////////////////////

static inline int getSocketError()
{
#ifdef PEGASUS_OS_TYPE_WINDOWS
    return WSAGetLastError();
#else
    return errno;
#endif
}

//------------------------------------------------------------------------------
//
// PEGASUS_INVALID_SOCKET
//
//------------------------------------------------------------------------------

#ifdef PEGASUS_OS_TYPE_WINDOWS
#   define PEGASUS_INVALID_SOCKET INVALID_SOCKET
#else
#   define PEGASUS_INVALID_SOCKET (-1)
#endif

//------------------------------------------------------------------------------
//
// SocketHandle
//
//------------------------------------------------------------------------------

#ifdef PEGASUS_OS_TYPE_WINDOWS
typedef SOCKET SocketHandle;
#else
typedef int SocketHandle;
#endif

//------------------------------------------------------------------------------
//
// SocketLength
//
//------------------------------------------------------------------------------

#if defined(PEGASUS_PLATFORM_NSK_NONSTOP_NMCPLUS) || \
    defined(PEGASUS_PLATFORM_OS400_ISERIES_IBM) || \
    defined(PEGASUS_PLATFORM_SOLARIS_SPARC_GNU) || \
    defined(PEGASUS_PLATFORM_TRU64_ALPHA_DECCXX) || \
    defined(PEGASUS_PLATFORM_WIN32_IX86_MSVC)
    typedef int SocketLength;
#elif defined(PEGASUS_PLATFORM_VMS_ALPHA_DECCXX) || \
    defined(PEGASUS_PLATFORM_VMS_IA64_DECCXX) || \
    defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM)
    typedef size_t SocketLength;
#elif defined(PEGASUS_PLATFORM_HPUX_IA64_ACC) && \
    !defined(_XOPEN_SOURCE_EXTENDED)
    typedef int SocketLength;
#elif defined(PEGASUS_PLATFORM_HPUX_PARISC_ACC) && \
    !defined(_XOPEN_SOURCE_EXTENDED)
    typedef int SocketLength;
#elif defined(PEGASUS_PLATFORM_SOLARIS_SPARC_CC) && \
    defined(SUNOS_5_6)
    typedef int SocketLength;
#else
    typedef socklen_t SocketLength;
#endif

#endif  /* Pegasus_Network_h */