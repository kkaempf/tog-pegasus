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
// Author: Chuck Carmack (carmack@us.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

// This is a sample provider that test globalization support, including unicode
// support and localized message loads.

// Provider Types Tested:
//
// Instance
// Method

// Testcases:
//
// 1) Round Trip Test: Verifies that a string and char16 containing Unicode
// chars has no characters lost on a round trip.  The client sends a Unicode
// string and char16 in properties for createInstance and modifyInstance.
// We check these against an expected string and char16. If it miscompares then
// some Unicode chars were lost.  The Unicode string and char16 are then saved along 
// with the instance sent by the client. When the client does a getInstance it can
// compare the string and char16 to an expected values on the client side.
// 
// Test Properties: RoundTripString, RoundTripChar
// 
// 2) Resource Bundle Test:  One of the properties on the instances
// returned is a read-only string property where the string is loaded 
// from a resource bundle.  The language of the string is determined
// the preferred languages sent by the client in the AcceptLanguages
// header.
//
// Test Property: ResourceBundleString
//
// 3) Content Languages Test.  This tests that the client can set a 
// r/w string and associate a content language with that string.  The client
// should be able to retrieve that string with the same associated content
// language tag.
//
// Test Property: ContentLanguageString
//
//
// OVERALL DESIGN NOTE:
//
// We have two localized properties (ResourceBundleString
// and ContentLanguageString).  The first is r/o and the second is r/w. 
// The client can set ContentLanguageString with an associated content
// language tag.  The last content language tag set by the client for this 
// property is stored in instanceLangs[].
//
// However, when the client gets an instance, the content language in the response
// applies to the whole instance.  So, to decide what language to set for returned
// instance, we do the following:
//
// -- If the client had previously set a content language for 
// ContentLanguageString, then we will return that string with the
// content language previously set by the client.  We will try to 
// load the ResourceBundleString from the resource bundle in that
// same content language.  If that content language is not supported by
// a resource bundle, this will result in a default string being
// returned to the client in ResourceBundleString.
//
// -- If the client had not previously set a content language for 
// ContentLanguageString, or the last set was empty, then we will
// load both properties from the resource bundle based on the 
// AcceptLanguages that was set into our thread by Pegasus (this
// is the same as the AcceptLanguages requested by the client) 
   

// l10n TODO
// -- implement test providers for other provider types
// -- expand the instance provider to test aggregation across classes

// Globalization headers
#include "LocalizedProvider.h"
#include <Pegasus/Common/AcceptLanguages.h>
#include <Pegasus/Common/ContentLanguages.h>
#include <Pegasus/Common/MessageLoader.h>

PEGASUS_NAMESPACE_BEGIN

// Class name and references
#define CLASSNAME "Sample_LocalizedProviderClass"
#define REFERENCE1 "Sample_LocalizedProviderClass.Identifier=0"
#define REFERENCE2 "Sample_LocalizedProviderClass.Identifier=1"
 
// Property names 
#define IDENTIFIER_PROP "Identifier"		// Key
#define ROUNDTRIPSTRING_PROP "RoundTripString"	// Property for round trip test (R/W)
#define ROUNDTRIPCHAR_PROP "RoundTripChar"	// Property for round trip test (R/W)
#define RB_STRING_PROP "ResourceBundleString" // Property for resource bundle
											// test (R/O)
#define CONTENTLANG_PROP "ContentLanguageString" // Property from content 
												// language test (R/W)

// The name of the *root* resource bundle.
// Note: we are using a relative path to $PEGASUS_HOME
#define RESOURCEBUNDLE "provider/localizedProvider/localizedProvider";

// Message IDs and default message strings
#define RB_STRING_MSGID "ResourceBundleString" // ID for property loaded from resource bundle
#define RB_STRING_DFT "ResourceBundleString DEFAULT"

#define CONTENTLANG_STRING_MSGID "ContentLanguageString" // ID for property loaded from resource bundle
#define CONTENTLANG_STRING_DFT "ContentLanguageString DEFAULT"

