//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
// Author: Carol Ann Krug Graves, Hewlett-Packard Company 
//             (carolann_graves@hp.com)
//
// Modified By:  
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_SubscriptionRepository_h
#define Pegasus_SubscriptionRepository_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Server/Linkage.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/Common/AcceptLanguages.h>
#include <Pegasus/Common/ContentLanguages.h>
#include <Pegasus/Repository/CIMRepository.h>


PEGASUS_NAMESPACE_BEGIN

/**

    The SubscriptionRepository class manages Subscription, Filter, and
    Handler instances in the Repository.  

    @author  Hewlett-Packard Company

 */

class PEGASUS_SERVER_LINKAGE SubscriptionRepository
{
public:

    /**
        Constructs SubscriptionRepository instance.
     */
    SubscriptionRepository (
        CIMRepository * repository);

    /**
        Destructs SubscriptionRepository instance.
     */
    ~SubscriptionRepository (void);

    /**
        Adds a Filter, Handler or Subscription instance in the repository.  
        Adds or sets properties as necessary.

        @param   instance              instance to be added
        @param   nameSpace             namespace in which instance to be added
        @param   userName              user issuing request
        @param   acceptLanguages       acceptLanguages of request
        @param   contentLanguages      contentLanguages of request
        @param   enabled               indicates instance is a subscription 
                                         instance and is enabled

        @return  the CIMObjectPath for the instance
     */
    CIMObjectPath createInstance (
        CIMInstance instance,
        const CIMNamespaceName & nameSpace,
        const String & userName,
        const AcceptLanguages & acceptLanguages,
        const ContentLanguages & contentLanguages,
        Boolean enabled);

    /**
        Retrieves list of enabled subscription instances in all namespaces from
        the repository.  If an invalid instance is found, it is skipped, and 
        processing of remaining valid subscriptions continues.

        @param   activeSubscriptions  the returned subscription instances

        @return  True, if an invalid instance was found;
                 False otherwise
     */
    Boolean getActiveSubscriptions (
        Array <CIMInstance> & activeSubscriptions) const;

    /**
        Retrieves list of subscriptions in the specified namespace.

        @param   nameSpace             the namespace

        @return   List of subscription instances
     */
    Array <CIMInstance> getSubscriptions (
        const CIMNamespaceName & nameSpaceName) const;

    /**
        Gets the value of the SubscriptionState property from the specified
        Subscription instance.  If this function returns False, the value of
        the state parameter is unchanged.

        @param   instance              subscription instance
        @param   state                 value of SubscriptionState property if
                                       retrieved

        @return  True, if the value of the SubscriptionState property was
                       retrieved;
                 False if SubscriptionState property was missing, null, or of an
                       incorrect type
     */
    Boolean getState (
        const CIMInstance & instance,
        Uint16 & state) const;

    /**
        Deletes the specified subscription.  

        Note: If the specified subscription cannot be successfully retrieved, 
        it may have already been deleted by another thread.  In that case, an 
        uninitialized CIMInstance is returned.

        @param   subscription          the subscription object path

        @return  CIMInstance representing deleted subscription
     */
    CIMInstance deleteSubscription (
        CIMObjectPath & subscription);

    /**
        Deletes subscriptions referencing the specified handler.

        @param   nameSpace             the name space
        @param   referenceProperty     the name of the reference property in the
                                           subscription instance
        @param   handler               the handler reference

        @return  Array of CIMInstance objects representing deleted subscriptions
     */
    Array <CIMInstance> deleteReferencingSubscriptions (
        const CIMNamespaceName & nameSpace,
        const CIMName & referenceProperty,
        const CIMObjectPath & handler);

    /**
        Retrieves the Handler CIMInstance representing the handler of the
        specified subscription.

        @param   subscription          the subscription

        @return  a Handler CIMInstance for the subscription's handler
     */
    CIMInstance getHandler (
        const CIMInstance & subscription) const;

    /**
        Determines if specified handler is Transient.

        @param   nameSpace             the name space
        @param   handler               the handler reference

        @return  True, if the Handler is Transient;
                 False otherwise
     */
    Boolean isTransient (
        const CIMNamespaceName & nameSpace,
        const CIMObjectPath & handler) const;

    /**
        Retrieves the values of the filter query, source namespace,
        and query language properties for the specified subscription instance.

        @param   subscription      Input subscription instance
        @param   nameSpaceName     Input namespace name
        @param   query             Output query for the filter
        @param   sourceNameSpace   Output source namespace for the filter
                                       subscription
        @param   queryLanguage     Output query language in which the filter
                                       query is expressed
     */
    void getFilterProperties (
        const CIMInstance & subscription,
        const CIMNamespaceName & nameSpaceName,
        String & query,
        CIMNamespaceName & sourceNameSpace,
        String & queryLanguage);

    /**
        Retrieves the values of the filter query and source namespace
        properties for the specified subscription instance.

        @param   subscription      Input subscription instance
        @param   nameSpaceName     Input namespace name
        @param   query             Output query for the filter
        @param   sourceNameSpace   Output source namespace for the filter
                                       subscription
     */
    void getFilterProperties (
        const CIMInstance & subscription,
        const CIMNamespaceName & nameSpaceName,
        String & query,
        CIMNamespaceName & sourceNameSpace);

    /**
        Retrieves the value of the filter query property 
        for the specified subscription instance.

        @param   subscription      Input subscription instance
        @param   nameSpaceName     Input namespace name
        @param   query             Output query for the filter
     */
    void getFilterProperties (
        const CIMInstance & subscription,
        const CIMNamespaceName & nameSpaceName,
        String & query);

