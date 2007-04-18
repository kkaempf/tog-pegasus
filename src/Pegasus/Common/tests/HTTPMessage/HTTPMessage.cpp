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


#include <Pegasus/Common/HTTPMessage.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/PegasusAssert.h>

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

static void _testContentType()
{

    String contentType;

    contentType = "application/xml    ;  charset =     \"utf-8\"  ";
    PEGASUS_TEST_ASSERT (HTTPMessage::isSupportedContentType (contentType));

    contentType = "text/xml    ;  CHARSET = \"utf-8\"";
    PEGASUS_TEST_ASSERT (HTTPMessage::isSupportedContentType (contentType));

    contentType = "   application/xml;charset=\"UTF-8\"   ";
    PEGASUS_TEST_ASSERT (HTTPMessage::isSupportedContentType (contentType));

    contentType = "application/xml";
    PEGASUS_TEST_ASSERT (HTTPMessage::isSupportedContentType (contentType));

    contentType = "TexT/XmL";
    PEGASUS_TEST_ASSERT (HTTPMessage::isSupportedContentType (contentType));

    contentType = "APPLICATION/XML;charset=\"utf-8\"";
    PEGASUS_TEST_ASSERT (HTTPMessage::isSupportedContentType (contentType));

    contentType = "TEXT/XML;CHARSET=\"UTF-8\"";
    PEGASUS_TEST_ASSERT (HTTPMessage::isSupportedContentType (contentType));

    // Test failure cases
    contentType = "application/xml   ;";
    PEGASUS_TEST_ASSERT (!HTTPMessage::isSupportedContentType (contentType));

    contentType = "application/xml;  charset = \"utf-88\"";
    PEGASUS_TEST_ASSERT (!HTTPMessage::isSupportedContentType (contentType));

    contentType = "application/xml ;  charset = \"utf-8\" ;";
    PEGASUS_TEST_ASSERT (!HTTPMessage::isSupportedContentType (contentType));

    contentType = "application/xml ;  charset  =";
    PEGASUS_TEST_ASSERT (!HTTPMessage::isSupportedContentType (contentType));

    contentType = "application/xml ;  charset";
    PEGASUS_TEST_ASSERT (!HTTPMessage::isSupportedContentType (contentType));

    contentType = "application/xml    ;   = \"utf-8\"";
    PEGASUS_TEST_ASSERT (!HTTPMessage::isSupportedContentType (contentType));
}

int main(int argc, char** argv)
{
    _testContentType();

    cout << argv[0] << " +++++ passed all tests" << endl;

    return 0;
}