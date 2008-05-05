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
//=============================================================================
//
//%////////////////////////////////////////////////////////////////////////////

#include <cctype>
#include <cstdio>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/MessageLoader.h>
#include <Pegasus/WsmServer/WsmConstants.h>
#include "CimToWsmResponseMapper.h"

PEGASUS_NAMESPACE_BEGIN

CimToWsmResponseMapper::CimToWsmResponseMapper()
{
}

CimToWsmResponseMapper::~CimToWsmResponseMapper()
{
}

WsmResponse* CimToWsmResponseMapper::mapToWsmResponse(
    const WsmRequest* wsmRequest,
    const CIMResponseMessage* message)
{
    AutoPtr<WsmResponse> wsmResponse;

    if (message->cimException.getCode() != CIM_ERR_SUCCESS)
    {
        wsmResponse.reset(_mapToWsmFaultResponse(wsmRequest, message));
    }
    else
    {
        switch (message->getType())
        {
            case CIM_GET_INSTANCE_RESPONSE_MESSAGE:
                wsmResponse.reset(_mapToWsmGetResponse(
                    (WsmGetRequest*) wsmRequest,
                    (CIMGetInstanceResponseMessage*) message));
                break;

            case CIM_MODIFY_INSTANCE_RESPONSE_MESSAGE:
                wsmResponse.reset(_mapToWsmPutResponse(
                    (WsmPutRequest*) wsmRequest,
                    (CIMModifyInstanceResponseMessage*) message));
                break;

            case CIM_CREATE_INSTANCE_RESPONSE_MESSAGE:
                wsmResponse.reset(_mapToWsmCreateResponse(
                    (WsmCreateRequest*) wsmRequest,
                    (CIMCreateInstanceResponseMessage*) message));
                break;

            case CIM_DELETE_INSTANCE_RESPONSE_MESSAGE:
                wsmResponse.reset(_mapToWsmDeleteResponse(
                    (WsmDeleteRequest*) wsmRequest,
                    (CIMDeleteInstanceResponseMessage*) message));
                break;

            default:
                PEGASUS_ASSERT(0);
                break;
        }
    }
    return wsmResponse.release();
}

WsmFaultResponse* CimToWsmResponseMapper::_mapToWsmFaultResponse(
    const WsmRequest* wsmRequest,
    const CIMResponseMessage* response)
{
    WsmFault fault = mapCimExceptionToWsmFault(response->cimException);

    WsmFaultResponse* wsmResponse = new WsmFaultResponse(wsmRequest, fault);

    return wsmResponse;
}

WsmFault CimToWsmResponseMapper::mapCimExceptionToWsmFault(
    const CIMException& cimException)
{
    WsmFault::Subcode subcode;
    const ContentLanguageList& languageList =
        cimException.getContentLanguages();
    String reason = TraceableCIMException(cimException).getDescription();
    String faultDetail;

    switch (cimException.getCode())
    {
        case CIM_ERR_FAILED:                // Too general to specify fault
        case CIM_ERR_CLASS_HAS_CHILDREN:    // Only ModifyClass and DeleteClass
        case CIM_ERR_CLASS_HAS_INSTANCES:   // Only ModifyClass and DeleteClass
        case CIM_ERR_INVALID_SUPERCLASS:    // Only CreateClass and ModifyClass
        // These could map to wsa_ActionNotSupported if InvokeMethod is
        // supported through Custom Actions.
        case CIM_ERR_METHOD_NOT_FOUND:      // Only InvokeMethod
        case CIM_ERR_METHOD_NOT_AVAILABLE:  // Only InvokeMethod
        case CIM_ERR_NO_SUCH_PROPERTY:      // Only GetProperty and SetProperty
        case CIM_ERR_TYPE_MISMATCH:         // Only SetProperty
            subcode = WsmFault::wsman_InternalError;
            break;

        case CIM_ERR_ACCESS_DENIED:
            subcode = WsmFault::wsman_AccessDenied;
            break;

        case CIM_ERR_ALREADY_EXISTS:
            subcode = WsmFault::wsman_AlreadyExists;
            break;

        case CIM_ERR_INVALID_CLASS:
            subcode = WsmFault::wsa_DestinationUnreachable;
            faultDetail = WSMAN_FAULTDETAIL_INVALIDRESOURCEURI;
            break;

        case CIM_ERR_INVALID_NAMESPACE:
            // Consider wsman_InvalidSelectors?
            subcode = WsmFault::wsa_DestinationUnreachable;
            // faultDetail is not set to WSMAN_FAULTDETAIL_INVALIDRESOURCEURI
            // since this error reflects an incorrect Selector value rather
            // than an incorrect ResourceURI.
            break;

        case CIM_ERR_INVALID_PARAMETER:
            // For InvokeMethod, this would map to wsman_InvalidParameter.
            // It is difficult to discern the correct fault for other
            // operations.  It make sense to use wxf_InvalidRepresentation for
            // Create and Put, and wsman_InvalidSelectors for Get and Put.
            subcode = WsmFault::wsman_InvalidParameter;
            break;

        case CIM_ERR_INVALID_QUERY:
            // Note that Enumerate operations have a different subcode:
            // wsen_CannotProcessFilter.
            subcode = WsmFault::wsen_CannotProcessFilter;
            break;

        case CIM_ERR_NOT_FOUND:
            // DSP0226 Table 10 of master faults calls for
            // DestinationUnreachable in cases when the resource is no found.
            subcode = WsmFault::wsa_DestinationUnreachable;
            break;

        case CIM_ERR_NOT_SUPPORTED:
            subcode = WsmFault::wsa_ActionNotSupported;
            faultDetail = WSMAN_FAULTDETAIL_ACTIONMISMATCH;
            break;

        case CIM_ERR_QUERY_LANGUAGE_NOT_SUPPORTED:
            // DSP0227 section 15.1.11 indicates that ExecuteQuery operations
            // through WS-Management use CQL filter dialect.  If this status
            // code results, it is fair to assume filtering is not supported
            // at all.  Another option would be to use
            // wsen_FilterDialectRequestedUnavailable.
            subcode = WsmFault::wsen_FilteringNotSupported;
            break;

        default:
            PEGASUS_ASSERT(0);
    }

    return WsmFault(subcode, reason, languageList, faultDetail);
}

