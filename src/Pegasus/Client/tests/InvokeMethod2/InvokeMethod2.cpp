//%/////////////////////////////////////////////////////////////////////////////
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
// Author: Warren Otsuka (warren_otsuka@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <cassert>
#include <Pegasus/Client/CIMClient.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

const String NAMESPACE = "root/SampleProvider";
const String classname = "Sample_MethodProviderClass";
const String methodName = "SayHello";
const String GOODREPLY = "Hello";

int main(int argc, char** argv)
{
    CIMClient client;
    client.connectLocal();

    CIMReference instanceName = "Sample_MethodProviderClass.Identifier=1";

    try
    {
	Array<CIMParamValue> inParams;
	Array<CIMParamValue> outParams;

	CIMValue retValue = client.invokeMethod(
	    NAMESPACE, 
	    instanceName, 
	    methodName,
	    inParams, 
	    outParams);

	if( retValue.toString() != GOODREPLY )
	  {
	    PEGASUS_STD(cerr) << "Error: bad reply \"" <<
	      retValue.toString() << "\"" << PEGASUS_STD(endl);
	    exit( 1 );
	  }
	else
	  {
	    PEGASUS_STD(cout) << "+++++ InvokeMethod2 passed all tests" << 
	      PEGASUS_STD(endl);
    
	    return 0;
	  }
   }
    catch(Exception& e)
    {
	PEGASUS_STD(cerr) << "Error: " << e.getMessage() << PEGASUS_STD(endl);
	exit(1);
    }
}
