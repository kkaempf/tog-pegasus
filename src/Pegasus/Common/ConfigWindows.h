//BEGIN_LICENSE
//
// Copyright (c) 2000 The Open Group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//END_LICENSE
//BEGIN_HISTORY
//
// Author:
//
// $Log: ConfigWindows.h,v $
// Revision 1.1  2001/01/14 19:50:40  mike
// Initial revision
//
//
//END_HISTORY

////////////////////////////////////////////////////////////////////////////////
//
// Config_iX86_win98_msvc.h
//
//	This file contains definitions for the Intel X86 running Windows98
//	using the Microsoft Visual C++ compiler. This file must
//	be included in Config.h to be used.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Config_iX86_win98_msvc_h
#define Pegasus_Config_iX86_win98_msvc_h

#define PEGASUS_MACHINE_IX86
#define PEGASUS_OS_WIN98
// #define PEGASUS_OS_TYPE_WINDOWS
#define PEGASUS_COMPILER_MSVC

// ATTN: use full qualification of cout!

// namespace std { };
// using namespace std;

#define PEGASUS_NAMESPACE_BEGIN namespace Pegasus {

#define PEGASUS_NAMESPACE_END }

PEGASUS_NAMESPACE_BEGIN

#ifdef PEGASUS_COMMON_INTERNAL
# define PEGASUS_COMMON_LINKAGE __declspec(dllexport)
#else
# define PEGASUS_COMMON_LINKAGE __declspec(dllimport)
#endif

#ifdef PEGASUS_REPOSITORY_INTERNAL
# define PEGASUS_REPOSITORY_LINKAGE __declspec(dllexport)
#else
# define PEGASUS_REPOSITORY_LINKAGE __declspec(dllimport)
#endif

#ifdef PEGASUS_PROTOCOL_INTERNAL
# define PEGASUS_PROTOCOL_LINKAGE __declspec(dllexport)
#else
# define PEGASUS_PROTOCOL_LINKAGE __declspec(dllimport)
#endif

#ifdef PEGASUS_CLIENT_INTERNAL
# define PEGASUS_CLIENT_LINKAGE __declspec(dllexport)
#else
# define PEGASUS_CLIENT_LINKAGE __declspec(dllimport)
#endif

#ifdef PEGASUS_SERVER_INTERNAL
# define PEGASUS_SERVER_LINKAGE __declspec(dllexport)
#else
# define PEGASUS_SERVER_LINKAGE __declspec(dllimport)
#endif

#define for if (0) ; else for

typedef unsigned char Uint8;
typedef char Sint8;
typedef unsigned short Uint16;
typedef short Sint16;
typedef unsigned int Uint32;
typedef int Sint32;
typedef float Real32;
typedef double Real64;
typedef bool Boolean;
typedef unsigned __int64 Uint64;
typedef __int64 Sint64;

#pragma warning ( disable : 4251 )

#define PEGASUS_IOS_BINARY , ios::binary

PEGASUS_NAMESPACE_END

#endif  /* Pegasus_Config_iX86_win98_msvc_h */
