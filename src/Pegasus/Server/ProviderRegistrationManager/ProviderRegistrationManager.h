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
//%////////////////////////////////////////////////////////////////////////////


#ifndef Pegasus_ProviderRegistrationManager_h
#define Pegasus_ProviderRegistrationManager_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Server/ProviderRegistrationManager/Linkage.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/MessageQueueService.h>
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Provider/CIMInstanceProvider.h>
#include <Pegasus/Common/ModuleController.h>
#include <Pegasus/Common/CIMMessage.h>


PEGASUS_NAMESPACE_BEGIN

struct RegistrationTable;

struct ClassNamespaceTable;

/**
   The name of the provider module name  property for provider capabilities 
   class and PG_Provider class
*/
static const CIMName _PROPERTY_PROVIDERMODULENAME = 
    CIMName ("ProviderModuleName");

/**
   The name of the Vendor property for PG_ProviderModule class
*/
static const CIMName _PROPERTY_VENDOR  = CIMName ("Vendor");

/**
   The name of the Version property for PG_ProviderModule class
*/
static const CIMName _PROPERTY_VERSION  = CIMName ("Version");

/**
   The name of the interface type property for PG_ProviderModule class
*/
static const CIMName _PROPERTY_INTERFACETYPE  = CIMName ("InterfaceType");

/**
   The name of the interface version property for PG_ProviderModule class
*/
static const CIMName _PROPERTY_INTERFACEVERSION  = CIMName ("InterfaceVersion");

/**
   The name of the location property for PG_ProviderModule class
*/
static const CIMName _PROPERTY_LOCATION  = CIMName ("Location");

/**
   The name of the CapabilitiesID property for provider capabilities class
*/
static const CIMName _PROPERTY_CAPABILITIESID = CIMName ("CapabilityID");

/**
   The name of the provider name  property for provider capabilities class
*/
static const CIMName _PROPERTY_PROVIDERNAME = CIMName ("ProviderName");

/**
   The name of the classname property for provider capabilities class
*/
static const CIMName _PROPERTY_CLASSNAME = CIMName ("ClassName");

/**
   The name of the Namespace property for provider capabilities class
*/
static const CIMName _PROPERTY_NAMESPACES = CIMName ("Namespaces");

/**
   The name of the provider type  property for provider capabilities class
*/
static const CIMName _PROPERTY_PROVIDERTYPE = CIMName ("ProviderType");

/**
   The name of the supported properties property for provider capabilities class
*/
static const CIMName _PROPERTY_SUPPORTEDPROPERTIES = 
    CIMName ("SupportedProperties");

/**
   The name of the supported methods property for provider capabilities class
*/
static const CIMName _PROPERTY_SUPPORTEDMETHODS = CIMName ("SupportedMethods");

/**
   The name of the Destinations property for PG_ConsumerCapabilities class
*/
static const CIMName _PROPERTY_INDICATIONDESTINATIONS =
                                                CIMName ("Destinations");

/**
   The name of the AutoStart property for PG_Provider class
*/
static const CIMName _PROPERTY_AUTOSTART = CIMName ("AutoStart");

/**
   Registered instance provider type
*/
static const Uint16 _INSTANCE_PROVIDER    = 2;

/**
   Registered association provider type
*/
static const Uint16 _ASSOCIATION_PROVIDER    = 3;

/**
   Registered indication provider type
*/
static const Uint16 _INDICATION_PROVIDER    = 4;

/**
   Registered method provider type
*/
static const Uint16 _METHOD_PROVIDER    = 5;

/**
   Registered method provider type
*/
static const Uint16 _INSTANCE_QUERY_PROVIDER    = 7;

class PEGASUS_PRM_LINKAGE ProviderRegistrationManager
{
public:
    //ProviderRegistrationManager(ServiceCIMOMHandle & cimom);
    ProviderRegistrationManager(CIMRepository* repository);

    virtual ~ProviderRegistrationManager(void);

    Boolean lookupInstanceProvider(
        const CIMNamespaceName & nameSpace, 
        const CIMName & className,
        CIMInstance & provider, 
        CIMInstance & providerModule,
        Boolean is_assoc = false,
    Boolean * has_no_query = NULL);