#define NOT_SUPPORTED_ERROR_MSGID "NotSupportedError" // ID for not supported error msg
#define NOT_SUPPORTED_ERROR_DFT "LocalizedProvider: Operation Not Supported (default message)"

#define ROUND_TRIP_ERROR_MSGID "RoundTripError"	 // ID for round trip error msg
#define ROUND_TRIP_ERROR_DFT "LocalizedProvider: Received bad value for round trip string (default message)"
#define ROUND_TRIP_STRING_DFT "RoundTripString DEFAULT"

// Expected round-trip string from the client. 
// Includes a UTF-16 surrogate pair (last two elements)
static const Char16 roundTripChars[] = 
			{0x6A19, 
			0x6E96,
			0x842C, 
			0x570B,
			0x78BC,
			0xdbc0,
			0xdc01,
			0x00};	

// Constructor - initializes parameters for the MessageLoader that won't
// be changing.
LocalizedProvider::LocalizedProvider() :
	msgParms(RB_STRING_MSGID, RB_STRING_DFT),
	notSupportedErrorParms(NOT_SUPPORTED_ERROR_MSGID, NOT_SUPPORTED_ERROR_DFT),
	contentLangParms(CONTENTLANG_STRING_MSGID, CONTENTLANG_STRING_DFT),
	roundTripErrorParms(ROUND_TRIP_ERROR_MSGID, ROUND_TRIP_ERROR_DFT),
	roundTripString(roundTripChars)
{
	msgParms.msg_src_path = RESOURCEBUNDLE;
	contentLangParms.msg_src_path = RESOURCEBUNDLE;
	notSupportedErrorParms.msg_src_path = RESOURCEBUNDLE;
	roundTripErrorParms.msg_src_path = RESOURCEBUNDLE;			
}

LocalizedProvider::~LocalizedProvider(void)
{

}

void LocalizedProvider::initialize(CIMOMHandle & cimom)
{
	// create default instances

	// Instance 1
	CIMInstance instance1(CLASSNAME);
	CIMObjectPath reference1(REFERENCE1);

	instance1.addProperty(CIMProperty(IDENTIFIER_PROP, Uint8(0)));   
	instance1.addProperty(CIMProperty(ROUNDTRIPSTRING_PROP, String(roundTripChars)));	
	instance1.addProperty(CIMProperty(ROUNDTRIPCHAR_PROP, roundTripChars[0]));						 
	_instances.append(instance1);
	_instanceNames.append(reference1);
	_instanceLangs.append(ContentLanguages::EMPTY);
	
	// Instance 2
	CIMInstance instance2(CLASSNAME);	
	CIMObjectPath reference2(REFERENCE2);	
 	
	instance2.addProperty(CIMProperty(IDENTIFIER_PROP, Uint8(1)));   
	instance2.addProperty(CIMProperty(ROUNDTRIPSTRING_PROP, String(roundTripChars)));
	instance2.addProperty(CIMProperty(ROUNDTRIPCHAR_PROP, roundTripChars[0]));	
	_instances.append(instance2);	
	_instanceNames.append(reference2);
	_instanceLangs.append(ContentLanguages::EMPTY);	
}

void LocalizedProvider::terminate(void)
{

}

void LocalizedProvider::getInstance(
    const OperationContext & context,
    const CIMObjectPath & instanceReference,
    const Boolean includeQualifiers,
    const Boolean includeClassOrigin,
    const CIMPropertyList & propertyList,
    InstanceResponseHandler & handler)
{
	// begin processing the request
	handler.processing();

	// Get the list of preferred languages that the client requested
	// to be returned in the response.
	AcceptLanguages clientAcceptLangs = getRequestAcceptLanguages(context);

	// instance index corresponds to reference index
	for(Uint32 i = 0, n = _instances.size(); i < n; i++)
	{
		if(instanceReference.getClassName() == _instanceNames[i].getClassName() &&
                   instanceReference.getKeyBindings() == _instanceNames[i].getKeyBindings()) 
		{
			// Load the localized properties and figure out what content
			// language to return, based on the design mentioned above.
			ContentLanguages rtnLangs = _loadLocalizedProps(clientAcceptLangs,
														_instanceLangs[i],
														_instances[i]);

			// We need to tag the instance we are returning with the
			// the content language.
			handler.setLanguages(rtnLangs);		
						
			// deliver requested instance
			handler.deliver(_instances[i]);

			break;
		}
	}

	// complete processing the request
	handler.complete();
}

