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

#ifndef Pegasus_ParamValueRep_h
#define Pegasus_ParamValueRep_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/InternalException.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Sharable.h>
#include <Pegasus/Common/CIMValue.h>
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/Buffer.h>

PEGASUS_NAMESPACE_BEGIN

class CIMParamValueRep : public Sharable
{
public:

    CIMParamValueRep(
        String parameterName,
        CIMValue value,
        Boolean isTyped=true);

    ~CIMParamValueRep();

    const String & getParameterName() const
    {
        return _parameterName;
    }

    const CIMValue & getValue() const
    {
        return _value;
    }

    Boolean isTyped() const
    {
        return _isTyped;
    }

    void setParameterName(String& parameterName);

    void setValue(CIMValue& value);

    void setIsTyped(Boolean isTyped);

    void toXml(Buffer& out) const;

    CIMParamValueRep* clone() const
    {
        return new CIMParamValueRep(*this);
    }

private:

    CIMParamValueRep(const CIMParamValueRep& x);

    CIMParamValueRep();    // Unimplemented
    CIMParamValueRep& operator=(const CIMParamValueRep& x);    // Unimplemented

    String _parameterName;
    CIMValue _value;
    Boolean _isTyped;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_ParamValueRep_h */
