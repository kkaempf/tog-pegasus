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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include "PropertyProvider.h"

PEGASUS_NAMESPACE_BEGIN

PropertyProvider::PropertyProvider(void)
{
}

PropertyProvider::~PropertyProvider(void)
{
}

void PropertyProvider::initialize(CIMOMHandle & cimom)
{
	CIMObjectPath reference1("Sample_PropertyProviderClass.Identifier=1");
	CIMProperty property1("Message", CIMValue("Hello World"));
	
	_properties.append(property1);
	_instanceNames.append(reference1);
	
	CIMObjectPath reference2("Sample_PropertyProviderClass.Identifier=2");
	CIMProperty property2("Message", CIMValue("Yo Planet"));
	
	_properties.append(property2);
	_instanceNames.append(reference2);
}

void PropertyProvider::terminate(void)
{
}

void PropertyProvider::getProperty(
	const OperationContext & context,
	const CIMObjectPath & instanceReference,
	const String & propertyName,
	ValueResponseHandler & handler)
{
	// convert a potential fully qualified reference into a local reference
	// (class name and keys only).
	CIMObjectPath localReference = CIMObjectPath(
		String(),
		CIMNamespaceName(),
		instanceReference.getClassName(),
		instanceReference.getKeyBindings());

	// begin processing the request
	handler.processing();
	
	for(Uint32 i = 0, n = _properties.size(); i < n; i++)
	{
		if(localReference == _instanceNames[i])
		{
			// deliver the property value associated with the specified instance
			handler.deliver(_properties[i].getValue());

			// exit loop
			break;
		}
	}

	// complete processing the request
	handler.complete();
}

void PropertyProvider::setProperty(
	const OperationContext & context,
	const CIMObjectPath & instanceReference,
	const String & propertyName,
	const CIMValue & newValue,
	ResponseHandler & handler)
{
	// convert a potential fully qualified reference into a local reference
	// (class name and keys only).
	CIMObjectPath localReference = CIMObjectPath(
		String(),
		CIMNamespaceName(),
		instanceReference.getClassName(),
		instanceReference.getKeyBindings());

	// begin processing the request
	handler.processing();
	
	for(Uint32 i = 0, n = _properties.size(); i < n; i++)
	{
		if(localReference == _instanceNames[i])
		{
			// update the property value associated with the specified instance
			_properties[i].setValue(newValue);

			// deliver the property value associated with the specified instance
			handler.deliver(_properties[i].getValue());

			// exit loop
			break;
		}
	}

	// complete processing the request
	handler.complete();
}

PEGASUS_NAMESPACE_END
