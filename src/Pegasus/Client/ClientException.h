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
// $Log: ClientException.h,v $
// Revision 1.1  2001/01/14 19:50:32  mike
// Initial revision
//
//
//END_HISTORY

////////////////////////////////////////////////////////////////////////////////
//
// Exception.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_ClientException_h
#define Pegasus_ClientException_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Exception.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_CLIENT_LINKAGE FailedToConnect : public Exception
{
public:

    static const char MSG[];

    FailedToConnect() : Exception(MSG) { }
};

class PEGASUS_CLIENT_LINKAGE AlreadyConnected: public Exception
{
public:

    static const char MSG[];

    AlreadyConnected() : Exception(MSG) { }
};

class PEGASUS_CLIENT_LINKAGE NotConnected: public Exception
{
public:

    static const char MSG[];

    NotConnected() : Exception(MSG) { }
};

class PEGASUS_CLIENT_LINKAGE TimedOut: public Exception
{
public:

    static const char MSG[];

    TimedOut() : Exception(MSG) { }
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_ClientException_h */
