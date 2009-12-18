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

/*******************************************************************************
*
* Command line options processing.  All option processing is based on the
* Pegasus OptionManager and the definition of options contained in this
* file. This file also contains the funtions to process input options.
*
* Once processed, the input options are placed into the options structure
* to be passed to the operation processors.
*
*******************************************************************************/
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/PegasusAssert.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/General/MofWriter.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/StringConversion.h>
#include <Pegasus/Common/ArrayInternal.h>
#include <Pegasus/Common/PegasusVersion.h>

#include "CIMCLIOptions.h"
#include "CIMCLIClient.h"
#include "CIMCLIHelp.h"
#include "CIMCLICommon.h"
#include "CIMCLIClient.h"

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

/*******************************************************************************
**
**  BuildOptionsTable
**
*******************************************************************************/
/*
    This function builds the complete options table from the entries defined
    below.  It then merges in the options from any config file and from any
    command line parameters. Thus, the command line parameters will override
    any config file parameters.
*/
void BuildOptionsTable(
    OptionManager& om,
    int& argc,
    char** argv,
    const String& testHome)
{
    // Table of available options to be used by cimcli.  Each of the possible
    // options is defined in one entry in the table below.

    static const char* outputFormats[] = { "xml", "mof", "txt", "table"};
    static const Uint32 NUM_OUTPUTFORMATS = sizeof(outputFormats) /
                                            sizeof(outputFormats[0]);
    static OptionRowWithMsg optionsTable[] =
        //optionname defaultvalue rqd  type domain domainsize clname msgkey
        // hlpmsg
    {
        // FUTURE TODO.  This strange number was a way to tell option parser
        // that the option was not found on the command line.  That concept
        //  does not exist today in the OptionManager. The error mechanism
        // tells you whether the option name is defined not whether there was
        // an option supplied.  Need to fix OptionManager
        {"count", "29346", false, Option::WHOLE_NUMBER, 0, 0, "count",
        "Clients.cimcli.CIMCLIClient.COUNT_OPTION_HELP",
        "Expected count of objects returned if summary set.\n"
            "    Tests this count and display difference.\n"
            "    Return nonzero if test fails"},

        {"debug", "false", false, Option::BOOLEAN, 0, 0, "d",
        "Clients.cimcli.CIMCLIClient.DEBUG_OPTION_HELP",
        "More detailed debug messages"},

        {"delay", "0", false, Option::WHOLE_NUMBER, 0, 0, "delay",
        "Clients.cimcli.CIMCLIClient.DELAY_OPTION_HELP",
        "Delay between connection and request"},

        {"Password", "", false, Option::STRING, 0, 0, "p",
        "Clients.cimcli.CIMCLIClient.PASSWORD_OPTION_HELP",
        "Defines password for authentication" },

        {"location", "", false, Option::STRING, 0, 0, "l",
        "Clients.cimcli.CIMCLIClient.LOCATION_OPTION_HELP",
        "Specifies system and port (HostName:port).\n"
            "    Port is optional" },

#ifdef PEGASUS_HAS_SSL
        {"ssl", "false", false, Option::BOOLEAN, 0, 0, "s",
        "Clients.cimcli.CIMCLIClient.SSL_OPTION_HELP",
        "Specifies to connect over HTTPS" },

        {"clientCert", "", false, Option::STRING, 0, 0, "-cert",
        "Clients.cimcli.CIMCLIClient.CLIENTCERT_OPTION_HELP",
        "Specifies a client certificate file path to present to the server.\n"
            "    This is optional and only has an effect on connections\n"
            "    made over HTTPS using -s" },

        {"clientKey", "", false, Option::STRING, 0, 0, "-key",
        "Clients.cimcli.CIMCLIClient.CLIENTKEY_OPTION_HELP",
        "Specifies a client private key file path.\n"
            "    This is optional and only has an effect on connections\n"
            "    made over HTTPS using -s" },
#endif
        {"User", "", false, Option::STRING, 0, 0, "u",
        "Clients.cimcli.CIMCLIClient.USER_OPTION_HELP",
        "Defines User Name for authentication" },

        {"namespace", "root/cimv2", false, Option::STRING, 0, 0, "n",
        "Clients.cimcli.CIMCLIClient.NAMESPACE_OPTION_HELP",
        "Specifies namespace to use for operation" },

        {"deepInheritance", "false", false, Option::BOOLEAN, 0, 0, "di",
        "Clients.cimcli.CIMCLIClient.DEEPINHERITANCE_OPTION_HELP",
        "If set deepInheritance parameter\n"
            "    set true"},

        // TODO - Drop this option completely
        {"localOnly", "true", false, Option::BOOLEAN, 0, 0, "lo",
        "Clients.cimcli.CIMCLIClient.LOCALONLY_OPTION_HELP",
        "DEPRECATED. This was used to set LocalOnly.\n"
            "    However, default should be true and we cannot use True\n"
            "    as default. See -nlo"},

        {"notLocalOnly", "false", false, Option::BOOLEAN, 0, 0, "nlo",
        "Clients.cimcli.CIMCLIClient.NOTLOCALONLY_OPTION_HELP",
        "When set, sets LocalOnly = false on\n"
            "    operations"},

        // TODO - Deprecated so we should drop this one completely
        {"includeQualifiers", "true", false, Option::BOOLEAN, 0, 0, "iq",
        "Clients.cimcli.CIMCLIClient.INCLUDEQUALIFIERS_OPTION_HELP",
        "DEPRECATED. Sets includeQualifiers = True.\n"
            "    However, default=true"},

        {"notIncludeQualifiers", "false", false, Option::BOOLEAN, 0, 0, "niq",
        "Clients.cimcli.CIMCLIClient.NOTINCLUDEQUALIFIERS_OPTION_HELP",
        "Sets includeQualifiers = false\n"
            "    on operations"},

        // Uses a magic string as shown below to indicate never used.
        {"propertyList", "###!###", false, Option::STRING, 0, 0, "pl",
        "Clients.cimcli.CIMCLIClient.PROPERTYLIST_OPTION_HELP",
        "Defines a propertyNameList. Format is p1,p2,p3\n"
            "    (without spaces). Use \"\" for empty"},

        {"assocClass", "", false, Option::STRING, 0, 0, "ac",
        "Clients.cimcli.CIMCLIClient.ASSOCCLASS_OPTION_HELP",
        "Defines a assocation Class string for Associator calls"},

        {"assocRole", "", false, Option::STRING, 0, 0, "ar",
        "Clients.cimcli.CIMCLIClient.ASSOCROLE_OPTION_HELP",
        "Defines a role string for Associators. AssocRole\n"
            "    parameter"},

        {"role", "", false, Option::STRING, 0, 0, "r",
        "Clients.cimcli.CIMCLIClient.ROLE_OPTION_HELP",
        "Defines a role string for reference role parameter"},

        {"resultClass", "", false, Option::STRING, 0, 0, "rc",
        "Clients.cimcli.CIMCLIClient.RESULTCLASS_OPTION_HELP",
        "Defines a resultClass string for References and\n"
            "    Associatiors"},

        {"resultRole", "", false, Option::STRING, 0, 0, "rr",
        "Clients.cimcli.CIMCLIClient.RESULTROLE_OPTION_HELP",
        "Defines a role string for associators operation resultRole\n"
            "    parameter"},

        {"inputParameters", "", false, Option::STRING, 0, 0, "ip",
        "Clients.cimcli.CIMCLIClient.INPUTPARAMETERS_OPTION_HELP",
        "Defines an invokeMethod input parameter list.\n"
            "    Format is p1=v1 p2=v2 .. pn=vn\n"
            "    (parameters are seperated by spaces)"},

        {"filter", "", false, Option::STRING, 0, 0, "f",
        "Clients.cimcli.CIMCLIClient.FILTER_OPTION_HELP",
        "Defines a filter to use for query. Single String input"},

        // KS change the output formats to use the enum options function
        // Deprecate this function.
        {"outputformats", "mof", false, Option::STRING, 0,NUM_OUTPUTFORMATS,
        "o",
        "Clients.cimcli.CIMCLIClient.OUTPUTFORMATS_OPTION_HELP",
        "Output in xml, mof, txt, table"},

        {"xmlOutput", "false", false, Option::BOOLEAN, 0,0, "x",
        "Clients.cimcli.CIMCLIClient.XMLOUTPUT_OPTION_HELP",
        "Output objects in xml format"},

        {"version", "false", false, Option::BOOLEAN, 0, 0, "-version",
        "Clients.cimcli.CIMCLIClient.VERSION_OPTION_HELP",
        "Displays software Version"},

        {"verbose", "false", false, Option::BOOLEAN, 0, 0, "v",
        "Clients.cimcli.CIMCLIClient.VERBOSE_OPTION_HELP",
        "Verbose Display. Outputs etailed parameter input\n"
            "    display and other request processing information"},

        {"summary", "false", false, Option::BOOLEAN, 0, 0, "-sum",
        "Clients.cimcli.CIMCLIClient.SUMMARY_OPTION_HELP",
        "Displays only summary count for enumerations,\n"
            "    associators, etc."},

        {"help", "false", false, Option::BOOLEAN, 0, 0, "h",
        "Clients.cimcli.CIMCLIClient.HELP_OPTION_HELP",
        "Prints help usage message"},

        {"full help", "false", false, Option::BOOLEAN, 0, 0, "-help",
        "Clients.cimcli.CIMCLIClient.FULLHELP_OPTION_HELP",
        "Prints full help message with commands, options,\n"
            "    examples"},

        {"help options", "false", false, Option::BOOLEAN, 0, 0, "ho",
        "Clients.cimcli.CIMCLIClient.HELPOPTIONS_OPTION_HELP",
        "Prints list of options"},

        {"help commands", "false", false, Option::BOOLEAN, 0, 0, "hc",
        "Clients.cimcli.CIMCLIClient.HELPCOMMANDS_OPTION_HELP",
        "Prints CIM Operation command list"},

        {"connecttimeout", "0", false, Option::WHOLE_NUMBER, 0, 0, "-timeout",
        "Clients.cimcli.CIMCLIClient.CONNECTIONTIMEOUT_OPTION_HELP",
        "Set the connection timeout in seconds."},

        {"interactive", "false", false, Option::BOOLEAN, 0, 0, "i",
        "Clients.cimcli.CIMCLIClient.INTERACTIVE_OPTION_HELP",
        "Interactively ask user to select instances.\n"
            "    Used with associator and reference operations"},

        {"trace", "0", false, Option::WHOLE_NUMBER, 0, 0, "trace",
        "Clients.cimcli.CIMCLIClient.TRACE_OPTION_HELP",
        "Set Pegasus Common Components Trace. Sets the Trace level.\n"
            "    0 is off"},

        {"repeat", "0", false, Option::WHOLE_NUMBER, 0, 0, "-r",
        "Clients.cimcli.CIMCLIClient.REPEAT_OPTION_HELP",
        "Number of times to repeat the operation.\n"
            "    Zero means one time"},

        {"time", "false", false, Option::BOOLEAN, 0, 0, "-t",
        "Clients.cimcli.CIMCLIClient.TIME_OPTION_HELP",
        "Measure time for the operation and present results"}

    };
    const Uint32 NUM_OPTIONS = sizeof(optionsTable) / sizeof(optionsTable[0]);

    om.registerOptions(optionsTable, NUM_OPTIONS);

    String configFile = "cimcli.conf";

    if (FileSystem::exists(configFile))
    {
        om.mergeFile(configFile);
    }

    om.mergeCommandLine(argc, argv);

    om.checkRequiredOptions();

}

