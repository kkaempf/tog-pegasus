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
#include "SoapResponse.h"
#include "WsmWriter.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

SoapResponse::SoapResponse(WsmResponse* response)
{
    String action;
    ContentLanguageList lang;

    _maxEnvelopeSize = response->getMaxEnvelopeSize();
    _queueId = response->getQueueId();
    _httpCloseConnect = response->getHttpCloseConnect();
    
    WsmWriter::appendSoapEnvelopeStart(_envStart);
    WsmWriter::appendSoapEnvelopeEnd(_envEnd);
    WsmWriter::appendSoapHeaderStart(_hdrStart);
    WsmWriter::appendSoapHeaderEnd(_hdrEnd);
    WsmWriter::appendSoapBodyStart(_bodyStart);
    WsmWriter::appendSoapBodyEnd(_bodyEnd);

    switch(response->getType())
    {
        case WS_TRANSFER_GET:
            action = WSM_ACTION_GET_RESPONSE;
            break;

        case WS_TRANSFER_PUT:
            action = WSM_ACTION_PUT_RESPONSE;
            break;

        case WS_TRANSFER_CREATE:
            action = WSM_ACTION_CREATE_RESPONSE;
            break;

        case WS_TRANSFER_DELETE:
            action = WSM_ACTION_DELETE_RESPONSE;
            break;

        case WS_ENUMERATION_ENUMERATE:
            action = WSM_ACTION_ENUMERATE_RESPONSE;
            break;

        case WS_ENUMERATION_PULL:
            action = WSM_ACTION_PULL_RESPONSE;
            break;

        case WS_ENUMERATION_RELEASE:
            action = WSM_ACTION_RELEASE_RESPONSE;
            break;

        case WSM_FAULT:
            action = ((WsmFaultResponse*) response)->getFault().getAction();
            WsmWriter::appendSoapHeader(_hdrContent, 
                action, response->getMessageId(), response->getRelatesTo());
            WsmWriter::appendWsmFaultBody(_bodyContent, 
                ((WsmFaultResponse*) response)->getFault());
            break;

        case SOAP_FAULT:
            action = String(WsmNamespaces::supportedNamespaces[
                WsmNamespaces::WS_ADDRESSING].extendedName) + String("/fault");
            WsmWriter::appendSoapFaultHeaders(_hdrContent, 
                ((SoapFaultResponse*) response)->getFault(),
                action, response->getMessageId(), response->getRelatesTo());
            WsmWriter::appendSoapFaultBody(_bodyContent, 
                ((SoapFaultResponse*) response)->getFault());
            break;

        default:
            PEGASUS_ASSERT(0);
    }

    WsmWriter::appendHTTPResponseHeader(_httpHeader, action, 
       response->getHttpMethod(), response->getContentLanguages(), 
       response->getType() == WSM_FAULT || response->getType() == SOAP_FAULT,
       0);

    // Make sure that fault response fits within MaxEnvelopeSize
    if (response->getType() == WSM_FAULT || response->getType() == SOAP_FAULT)
    {
        if (_maxEnvelopeSize && getEnvelopeSize() > _maxEnvelopeSize)
        {
            _bodyContent.clear();
            WsmFault fault(WsmFault::wsman_EncodingLimit,
                MessageLoaderParms(
                    "WsmServer.WsmResponseEncoder.FAULT_MAX_ENV_SIZE_EXCEEDED",
                    "Fault response could not be encoded within requested "
                    "envelope size limits."),
                WSMAN_FAULTDETAIL_MAXENVELOPESIZE);
            WsmWriter::appendWsmFaultBody(_bodyContent, fault);
        }
    }
    else
    {
        WsmWriter::appendSoapHeader(_hdrContent, 
            action, response->getMessageId(), response->getRelatesTo());
    }
}

Buffer SoapResponse::getResponseContent()
{
    Buffer out(WSM_MIN_MAXENVELOPESIZE_VALUE);
    out << _httpHeader << _envStart << _hdrStart << _hdrContent 
        << _hdrEnd << _bodyStart << _bodyHeader << _bodyContent 
        << _bodyTrailer << _bodyEnd << _envEnd;
    return out;
}

Boolean SoapResponse::appendHeader(Buffer& buf)
{
    if (_maxEnvelopeSize && 
        getEnvelopeSize() + buf.size() > _maxEnvelopeSize)
    {
        return false;
    }
    _hdrContent << buf;
    return true;
}

Boolean SoapResponse::appendBodyContent(Buffer& buf)
{
    if (_maxEnvelopeSize && 
        getEnvelopeSize() + buf.size() > _maxEnvelopeSize)
    {
        return false;
    }
    _bodyContent << buf;
    return true;
}

Boolean SoapResponse::appendBodyHeader(Buffer& buf)
{
    if (_maxEnvelopeSize && 
        getEnvelopeSize() + buf.size() > _maxEnvelopeSize)
    {
        return false;
    }
    _bodyHeader << buf;
    return true;
}

Boolean SoapResponse::appendBodyTrailer(Buffer& buf)
{
    if (_maxEnvelopeSize && 
        getEnvelopeSize() + buf.size() > _maxEnvelopeSize)
    {
        return false;
    }
    _bodyTrailer << buf;
    return true;
}

PEGASUS_NAMESPACE_END