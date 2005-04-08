//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Author: Carol Ann Krug Graves, Hewlett-Packard Company 
//             (carolann_graves@hp.com)
//
// Modified By: 
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_IndicationOperationAggregate_h
#define Pegasus_IndicationOperationAggregate_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Server/Linkage.h>

#include "ProviderClassList.h"

PEGASUS_NAMESPACE_BEGIN

/**

    IndicationOperationAggregate is the class that manages the aggregation of 
    indication provider responses to requests sent by the IndicationService.  
    This class is modeled on the OperationAggregate class used by the
    CIMOperationRequestDispatcher.

    @author  Hewlett-Packard Company

 */
class PEGASUS_SERVER_LINKAGE IndicationOperationAggregate
{
public:

    /**
        Constructs an IndicationOperationAggregate instance.

        @param   origRequest           the original request, if any, received by
                                           the Indication Service
        @param   indicationSubclasses  the list of indication subclasses for the
                                           subscription
     */
    IndicationOperationAggregate (
        CIMRequestMessage * origRequest,
        const Array <CIMName> & indicationSubclasses);

    ~IndicationOperationAggregate ();

    /**
        Determines if the instance is valid, based on the magic number set 
        in the constructor.

        @return  TRUE, if valid
                 FALSE, otherwise
    */
    Boolean valid ();
    
    /**
        Gets the original request, if any,  received by the IndicationService 
        for this aggregation.  The original request may be Create Instance, 
        Modify Instance, or Delete Instance.  In the cases of Deletion of an 
        Expired or Referencing Subscription, there is no original request.

        @return  a pointer to the request, if there is a request
                 0, otherwise
    */
    CIMRequestMessage * getOrigRequest ();

    /**
        Gets the message type of the original request, if any, received by the 
        IndicationService.

        @return  the request type, if there is a request
                 0, otherwise
    */
    Uint32 getOrigType ();

    /**
        Gets the message ID of the original request, if any, received by the 
        IndicationService.

        @return  the message ID, if there is a request
                 String::EMPTY, otherwise
    */
    String getOrigMessageId ();

    /**
        Gets the destination of the original request, if any, received by the 
        IndicationService.

        @return  the destination, if there is a request
                 0, otherwise
    */
    Uint32 getOrigDest ();

    /**
        Determines if the original request requires a response, based on the 
        type of the original request.  Create Instance, Modify Instance, and
        Delete Instance requests require a response.

        @return  TRUE, if original request requires a response
                 FALSE, otherwise
    */
    Boolean requiresResponse ();
    
    /**
        Gets the list of indication subclasses for the subscription.

        @return  the list of indication subclasses
    */
    Array <CIMName> & getIndicationSubclasses ();

    /**
        Stores the object path of the created instance in the operation 
        aggregate object, if original request was to create a subscription 
        instance.

        @param   path                  the object path of the created instance
    */
    void setPath (const CIMObjectPath & path);

    /**
        Gets the object path of the created instance, if original request was
        to create a subscription instance.

        @return  CIMObjectPath of the created instance
    */
    CIMObjectPath & getPath ();

    /**
        Gets the number of requests to be issued for this aggregation.

        @return  number of requests to be issued
    */
    Uint32 getNumberIssued ();

    /**
        Sets the number of requests to be issued for this aggregation.

        @param   i                     the number of requests
    */
    void setNumberIssued (Uint32 i);

    /**
        Appends a new response to the response list for this aggregation.  

        @param   response              the response

        @return  TRUE, if all expected responses have now been received
                 FALSE, otherwise
    */
    Boolean appendResponse (CIMResponseMessage * response);

    /**
        Gets the count of responses received for this aggregation.

        @return  count of responses received
    */
    Uint32 getNumberResponses ();

    /**
        Gets the response at the specified position in the list for this 
        aggregation.

        @return  a pointer to the response
    */
    CIMResponseMessage * getResponse (const Uint32 & pos);

    /**
        Deletes the response at the specified position in the list for this 
        aggregation.

        @param   pos                   the position in the list of the response
                                           to be deleted
    */
    void deleteResponse (const Uint32 & pos);

    /**
        Appends a new request to the request list for this aggregation.  

        @param   request               the request
    */
    void appendRequest (CIMRequestMessage * request);

    /**
        Gets the count of requests issued for this aggregation.

        @return  count of requests issued
    */
    Uint32 getNumberRequests ();

    /**
        Gets the request at the specified position in the list for this 
        aggregation.

        @return  a pointer to the request
    */
    CIMRequestMessage * getRequest (const Uint32 & pos);

    /**
        Deletes the request at the specified position in the list for this 
        aggregation.

        @param   pos                   the position in the list of the request
                                           to be deleted
    */
    void deleteRequest (const Uint32 & pos);

    /**
        Finds the provider that sent the response with the specified message ID.

        @return  a ProviderClassList struct for the provider that sent the 
                     response
    */
    ProviderClassList findProvider (const String & messageId);
    
private:
    /** 
        Hidden (unimplemented) copy constructor
     */
    IndicationOperationAggregate (const IndicationOperationAggregate & x) { }

    CIMRequestMessage * _origRequest;
    Array <CIMName> _indicationSubclasses;
    CIMObjectPath _path;
    Uint32 _numberIssued;
    Array <CIMRequestMessage *> _requestList;
    Mutex _appendRequestMutex;
    Array <CIMResponseMessage *> _responseList;
    Mutex _appendResponseMutex;
    Uint32 _magicNumber;
    static const Uint32 _theMagicNumber;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_IndicationOperationAggregate_h */
