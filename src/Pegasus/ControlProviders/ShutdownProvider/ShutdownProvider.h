//%////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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
//=============================================================================
//
// Author: Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//
// Modified By: 
//        Sushma Fernandes, Hewlett-Packard Company (sushma_fernandes@hp.com)
//        Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//
//%////////////////////////////////////////////////////////////////////////////


#ifndef ShutdownProvider_h
#define ShutdownProvider_h

///////////////////////////////////////////////////////////////////////////////
//  Shutdown Provider
///////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/ControlProviders/ShutdownProvider/Linkage.h>

#include <Pegasus/Provider/CIMMethodProvider.h>
//#include <Pegasus/Server/ShutdownService.h>


PEGASUS_NAMESPACE_BEGIN

/**
    The ShutdownProvider is a method provider that implements the shutdown
    method in the PG_ShutdownService class.  It provides response to the
    shutdown request from clients to shutdown cimom gracefully.
*/

class PEGASUS_SHUTDOWNPROVIDER_LINKAGE ShutdownProvider 
    : public CIMMethodProvider
{
public:

    /** Constructor */
    ShutdownProvider()
    {
        //
        // get an instance of Shutdown Service
        //
        // ATTN: How to find the shutdown service?
        //_shutdownService = ShutdownService::getInstance();
    }

    /** Destructor */
    virtual ~ShutdownProvider()
    {

    }

    void terminate(void)
    {

    }

    /**
        Invoke Method.  Used to shutdown cimom.
    */
    void invokeMethod(
        const OperationContext & context,
        const CIMObjectPath & objectReference,
        const CIMName & methodName,
        const Array<CIMParamValue> & inParameters,
        MethodResultResponseHandler & handler);

    /**
     Standard initialization function for the provider.
    */
    void initialize(CIMOMHandle& cimomHandle)
    {
        // This method should never be called, since this is an internal
        // control provider
    }

private:

    //
    // Shutdown Service Instance variable
    //
    //ShutdownService*      _shutdownService;
};

PEGASUS_NAMESPACE_END

#endif  // ShutdownProvider_h
