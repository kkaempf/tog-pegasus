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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By:
//              Nag Boranna, Hewlett-Packard Company(nagaraja_boranna@hp.com)
//              Yi Zhou, Hewlett-Packard Company(yi_zhou@hp.com)
//              Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//              Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//              Mike Day, IBM (mdday@us.ibm.com)
//              Adrian Schuur, IBM (schuur@de.ibm.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_ProviderManagerService_h
#define Pegasus_ProviderManagerService_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/MessageQueueService.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/OperationContextInternal.h>
#include <Pegasus/Common/AutoPtr.h>
#include <Pegasus/Common/List.h>
#include <Pegasus/Common/Mutex.h>
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Server/ProviderRegistrationManager/ProviderRegistrationManager.h>

#include <Pegasus/ProviderManager2/ProviderManagerRouter.h>

#include <Pegasus/ProviderManager2/Linkage.h>

#ifdef PEGASUS_ZOS_SECURITY
// This include file will not be provided in the OpenGroup CVS for now.
// Do NOT try to include it in your compile
#include <Pegasus/ProviderManager2/ProviderManagerzOS_inline.h>
#endif

PEGASUS_NAMESPACE_BEGIN

#define IDLE_LIMIT 300

class PEGASUS_PPM_LINKAGE ProviderManagerService : public MessageQueueService
{
public:
    ProviderManagerService(
        ProviderRegistrationManager* providerRegistrationManager,
        CIMRepository* repository,
	ProviderManager* (*createDefaultProviderManagerCallback)());

    virtual ~ProviderManagerService();

    void unloadIdleProviders();

    static void indicationCallback(CIMProcessIndicationRequestMessage* request);
    static void responseChunkCallback(
        CIMRequestMessage* request, CIMResponseMessage* response);

    /**
        Callback function to be called upon detection of failure of a
        provider module.
     */
    static void providerModuleFailureCallback (const String & moduleName,
        const String & userName, Uint16);

private:
    ProviderManagerService();

    virtual Boolean messageOK(const Message* message);
    virtual void handleEnqueue();
    virtual void handleEnqueue(Message* message);

    virtual void _handle_async_request(AsyncRequest* request);

    static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL handleCimOperation(
        void* arg) ;

    void handleCimRequest(AsyncOpNode* op, Message* message);

    Message* _processMessage(CIMRequestMessage* request);

    static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL
        _unloadIdleProvidersHandler(void* arg) throw();

    void _updateProviderModuleStatus(
        CIMInstance& providerModule,
        const Array<Uint16>& removeStatus,
        const Array<Uint16>& appendStatus);

    static ProviderManagerService* providerManagerService;

    CIMRepository* _repository;

    List<AsyncOpNode,Mutex> _incomingQueue;
    List<AsyncOpNode,Mutex> _outgoingQueue;

    ProviderManagerRouter* _basicProviderManagerRouter;
    ProviderManagerRouter* _oopProviderManagerRouter;

    ProviderRegistrationManager* _providerRegistrationManager;

    static Uint32 _indicationServiceQueueId;

    /**
        Indicates the number of threads currently attempting to unload idle
        providers.  This value is used to prevent multiple threads from
        unloading idle providers at the same time.
     */
    AtomicInt _unloadIdleProvidersBusy;


};

// Auto class to encapsulate enabling and disabling
// of the pthread_security on z/OS
// For all other platforms this should be an empty class
// Targets: avoid ifdefs and keep code readable(clean)
#ifndef PEGASUS_ZOS_THREADLEVEL_SECURITY
// not z/OS == empty class
class PEGASUS_PPM_LINKAGE AutoPThreadSecurity
{
public:    
    AutoPThreadSecurity(const OperationContext& context) {};
};
#else

class PEGASUS_PPM_LINKAGE AutoPThreadSecurity
{
public:
    AutoPThreadSecurity(const OperationContext& context)
    {
                int err_num=enablePThreadSecurity(context);
                if (err_num!=0)
                {
                        // need a new CIMException for this
                        throw CIMException(CIM_ERR_ACCESS_DENIED,String(strerror(err_num)));
                }
    };

    ~AutoPThreadSecurity()
    {
        disablePThreadSecurity();
    };
};

#endif

PEGASUS_NAMESPACE_END

#endif
