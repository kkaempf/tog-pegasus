
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
// Modified By: Yi Zhou (yi_zhou@hp.com)
//
// Modified By:
//         Bapu Patil, Hewlett-Packard Company ( bapu_patil@hp.com )
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <cassert>
#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Common/OptionManager.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

const String NAMESPACE = "root/cimv2";

static const char CERTIFICATE[] = "server.pem";
static const char RANDOMFILE[] = "ssl.rnd";

static void TestGetClass(CIMClient& client)
{
    CIMClass c = client.getClass(
	NAMESPACE, "CIM_ComputerSystem", false, false, true);

    // c.print();
}

static void TestClassOperations(CIMClient& client)
{
    // CreateClass:

    CIMClass c1("SubClass", "CIM_ManagedElement");
    c1.addQualifier(CIMQualifier("abstract", Boolean(true)));
    c1.addProperty(CIMProperty("count", Uint32(99)));
    c1.addProperty(CIMProperty("ratio", Real64(66.66)));
    c1.addProperty(CIMProperty("message", String("Hello World")));
    client.createClass(NAMESPACE, c1);

    // GetClass:

    CIMClass c2 = client.getClass(NAMESPACE, "SubClass", false);
    // assert(c1.identical(c2));

    // Modify the class:

    c2.removeProperty(c2.findProperty("message"));
    client.modifyClass(NAMESPACE, c2);

    // GetClass:

    CIMClass c3 = client.getClass(NAMESPACE, "SubClass", false);
    // assert(c3.identical(c2));

    // EnumerateClassNames:

    Array<String> classNames = client.enumerateClassNames(
	NAMESPACE, "CIM_ManagedElement", false);

    Boolean found = false;

    for (Uint32 i = 0; i < classNames.size(); i++)
    {
	if (CIMName::equal(classNames[i], "SubClass"))
	    found = true;
    }

    assert(found);

    // DeleteClass:

    client.deleteClass(NAMESPACE, "SubClass");

    // Get all the classes;


    classNames = client.enumerateClassNames(NAMESPACE, String(), false);

    Array<CIMClass> classDecls = client.enumerateClasses(
	NAMESPACE, String(), false, false, true, true);

    assert(classDecls.size() == classNames.size());

    for (Uint32 i = 0; i < classNames.size(); i++)
    {
	CIMClass tmp = client.getClass(
	    NAMESPACE, classNames[i], false, true, true);

	assert(CIMName::equal(classDecls[i].getClassName(), classNames[i]));

	assert(tmp.identical(classDecls[i]));
    }
}

static void TestQualifierOperations(CIMClient& client)
{
    // Create two qualifier declarations:

    CIMQualifierDecl qd1("qd1", false, CIMScope::CLASS, CIMFlavor::TOSUBCLASS);
    client.setQualifier(NAMESPACE, qd1);

    CIMQualifierDecl qd2("qd2", "Hello", CIMScope::PROPERTY | CIMScope::CLASS, 
	CIMFlavor::OVERRIDABLE);
    client.setQualifier(NAMESPACE, qd2);

    // Get them and compare:

    CIMQualifierDecl tmp1 = client.getQualifier(NAMESPACE, "qd1");
    assert(tmp1.identical(qd1));

    CIMQualifierDecl tmp2 = client.getQualifier(NAMESPACE, "qd2");
    assert(tmp2.identical(qd2));

    // Enumerate the qualifiers:

    Array<CIMQualifierDecl> qualifierDecls 
	= client.enumerateQualifiers(NAMESPACE);

    for (Uint32 i = 0; i < qualifierDecls.size(); i++)
    {
	CIMQualifierDecl tmp = qualifierDecls[i];

	if (CIMName::equal(tmp.getName(), "qd1"))
	    assert(tmp1.identical(tmp));

	if (CIMName::equal(tmp.getName(), "qd2"))
	    assert(tmp2.identical(tmp));
    }

    // Delete the qualifiers:

    client.deleteQualifier(NAMESPACE, "qd1");
    client.deleteQualifier(NAMESPACE, "qd2");
}

static void TestInstanceOperations(CIMClient& client)
{
    // Delete the class if it already exists:

    try
    {
        Array<CIMReference> instanceNames =
           client.enumerateInstanceNames(NAMESPACE, "MyClass");
	for (Uint32 i = 0; i < instanceNames.size(); i++)
        {
           client.deleteInstance(NAMESPACE, instanceNames[i]);
        }
// ATTN DE P1 30 March 2002 There may be a case-sensitivity problem with 
//      the Repository's use of class names to create file names.
//      Changing "myclass" to "MyClass" works-around the problem of 
//      running Client application multiple times.

	client.deleteClass(NAMESPACE, "MyClass");
    }
    catch (Exception&)
    {
	// Ignore delete class!
    }

    // Create a new class:

    CIMClass cimClass("MyClass");
    cimClass
	.addProperty(CIMProperty("last", String())
	    .addQualifier(CIMQualifier("key", true)))
	.addProperty(CIMProperty("first", String())
	    .addQualifier(CIMQualifier("key", true)))
	.addProperty(CIMProperty("age", Uint8(0))
	    .addQualifier(CIMQualifier("key", true)));
    client.createClass(NAMESPACE, cimClass);

    // Create an instance of that class:

    CIMInstance cimInstance("MyClass");
    cimInstance.addProperty(CIMProperty("last", "Smith"));
    cimInstance.addProperty(CIMProperty("first", "John"));
    cimInstance.addProperty(CIMProperty("age", Uint8(101)));
    CIMReference instanceName = cimInstance.getInstanceName(cimClass);
    CIMReference createdinstanceName = client.createInstance(NAMESPACE, cimInstance);

    // Get the instance and compare with created one:

    CIMInstance tmp = client.getInstance(NAMESPACE, instanceName);

    // cimInstance.print();
    // tmp.print();

    client.deleteInstance(NAMESPACE, instanceName);
}