void LocalizedProvider::enumerateInstances(
    const OperationContext & context,
    const CIMObjectPath & classReference,
    const Boolean includeQualifiers,
    const Boolean includeClassOrigin,
    const CIMPropertyList & propertyList,
    InstanceResponseHandler & handler)
{
	// begin processing the request
	handler.processing();

	// Get the list of preferred languages that the client requested
	// to be returned in the response.
	AcceptLanguages clientAcceptLangs = getRequestAcceptLanguages(context);

	ContentLanguages aggregatedLangs = ContentLanguages::EMPTY;
	Boolean langMismatch = false;

	for(Uint32 i = 0, n = _instances.size(); i < n; i++)
	{
		// Load the localized properties and figure out what content
		// language to return for this instance, based on the design 
		// mentioned above.
		ContentLanguages rtnLangs = _loadLocalizedProps(clientAcceptLangs,
														_instanceLangs[i],
														_instances[i]);	

		// Since we are returning more than one instance, and the 
		// content language we are returning applies to all the instances,
		// we need to 'aggregate' the languages of all the instances.
		// If all the instances have the same language, then return
		// that language in the content language to the client.  If there
		// is a language mismatch in the instances, then do not return
		// any ContentLanguages to the client.
		if (i == 0)
		{
			// Set the aggregated content language to the first instance lang.	
			aggregatedLangs = rtnLangs;	
		}
		else if (langMismatch == false && rtnLangs != aggregatedLangs)
		{
			// A mismatch was found.  Set the aggegrated content lang to empty	
			langMismatch = true;	
			aggregatedLangs = ContentLanguages::EMPTY;
		}
	
		// deliver instance
		handler.deliver(_instances[i]);
	}

	// Set the aggregated content language into the response
	handler.setLanguages(aggregatedLangs);

	// complete processing the request
	handler.complete();
}

void LocalizedProvider::enumerateInstanceNames(
    const OperationContext & context,
    const CIMObjectPath & classReference,
    ObjectPathResponseHandler & handler)
{
	// Not doing any localization here
	
	// begin processing the request
	handler.processing();

	for(Uint32 i = 0, n = _instances.size(); i < n; i++)
	{
		// deliver reference
		handler.deliver(_instanceNames[i]);
	}

	// complete processing the request
	handler.complete();
}

void LocalizedProvider::modifyInstance(
    const OperationContext & context,
    const CIMObjectPath & instanceReference,
    const CIMInstance & instanceObject,
    const Boolean includeQualifiers,
    const CIMPropertyList & propertyList,
    ResponseHandler & handler)
{
	// instanceReference does not have the key!
	CIMObjectPath localReference =	buildRefFromInstance(instanceObject);	
	
	// begin processing the request
	handler.processing();

        Uint32 i = 0;

	// instance index corresponds to reference index
	for(Uint32 n = _instances.size(); i < n; i++)
	{
		if(localReference == _instanceNames[i])
		{
			// We expect the client to send us specific values in the
			// round trip string and char16 properties.
                    // This is to test that the Unicode characters got to us properly.
			// (Note: this call can throw an exception)
			_checkRoundTripString(context, instanceObject);				
			
			// overwrite existing instance
			_instances[i] = instanceObject;
			
			// Get the language that the client tagged to the instance
			// Note: the ContentLanguageString property is the only r/w string that
			// can be tagged with a content language.  So the ContentLangauges
			// for the instance really only applies to that property.
			ContentLanguageListContainer cl_container = 
				(ContentLanguageListContainer)context.get(ContentLanguageListContainer::NAME);
			ContentLanguages contentLangs = cl_container.getLanguages();
	
			// Save the language of the ContentLanguageString	
			_instanceLangs[i] = contentLangs;			
			
			break;
		}
	}
	
	if (i == _instanceNames.size())
		throw CIMObjectNotFoundException(instanceReference.toString());

	// complete processing the request
	handler.complete();
}

