//%LICENSE////////////////////////////////////////////////////////////////
//
// Licensed to The Open Group (TOG) under one or more contributor license
// agreements.  Refer to the OpenPegasusNOTICE.txt file distributed with
// this work for additional information regarding copyright ownership.
// Each contributor licenses this file to you under the OpenPegasus Open
// Source License; you may not use this file except in compliance with the
// License.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//////////////////////////////////////////////////////////////////////////
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Config/ConfigManager.h>

#include "PAMBasicAuthenticator.h"


PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

/**
    Constant representing the Basic authentication challenge header.
*/
static const String BASIC_CHALLENGE_HEADER = "WWW-Authenticate: Basic ";

/* constructor. */
PAMBasicAuthenticator::PAMBasicAuthenticator()
{
    PEG_METHOD_ENTER(TRC_AUTHENTICATION,
        "PAMBasicAuthenticator::PAMBasicAuthenticator()");

    // Build Authentication parameter realm required for Basic Challenge
    // e.g. realm="HostName"

    _realm.assign("realm=");
    _realm.append(Char16('"'));
    _realm.append(System::getHostName());
    _realm.append(Char16('"'));

    PEG_METHOD_EXIT();
}

/* destructor. */
PAMBasicAuthenticator::~PAMBasicAuthenticator()
{
    PEG_METHOD_ENTER(TRC_AUTHENTICATION,
        "PAMBasicAuthenticator::~PAMBasicAuthenticator()");

    PEG_METHOD_EXIT();
}

AuthenticationStatus PAMBasicAuthenticator::authenticate(
    const String& userName,
    const String& password,
    Boolean isRemoteUser,
    AuthenticationInfo* authInfo)
{
    PEG_METHOD_ENTER(TRC_AUTHENTICATION,
        "PAMBasicAuthenticator::authenticate()");

    // FUTURE: PAM authentication is not implemented on non Unix platforms,
    // hence this function return false as default indicating authentication
    // failure.

    Boolean authenticated = false;

    PEG_METHOD_EXIT();
    return AuthenticationStatus(authenticated);
}

AuthenticationStatus PAMBasicAuthenticator::validateUser(
    const String& userName,
    AuthenticationInfo* authInfo)
{
    PEG_METHOD_ENTER(TRC_AUTHENTICATION,
        "PAMBasicAuthenticator::validateUser()");

    // FUTURE: PAM authentication is not implemented on non Unix platforms,
    // hence this function return false as default indicating validation
    // failure.

    Boolean authenticated = false;

    PEG_METHOD_EXIT();
    return AuthenticationStatus(authenticated);
}

//
// Create authentication response header
//
String PAMBasicAuthenticator::getAuthResponseHeader()
{
    PEG_METHOD_ENTER(TRC_AUTHENTICATION,
        "PAMBasicAuthenticator::getAuthResponseHeader()");

    //
    // build response header using realm
    //
    String responseHeader = BASIC_CHALLENGE_HEADER;
    responseHeader.append(_realm);

    PEG_METHOD_EXIT();
    return responseHeader;
}

PEGASUS_NAMESPACE_END
