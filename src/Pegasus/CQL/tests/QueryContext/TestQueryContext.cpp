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
// Authors: David Rosckes (rosckes@us.ibm.com)
//          Bert Rivero (hurivero@us.ibm.com)
//          Chuck Carmack (carmack@us.ibm.com)
//          Brian Lucier (lucier@us.ibm.com) 
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////


#include <cstdlib>
#include <iostream>
#include <cassert>
                                                                                                                                       
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Query/QueryCommon/QueryContext.h>
#include <Pegasus/Query/QueryCommon/QueryIdentifier.h>
#include <Pegasus/Provider/CIMOMHandleQueryContext.h>
#include <Pegasus/Repository/RepositoryQueryContext.h>
#include <Pegasus/CQL/CQLIdentifier.h>
#include <Pegasus/CQL/CQLChainedIdentifier.h>
#include <Pegasus/Provider/CIMOMHandle.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Repository/CIMRepository.h>
                                                                                                                     
PEGASUS_USING_PEGASUS;
                                                                                                                                       
PEGASUS_USING_STD;

void drive_FromList(QueryContext& _query)
{
	Array<String> alias;
	Array<CQLIdentifier> classes;
	alias.append("A");
	alias.append("B");
	alias.append("C");
   alias.append("D");  // alias == identifier, ignore alias
   alias.append("A");  // dup, should not be inserted
	classes.append(CQLIdentifier("APPLE"));
	classes.append(CQLIdentifier("BONGO"));
	classes.append(CQLIdentifier("CLAVE"));
	classes.append(CQLIdentifier("D"));   // alias == identifier, ignore alias
	classes.append(CQLIdentifier("APPLE"));  // dup, should not be inserted

	for(Uint32 i = 0; i < alias.size(); i++){
		_query.insertClassPath(classes[i],alias[i]);
	}

   //
   // Error inserts.  Keep before the from list test below
   //

   // empty identifier
   try
   {
     _query.insertClassPath(QueryIdentifier());
     assert(false);
   }
   catch (QueryParseException & e)
   {
   }

   // identifier is already an alias
   try
   {
     _query.insertClassPath(CQLIdentifier("A"));
     assert(false);
   }
   catch (QueryParseException & e)
   {
   }

   // alias is already in the from list
   try
   {
     _query.insertClassPath(CQLIdentifier("NEW"),String("BONGO"));
     assert(false);
   }
   catch (QueryParseException & e)
   {
   }

   // alias is already used for another from list entry
   try
   {
     _query.insertClassPath(CQLIdentifier("NEW"),String("B"));
     assert(false);
   }
   catch (QueryParseException & e)
   {
   }


   // check the from list
	Array<QueryIdentifier> fromList = _query.getFromList();
   assert(fromList.size() == 4);
   assert(fromList[0].getName() == "APPLE");
   assert(fromList[1].getName() == "BONGO");
   assert(fromList[2].getName() == "CLAVE");
   assert(fromList[3].getName() == "D");

   // check the from string
   String fromString = _query.getFromString();
   assert(fromString == String("FROM APPLE AS A , BONGO AS B , CLAVE AS C , D "));

   // identifier and alias lookup
   QueryIdentifier lookup = _query.findClass(String("C"));
   assert(lookup.getName() == "CLAVE");
   lookup = _query.findClass(String("BONGO"));
   assert(lookup.getName() == "BONGO");
   lookup = _query.findClass(String("D"));
   assert(lookup.getName() == "D");
   lookup = _query.findClass(String("notthere"));
   assert(lookup.getName() == CIMName());
}

void drive_WhereIds(QueryContext& _query)
{
  CQLChainedIdentifier chid1("fromclass.eo.scope1::prop");
  CQLChainedIdentifier chid2("fromclass.eo.scope2::prop");
  CQLChainedIdentifier chid3("fromclass.eo.scope1::prop#'ok'");
  CQLChainedIdentifier chid4("fromclass.eo.scope1::prop[1]");

  _query.addWhereIdentifier(chid1);
  _query.addWhereIdentifier(chid2);
  _query.addWhereIdentifier(chid3);
  _query.addWhereIdentifier(chid4);
  _query.addWhereIdentifier(chid1); // dup, ignored

  Array<QueryChainedIdentifier> qchids = _query.getWhereList();

  assert(qchids.size() == 4);
  assert(qchids[0].getSubIdentifiers().size() == 3);
  assert(qchids[1].getSubIdentifiers().size() == 3);
  assert(qchids[2].getSubIdentifiers().size() == 3);
  assert(qchids[3].getSubIdentifiers().size() == 3);
}

void drive_Schema(QueryContext& _query)
{
  CIMName base("CQL_TestElement");
  CIMClass _class = _query.getClass(base);
  assert(_class.getClassName() == base);

  Array<CIMName> names = _query.enumerateClassNames(base);
  assert(names.size() == 2);

  CIMName derived("CQL_TestPropertyTypes");

  assert(_query.isSubClass(base, derived));
  assert(!_query.isSubClass(derived, base));

  assert(_query.getClassRelation(base, base) == QueryContext::SAMECLASS);
  assert(_query.getClassRelation(base, derived) == QueryContext::SUBCLASS);
  assert(_query.getClassRelation(derived, base) == QueryContext::SUPERCLASS);

  CIMName unrelated("CIM_Process");
  assert(_query.getClassRelation(base, unrelated) == QueryContext::NOTRELATED);
  assert(_query.getClassRelation(unrelated, base) == QueryContext::NOTRELATED);
}

void drive_CIMOMHandleQueryContext()
{
	CIMNamespaceName _ns("root/SampleProvider");
	CIMOMHandle _ch;
	CIMOMHandleQueryContext _queryOrig(_ns,_ch);

   CIMOMHandleQueryContext _query = _queryOrig;

   assert(_query.getNamespace() == _ns);

   drive_FromList(_query);
   drive_WhereIds(_query);
   drive_Schema(_query);
}

void drive_RepositoryQueryContext()
{
	const char* env = getenv("PEGASUS_HOME");

   if (env == NULL)
     exit(-1);

	String repositoryDir(env);
	repositoryDir.append("/repository");
	CIMNamespaceName _ns("root/SampleProvider");
	CIMRepository *_rep = new CIMRepository(repositoryDir);
	RepositoryQueryContext _queryOrig(_ns, _rep);

	RepositoryQueryContext _query = _queryOrig;   

   assert(_query.getNamespace() == _ns);

   drive_FromList(_query);
   drive_WhereIds(_query);
   drive_Schema(_query);
}

int main( int argc, char *argv[] )
{
  //
  // NOTE: this test needs to be in poststarttests
  // because the CIMOMHandle uses CIMClient local connect
  //

  //BEGIN TESTS....

  try
  {
	drive_CIMOMHandleQueryContext();
   drive_RepositoryQueryContext();
  }
  catch (Exception & e)
  {
    cout << "Received exception: " << e.getMessage() << endl;
    cout << argv[0] << "+++++ failed" << endl;
    return -1;
  }

	//END TESTS....

   cout << argv[0] << " +++++ passed all tests" << endl;
                             
   return 0;
}