void lookupStringOption(Options& opts,
                  OptionManager& om,
                  const char* optionName,
                  String& optsTarget,
                  const String& defaultValue)
{
    optsTarget = defaultValue;
    if(om.lookupValue(optionName, optsTarget))
    {
        if (opts.verboseTest && opts.debug)
            cout << optionName << " = " << optsTarget << endl;
    }
}

// Set the correct empty string into the variable.
void lookupStringOptionEMPTY(Options& opts,
                  OptionManager& om,
                  const char* optionName,
                  String& optsTarget)
{
    String temp;
    if(om.lookupValue(optionName, temp))
    {
        optsTarget = (temp.size() == 0)? String::EMPTY : temp;

        if (opts.verboseTest && opts.debug)
            cout << optionName << " = " << optsTarget << endl;
    }
}

void lookupCIMNameOption(Options& opts,
                   OptionManager& om,
                   const char* optionName,
                   CIMName& optsTarget,
                   const CIMName& defaultValue)
{
    String temp;
    if(om.lookupValue(optionName, temp))
    {
       if (temp != "")
       {
           //Assigning to CIMName can cause exception.
           try
           {
               optsTarget = temp;
           }
           catch(Exception& e)
           {
               cerr << "Parse Error in " << optionName << " Class. Exception "
                    << e.getMessage()
                    << endl;
               exit(CIMCLI_INPUT_ERR);
           }
       }
       else
           optsTarget = defaultValue;

       if (opts.verboseTest && opts.debug && temp != "")
           cout << optionName << " = " << optsTarget.getString() << endl;
    }
}

