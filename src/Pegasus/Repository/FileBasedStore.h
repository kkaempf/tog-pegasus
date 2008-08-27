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

#ifndef Pegasus_FileBasedStore_h
#define Pegasus_FileBasedStore_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/CIMQualifierDecl.h>
#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/AutoStreamer.h>
#include <Pegasus/Common/Pair.h>
#include <Pegasus/Common/HashTable.h>

PEGASUS_NAMESPACE_BEGIN

class ClassAssociation
{
public:

    ClassAssociation(
        const CIMName& assocClassName_,
        const CIMName& fromClassName_,
        const CIMName& fromPropertyName_,
        const CIMName& toClassName_,
        const CIMName& toPropertyName_)
        : assocClassName(assocClassName_),
          fromClassName(fromClassName_),
          fromPropertyName(fromPropertyName_),
          toClassName(toClassName_),
          toPropertyName(toPropertyName_)
    {
    }

    CIMName assocClassName;
    CIMName fromClassName;
    CIMName fromPropertyName;
    CIMName toClassName;
    CIMName toPropertyName;
};

class InstanceAssociation
{
public:

    InstanceAssociation(
        const String& assocInstanceName_,
        const CIMName& assocClassName_,
        const String& fromInstanceName_,
        const CIMName& fromClassName_,
        const CIMName& fromPropertyName_,
        const String& toInstanceName_,
        const CIMName& toClassName_,
        const CIMName& toPropertyName_)
        : assocInstanceName(assocInstanceName_),
          assocClassName(assocClassName_),
          fromInstanceName(fromInstanceName_),
          fromClassName(fromClassName_),
          fromPropertyName(fromPropertyName_),
          toInstanceName(toInstanceName_),
          toClassName(toClassName_),
          toPropertyName(toPropertyName_)
    {
    }

    String assocInstanceName;
    CIMName assocClassName;
    String fromInstanceName;
    CIMName fromClassName;
    CIMName fromPropertyName;
    String toInstanceName;
    CIMName toClassName;
    CIMName toPropertyName;
};

class NamespaceDefinition
{
public:

    NamespaceDefinition(const CIMNamespaceName& name_)
        : name(name_),
          shareable(false),
          updatesAllowed(true),
          parentNameSpace(),
          remoteInfo()
    {
    }

    CIMNamespaceName name;
    Boolean shareable;
    Boolean updatesAllowed;
    CIMNamespaceName parentNameSpace;
    String remoteInfo;    // Only used with Remote CMPI
};

class PEGASUS_REPOSITORY_LINKAGE FileBasedStore
{
public:
    FileBasedStore(
        const String& repositoryPath,
        ObjectStreamer* streamer,
        Boolean compressMode);

    ~FileBasedStore();

    Array<NamespaceDefinition> enumerateNameSpaces();
    void createNameSpace(
        const CIMNamespaceName& nameSpace,
        Boolean shareable,
        Boolean updatesAllowed,
        const String& parent);
    void modifyNameSpace(
        const CIMNamespaceName& nameSpace,
        Boolean shareable,
        Boolean updatesAllowed);
    void deleteNameSpace(const CIMNamespaceName& nameSpace);
    Boolean isNameSpaceEmpty(const CIMNamespaceName& nameSpace);

    Array<CIMQualifierDecl> enumerateQualifiers(
        const CIMNamespaceName& nameSpace);
    /**
        Gets a qualifier declaration for a specified qualifier name in a
        specified namespace.  Returns an uninitialized object if the qualifier
        is not found.
    */
    CIMQualifierDecl getQualifier(
        const CIMNamespaceName& nameSpace,
        const CIMName& qualifierName);
    void setQualifier(
        const CIMNamespaceName& nameSpace,
        const CIMQualifierDecl& qualifierDecl);
    void deleteQualifier(
        const CIMNamespaceName& nameSpace,
        const CIMName& qualifierName);

    Array<Pair<String, String> > enumerateClassNames(
        const CIMNamespaceName& nameSpace);
    CIMClass getClass(
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        const CIMName& superClassName);
    /**
        Creates a class definition.  If the class is an association class,
        the class association entries are also added.
    */
    void createClass(
        const CIMNamespaceName& nameSpace,
        const CIMClass& newClass,
        const Array<ClassAssociation>& classAssocEntries);
    /**
        Modifies a class definition.  If the class is an association class,
        the class association entries are also updated to the specified set.
    */
    void modifyClass(
        const CIMNamespaceName& nameSpace,
        const CIMClass& modifiedClass,
        Boolean isAssociation,
        const Array<ClassAssociation>& classAssocEntries);
    /**
        Deletes a class definition.  If the class is an association class,
        the class association entries are also deleted.  It is expected to
        have already been verified that no instances of this class exist.  A
        list of dependent namespace names is provided to allow appropriate
        clean-up of instance files, if necessary.
    */
    void deleteClass(
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        const CIMName& superClassName,
        Boolean isAssociation,
        const Array<CIMNamespaceName>& dependentNameSpaceNames);

