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
// Author: Nag Boranna (nagaraja_boranna@hp.com)
//
// Modified By: Yi Zhou (yi_zhou@hp.com)
//              Sushma Fernandes (sushma_fernandes@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// 
// This file has implementation for the trace property owner class.
//
///////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/Destroyer.h>
#include "ConfigManager.h"
#include "TracePropertyOwner.h"


PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
//  TracePropertyOwner
//
//  When a new trace property is added, make sure to add the property name
//  and the default attributes of that property in the table below.
///////////////////////////////////////////////////////////////////////////////

static struct ConfigPropertyRow properties[] =
{
    {"traceLevel", "1", 1, 0, 0},
    {"traceFilePath", "cimserver.trc", 1, 0, 0},
    {"traceComponents", "", 1, 0, 0},
};

const Uint32 NUM_PROPERTIES = sizeof(properties) / sizeof(properties[0]);


// 
// Checks if the trace level is valid
//
Boolean isLevelValid(const String& traceLevel)
{
    //
    // Check if the level is valid
    //
    if ( traceLevel == "1" || traceLevel == "2" || 
	 traceLevel == "3" || traceLevel == "4")
    {
        return 1;
    }
    else
    {
        return 0; 
    }
}

//
// Get the appropriate trace level
//
Uint32 getTraceLevel(const String& traceLevel)
{
    if ( traceLevel == "1" )
    {
        return (Tracer::LEVEL1);
    }
    else if ( traceLevel == "2" )
    {
        return (Tracer::LEVEL2);
    }
    else if ( traceLevel == "3" )
    {
        return (Tracer::LEVEL3);
    }
    else 
    {
        return (Tracer::LEVEL4);
    }
}

/** Constructors  */
TracePropertyOwner::TracePropertyOwner()
{
    _traceLevel = new ConfigProperty;
    _traceFilePath = new ConfigProperty;
    _traceComponents = new ConfigProperty;
}

/** Destructor  */
TracePropertyOwner::~TracePropertyOwner()
{
    delete _traceLevel;
    delete _traceFilePath;
    delete _traceComponents;
}

/**
Initialize the config properties.
*/
void TracePropertyOwner::initialize()
{
    for (Uint32 i = 0; i < NUM_PROPERTIES; i++)
    {
        //
        // Initialize the properties with default values
        //
        if (String::equalNoCase(properties[i].propertyName, "traceComponents"))
        {
            _traceComponents->propertyName = properties[i].propertyName;
            _traceComponents->defaultValue = properties[i].defaultValue;
            _traceComponents->currentValue = properties[i].defaultValue;
            _traceComponents->plannedValue = properties[i].defaultValue;
            _traceComponents->dynamic = properties[i].dynamic;
            _traceComponents->domain = properties[i].domain;
            _traceComponents->domainSize = properties[i].domainSize;
        }
        else if (String::equalNoCase(properties[i].propertyName, "traceLevel"))
        {
            _traceLevel->propertyName = properties[i].propertyName;
            _traceLevel->defaultValue = properties[i].defaultValue;
            _traceLevel->currentValue = properties[i].defaultValue;
            _traceLevel->plannedValue = properties[i].defaultValue;
            _traceLevel->dynamic = properties[i].dynamic;
            _traceLevel->domain = properties[i].domain;
            _traceLevel->domainSize = properties[i].domainSize;
        }
        else if (String::equalNoCase(properties[i].propertyName, "traceFilePath"))
        {
            _traceFilePath->propertyName = properties[i].propertyName;
            _traceFilePath->defaultValue = properties[i].defaultValue;
            _traceFilePath->currentValue = properties[i].defaultValue;
            _traceFilePath->plannedValue = properties[i].defaultValue;
            _traceFilePath->dynamic = properties[i].dynamic;
            _traceFilePath->domain = properties[i].domain;
            _traceFilePath->domainSize = properties[i].domainSize;
        }
    }
    if (_traceFilePath->defaultValue != String::EMPTY)
    {
        String pegasusHome = ConfigManager::getPegasusHome();

	// Set the file path to  $PEGASUS_HOME directory 
        _traceFilePath->defaultValue = pegasusHome + String("/") + 
                                          _traceFilePath->defaultValue;
        _traceFilePath->currentValue = _traceFilePath->defaultValue;
        _traceFilePath->plannedValue = _traceFilePath->defaultValue;
    }

    if (_traceLevel->defaultValue != String::EMPTY)
    {
        if (_traceLevel->defaultValue == "1")
        {
            Tracer::setTraceLevel(Tracer::LEVEL1);
        }
        else if (_traceLevel->defaultValue == "2")
        {
            Tracer::setTraceLevel(Tracer::LEVEL2);
        }
        else if (_traceLevel->defaultValue == "3")
        {
            Tracer::setTraceLevel(Tracer::LEVEL3);
        }
        else if (_traceLevel->defaultValue == "4")
        {
            Tracer::setTraceLevel(Tracer::LEVEL4);
        }
    }
}

struct ConfigProperty* TracePropertyOwner::_lookupConfigProperty(
    const String& name)
{
    if (String::equalNoCase(_traceComponents->propertyName, name))
    {
        return _traceComponents;
    }
    else if (String::equalNoCase(_traceLevel->propertyName, name))
    {
        return _traceLevel;
    }
    else if (String::equalNoCase(_traceFilePath->propertyName, name))
    {
        return _traceFilePath;
    }
    else
    {
        throw UnrecognizedConfigProperty(name);
    }
}