    /**
        Validates that the specified class is a subclass of the Indication 
        class.

        @param   indicationClassName   the class name to be validated
        @param   nameSpaceName         the namespace

        @return  String containing the indication class name
     */
    Boolean validateIndicationClassName (
        const CIMName & indicationClassName,
        const CIMNamespaceName & nameSpaceName) const;

    /**
        Retrieves the subclass names of the specified indication class.

        @param   nameSpace             the namespace
        @param   indicationClassName   the indication class name

        @return  List of indication subclass names, including original class
     */
    Array <CIMName> getIndicationSubclasses (
        const CIMNamespaceName & nameSpace,
        const CIMName & indicationClassName) const;

    /**
        Implements the subscription's On Fatal Error Policy.
        This function is called when a fatal error has occurred in the
        indication provider or indication handler, and the subscription can no
        longer be served.
        If the subscription's policy is Disable, the Subscription State is
        set to Disabled.
        If the subscription's policy is Remove, the subscription instance is 
        deleted.

        @param   subscription          the subscription instance

        @return  True, if the subscription has been disabled or deleted;
                 False otherwise
     */
    Boolean reconcileFatalError (
        const CIMInstance subscription);

    /**
        Retrieves the specified class object from the repository.

        @param   nameSpaceName         the namespace
        @param   className             the class name
        @param   localOnly             return only local elements
        @param   includeQualifiers     return qualifier elements
        @param   includeClassOrigin    return ClassOrigin attribute
        @param   propertyList          return specified properties

        @return  CIMClass object for the specified class
     */
    CIMClass getClass (
        const CIMNamespaceName & nameSpaceName,
        const CIMName & className,
        Boolean localOnly = true,
        Boolean includeQualifiers = true,
        Boolean includeClassOrigin = false,
        const CIMPropertyList & propertyList = CIMPropertyList()) const;

    /**
        Retrieves the specified instance object from the repository.

        @param   nameSpaceName         the namespace
        @param   instanceName          the instance object path
        @param   localOnly             return only local elements
        @param   includeQualifiers     return qualifier elements
        @param   includeClassOrigin    return ClassOrigin attribute
        @param   propertyList          return specified properties

        @return  CIMInstance object for the specified instance
     */
    CIMInstance getInstance (
        const CIMNamespaceName & nameSpace,
        const CIMObjectPath & instanceName,
        Boolean localOnly = true,
        Boolean includeQualifiers = false,
        Boolean includeClassOrigin = false,
        const CIMPropertyList & propertyList = CIMPropertyList ());

    /**
        Modifies the specified instance in the repository.

        @param   nameSpaceName         the namespace
        @param   modifiedInstance      the modified instance object
        @param   includeQualifiers     return qualifier elements
        @param   propertyList          return specified properties
     */
    void modifyInstance (
        const CIMNamespaceName & nameSpace,
        const CIMInstance & modifiedInstance,
        Boolean includeQualifiers = true,
        const CIMPropertyList & propertyList = CIMPropertyList ());

    /**
        Deletes the specified instance from the repository.

        @param   nameSpaceName         the namespace
        @param   instanceName          the instance object path
     */
    void deleteInstance (
        const CIMNamespaceName & nameSpace,
        const CIMObjectPath & instanceName);

    /**
        Enumerates instances of the specified class from the repository.

        @param   nameSpaceName         the namespace
        @param   className             the class name
        @param   deepInheritance       return inherited properties
        @param   localOnly             return only local elements
        @param   includeQualifiers     return qualifier elements
        @param   includeClassOrigin    return ClassOrigin attribute
        @param   includeInheritance    return instances of subclasses
        @param   propertyList          return specified properties

        @return  Array of CIMInstance objects for the specified enumeration
     */
    Array <CIMInstance> enumerateInstancesForClass (
        const CIMNamespaceName & nameSpace,
        const CIMName & className,
        Boolean deepInheritance = true,
        Boolean localOnly = true,
        Boolean includeQualifiers = false,
        Boolean includeClassOrigin = false,
        Boolean includeInheritance = false,
        const CIMPropertyList & propertyList = CIMPropertyList ());

    /**
        Enumerates instance names of the specified class from the repository.

        @param   nameSpaceName         the namespace
        @param   className             the class name
        @param   includeInheritance    return names of instances of subclasses

        @return  Array of CIMObjectPath objects for the specified enumeration
     */
    Array <CIMObjectPath> enumerateInstanceNamesForClass (
        const CIMNamespaceName & nameSpace,
        const CIMName & className,
        const Boolean includeInheritance = true);

private:

    /**
        Disables the subscription.
        This function is called when a fatal error has occurred in the
        indication provider or indication handler, the subscription can no
        longer be served, and the subscription's policy is Disable.
        The Subscription State is set to Disabled.

        @param   subscription          the subscription instance
     */
    void _disableSubscription (
        CIMInstance subscription);

    /**
        Deletes the subscription instance.
        This function is called when a fatal error has occurred in the
        indication provider or indication handler, the subscription can no
        longer be served, and the subscription's policy is Remove.
        The subscription instance is deleted.

        @param   subscription          the subscription instance
     */
    void _deleteSubscription (
        const CIMInstance subscription);

    CIMRepository * _repository;
};

PEGASUS_NAMESPACE_END

#endif  /* Pegasus_SubscriptionRepository_h */