void LocalizedProvider::createInstance(
    const OperationContext & context,
    const CIMObjectPath & instanceReference,
    const CIMInstance & instanceObject,
    ObjectPathResponseHandler & handler)
{
	// instanceReference does not have the key!
	CIMObjectPath localReference =	buildRefFromInstance(instanceObject);

	// instance index corresponds to reference index
	for(Uint32 i = 0, n = _instanceNames.size(); i < n; i++)
	{
		if(localReference == _instanceNames[i])
		{
			// Note: Since localReference is a CIMObjectPath,
			// and that is our "message" here, no need to
			// to localize.
			throw CIMObjectAlreadyExistsException(
                                  localReference.toString());
		}
	}
			
	// begin processing the request
	handler.processing();

        // We expect the client to send us specific values in the
	// round trip string and char16 properties.
        // This is to test that the Unicode characters got to us properly.
        // (Note: this call can throw an exception)
	_checkRoundTripString(context, instanceObject);	

	// add the new instance to the array
	_instances.append(instanceObject);
	_instanceNames.append(localReference);

	// Get the language that the client tagged to the instance
	// Note: the ContentLanguageString property is the only r/w string that
	// can be tagged with a content language.  So the ContentLanguages
	// for the instance really only applies to that property.	
	ContentLanguageListContainer cl_container = 
		(ContentLanguageListContainer)context.get(ContentLanguageListContainer::NAME);
	ContentLanguages contentLangs = cl_container.getLanguages();
	
	// Save the language of the ContentLanguageString	
	_instanceLangs.append(contentLangs);

	// deliver the new instance
	handler.deliver(_instanceNames[_instanceNames.size() - 1]);

	// complete processing the request
	handler.complete();
}

void LocalizedProvider::deleteInstance(
    const OperationContext & context,
    const CIMObjectPath & instanceReference,
    ResponseHandler & handler)
{
	// We're not going to support this for instances 0 or 1

	// instance index corresponds to reference index
	Uint32 i = 0;
	for(Uint32 n = _instanceNames.size(); i < n; i++)
	{
		if(instanceReference.getClassName() == _instanceNames[i].getClassName() &&
                   instanceReference.getKeyBindings() == _instanceNames[i].getKeyBindings()) 
		{
			if (i < 2)
			{
                		// Throw an exception with a localized message using the
	            	// AcceptLanguages set into our thread by Pegasus.
	            	// (this equals the AcceptLanguages requested by the client)
                		//  Note: the exception object will load the string for us. 		
                		throw CIMNotSupportedException(notSupportedErrorParms);
			}
			break;
		}
	}
	
	if (i == _instanceNames.size())
		throw CIMObjectNotFoundException(instanceReference.toString());

	handler.processing();
	
	_instanceNames.remove(i);
	_instanceLangs.remove(i);
	_instances.remove(i);
	
	handler.complete();
}

/*
void LocalizedProvider::associators(
    const OperationContext & context,
    const CIMObjectPath & objectName,
    const CIMName & associationClass,
    const CIMName & resultClass,
    const String & role,
    const String & resultRole,
    const Boolean includeQualifiers,
    const Boolean includeClassOrigin,
    const CIMPropertyList & propertyList,
    ObjectResponseHandler & handler)
{

}

void LocalizedProvider::associatorNames(
    const OperationContext & context,
    const CIMObjectPath & objectName,
    const CIMName & associationClass,
    const CIMName & resultClass,
    const String & role,
    const String & resultRole,
    ObjectPathResponseHandler & handler)
{

}

void LocalizedProvider::references(
    const OperationContext & context,
    const CIMObjectPath & objectName,
    const CIMName & resultClass,
    const String & role,
    const Boolean includeQualifiers,
    const Boolean includeClassOrigin,
    const CIMPropertyList & propertyList,
    ObjectResponseHandler & handler)
{

}

void LocalizedProvider::referenceNames(
    const OperationContext & context,
    const CIMObjectPath & objectName,
    const CIMName & resultClass,
    const String & role,
    ObjectPathResponseHandler & handler)
{

}
*/

