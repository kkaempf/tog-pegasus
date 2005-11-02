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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include "TestProviderB.h"

PEGASUS_NAMESPACE_BEGIN

static CIMObjectPath _convertObjectPathBtoA(const CIMObjectPath & objectPath)
{
    CIMObjectPath temp = objectPath;

    temp.setClassName("TST_InstanceA");

    return(temp);
}

static CIMObjectPath _convertObjectPathAtoB(const CIMObjectPath & objectPath)
{
    CIMObjectPath temp = objectPath;

    temp.setClassName("TST_InstanceB");

    return(temp);
}

static CIMInstance _convertInstanceAtoB(const CIMInstance & instance)
{
    CIMInstance temp("TST_InstanceB");

    temp.addProperty(CIMProperty("Name", instance.getProperty(instance.findProperty("Name")).getValue()));
    temp.addProperty(CIMProperty("s", instance.getPath().toString()));

    temp.setPath(_convertObjectPathAtoB(instance.getPath()));

    return(temp);
}

TestProviderB::TestProviderB(void)
{
}

TestProviderB::~TestProviderB(void)
{
}

void TestProviderB::initialize(CIMOMHandle & cimom)
{
	_cimom = cimom;
}

void TestProviderB::terminate(void)
{
    delete this;
}

void TestProviderB::getInstance(
	const OperationContext & context,
	const CIMObjectPath & instanceReference,
	const Boolean includeQualifiers,
	const Boolean includeClassOrigin,
	const CIMPropertyList & propertyList,
	InstanceResponseHandler & handler)
{
    handler.processing();

    CIMInstance cimInstance = 
        _cimom.getInstance(
            context,
            instanceReference.getNameSpace(),
            _convertObjectPathBtoA(instanceReference),
            false,
            includeQualifiers,
            includeClassOrigin,
            propertyList);

    handler.deliver(_convertInstanceAtoB(cimInstance));

    handler.complete();
}

void TestProviderB::enumerateInstances(
	const OperationContext & context,
	const CIMObjectPath & classReference,
	const Boolean includeQualifiers,
	const Boolean includeClassOrigin,
	const CIMPropertyList & propertyList,
	InstanceResponseHandler & handler)
{
    handler.processing();

    Array<CIMInstance> cimInstances =
        _cimom.enumerateInstances(
            context,
            classReference.getNameSpace(),
            _convertObjectPathBtoA(classReference).getClassName(),
            true,
            false,
            includeQualifiers,
            includeClassOrigin,
            propertyList);

    for(Uint32 i = 0, n = cimInstances.size(); i < n; i++)
    {
        handler.deliver(_convertInstanceAtoB(cimInstances[i]));
    }

    handler.complete();
}

void TestProviderB::enumerateInstanceNames(
	const OperationContext & context,
	const CIMObjectPath & classReference,
	ObjectPathResponseHandler & handler)
{
    handler.processing();

    Array<CIMObjectPath> cimInstanceNames =
        _cimom.enumerateInstanceNames(
            context,
            classReference.getNameSpace(),
            _convertObjectPathBtoA(classReference).getClassName());

    for(Uint32 i = 0, n = cimInstanceNames.size(); i < n; i++)
    {
        handler.deliver(_convertObjectPathAtoB(cimInstanceNames[i]));
    }

    handler.complete();
}

void TestProviderB::modifyInstance(
	const OperationContext & context,
	const CIMObjectPath & instanceReference,
	const CIMInstance & instanceObject,
	const Boolean includeQualifiers,
	const CIMPropertyList & propertyList,
	ResponseHandler & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

void TestProviderB::createInstance(
	const OperationContext & context,
	const CIMObjectPath & instanceReference,
	const CIMInstance & instanceObject,
	ObjectPathResponseHandler & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

void TestProviderB::deleteInstance(
	const OperationContext & context,
	const CIMObjectPath & instanceReference,
	ResponseHandler & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

PEGASUS_NAMESPACE_END