// Lookup a single Uin32 option.  NOTE: The issue here is with detecting
// whether the option exists or we should use the internal default.
// Return from the option manager is the defined default which is itself
// an integer.
void lookupUint32Option(Options& opts,
                   OptionManager& om,
                   const char* optionName,
                   Uint32& optsTarget,
                   Uint32 defaultValue,
                   const char* units = "")
{
    optsTarget = 0;
    if (!om.lookupIntegerValue(optionName, optsTarget))
    {
        optsTarget = 0;
    }

    if (opts.verboseTest && opts.debug && optsTarget != 0)
    {
        cout << optionName << " = " << optsTarget << units
            << endl;
    }
}

void lookupBooleanOption(Options& opts,
                   OptionManager& om,
                   const char* optionName,
                   Boolean& optsTarget)
{
    optsTarget = om.isTrue(optionName);
    if (optsTarget  && opts.verboseTest && opts.debug)
        cout << optionName << " = " << _toString(optsTarget) << endl;
}

void lookupBooleanOptionNegate(Options& opts,
                   OptionManager& om,
                   const char* optionName,
                   Boolean& optsTarget)
{
    optsTarget = !om.isTrue(optionName);
    if (optsTarget  && opts.verboseTest && opts.debug)
        cout << optionName << " = " << _toString(optsTarget) << endl;
    cout << optionName << " temp = " << _toString(optsTarget) << endl;
}

