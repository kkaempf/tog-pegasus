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

#ifndef ConsoleManager_zOS_h
#define ConsoleManager_zOS_h

#include <Pegasus/Common/Config.h>

PEGASUS_NAMESPACE_BEGIN

class ZOSConsoleManager
{

public:

    static void startConsoleWatchThread(void);

    /**
        Parse and execute a MODIFY command received from the console
    */
    static void processModifyCommand( char* command );

private:

    /** Constructor. */
    ZOSConsoleManager();

    static void _issueSyntaxError(const char* command);

    static void _displayServiceLevel();

    static char* _skipBlanks( char* commandPtr);

    static void _stripTrailingBlanks( char* token );

    static void _updateConfiguration( const String& configProperty,
                                     const String& propertyValue,
                                     Boolean currentValueIsNull,
                                     Boolean planned);

    static void _updateEnvironment(const char* envVarName,
                                  const char* envVarValue);

    static void _showEnvironment(const char* envVarName);

    static void _sendNotifyConfigChangeMessage( const String& propertyName,
                                                const String& newPropertyValue,
                                                Boolean currentValueModified);

    static void* _consoleCommandWatchThread(void*);

};

PEGASUS_NAMESPACE_END

#endif //ConsoleManager_zOS_h
