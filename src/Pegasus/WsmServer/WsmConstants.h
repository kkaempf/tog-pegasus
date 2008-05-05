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

#ifndef Pegasus_WsmConstants_h
#define Pegasus_WsmConstants_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMName.h>

//
// Message Queue Names
//

#define PEGASUS_QUEUENAME_WSMREQDECODER       "WsmRequestDecoder"
#define PEGASUS_QUEUENAME_WSMRESPENCODER      "WsmResponseEncoder"
#define PEGASUS_QUEUENAME_WSMPROCESSOR        "WsmProcessor"

//
// Minimum allowable value for MaxEnvelopeSize
//

#define WSM_MIN_MAXENVELOPESIZE_VALUE 8192

//
// WS-Management URIs
//

#define WSM_ADDRESS_ANONYMOUS \
    "http://schemas.xmlsoap.org/ws/2004/08/addressing/role/anonymous"

// See DSP0227 R5.2-1, R6-1, and R7.1-7.
#define WSM_RESOURCEURI_ALLCLASSES \
    "http://schemas.dmtf.org/wbem/wscim/1/*"

#define WSM_RESOURCEURI_CIMSCHEMAV2 \
    "http://schemas.dmtf.org/wbem/wscim/1/cim-schema/2"


//
// Action URIs
//

#define WSM_ACTION_GET \
    "http://schemas.xmlsoap.org/ws/2004/09/transfer/Get"

#define WSM_ACTION_GET_RESPONSE \
    "http://schemas.xmlsoap.org/ws/2004/09/transfer/GetResponse"

#define WSM_ACTION_PUT \
    "http://schemas.xmlsoap.org/ws/2004/09/transfer/Put"

#define WSM_ACTION_PUT_RESPONSE \
    "http://schemas.xmlsoap.org/ws/2004/09/transfer/PutResponse"

#define WSM_ACTION_CREATE \
    "http://schemas.xmlsoap.org/ws/2004/09/transfer/Create"

#define WSM_ACTION_CREATE_RESPONSE \
    "http://schemas.xmlsoap.org/ws/2004/09/transfer/CreateResponse"

#define WSM_ACTION_DELETE \
    "http://schemas.xmlsoap.org/ws/2004/09/transfer/Delete"

#define WSM_ACTION_DELETE_RESPONSE \
    "http://schemas.xmlsoap.org/ws/2004/09/transfer/DeleteResponse"

#define WSM_ACTION_ENUMERATE \
    "http://schemas.xmlsoap.org/ws/2004/09/enumeration/Enumerate"

#define WSM_ACTION_ENUMERATE_RESPONSE \
    "http://schemas.xmlsoap.org/ws/2004/09/enumeration/EnumerateResponse"

#define WSM_ACTION_PULL \
    "http://schemas.xmlsoap.org/ws/2004/09/enumeration/Pull"

#define WSM_ACTION_PULL_RESPONSE \
    "http://schemas.xmlsoap.org/ws/2004/09/enumeration/PullResponse"

#define WSM_ACTION_RELEASE \
    "http://schemas.xmlsoap.org/ws/2004/09/enumeration/Release"

#define WSM_ACTION_RELEASE_RESPONSE \
    "http://schemas.xmlsoap.org/ws/2004/09/enumeration/ReleaseResponse"

#define WSM_ACTION_WSMAN_FAULT \
    "http://schemas.dmtf.org/wbem/wsman/1/wsman/fault"

#define WSM_ACTION_WSA_FAULT \
    "http://schemas.xmlsoap.org/ws/2004/08/addressing/fault"

#define WSM_ACTION_WXF_FAULT \
    "http://schemas.xmlsoap.org/ws/2004/09/transfer/fault"

#define WSM_ACTION_WSEN_FAULT \
    "http://schemas.xmlsoap.org/ws/2004/09/enumeration/fault"

#define WSM_ACTION_WSE_FAULT \
    "http://schemas.xmlsoap.org/ws/2004/08/eventing/fault"


//
// WS-Management FaultDetail URI values
//

#define WSMAN_FAULTDETAIL_ACTIONMISMATCH \
    "http://schemas.dmtf.org/wbem/wsman/1/wsman/faultDetail/ActionMismatch"

#define WSMAN_FAULTDETAIL_ADDRESSINGMODE \
    "http://schemas.dmtf.org/wbem/wsman/1/wsman/faultDetail/AddressingMode"

#define WSMAN_FAULTDETAIL_CHARACTERSET \
    "http://schemas.dmtf.org/wbem/wsman/1/wsman/faultDetail/CharacterSet"

#define WSMAN_FAULTDETAIL_FRAGMENTLEVELACCESS \
    "http://schemas.dmtf.org/wbem/wsman/1/wsman/faultDetail/FragmentLevelAccess"

#define WSMAN_FAULTDETAIL_INVALIDRESOURCEURI \
    "http://schemas.dmtf.org/wbem/wsman/1/wsman/faultDetail/InvalidResourceURI"

#define WSMAN_FAULTDETAIL_INVALIDVALUE \
    "http://schemas.dmtf.org/wbem/wsman/1/wsman/faultDetail/InvalidValue"

#define WSMAN_FAULTDETAIL_LOCALE \
    "http://schemas.dmtf.org/wbem/wsman/1/wsman/faultDetail/Locale"

#define WSMAN_FAULTDETAIL_OPERATIONTIMEOUT \
    "http://schemas.dmtf.org/wbem/wsman/1/wsman/faultDetail/OperationTimeout"

#define WSMAN_FAULTDETAIL_TYPEMISMATCH \
    "http://schemas.dmtf.org/wbem/wsman/1/wsman/faultDetail/TypeMismatch"

#define WSMAN_FAULTDETAIL_UNEXPECTEDSELECTORS \
    "http://schemas.dmtf.org/wbem/wsman/1/wsman/faultDetail/UnexpectedSelectors"

#define WSMAN_FAULTDETAIL_MINIMUMENVELOPELIMIT \
   "http://schemas.dmtf.org/wbem/wsman/1/wsman/faultDetail/MinimumEnvelopeLimit"

#define WSMAN_FAULTDETAIL_MAXENVELOPESIZE \
    "http://schemas.dmtf.org/wbem/wsman/1/wsman/faultDetail/MaxEnvelopeSize"

#define WSMAN_FAULTDETAIL_UNREPORTABLESUCCESS \
    "http://schemas.dmtf.org/wbem/wsman/1/wsman/faultDetail/UnreportableSuccess"

PEGASUS_NAMESPACE_BEGIN

//
// CIM Namespace Names
//

extern const CIMNamespaceName PEGASUS_DEFAULT_WSM_NAMESPACE;

PEGASUS_NAMESPACE_END

#endif /* Pegasus_WsmConstants_h */