WsmGetResponse* CimToWsmResponseMapper::_mapToWsmGetResponse(
    const WsmGetRequest* wsmRequest,
    const CIMGetInstanceResponseMessage* response)
{
    WsmInstance wsmInstance;

    convertCimToWsmInstance(response->cimInstance, wsmInstance);

    WsmGetResponse* wsmResponse =
        new WsmGetResponse(
            wsmInstance,
            wsmRequest,
            _getContentLanguages(response->operationContext));

    return wsmResponse;
}

WsmPutResponse* CimToWsmResponseMapper::_mapToWsmPutResponse(
    const WsmPutRequest* wsmRequest,
    const CIMModifyInstanceResponseMessage* response)
{
    WsmPutResponse* wsmResponse =
        new WsmPutResponse(
            wsmRequest,
            _getContentLanguages(response->operationContext));

    return wsmResponse;
}

WsmCreateResponse* CimToWsmResponseMapper::_mapToWsmCreateResponse(
    const WsmCreateRequest* wsmRequest,
    const CIMCreateInstanceResponseMessage* response)
{
    WsmEndpointReference epr;

    convertObjPathToEPR(response->instanceName, epr);

    WsmCreateResponse* wsmResponse =
        new WsmCreateResponse(
            epr,
            wsmRequest,
            _getContentLanguages(response->operationContext));

    return wsmResponse;
}

WsmDeleteResponse* CimToWsmResponseMapper::_mapToWsmDeleteResponse(
    const WsmDeleteRequest* wsmRequest,
    const CIMDeleteInstanceResponseMessage* response)
{
    WsmDeleteResponse* wsmResponse =
        new WsmDeleteResponse(
            wsmRequest,
            _getContentLanguages(response->operationContext));

    return wsmResponse;
}

void CimToWsmResponseMapper::convertCimToWsmInstance(
    const CIMInstance& cimInstance,
    WsmInstance& wsmInstance)
{
    wsmInstance.setClassName(cimInstance.getClassName().getString());

    for (Uint32 i = 0, n = cimInstance.getPropertyCount(); i < n; i++)
    {
        CIMConstProperty cimProperty = cimInstance.getProperty(i);
        const String& propertyName = cimProperty.getName().getString();
        const CIMValue& cimValue = cimProperty.getValue();

        WsmValue wsmValue;
        convertCimToWsmValue(cimValue, wsmValue);

        WsmProperty wsmProperty(propertyName, wsmValue);
        wsmInstance.addProperty(wsmProperty);
    }
}