/*
void LocalizedProvider::getProperty(
    const OperationContext & context,
    const CIMObjectPath & instanceReference,
    const CIMName & propertyName,
    ValueResponseHandler & handler)
{

}

void LocalizedProvider::setProperty(
    const OperationContext & context,
    const CIMObjectPath & instanceReference,
    const CIMName & propertyName,
    const CIMValue & newValue,
    ResponseHandler & handler)
{

}
*/

void LocalizedProvider::invokeMethod(
    const OperationContext & context,
    const CIMObjectPath & objectReference,
    const CIMName & methodName,
    const Array<CIMParamValue> & inParameters,
    MethodResultResponseHandler & handler)
{
    handler.processing();
 
    String utf16String(roundTripChars);
    String expectedString(roundTripChars);
    Char16 expectedChar16 = roundTripChars[1];

    String outString(roundTripChars);
    Char16 outChar16 = roundTripChars[2];

    if (objectReference.getClassName().equal (CLASSNAME))
    {	
	if (methodName.equal ("UTFMethod"))
	{
            // The method was called to test UTF-16 support for input
            // parameters, output parameters, and return value.
	    if( inParameters.size() == 2 )
            {
                String inString;
                Char16 inChar16;

                // Verify that we got the expected UTF-16 chars
                // in the input parameters
	        CIMValue paramVal = inParameters[0].getValue();
                paramVal.get( inString );
 	        if( inString != expectedString )
		{
	            throw CIMInvalidParameterException(roundTripErrorParms);
		}

	        paramVal = inParameters[1].getValue();
                paramVal.get( inChar16 );
 	        if( inChar16 != expectedChar16 )
		{
	            throw CIMInvalidParameterException(roundTripErrorParms);
		}

                // Return the UTF-16 chars in the output parameters
                handler.deliverParamValue(
                          CIMParamValue( "outStr",
                                         CIMValue(outString) ) );

                handler.deliverParamValue(
                          CIMParamValue( "outChar16",
                                         CIMValue(outChar16) ) );

                // Return UTF-16 chars in the return string
                handler.deliver( CIMValue( outString ) );
	    }
            else
            {
                throw CIMException(CIM_ERR_FAILED);
            }
	}
        else if (methodName.equal (utf16String))
        {
            // The method was called that has UTF-16 chars in
            // in the method name.  The purpose of this test
            // is to verify the URI encoding/decoding of UTF-8
            // in the CIMMethod HTTP header.

            // Return UTF-16 chars in the return string
            handler.deliver( CIMValue( outString ) );
        }
        else
        {
            throw CIMMethodNotFoundException(methodName);
        }
   }
   else
   {
       throw CIMObjectNotFoundException(objectReference.getClassName());
   }
	
   handler.complete();
}

/*
void LocalizedProvider::executeQuery(
    const OperationContext & context,
    const CIMNamespaceName & nameSpace,
    const String & queryLanguage,
    const String & query,
    ObjectResponseHandler & handler)
{

}

void LocalizedProvider::enableIndications(IndicationResponseHandler & handler)
{

}

void LocalizedProvider::disableIndications(void)
{

}

void LocalizedProvider::createSubscription(
    const OperationContext & context,
    const CIMObjectPath & subscriptionName,
    const Array<CIMObjectPath> & classNames,
    const CIMPropertyList & propertyList,
    const Uint16 repeatNotificationPolicy)
{

}

void LocalizedProvider::modifySubscription(
    const OperationContext & context,
    const CIMObjectPath & subscriptionName,
    const Array<CIMObjectPath> & classNames,
    const CIMPropertyList & propertyList,
    const Uint16 repeatNotificationPolicy)
{

}

void LocalizedProvider::deleteSubscription(
    const OperationContext & context,
    const CIMObjectPath & subscriptionName,
    const Array<CIMObjectPath> & classNames)
{

}

// CIMIndicationConsumer interface
void LocalizedProvider::handleIndication(
        const OperationContext & context,
        const CIMInstance & indication,
        IndicationResponseHandler & handler)
{

}
*/

