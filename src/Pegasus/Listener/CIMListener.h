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
// Author: Dong Xiang, EMC Corporation (xiang_dong@emc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Listener_h
#define Pegasus_Listener_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Listener/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

class SSLContext;
class CIMIndicationConsumer;


/** CIMListener provides the interfaces that allows CIMListeners to be
    created and managed. CIMListeners are the receivers for cim/xml indications.
    ATTN: KS needs to be completed.TBD
*/
class PEGASUS_LISTENER_LINKAGE CIMListener
{
public:
	/**
	 * Constructs a CIMListener object.
	 * 
	 * @param portNumber	the specified socket port the listener will listen to
   * @param sslContext	the specifed SSL context 
	 */
	CIMListener(Uint32 portNumber, SSLContext* sslContext=NULL);
	/**
	 * Destructor of a CIMLIstener object.
	 */
    ~CIMListener();

	/**
	 * Returns the socket port number
	 *
	 * @return the socket port number.
    */
	Uint32 getPortNumber() const;

	/**
	 * Returns the SSL context
	 *
	 * @return the SSL context.
    */
	SSLContext* getSSLContext() const;

	/** 
	 * Sets the SSL context
	 *
	 * @param the SSL context.
    */
	void setSSLContext(SSLContext* sslContext);

	/** 
	 * Starts for listening.
    */
	void start();

	/**
	 * Stops listening
    */
	void stop();

	/**
	 * Returns if the listener is active
	 *
	 * @return true if the listener is active;
	 *				 false otherwise.
    */
	Boolean isAlive();

	/** 
	 * Adds a CIMIndicationConsumer to the listener
   *
	 * @param the CIMIndicationConsumer to add.
	 * @return true if the indication consumer has been added successfully
	 *				 false otherwise.
	 */
	Boolean addConsumer(CIMIndicationConsumer* consumer);
	/** 
	 * Removes a CIMIndicationConsumer from the listener.
	 *
	 * @param the CIMIndicationConsumer to remove.
	 * @return true if the indication consumer has been removed successfully
	 *				 false otherwise.
	 */
	Boolean removeConsumer(CIMIndicationConsumer* consumer);

private:
	void* _rep;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Listener_h */
