//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:	 Karl Schopmeyer(k.schopmeyer@opengroup.org)
//               Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Flavor_h
#define Pegasus_Flavor_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

/**
    The CIMFlavor class implements the concept of the CIM qualifier flavor.
    The qualifier flavor concept encapsulates the propagation and override 
    rules for the qualifier.  The propagation rules define whether a qualifier
    may be propagated from classes to derived classes, or from classes to 
    instances.  The override rules define whether or not derived classes may
    override a qualifier value, or whether it must be fixed for an entire 
    class hierarchy.

    The possible values are: OVERRIDABLE, TOSUBCLASS, TOINSTANCE, 
    TRANSLATABLE, DISABLEOVERRIDE, RESTRICTED.
    The flavor is a set of zero or more of these possible values.
 */
class PEGASUS_COMMON_LINKAGE CIMFlavor
{
public:

    /**
        Constructs a CIMFlavor object with no flavor values set (default 
        constructor).
     */
    CIMFlavor ();

    /**
        Constructs a CIMFlavor object from an existing CIMFlavor object (copy 
        constructor).

        @param   flavor                another CIMFlavor object
     */
    CIMFlavor (const CIMFlavor & flavor);

    /**
        Constructs a CIMFlavor object with the specified set of values.

        @param   flavor                a Uint32 representing the set of flavor 
                                       values

        @exception InvalidFlavor  if the set of flavor values is invalid
     */
    CIMFlavor (const Uint32 flavor);

    /**
        Assigns the value of one CIMFlavor object to another (assignment 
        operator).

        @param   flavor                a CIMFlavor object

        @return  the CIMFlavor object
     */
    CIMFlavor & operator= (const CIMFlavor & flavor);

    /**
        Adds the specified set of flavor values to the CIMFlavor object.

        @param   flavor                a Uint32 representing the set of flavor 
                                       values

        @exception InvalidFlavor  if the set of flavor values is invalid
     */
    void addFlavor (const Uint32 flavor);

    /**
        Adds the set of flavor values of the specified CIMFlavor object to this
        CIMFlavor object.

        @param   flavor                a CIMFlavor object
     */
    void addFlavor (const CIMFlavor & flavor);

    /**
        Removes the specified set of flavor values from the CIMFlavor object.

        @param   flavor                a Uint32 representing the set of flavor 
                                       values

        @exception InvalidFlavor  if the set of flavor values is invalid
     */
    void removeFlavor (const Uint32 flavor);

    /**
        Determines if every value in the specified set of flavor values is 
        included in the CIMFlavor object.

        @param   flavor                a Uint32 representing a set of flavor 
                                       values

        @return  True if every value in the set of flavor values is included in
                   the CIMFlavor object,
                 False otherwise
     */
    Boolean hasFlavor (const Uint32 flavor) const;

    /**
        Determines if every value in the specified CIMFlavor object is included 
        in this CIMFlavor object.

        @param   flavor                a CIMFlavor object

        @return  True if every value in the specified CIMFlavor object is 
                   included in the CIMFlavor object,
                 False otherwise
     */
    Boolean hasFlavor (const CIMFlavor & flavor) const;

    /** 
        Compares two CIMFlavor objects.

        @param   flavor                a CIMFlavor object

        @return True if the two CIMFlavor objects are equal, 
                False otherwise
     */
    Boolean equal (const CIMFlavor & flavor) const;

    /** 
        Returns a String representation of the CIMFlavor object.
        This method is for diagnostic purposes.  The format of the output
        is subject to change.
     */
    String toString () const;

    /** 
        Indicates that the qualifier has no flavors
     */
    static const Uint32 NONE;

    /** 
        Indicates that the qualifier may be overridden
     */
    static const Uint32 OVERRIDABLE;
    static const Uint32 ENABLEOVERRIDE;

    /** 
        Indicates that the qualifier may not be overridden
     */
    static const Uint32 DISABLEOVERRIDE;

    /** 
        Indicates that the qualifier is propagated to the qualifier in the
	subclass with the same name
     */
    static const Uint32 TOSUBCLASS;

    /** 
        Indicates that the qualifier is not propagated to the qualifier in the
	subclass with the same name; it applies only to the class in which it 
        is declared
     */
    static const Uint32 RESTRICTED;

    /** 
        Indicates that the qualifier is propagated to the qualifier in the
	instance with the same name
     */
    static const Uint32 TOINSTANCE;

    /** 
        Indicates that the qualifier is translatable (for internationalization)
     */
    static const Uint32 TRANSLATABLE;

    /** 
        Indicates default flavor setting (OVERRIDABLE | TOSUBCLASS)
     */
    static const Uint32 DEFAULTS;

    /** 
        Indicates both toSubclass and toInstance (TOSUBCLASS | TOINSTANCE)
     */
    static const Uint32 TOSUBELEMENTS;

    /** 
        Indicates the set of all possible flavor values: OVERRIDABLE, 
        TOSUBCLASS, TOINSTANCE, TRANSLATABLE, DISABLEOVERRIDE, RESTRICTED
        Note that some of these possible values are mutually exclusive, and 
        thus CIMFlavor:ALL does not represent a valid qualifier flavor object
        It may be used, for example, to clear the flavor settings
     */
    static const Uint32 ALL;

private:

    void _checkFlavor (Uint32 flavor);

    Uint32 cimFlavor;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Flavor_h */