    Boolean lookupMethodProvider(
        const CIMNamespaceName & nameSpace, 
        const CIMName & className, 
        const CIMName & method, 
        CIMInstance & provider, 
        CIMInstance & providerModule); 

    Boolean lookupAssociationProvider(
        const CIMNamespaceName & nameSpace, 
        const CIMName & assocClassName,
        Array<CIMInstance>& provider, 
        Array<CIMInstance>& providerModule);

    Boolean lookupIndicationConsumer(
        const String & destinationPath,
            CIMInstance & provider, 
        CIMInstance & providerModule);

    Boolean getIndicationProviders(
        const CIMNamespaceName & nameSpace, 
        const CIMName & className,
        const CIMPropertyList & requiredProperties,
        Array<CIMInstance> & provider,
        Array<CIMInstance> & providerModule); 

        /**
        This function is called with an (CIMObjectPath ref) specifying a
            instance of PG_ProviderModule, or instance of PG_Provider, or
        instance of PG_ProviderCapabilities, or instance of 
            PG_ConsumerCapabilities to be returned.

            @param ref specifies the fully qualified object path of the 
           instance of interest.

            @param includeQualifiers indicates whether the returned instance 
                   must include the qualifiers for the instance and properties.

            @param includeClassOrigin indicates whether the returned instance 
           must include the class origin for each of the instance 
                   elements.

            @param propertyList specifies the minimum set of properties 
                   required in instances returned by this operation.
        */
            
    CIMInstance getInstance(
            const CIMObjectPath & ref,
            const Boolean includeQualifiers = false,
            const Boolean includeClassOrigin = false,
            const CIMPropertyList & propertyList = CIMPropertyList());

    Array<CIMInstance> enumerateInstancesForClass(
               const CIMObjectPath & classReference,
                   const Boolean includeQualifiers = false,
                   const Boolean includeClassOrigin = false,
                   const CIMPropertyList & propertyList = CIMPropertyList());

    Array<CIMObjectPath> enumerateInstanceNamesForClass(
            const CIMObjectPath & ref);

    void modifyInstance(const CIMObjectPath & ref, 
        const CIMInstance & instance,
        const Boolean includeQualifiers,
        const Array<CIMName> & propertyList);

    CIMObjectPath createInstance(const CIMObjectPath & ref, 
        const CIMInstance & instance);

    void deleteInstance(const CIMObjectPath & ref);

    Array<Uint16> getProviderModuleStatus(const String & providerModuleName);

        /**
            Updates the Provider Module Status.  The value of the
            OperationalStatus property is updated in the Provider Module
            instance in the repository, and in the provider registration table.
            If any removeStatus values are specified, each one of the values
            found in the current OperationalStatus property value is removed
            from the array.  If any appendStatus values are specified, each one
            of the values not found in the current OperationalStatus property
            value is appended to the array.  The resulting OperationalStatus
            property value is returned in the output outStatus parameter.

            @param    providerModuleName String specifying name of the provider
                      module whose status is to be updated

            @param    removeStatus Array of Uint16 specifying values to be 
                      removed from the status array (may be empty)

            @param    appendStatus Array of Uint16 specifying values to be 
                      appended to the status array (may be empty)

            @param    outStatus OUTPUT Array of Uint16 returning the resulting
                      status array after update

            @return   True if provider module status is successfully updated; 
                      False, otherwise
        */
        Boolean updateProviderModuleStatus(
            const String& providerModuleName,
            const Array<Uint16>& removeStatus,
            const Array<Uint16>& appendStatus,
            Array<Uint16>& outStatus);

        /**
            Determines whether specified provider is an indication provider.

            @param    moduleName specifies name of the provider module which 
                      the provider is in.

        @param    providerName specifies name of the provider which
                      needs to be determine.

            @return   True if the provider is an indication provider; 
                      Otherwise, return false.
        */
        Boolean isIndicationProvider(
           const String & moduleName,
           const String & providerName);

    enum Operation {OP_CREATE = 1, OP_DELETE = 2, OP_MODIFY = 3};

protected:

    CIMRepository * _repository;

