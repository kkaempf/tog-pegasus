//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
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
//==============================================================================
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMDateTime.h>

#include "IndicationProvider.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

static IndicationResponseHandler * _handler = 0; 
static Boolean _enabled = false;
static Uint32 _nextUID = 0;
static Uint32 _numSubscriptions = 0;

void _generateIndication (
    IndicationResponseHandler * handler,
    const CIMName methodName);

IndicationProvider::IndicationProvider (void) throw ()
{
}

IndicationProvider::~IndicationProvider (void) throw ()
{
}

void IndicationProvider::initialize (CIMOMHandle & cimom)
{
}

void IndicationProvider::terminate (void)
{
}

void IndicationProvider::enableIndications (
    IndicationResponseHandler & handler)
{
    _enabled = true;
    _handler = &handler;
}

void _generateIndication (
    IndicationResponseHandler * handler,
    const CIMName methodName)
{
    if (_enabled)
    {
	CIMInstance indicationInstance (CIMName("RT_TestIndication"));

        CIMObjectPath path ;
        path.setNameSpace("root/SampleProvider");
        path.setClassName("RT_TestIndication");

        indicationInstance.setPath(path);

        char buffer[32];
        sprintf(buffer, "%d", _nextUID++);
        indicationInstance.addProperty
            (CIMProperty ("IndicationIdentifier",String(buffer)));

	CIMDateTime currentDateTime = CIMDateTime::getCurrentDateTime ();
	indicationInstance.addProperty
            (CIMProperty ("IndicationTime", currentDateTime));

	Array<String> correlatedIndications; 
	indicationInstance.addProperty
            (CIMProperty ("CorrelatedIndications", correlatedIndications));

        indicationInstance.addProperty
            (CIMProperty ("MethodName", CIMValue(methodName.getString())));
        
        CIMIndication cimIndication (indicationInstance);

        handler->deliver (indicationInstance);
    }
}

void IndicationProvider::disableIndications (void)
{
    _enabled = false;
    _handler->complete ();
}

void IndicationProvider::createSubscription (
    const OperationContext & context,
    const CIMObjectPath & subscriptionName,
    const Array <CIMObjectPath> & classNames,
    const CIMPropertyList & propertyList,
    const Uint16 repeatNotificationPolicy)
{
    _numSubscriptions++;
}

void IndicationProvider::modifySubscription (
    const OperationContext & context,
    const CIMObjectPath & subscriptionName,
    const Array <CIMObjectPath> & classNames,
    const CIMPropertyList & propertyList,
    const Uint16 repeatNotificationPolicy)
{
    _generateIndication(_handler, "modifySubscription");
}

void IndicationProvider::deleteSubscription (
    const OperationContext & context,
    const CIMObjectPath & subscriptionName,
    const Array <CIMObjectPath> & classNames)
{
    _numSubscriptions--;

    if (_numSubscriptions == 0)
        _enabled = false;
}

void IndicationProvider::invokeMethod(
        const OperationContext & context,
        const CIMObjectPath & objectReference,
        const CIMName & methodName,
        const Array<CIMParamValue> & inParameters,
        MethodResultResponseHandler & handler)
{
        Boolean sendIndication = false;
        handler.processing();

        if (objectReference.getClassName().equal ("RT_TestIndication") &&
	    _enabled)
        {                
            if(methodName.equal("SendTestIndication"))
            {
                sendIndication = true;
                handler.deliver( CIMValue( 0 ) );
             }
        }

        else
        {
             handler.deliver( CIMValue( 1 ) );
	     PEGASUS_STD(cout) << "Provider is not enabled." << PEGASUS_STD(endl);
        }

        handler.complete();

        if (sendIndication)
           _generateIndication(_handler,"generateIndication");
}

PEGASUS_NAMESPACE_END