/** 
Get information about the specified property.
*/
void TracePropertyOwner::getPropertyInfo(
    const String& name, 
    Array<String>& propertyInfo)
{
    propertyInfo.clear();

    struct ConfigProperty* configProperty = _lookupConfigProperty(name);

    propertyInfo.append(configProperty->propertyName);
    propertyInfo.append(configProperty->defaultValue);
    propertyInfo.append(configProperty->currentValue);
    propertyInfo.append(configProperty->plannedValue);
    if (configProperty->dynamic)
    {
        propertyInfo.append(STRING_TRUE);
    }
    else
    {
        propertyInfo.append(STRING_FALSE);
    }
}

/** 
Get default value of the specified property.
*/
const String TracePropertyOwner::getDefaultValue(const String& name)
{
    struct ConfigProperty* configProperty = _lookupConfigProperty(name);
    return configProperty->defaultValue;
}

/** 
Get current value of the specified property.
*/
const String TracePropertyOwner::getCurrentValue(const String& name)
{
    struct ConfigProperty* configProperty = _lookupConfigProperty(name);
    return configProperty->currentValue;
}

/** 
Get planned value of the specified property.
*/
const String TracePropertyOwner::getPlannedValue(const String& name)
{
    struct ConfigProperty* configProperty = _lookupConfigProperty(name);
    return configProperty->plannedValue;
}

/** 
Init current value of the specified property to the specified value.
*/
void TracePropertyOwner::initCurrentValue(
    const String& name, 
    const String& value)
{
    if (String::equalNoCase(_traceComponents->propertyName, name))
    {
        _traceComponents->currentValue = value;
	Tracer::setTraceComponents(_traceComponents->currentValue);
        if (_traceFilePath->currentValue != String::EMPTY && _traceComponents->currentValue != String::EMPTY)
        {
            CString fileName = _traceFilePath->currentValue.getCString();
	    if (Tracer::isValidFileName(fileName))
	    { 
                Uint32 retCode = Tracer::setTraceFile(fileName);
	        // Check whether the filepath was set
	        if ( retCode == 1 )
	        {
	            Logger::put(Logger::DEBUG_LOG,System::CIMSERVER,
	                Logger::WARNING,
	                "Unable to write to trace file $0",
	                (const char*)fileName);

	            _traceFilePath->currentValue = "";
                }
            }
        }
    }
    else if (String::equalNoCase(_traceLevel->propertyName, name))
    {
        _traceLevel->currentValue = value;
	Uint32 traceLevel = getTraceLevel( _traceLevel->currentValue );
        Tracer::setTraceLevel(traceLevel);
    }
    else if (String::equalNoCase(_traceFilePath->propertyName, name))
    {
        _traceFilePath->currentValue = value;
        if (_traceFilePath->currentValue != String::EMPTY && 
            _traceComponents->currentValue != String::EMPTY)
        {
            CString fileName = _traceFilePath->currentValue.getCString();
	    if (Tracer::isValidFileName(fileName))
	    { 
                Uint32 retCode = Tracer::setTraceFile(fileName);

	        // Check whether the filepath was set
	        if ( retCode == 1 )
	        {
	            Logger::put(Logger::DEBUG_LOG,System::CIMSERVER,
	             Logger::WARNING,
	             "Unable to write to trace file $0",
	             (const char*)fileName);
	            _traceFilePath->currentValue = "";
                }
            }
        }
    }
    else
    {
        throw UnrecognizedConfigProperty(name);
    }
}


/** 
Init planned value of the specified property to the specified value.
*/
void TracePropertyOwner::initPlannedValue(
    const String& name, 
    const String& value)
{
    struct ConfigProperty* configProperty = _lookupConfigProperty(name);
    configProperty->plannedValue = value;
}

/** 
Update current value of the specified property to the specified value.
*/
void TracePropertyOwner::updateCurrentValue(
    const String& name, 
    const String& value) 
{
    //
    // make sure the property is dynamic before updating the value.
    //
    if (!isDynamic(name))
    {
        throw NonDynamicConfigProperty(name); 
    }

    //
    // Update does the same thing as initialization
    //
    initCurrentValue(name, value);
}


/** 
Update planned value of the specified property to the specified value.
*/
void TracePropertyOwner::updatePlannedValue(
    const String& name, 
    const String& value)
{
    struct ConfigProperty* configProperty = _lookupConfigProperty(name);
    configProperty->plannedValue = value;
}

/** 
Checks to see if the given value is valid or not.
*/
Boolean TracePropertyOwner::isValid(const String& name, const String& value)
{
    if (String::equalNoCase(_traceComponents->propertyName, name))
    {
	String newValue          = value;
	String invalidComponents = String::EMPTY;

	//
	// Check if the trace components are valid
	//
	if (!Tracer::isValidComponents(newValue,invalidComponents))
	{
	    throw InvalidPropertyValue(name, invalidComponents);
        }

        return true;
    }
    else if (String::equalNoCase(_traceLevel->propertyName, name))
    {
	//
        // Check if the level is valid
	//
        if ( isLevelValid( value ) )
        {
            return true;
        }
	else
	{
	    throw InvalidPropertyValue(name, value);
        }
    }
    else if (String::equalNoCase(_traceFilePath->propertyName, name))
    {
	//
        // Check if the file path is valid
	//
        if ( value == String::EMPTY ) 
        {
            throw InvalidPropertyValue(name, value);
        }
	if (!Tracer::isValidFileName(value.getCString()))
	{
	    throw InvalidPropertyValue(name, value);
	}
        return true;
    }
    else
    {
        throw UnrecognizedConfigProperty(name);
    }
}

/** 
Checks to see if the specified property is dynamic or not.
*/
Boolean TracePropertyOwner::isDynamic(const String& name)
{
    struct ConfigProperty* configProperty = _lookupConfigProperty(name);
    return configProperty->dynamic;
}


PEGASUS_NAMESPACE_END