template<class T>
static void _convertCimToWsmArrayValue(
    const CIMValue& cimValue,
    WsmValue& wsmValue)
{
    Array<T> arr;
    Array<String> strs;

    cimValue.get(arr);
    for (Uint32 i = 0, n = arr.size(); i < n; i++)
    {
        CIMValue val(arr[i]);
        String str(val.toString());
        if (val.getType() == CIMTYPE_BOOLEAN)
        {
            str.toLower();
        }
        strs.append(str);
    }
    wsmValue.set(strs);
}

void CimToWsmResponseMapper::convertCimToWsmValue(
     const CIMValue& cimValue,
     WsmValue& wsmValue)
{
    if (cimValue.isNull())
    {
        wsmValue.setNull();
        return;
    }

    if (cimValue.isArray())
    {
        switch (cimValue.getType())
        {
            case CIMTYPE_BOOLEAN:
            {
                _convertCimToWsmArrayValue<Boolean>(cimValue, wsmValue);
                break;
            }

            case CIMTYPE_UINT8:
            {
                _convertCimToWsmArrayValue<Uint8>(cimValue, wsmValue);
                break;
            }

            case CIMTYPE_SINT8:
            {
                _convertCimToWsmArrayValue<Sint8>(cimValue, wsmValue);
                break;
            }

            case CIMTYPE_UINT16:
            {
                _convertCimToWsmArrayValue<Uint16>(cimValue, wsmValue);
                break;
            }

            case CIMTYPE_SINT16:
            {
                _convertCimToWsmArrayValue<Sint16>(cimValue, wsmValue);
                break;
            }

            case CIMTYPE_UINT32:
            {
                _convertCimToWsmArrayValue<Uint32>(cimValue, wsmValue);
                break;
            }

            case CIMTYPE_SINT32:
            {
                _convertCimToWsmArrayValue<Sint32>(cimValue, wsmValue);
                break;
            }

            case CIMTYPE_UINT64:
            {
                _convertCimToWsmArrayValue<Uint64>(cimValue, wsmValue);
                break;
            }

            case CIMTYPE_SINT64:
            {
                _convertCimToWsmArrayValue<Sint64>(cimValue, wsmValue);
                break;
            }

            case CIMTYPE_REAL32:
            {
                _convertCimToWsmArrayValue<Real32>(cimValue, wsmValue);
                break;
            }

            case CIMTYPE_REAL64:
            {
                _convertCimToWsmArrayValue<Real64>(cimValue, wsmValue);
                break;
            }

            case CIMTYPE_CHAR16:
            {
                _convertCimToWsmArrayValue<Char16>(cimValue, wsmValue);
                break;
            }

            case CIMTYPE_STRING:
            {
                _convertCimToWsmArrayValue<String>(cimValue, wsmValue);
                break;
            }

            case CIMTYPE_DATETIME:
            {
                Array<CIMDateTime> dates;
                Array<String> strs;
                cimValue.get(dates);
                for (Uint32 i = 0, n = dates.size(); i < n; i++)
                {
                    strs.append(dates[i].toString());
                }
                wsmValue.set(strs);
                break;
            }

            case CIMTYPE_REFERENCE:
            {
                Array<WsmEndpointReference> eprs;
                Array<CIMObjectPath> objPaths;
                cimValue.get(objPaths);

                for (Uint32 i = 0, n = objPaths.size(); i < n; i++)
                {
                    WsmEndpointReference epr;
                    convertObjPathToEPR(objPaths[i], epr);
                    eprs.append(epr);
                }
                wsmValue.set(eprs);
                break;
            }

            case CIMTYPE_OBJECT:
            {
                Array<WsmInstance> wsmInstances;
                Array<CIMObject> cimObjects;
                cimValue.get(cimObjects);
                for (Uint32 i = 0, n = cimObjects.size(); i < n; i++)
                {
                    if (cimObjects[i].isInstance())
                    {
                        WsmInstance wsmInstance;
                        convertCimToWsmInstance(
                            CIMInstance(cimObjects[i]), wsmInstance);
                        wsmInstances.append(wsmInstance);
                    }
                    else
                    {
                        throw WsmFault(WsmFault::wsman_InternalError,
                            MessageLoaderParms(
                                "WsmServer.CimToWsmResponseMapper."
                                    "EMBEDDED_CLASS_NOT_SUPPORTED",
                                "Embedded class objects in WS-Management "
                                    "responses are not supported."));
                    }
                }
                wsmValue.set(wsmInstances);
                break;
            }

#ifdef PEGASUS_EMBEDDED_INSTANCE_SUPPORT
            case CIMTYPE_INSTANCE:
            {
                Array<WsmInstance> wsmInstances;
                Array<CIMInstance> cimInstances;
                cimValue.get(cimInstances);
                for (Uint32 i = 0, n = cimInstances.size(); i < n; i++)
                {
                    WsmInstance wsmInstance;
                    convertCimToWsmInstance(cimInstances[i], wsmInstance);
                    wsmInstances.append(wsmInstance);
                }
                wsmValue.set(wsmInstances);
                break;
            }
#endif

            default:
            {
                PEGASUS_ASSERT(0);
            }
        }
    }
    else
    {
        switch (cimValue.getType())
        {
            case CIMTYPE_BOOLEAN:
            {
                String str(cimValue.toString());
                str.toLower();
                wsmValue.set(str);
                break;
            }
            case CIMTYPE_UINT8:
            case CIMTYPE_SINT8:
            case CIMTYPE_UINT16:
            case CIMTYPE_SINT16:
            case CIMTYPE_UINT32:
            case CIMTYPE_SINT32:
            case CIMTYPE_UINT64:
            case CIMTYPE_SINT64:
            case CIMTYPE_REAL32:
            case CIMTYPE_REAL64:
            case CIMTYPE_CHAR16:
            case CIMTYPE_STRING:
            case CIMTYPE_DATETIME:
            {
                wsmValue.set(cimValue.toString());
                break;
            }
            case CIMTYPE_REFERENCE:
            {
                WsmEndpointReference epr;
                CIMObjectPath objPath;
                cimValue.get(objPath);
                convertObjPathToEPR(objPath, epr);
                wsmValue.set(epr);
                break;
            }
            case CIMTYPE_OBJECT:
            {
                CIMObject cimObject;
                cimValue.get(cimObject);
                if (cimObject.isInstance())
                {
                    WsmInstance wsmInstance;
                    convertCimToWsmInstance(
                        CIMInstance(cimObject), wsmInstance);
                    wsmValue.set(wsmInstance);
                }
                else
                {
                    throw WsmFault(WsmFault::wsman_InternalError,
                        MessageLoaderParms(
                            "WsmServer.CimToWsmResponseMapper."
                                "EMBEDDED_CLASS_NOT_SUPPORTED",
                            "Embedded class objects in WS-Management "
                                "responses are not supported"));
                }
                break;
            }
#ifdef PEGASUS_EMBEDDED_INSTANCE_SUPPORT
            case CIMTYPE_INSTANCE:
            {
                WsmInstance wsmInstance;
                CIMInstance cimInstance;
                cimValue.get(cimInstance);
                convertCimToWsmInstance(cimInstance, wsmInstance);
                wsmValue.set(wsmInstance);
                break;
            }
#endif
            default:
            {
                PEGASUS_ASSERT(0);
            }
        }
    }
}