int CheckCommonOptionValues(OptionManager& om, char** argv, Options& opts)
{
    Uint32 lineLength = 75;
    // Catch the verbose and debug options first so they can control other
    // processing
    Boolean verboseTest = (om.valueEquals("verbose", "true")) ? true :false;
    Boolean debug = (om.valueEquals("debug", "true")) ? true :false;

    if (verboseTest)
        opts.verboseTest = verboseTest;

    if (debug)
    {
        opts.debug= debug;
    }

    if (om.isTrue("full help"))
    {
        showFullHelpMsg(argv[0], om, lineLength);
        exit(0);
    }

    // show usage for a single operation and exit
    if (om.isTrue("help"))
    {
        if (!showOperationUsage(argv[1], om, lineLength))
            exit(1);

        showUsage();
        exit(CIMCLI_RTN_CODE_OK);
    }

    // show version number
    if (om.isTrue("version"))
    {
        showVersion(argv[0], om);
        exit(CIMCLI_RTN_CODE_OK);
    }

    // show all help options
    if (om.isTrue("help options"))
    {
        showOptions(argv[0], om);
        exit(CIMCLI_RTN_CODE_OK);
    }

    // show help Operation list
    if (om.isTrue("help commands"))
    {
        showOperations(argv[0], lineLength);
        exit(CIMCLI_RTN_CODE_OK);
    }

    lookupStringOption(opts, om, "namespace", opts.nameSpace, "root/cimv2");

    lookupStringOptionEMPTY(opts, om, "role", opts.role);

    lookupStringOptionEMPTY(opts, om, "resultRole", opts.resultRole);

    lookupStringOption(opts, om, "location", opts.location, String::EMPTY);

#ifdef PEGASUS_HAS_SSL
    // Determine whether to connect over HTTPS
    opts.ssl = om.isTrue("ssl");

    // Get value for client certificate
    om.lookupValue("clientCert", opts.clientCert);

    // Get value for client key
    om.lookupValue("clientKey", opts.clientKey);

    if (verboseTest && debug && opts.ssl)
    {
        cout << "ssl = true" << endl;
        if (opts.clientCert != "" && opts.clientKey != "")
        {
            cout << "clientCert = " << opts.clientCert << endl;
            cout << "clientKey = " << opts.clientKey << endl;
        }
    }
#endif

    // Assign the result class
    lookupCIMNameOption(opts, om, "resultClass", opts.resultClass, CIMName());

    lookupCIMNameOption(opts, om, "assocClass", opts.assocClass, CIMName());

    // Evaluate connectiontimeout option.
    lookupUint32Option(opts, om, "connecttimeout", opts.connectionTimeout, 0,
        "seconds");

    lookupUint32Option(opts, om, "delay", opts.delay, 0, "seconds");


    // Set the interactive request flag based on input

    lookupBooleanOption(opts, om,"interactive", opts.interactive);

    // set the deepInheritance flag based on input
    lookupBooleanOption(opts, om,"deepInheritance", opts.deepInheritance);

    // process localOnly and notlocalOnly parameters
    opts.localOnly = om.isTrue("localOnly");
    if (om.isTrue("notLocalOnly"))
    {
        opts.localOnly = false;
    }

    // Used the not version because the DMTF and pegasus default is true
    if (verboseTest && debug && om.isTrue("notLocalOnly"))
    {
        cout << "localOnly= " << _toString(opts.localOnly) << endl;;
    }

    // Process notincludeQualifiers

    lookupBooleanOption(opts, om,"notIncludeQualifiers",
                        opts.includeQualifiers );

    opts.includeQualifiers = om.isTrue("includeQualifiers");

    if (om.isTrue("notIncludeQualifiers"))
    {
        opts.includeQualifiers = false;
    }

    if (verboseTest && debug && om.isTrue("notIncludeQualifiers"))
    {
        cout << "includeQualifiers = " << _toString(opts.includeQualifiers)
            << endl;
    }


    lookupBooleanOption(opts, om,"includeClassOrigin",
                        opts.includeClassOrigin );

    lookupBooleanOption(opts, om,"time", opts.time);

    if(!om.lookupIntegerValue("trace", opts.trace))
    {
            opts.trace = 0;
    }
    else
    {
        Uint32 traceLevel = 0;
        switch (opts.trace)
        {
            case 0:             // This covers the default.
                break;
            case 1 :
                traceLevel = Tracer::LEVEL1;
                break;
            case 2 :
                traceLevel = Tracer::LEVEL2;
                break;
            case 3 :
                traceLevel = Tracer::LEVEL3;
                break;
            case 4 :
                traceLevel = Tracer::LEVEL4;
                break;
            default:
                cout << "Illegal value for Trace. Max = 4" << endl;
        }
        opts.trace = traceLevel;
    }
    if (verboseTest && debug && opts.trace != 0)
    {
        cout << "Pegasus Trace set to  Level  " << opts.trace << endl;
    }

    lookupBooleanOption(opts, om,"summary", opts.summary);

    // get User name and password if set.
    lookupStringOptionEMPTY(opts, om, "User", opts.user);

    lookupStringOptionEMPTY(opts, om, "Password", opts.password);

    // Create a variable with the format output and a correponding type.
    // Suggest we might change this whole thing to the option type that
    // mike used in the example of colors so that  you could do -red -blue
    // or in our case -mof -xml, etc.

     opts.isXmlOutput = om.isTrue("xmlOutput");
     if (opts.isXmlOutput  && debug && verboseTest)
         cout << "xmlOutput set" << endl;

    if(om.lookupValue("outputformats", opts.outputFormat))
     {
        if (debug && verboseTest)
            cout << "Output Format = " << opts.outputFormat << endl;
     }

    // Get the output format parameter and save it
    Uint32 cnt = 0;
    opts.outputFormat.toLower();

    for( ; cnt < NUM_OUTPUTS; cnt++ )
    {
        if (opts.outputFormat == OutputTable[cnt].OutputName)
                break;
    }
    // Note that this makes no notice if a not found
    if (cnt != NUM_OUTPUTS)
    {
        opts.outputFormatType = cnt;
        opts.outputType = OutputTable[cnt].OutputType;
    }


    lookupUint32Option(opts, om, "repeat", opts.repeat, 0, "times");

    lookupUint32Option(opts, om, "count", opts.count, 0, "Comparison Count");

    if (opts.count != 29436)
    {
        opts.executeCountTest = true;
    }

    /*  Property List parameter.
        Separate an input stream into an array of Strings
        Two special situations, empty list and NULL list
        Use NULL when there is no list. This means return all
        Use empty if if you want no properties in the response
        NOTE: We use the ###!### to represent no input of parameter
    */
    {
        String properties;
        if(om.lookupValue("propertyList", properties))
        {
            // om default.  No property list input
            if (properties == "###!###")
            {
                opts.propertyList.clear();
            }
            // propertylist input empty.
            // Account for inputter error where they try to input string
            // representing two quotes
            else if(properties.size() == 0 || properties == "\"\"")
            {
                Array<CIMName> pList;
                opts.propertyList = pList;
            }
            else
            {
                Array<CIMName> pList;
                // tokenize everything separated by commas
                Array<String> pListString =  _tokenize(properties, ',', true);

                for (Uint32 i = 0 ; i < pListString.size(); i++)
                {
                    pList.append(CIMName(pListString[i]));
                }
                opts.propertyList.set(pList);
            }
            if (debug && verboseTest && properties != "###!###")
            {
                cout << "PropertyList= "
                    << _toString(opts.propertyList)
                    << endl;
            }
        }
    }

    return 0;
}

PEGASUS_NAMESPACE_END
// END_OF_FILE