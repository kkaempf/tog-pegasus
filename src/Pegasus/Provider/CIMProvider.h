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

#ifndef Pegasus_CIMProvider_h
#define Pegasus_CIMProvider_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/OperationContext.h>
#include <Pegasus/Provider/CIMOMHandle.h>
#include <Pegasus/Provider/ProviderException.h>
#include <Pegasus/Common/ResponseHandler.h>
#include <Pegasus/Provider/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

/**
Parent class for all provider interfaces.

<p>The <i>CIMProvider</i> is the parent class for all provider
interface types. The currently supported interfaces are:</p>

<ul>
<li><b>{@link CIMInstanceProvider CIMInstanceProvider}</b> - supports
manipulation of CIM instances and their properties</li>
<li><b>{@link CIMMethodProvider CIMMethodProvider}</b> - supports
invocation of methods defined on CIM instances</li>
</ul>

<p>A provider may inherit from either or both of these
interface classes. A provider <i>must</i> implement every
function in the chosen interface(s). However, it is not
required that all operations be supported. If an operation
is not supported, then a minimal implementation of
the corresponding function must throw a
{@link NotSupported NotSupported} exception.</p>

<p>\Label{mainParams}Certain parameters are passed in several of the functions
in the provider interfaces. These are also described in their own
sections, and include:</p>

<p><ul>
<li><b>{@link OperationContext OperationContext}</b> - contains
information about the client's context, including the User ID.
    The provider must determine whether the specified user
    should be permitted to perform the operation. If the
    operation should not be permitted, the provider must throw
    an {@link AccessDenied AccessDenied} exception.
</li>

<li><b>{@link CIMObjectPath CIMObjectPath}</b> - specifies the
CIM object on which the operation is to be performed. This
parameter specifies the hostname, namespace, classname, and key values that
uniquely identify an instance of a CIM object.
<p><b>hostname</b> - specifies the name of the system on which
the object resides. This is not required to be the system on
which the CIM server is running, but generally will be.</p>

<p><b>namespace</b> - specifies the <i>namespace</i> on the
aforementioned host in which the object resides.</p>

<p><b>classname</b> - specifies the class on which the requested
operation is to be performed.</p>

<p><b>keybindings</b> - the set of key properties for the aforementioned
class. The set of keys uniquely identifies a CIM instance in the
host and namespace. It is permissible for clients to specify,
and providers should accept,
empty strings for key values when this would not be ambiguous.
If the specification is ambiguous, or if a key name is invalid or
missing, the provider
should throw an {@link InvalidParameter InvalidParameter}
exception.</li>

<li><b>{@link CIMInstance CIMInstance}</b> - contains a
representation of an instance of a CIM object to be used
by the requested operation. This parameter should contain
all of the key properties, as well as all properties
necessary to perform the requested operation.</li>

<li><b>{@link CIMPropertyList CIMPropertyList}</b> - specifies
the properties on which this operation should be performed.
The exact use of this parameter depends on the specific
operation, and is described in the respective section.</li>

<li><b>{@link ResponseHandler ResponseHandler}</b> - a
<i>callback</i> handle used to return results to the CIM Server
for subsequent return to the client.</li>
</ul></p>

<p>Certain exceptions can be thrown by several of the functions
in the provider interfaces. These are described in their own
sections, and include:</p>
<p><ul>
<li><b>{@link CIMNotSupportedException CIMNotSupportedException}</b> - the operation is not
supported.</li>
<li><b>{@link CIMInvalidParameterException CIMInvalidParameterException}</b> - a parameter's
value was invalid. This could be an unknown property or key name, an
invalid flag, or other.</li>
<li><b>{@link CIMObjectNotFoundException CIMObjectNotFoundException}</b> - the object specified
in the {@link CIMObjectPath CIMObjectPath} parameter could not be
found or does not exist.</li>
<li><b>{@link CIMObjectAlreadyExistsException CIMObjectAlreadyExistsException}</b> - the object specified in a
<tt>{@link createInstance createInstance}</tt> operation already
exists.</li>
<li><b>{@link CIMAccessDeniedException CIMAccessDeniedException}</b> - the requested
operation is not permitted. This can be because the user specified in
the {@link OperationContext} parameter is not authorized to perform
the requested operation, or another reason.</li>
<li><b>{@link CIMOperationFailedException CIMOperationFailedException}</b> - a failure occurred during
processing of the operation.</li>
</ul></p>

<!-- save this material for later
<h3>Classes Instrumented</h3>

<p>A provider may be registered to perform
operations on elements of more than one class. This
is useful when different classes of object are related
in the system resources that they manipulate, or
the system services that they call. It is also possible
for a provider to perform operations on several levels
of the same line of descent.</p>
-->

<p>The CIMProvider interface contains two functions that are
inherited by all provider interfaces:</p>

<p><ul>
<li><tt>{@link initialize initialize}</tt> - Called before the
first call to any client-requested operation; the provider
should perform any processing
that may be required before normal operation can begin.</li>
<li><tt>{@link terminate terminate}</tt> - Called prior to
the provider being stopped; the provider should perform any
final processing that may be required.</li>
</ul></p>

<p>Providers <i>must</i> implement these functions. A minimal
implementation may simply return to the caller.</p>
*/
class PEGASUS_PROVIDER_LINKAGE CIMProvider
{
public:
    ///
    CIMProvider(void);

    ///
    virtual ~CIMProvider(void);

    /**
    Perform any setup required before normal operation.
    <p>The <TT>initialize</TT> function allows the provider to conduct the
    necessary preparations to handle requests.
    It is called only once during the lifetime of the provider. This
    function must complete before the CIM Server invokes any other function of
    the provider, other than terminate.</p>

    @param cimom Reserved for future use.

    @exception None
    */
    virtual void initialize(CIMOMHandle & cimom) = 0;

    /**
    Perform any cleanup required before termination.
    <p>The <TT>terminate</TT> function allows the provider to conduct the
    necessary preparations for termination. This function
    may be called by the CIM Server
    at any time, including initialization. Once invoked, no other provider
    functions are invoked until after an eventual call to <tt>initialize</tt>.</p>
    <p>The provider may, for example, do the following in the
    <tt>terminate</tt> function:</p>
    <ul>
    <li>close files or I/O streams</li>
    <li>release resources such as shared memory</li>
    <li>inform concurrently executing
    requests to complete immediately (this may be done by
    setting a global flag)</li>
    <li>kill subprocesses</li>
    <li>etc.</li>
    </ul>
    <p>If the provider instance was created on the heap with
    <i>new</i> in <tt>PegasusCreateProvider</tt>, then it must
    be deleted in <tt>terminate</tt>:
    <pre>void MyProvider::terminate()
    {
    ...
    delete this;
    ...
    return;
    }</pre>

    @exception None
    */
    virtual void terminate(void) = 0;

    /** 
    Allow a provider to decline a terminate call. If the provider
    is unable to terminate, it should return false. Otherwise, 
    it should call its <code>terminate</code> function and then
    return true, as in the default implementation.

    @exception None
    */
    virtual Boolean tryTerminate(void) 
      {
	terminate();
	return true;
      }

};

#ifndef PEGASUS_REMOVE_DEPRECATED
typedef CIMProvider CIMBaseProvider;
#endif



PEGASUS_NAMESPACE_END

#endif
