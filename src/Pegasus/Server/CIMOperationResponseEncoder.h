//%////-*-c++-*-////////////////////////////////////////////////////////////////
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
//==============================================================================
//
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CIMOperationResponseEncoder_h
#define Pegasus_CIMOperationResponseEncoder_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/MessageQueueService.h>
#include <Pegasus/Common/CIMMessage.h>

PEGASUS_NAMESPACE_BEGIN

/** This class encodes CIM operation requests and passes them up-stream.
 */
class CIMOperationResponseEncoder : public MessageQueueService
{
   public:
  
      typedef MessageQueueService Base;
  

      CIMOperationResponseEncoder();

      ~CIMOperationResponseEncoder();

      void sendResponse(Uint32 queueId, Array<Sint8>& message);

      void sendMethodError(
	 Uint32 queueId, 
	 const String& messageId,
	 const String& methodName,
	 const CIMException& cimException);

      void sendMethodError(
	 CIMResponseMessage* response,
	 const String& cimMethodName);

      void sendIMethodError(
	 Uint32 queueId, 
	 const String& messageId,
	 const String& methodName,
	 const CIMException& cimException);

      void sendIMethodError(
	 CIMResponseMessage* response,
	 const String& cimMethodName);

      virtual void handleEnqueue(Message *);
      
      virtual void handleEnqueue();


      virtual const char* getQueueName() const;

      void encodeCreateClassResponse(
	 CIMCreateClassResponseMessage* response);

      void encodeGetClassResponse(
	 CIMGetClassResponseMessage* response);

      void encodeModifyClassResponse(
	 CIMModifyClassResponseMessage* response);

      void encodeEnumerateClassNamesResponse(
	 CIMEnumerateClassNamesResponseMessage* response);

      void encodeEnumerateClassesResponse(
	 CIMEnumerateClassesResponseMessage* response);

      void encodeDeleteClassResponse(
	 CIMDeleteClassResponseMessage* response);

      void encodeCreateInstanceResponse(
	 CIMCreateInstanceResponseMessage* response);

      void encodeGetInstanceResponse(
	 CIMGetInstanceResponseMessage* response);

      void encodeModifyInstanceResponse(
	 CIMModifyInstanceResponseMessage* response);

      void encodeEnumerateInstanceNamesResponse(
	 CIMEnumerateInstanceNamesResponseMessage* response);

      void encodeEnumerateInstancesResponse(
	 CIMEnumerateInstancesResponseMessage* response);

      void encodeDeleteInstanceResponse(
	 CIMDeleteInstanceResponseMessage* response);

      void encodeGetPropertyResponse(
	 CIMGetPropertyResponseMessage* response);

      void encodeSetPropertyResponse(
	 CIMSetPropertyResponseMessage* response);

      void encodeSetQualifierResponse(
	 CIMSetQualifierResponseMessage* response);

      void encodeGetQualifierResponse(
	 CIMGetQualifierResponseMessage* response);

      void encodeEnumerateQualifiersResponse(
	 CIMEnumerateQualifiersResponseMessage* response);

      void encodeDeleteQualifierResponse(
	 CIMDeleteQualifierResponseMessage* response);

      void encodeReferenceNamesResponse(
	 CIMReferenceNamesResponseMessage* response);

      void encodeReferencesResponse(
	 CIMReferencesResponseMessage* response);

      void encodeAssociatorNamesResponse(
	 CIMAssociatorNamesResponseMessage* response);

      void encodeAssociatorsResponse(
	 CIMAssociatorsResponseMessage* response);

      void encodeExecQueryResponse(
	 CIMExecQueryResponseMessage* response);

      void encodeInvokeMethodResponse(
	 CIMInvokeMethodResponseMessage* response);
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_CIMOperationResponseEncoder_h */