    /**
            HashTable to store instances of PG_ProviderModule, instances of
            PG_Provider, and instances of PG_ProviderCapability 
    */
    RegistrationTable* _registrationTable;

        /**
            A lock used to control access to the _registrationTable.  Before
            accessing the _registrationTable, one must first lock this for
            read access.  Before updating the _registrationTable, one must
            first lock this for write access.  One should never attempt to
            lock the _registrationTableLock while holding the repository
            lock.
        */
        ReadWriteSem _registrationTableLock;

    String _generateKey(const String & name,
        const String & provider);

    String _generateKey(const CIMNamespaceName & namespaceName,
        const CIMName & className,
        const String & providerType);

    String _generateKey(const CIMNamespaceName & namespaceName,
        const CIMName & className,
        const String & supportedMethod,
        const String & providerType);

    MessageQueueService * _getIndicationService();

    void _sendMessageToSubscription(
        CIMNotifyProviderRegistrationRequestMessage * notify);

private:

        /**
            Initialize the registration table.  The caller must first lock
            _registrationTableLock for write access.
        */
    void _initialRegistrationTable();

        /**
            Adds an entry to the registration table for the specified
            instances.  The caller must first lock _registrationTableLock
            for write access.
        */
    void _addInstancesToTable(const String & key,
        Array<CIMInstance> & instances); 

        /**
            Adds an entry to the registration table for the specified
            instances.  This method is intended for use in the initialization
            routine.  The caller must first lock _registrationTableLock for
            write access.
        */
    void _addInitialInstancesToTable(const String & key,
        const Array<CIMInstance> & instances); 

        /**
            Get the provider instance and module instance corresponding to
            the specified provider name or provider module name from the
            registration table.  The caller must first lock
            _registrationTableLock for read (or write) access.
        */
    void _getInstances(const String & providerName,
        const String & moduleName,
        CIMInstance & providerInstance,
        CIMInstance & moduleInstance);

    void _getPropertyNames(const CIMInstance & instance,
        CIMPropertyList & propertyNames);

        /**
            Notify the subscription service that the specified provider
            capability instance was deleted.  The caller must first lock
            _registrationTableLock for read (or write) access.
        */
    void _sendDeleteNotifyMessage(const CIMInstance & instance);

        /**
            Notify the subscription service that the specified provider
            capability instance was modified.  The caller must first lock
            _registrationTableLock for read (or write) access.
        */
    void _sendModifyNotifyMessage(const CIMInstance & instance,
                      const CIMInstance & origInstance);

        /**
            Register a provider.  The caller must first lock
            _registrationTableLock for write access.
        */
    CIMObjectPath _createInstance(const CIMObjectPath & ref, 
        const CIMInstance & instance, Operation flag);

        /**
            Unregister a provider.  The caller must first lock
            _registrationTableLock for write access.
        */
    void _deleteInstance(const CIMObjectPath & ref, Operation flag);

        /**
            Set the status of the specified provider module instance and
            update the repository.  The caller must first lock the repository
            for write access.
        */
    void _setStatus(const Array<Uint16> & status,
            CIMInstance & moduleInstance); 

        /**
            Gets the Provider Module Status.  The value of the OperationalStatus
            property is retrieved from the Provider Module instance in the
            provider registration table.

            @param    providerModuleName String specifying name of the provider
                      module whose status is to be retrieved

            @exception CIMException(CIM_ERR_NOT_FOUND) if the specified provider
                       module is not found.

            @return   Array of Uint16 containing the OperationalStatus property
                      value
        */
        Array<Uint16> _getProviderModuleStatus(
            const String& providerModuleName);
};

class PEGASUS_PRM_LINKAGE WildCardNamespaceNames {
   static Array<String> _nsstr;
   static Array<CIMNamespaceName> _ns;
   static Array<Uint32> _nsl;
  public:
   static String add(String ns);
//   static String & check(const String & in);
   static const CIMNamespaceName & check(const CIMNamespaceName & in);
   static void remap(CIMRepository *repos,
        Array<String> & in, Array<CIMNamespaceName> & names);
   static const Array<String> & getArray();
};

PEGASUS_NAMESPACE_END

#endif