void LocalizedProvider::_checkRoundTripString(const OperationContext & context,
								const CIMInstance& instanceObject)
{
    // Get the round trip string sent by the client
    String roundTripStringProp; 
    instanceObject.getProperty(instanceObject.findProperty(ROUNDTRIPSTRING_PROP)).
            getValue().
            get(roundTripStringProp);   

    // Get the round trip char16 sent by the client
    Char16 roundTripCharProp; 
    instanceObject.getProperty(instanceObject.findProperty(ROUNDTRIPCHAR_PROP)).
            getValue().
            get(roundTripCharProp);  	

    // Now compare the string and char16 from the client to the ones we expect
    // This checks that Unicode chars were not lost
    if ((roundTripString != roundTripStringProp) ||
        (roundTripChars[0] != roundTripCharProp))
    {
	// A miscompare.  The Unicode was not preserved from the
	// client, through Pegasus, down to us.  	
			
	// Throw an exception with a localized message using the
	// AcceptLanguages set into our thread by Pegasus.
	// (this equals the AcceptLanguages requested by the client)
	//  Note: the exception object will load the string for us. 
	throw CIMInvalidParameterException(roundTripErrorParms);		
    }	
}

AcceptLanguages LocalizedProvider::getRequestAcceptLanguages(
											const OperationContext & context)
{
	// Get the client's list of preferred languages for the response
	AcceptLanguageListContainer al_container = 
		(AcceptLanguageListContainer)context.get(AcceptLanguageListContainer::NAME);
	return al_container.getLanguages();		
}	

CIMObjectPath LocalizedProvider::buildRefFromInstance(const CIMInstance& instanceObject)
{
    // Build the local reference (maybe there is a better way to do this)
    String local = CLASSNAME;
    Uint32 index = instanceObject.findProperty(IDENTIFIER_PROP);	
    if (index == PEG_NOT_FOUND)
    {
       throw CIMPropertyNotFoundException(IDENTIFIER_PROP);
    }

    Uint8 id;
    instanceObject.getProperty(index).getValue().get(id);   	
    char bfr[8] = {0};
    sprintf(bfr,"%d",id);
    local.append(".");
    local.append(IDENTIFIER_PROP);
    local.append("=");
    local.append(bfr);
    CIMObjectPath localReference(local);
    return localReference;
}

ContentLanguages LocalizedProvider::_loadLocalizedProps(
												AcceptLanguages & acceptLangs,
												ContentLanguages & contentLangs,
												CIMInstance & instance)			
{												
	// The content languages to be returned to the client
	ContentLanguages rtnLangs = ContentLanguages::EMPTY;
			
	// Attempt to match one of the accept languages from the client with
	// the last content language saved for ContentLanguageString
	// (Note:  Using the AcceptLanguages iterator allows us
	// to scan the client's accept languages in preferred order)
	AcceptLanguageElement ale = AcceptLanguageElement::EMPTY;
	ContentLanguageElement cle = ContentLanguageElement::EMPTY;
			
	Boolean matchFound = false;
	acceptLangs.itrStart();
	while((ale = acceptLangs.itrNext()) != AcceptLanguageElement::EMPTY)
	{
		contentLangs.itrStart();
		while((cle = contentLangs.itrNext()) != ContentLanguageElement::EMPTY)
		{
			if (ale.getLanguageTag() == cle.getLanguageTag())
			{
				matchFound = true;	
				break;					
			}
		}	
	}			
			
	if (!matchFound)
	{
		// We have not found a match.		
		// Load both localized properties from the resource bundle, using
		// the AcceptLanguages that was set into our thread by Pegasus.
		// (this equals the AcceptLanguages requested by the client)		
		
		// First, load the ResourceBundleString and put the string into
		// the instance we are returning.
		rtnLangs = _addResourceBundleProp(instance);

		// Load the ContentLanguageString from the resource bundle
		// into the instance we are returning.
		ContentLanguages clPropLangs = _addContentLanguagesProp(instance);					

		// The calls above returned the ContentLanguages for the language
		// of the resource bundle that was found.
		// If the two resource bundle langs don't match then
		// we have a bug in our resource bundles.														
		if (rtnLangs != clPropLangs)
		{
			throw new CIMOperationFailedException(String::EMPTY);
		}			
	}
	else
	{
		// We have found a match.  Attempt
		// to load ResourceBundleString using the matching 
		// content language.
		AcceptLanguages tmpLangs;					
		tmpLangs.add(ale);
		(void)_addResourceBundleProp(tmpLangs, instance);
				
		// Send the matching content language back to the client.
		rtnLangs.append(cle);				
	}
	
	return rtnLangs;			
}													