void CimToWsmResponseMapper::convertObjPathToEPR(
    const CIMObjectPath& objPath,
    WsmEndpointReference& epr)
{
    // Since the connection protocol is unknown at this point, we assume
    // it's http and form an EPR address by concatenating "http://" with
    // the host name in the object path,  with "/wsman".
    if (objPath.getHost().size() > 0)
        epr.address = "http://" + objPath.getHost() + "/wsman";
    else
        epr.address = WSM_ADDRESS_ANONYMOUS;

    epr.resourceUri = String(WSM_RESOURCEURI_CIMSCHEMAV2) + "/" +
        objPath.getClassName().getString();

    CIMNamespaceName cimNS = objPath.getNameSpace();
    if (!cimNS.isNull())
    {
        WsmSelector selector(String("__cimnamespace"), cimNS.getString());
        epr.selectorSet->selectors.append(selector);
    }

    const Array<CIMKeyBinding>& keyBindings = objPath.getKeyBindings();
    for (Uint32 i = 0, n = keyBindings.size(); i < n; i++)
    {
        CIMKeyBinding binding = keyBindings[i];
        if (binding.getType() == CIMKeyBinding::REFERENCE)
        {
            CIMObjectPath cimRef = binding.getValue();
            WsmEndpointReference wsmRef;
            convertObjPathToEPR(cimRef, wsmRef);
            WsmSelector selector(binding.getName().getString(), wsmRef);
            epr.selectorSet->selectors.append(selector);
        }
        else
        {
            WsmSelector selector(binding.getName().getString(),
                binding.getValue());
            epr.selectorSet->selectors.append(selector);
        }
    }
}

PEGASUS_NAMESPACE_END