    Array<CIMObjectPath> enumerateInstanceNamesForClass(
        const CIMNamespaceName& nameSpace,
        const CIMName& className);
    Array<CIMInstance> enumerateInstancesForClass(
        const CIMNamespaceName& nameSpace,
        const CIMName& className);
    CIMInstance getInstance(
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& instanceName);
    /**
        Creates an instance definition.  If it is an association instance,
        the instance association entries are also added.
    */
    void createInstance(
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& instanceName,
        const CIMInstance& cimInstance,
        const Array<InstanceAssociation>& instAssocEntries);
    void modifyInstance(
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& instanceName,
        const CIMInstance& cimInstance);
    /**
        Deletes an instance definition.  If it is an association instance,
        the instance association entries are also deleted.
    */
    void deleteInstance(
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& instanceName);
    Boolean instanceExists(
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& instanceName);

    void getClassAssociatorNames(
        const CIMNamespaceName& nameSpace,
        const Array<CIMName>& classList,
        const Array<CIMName>& assocClassList,
        const Array<CIMName>& resultClassList,
        const String& role,
        const String& resultRole,
        Array<String>& associatorNames);
    void getClassReferenceNames(
        const CIMNamespaceName& nameSpace,
        const Array<CIMName>& classList,
        const Array<CIMName>& resultClassList,
        const String& role,
        Array<String>& referenceNames);

    void getInstanceAssociatorNames(
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& instanceName,
        const Array<CIMName>& assocClassList,
        const Array<CIMName>& resultClassList,
        const String& role,
        const String& resultRole,
        Array<String>& associatorNames);
    void getInstanceReferenceNames(
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& instanceName,
        const Array<CIMName>& resultClassList,
        const String& role,
        Array<String>& referenceNames);

private:

    void _rollbackIncompleteTransactions();

    /**
        Converts a namespace name into a directory path.  The specified
        namespace name is not required to match the case of the namespace
        name that was originally created.

        @param nameSpace The namespace for which to determine the directory
            path.
        @return A string containing the directory path for the namespace.
     */
    String _getNameSpaceDirPath(const CIMNamespaceName& nameSpace) const;

    /** Returns the path of the qualifier file.

        @param   nameSpace      the namespace of the qualifier
        @param   qualifierName  the name of the qualifier

        @return  a string containing the qualifier file path
     */
    String _getQualifierFilePath(
        const CIMNamespaceName& nameSpace,
        const CIMName& qualifierName) const;

    /** Returns the path of the class file.

        @param   nameSpace  the namespace of the class
        @param   className  the name of the class

        @return  a string containing the class file path
     */
    String _getClassFilePath(
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        const CIMName& superClassName) const;

    /** Returns the path of the instance index file.

        @param   nameSpace      the namespace of the instance
        @param   className      the name of the class

        @return  a string containing the index file path
     */
    String _getInstanceIndexFilePath(
        const CIMNamespaceName& nameSpace,
        const CIMName& className) const;

    /** Returns the path of the instance file.

        @param   nameSpace      the namespace of the instance
        @param   className      the name of the class

        @return  a string containing the instance file path
     */
    String _getInstanceDataFilePath(
        const CIMNamespaceName& nameSpace,
        const CIMName& className) const;

    /** Returns the path of the class association file.

        @param   nameSpace      the namespace of the associations

        @return  a string containing the class association file path
     */
    String _getAssocClassPath(const CIMNamespaceName& nameSpace) const;

    /** Returns the path of the instance association file.

        @param   nameSpace      the namespace of the associations

        @return  a string containing the instance association file path
     */
    String _getAssocInstPath(const CIMNamespaceName& nameSpace) const;

    Boolean _loadInstance(
        const String& path,
        CIMInstance& object,
        Uint32 index,
        Uint32 size);

    /** loads all the instance objects from disk to memeory.  Returns true
        on success.

        @param   nameSpace      the namespace of the instances to be loaded
        @param   className      the class of the instances to be loaded
        @param   namedInstances an array of CIMInstance objects to which
                                the loaded instances are appended

        @return  true      if successful
                 false     if an error occurs in loading the instances
     */
    Boolean _loadAllInstances(
        const CIMNamespaceName& nameSpace,
        const CIMName& className,
        Array<CIMInstance>& namedInstances);

    void _addClassAssociationEntries(
        const CIMNamespaceName& nameSpace,
        const Array<ClassAssociation>& classAssocEntries);
    void _removeClassAssociationEntries(
        const CIMNamespaceName& nameSpace,
        const CIMName& assocClassName);

    void _addInstanceAssociationEntries(
        const CIMNamespaceName& nameSpace,
        const Array<InstanceAssociation>& instanceAssocEntries);
    void _removeInstanceAssociationEntries(
        const CIMNamespaceName& nameSpace,
        const CIMObjectPath& assocInstanceName);

    String _repositoryPath;
    ObjectStreamer* _streamer;
    Boolean _compressMode;

    /**
        Maps namespace names to directory paths
    */
    HashTable<String, String, EqualNoCaseFunc, HashLowerCaseFunc>
        _nameSpacePathTable;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_FileBasedStore_h */