ContentLanguages LocalizedProvider::_addResourceBundleProp(
												AcceptLanguages & acceptLangs,
												CIMInstance & instance)
{
	// Get the string out of the resource bundle in one
	// of the languages requested by the caller of this function.
	msgParms.acceptlanguages = acceptLangs;	
	String localizedMsgProp = MessageLoader::getMessage(msgParms);					

	// Replace the string into the instance
	_replaceRBProperty(instance, localizedMsgProp);

	// The MessageLoader set the contentlanguages member
	// of msgParms to the language that it found for the message.
	return msgParms.contentlanguages;
}


ContentLanguages LocalizedProvider::_addResourceBundleProp(
												CIMInstance & instance)
{
	// Get the ResourceBundleString of the resource bundle using the 
	// AcceptLanguages set into our thread by Pegasus.
	// (this equals the AcceptLanguages requested by the client)	
	
	// First, need to clear any old AcceptLanguages in the message loader
	// parms because that will override the thread's AcceptLanguages.
	msgParms.acceptlanguages = AcceptLanguages::EMPTY;	
	msgParms.useThreadLocale = true;  // Don't really need to do this
									// because the default is true
									
	// Load the string from the resource bundle										
	String localizedMsgProp = MessageLoader::getMessage(msgParms);					

	// Replace the string into the instance
	_replaceRBProperty(instance, localizedMsgProp);

	// The MessageLoader set the contentlanguages member
	// of msgParms to the language that it found for the message.
	return msgParms.contentlanguages;
}


void LocalizedProvider::_replaceRBProperty(CIMInstance & instance, String newProp)
{
	// Remove the old string property 
	Uint32 index = instance.findProperty(RB_STRING_PROP);
	if (index != PEG_NOT_FOUND)
	{
		instance.removeProperty(index);
	}
			
	// Add the localized string property to the instance
	instance.addProperty(CIMProperty(RB_STRING_PROP, newProp));	
}


ContentLanguages LocalizedProvider::_addContentLanguagesProp(CIMInstance & instance)
{
	// Get the ContentLanguageString out of the resource bundle using the 
	// AcceptLanguages set into our thread by Pegasus.
	// (this equals the AcceptLanguages requested by the client)

	// First, need to clear any old AcceptLanguages in the message loader
	// parms because that will override the thread's AcceptLanguages.
	contentLangParms.acceptlanguages = AcceptLanguages::EMPTY;	
	contentLangParms.useThreadLocale = true;  // Don't really need to do this
									// because the default is true	

	// Load the string from the resource bundle		
	String localizedMsgProp = MessageLoader::getMessage(contentLangParms);	

	// Remove the old string property 
	Uint32 index = instance.findProperty(CONTENTLANG_PROP);
	if (index != PEG_NOT_FOUND)
	{
		instance.removeProperty(index);
	}
			
	// Add the localized string property to the instance
	instance.addProperty(CIMProperty(CONTENTLANG_PROP, localizedMsgProp));	

	// The MessageLoader set the contentlanguages member
	// of msgParms to the language that it found for the message.
	return contentLangParms.contentlanguages;
}

PEGASUS_NAMESPACE_END