static void TestAssociators(CIMClient& client)
{
    CIMReference instanceName = "Person.name=\"Mike\"";

    Array<CIMObjectWithPath> result = client.associators(
	NAMESPACE, 
	instanceName, 
	"Lineage", 
	"Person", 
	"parent", 
	"child", 
	true, 
	true);

    for (Uint32 i = 0; i < result.size(); i++)
    {
	CIMObjectWithPath current = result[i];
	CIMReference ref = current.getReference();
	cout << "[" << ref << "]" << endl;
    }

    cout << endl;
}

static void TestAssociatorNames(CIMClient& client)
{
    CIMReference instanceName = "Person.name=\"Mike\"";

    Array<CIMReference> result = client.associatorNames(
	NAMESPACE, 
	instanceName, 
	"Lineage", 
	"Person", 
	"parent", 
	"child");

    for (Uint32 i = 0; i < result.size(); i++)
	cout << "[" << result[i] << "]" << endl;

    cout << endl;
}

static void TestAssociatorClassNames(CIMClient& client)
{
    CIMReference className = "Person";

    Array<CIMReference> result = client.associatorNames(
	NAMESPACE, 
	className, 
	"Lineage", 
	"Person", 
	"parent", 
	"child");

    for (Uint32 i = 0; i < result.size(); i++)
	cout << "[" << result[i] << "]" << endl;

    cout << endl;
}

static void TestReferenceNames(CIMClient& client)
{
    CIMReference instanceName = "Person.name=\"Mike\"";

    Array<CIMReference> result = client.referenceNames(
	NAMESPACE, 
	instanceName, 
	"Lineage", 
	"parent");

    for (Uint32 i = 0; i < result.size(); i++)
	cout << "[" << result[i] << "]" << endl;

    cout << endl;
}

static void TestReferences(CIMClient& client)
{
    CIMReference instanceName = "Person.name=\"Mike\"";

    Array<CIMObjectWithPath> result = client.references(
	NAMESPACE, 
	instanceName, 
	"Lineage", 
	"parent");

    for (Uint32 i = 0; i < result.size(); i++)
    {
	CIMObjectWithPath current = result[i];
	CIMReference ref = current.getReference();
	cout << "[" << ref << "]" << endl;
    }

    cout << endl;
}

static void TestReferenceClassNames(CIMClient& client)
{
    CIMReference className = "Person";

    Array<CIMReference> result = client.referenceNames(
	NAMESPACE, 
	className, 
	String(),
	"parent");

    for (Uint32 i = 0; i < result.size(); i++)
	cout << "[" << result[i] << "]" << endl;

    cout << endl;
}

int main(int argc, char** argv)
{

    Boolean useSSL =  false;

    if ( argc > 1 )
    {
      char *temp = argv[1];
      if ( ! String::compareNoCase(temp, "-ssl") )
      {
        cout << "SSL Enabled " << endl;
        useSSL = true;
      }
      else
      {
        cout << "SSL not Enabled " << endl;
      }
    }
    try
    {
	CIMClient client;

        if (useSSL)
        {

               //
               // Get environment variables:
               //
               const char* pegasusHome = getenv("PEGASUS_HOME");

               String certpath = String::EMPTY;
               if (pegasusHome)
               {
                     certpath.append(pegasusHome);
                     certpath.append("/");
               }
               certpath.append(CERTIFICATE);


#ifdef PEGASUS_SSL_RANDOMFILE

               String randFile = String::EMPTY;
               if (pegasusHome)
               {
                  randFile.append(pegasusHome);
                  randFile.append("/");
               }
               randFile.append(RANDOMFILE);

               SSLContext * sslcontext = new SSLContext(certpath, randFile, true);
#else
               SSLContext * sslcontext = new SSLContext(certpath);

#endif

               client.connect("localhost:5988", sslcontext);
        }
        else
        { 
	      client.connect("localhost:5988");
        }

	TestGetClass(client);
	TestQualifierOperations(client);
	TestClassOperations(client);
	TestInstanceOperations(client);

	// To run the following test, first compile "test.mof" into the
	// repository!
	TestAssociatorNames(client);
	TestAssociators(client);
	TestReferenceNames(client);
	TestReferences(client);
	TestAssociatorClassNames(client);
	TestReferenceClassNames(client);
    }
    catch(Exception& e)
    {
	PEGASUS_STD(cerr) << "Error: " << e.getMessage() << PEGASUS_STD(endl);
	exit(1);
    }

    PEGASUS_STD(cout) << "+++++ passed all tests" << PEGASUS_STD(endl);

    return 0;
}
