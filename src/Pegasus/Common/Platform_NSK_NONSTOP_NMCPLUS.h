//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000 The Open Group, BMC Software, Tivoli Systems, IBM,
// Compaq Computer Corporation
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
//==============================================================================
//
// Author: Rudy Schuet (rudy.schuet@compaq.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Platform_NSK_NONSTOP_NMCPLUS_h
#define Pegasus_Platform_NSK_NONSTOP_NMCPLUS_h

#include <Pegasus/Common/ConfigNsk.h> 

#define PEGASUS_OS_NSK   

#define PEGASUS_ARCHITECTURE_NONSTOP

#define PEGASUS_COMPILER_NMCPLUS

#define PEGASUS_UINT64 unsigned long

#define PEGASUS_SINT64 long long

#define PEGASUS_LLONG_MAX 9223372036854775807LL
#define PEGASUS_LLONG_MIN (-PEGASUS_LLONG_MAX - 1LL)

#define PEGASUS_ULLONG_MAX 18446744073709551615ULL

#define PEGASUS_HAVE_NAMESPACES

#define PEGASUS_HAVE_NO_STD

#define PEGASUS_HAVE_EXPLICIT

#define PEGASUS_HAVE_MUTABLE

#define PEGASUS_HAVE_FOR_SCOPE

#define PEGASUS_HAVE_TEMPLATE_SPECIALIZATION

#define PEGASUS_HAVE_BOOLEAN

#endif /* Pegasus_Platform_NSK_NONSTOP_NMCPLUS_h */
