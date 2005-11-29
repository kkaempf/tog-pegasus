//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Author: Yi Zhou, Hewlett-Packard Company (yi.zhou@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <iostream>

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include "snmpDeliverTrap.h"

PEGASUS_NAMESPACE_BEGIN

static const char _MSG_SESSION_OPEN_FAILED [] = 
    "Snmp Indication Handler failed to open the SNMP session: "; 
static const char _MSG_SESSION_OPEN_FAILED_KEY [] = 
    "Handler.snmpIndicationHandler.snmpDeliverTrap_netsnmp._MSG_SESSION_OPEN_FAILED";

static const char _MSG_GET_SESSION_POINT_FAILED [] = 
    "Snmp Indication Handler failed to get the SNMP session pointer: "; 
static const char _MSG_GET_SESSION_POINTER_FAILED_KEY [] = 
    "Handler.snmpIndicationHandler.snmpDeliverTrap_netsnmp._MSG_GET_SESSION_POINT_FAILED";

static const char _MSG_PDU_CREATE_FAILED [] = 
    "Snmp Indication Handler failed to create the SNMP PDU."; 
static const char _MSG_PDU_CREATE_FAILED_KEY [] = 
    "Handler.snmpIndicationHandler.snmpDeliverTrap_netsnmp._MSG_PDU_CREATE_FAILED";

static const char _MSG_VERSION_NOT_SUPPORTED [] = 
    "SNMPv1 Trap and SNMPv2C Trap are the only supported SNMPVersion values.";
static const char _MSG_VERSION_NOT_SUPPORTED_KEY [] = 
    "Handler.snmpIndicationHandler.snmpDeliverTrap_netsnmp._MSG_VERSION_NOT_SUPPORTED";

static const char _MSG_SESSION_SEND_FAILED [] = 
    "Snmp Indication Handler failed to send the trap: "; 
static const char _MSG_SESSION_SEND_FAILED_KEY [] = 
    "Handler.snmpIndicationHandler.snmpDeliverTrap_netsnmp._MSG_SESSION_SEND_FAILED";

static const char _MSG_PACK_TRAP_INFO_INTO_PDU_FAILED [] = 
    "Snmp Indication Handler failed to pack trap information into the SNMP PDU: \"$0\".";
static const char _MSG_PACK_TRAP_INFO_INTO_PDU_FAILED_KEY [] = 
    "Handler.snmpIndicationHandler.snmpDeliverTrap_netsnmp._MSG_PACK_TRAP_INFO_INTO_PDU_FAILED";

static const char _MSG_ADD_SYSUPTIME_TO_PDU_FAILED [] = 
    "Snmp Indication Handler failed to add sysUpTime to the SNMP PDU: \"$0\".";
static const char _MSG_ADD_SYSUPTIME_TO_PDU_FAILED_KEY [] = 
    "Handler.snmpIndicationHandler.snmpDeliverTrap_netsnmp._MSG_ADD_SYSUPTIME_TO_PDU_FAILED";

static const char _MSG_ADD_SNMP_TRAP_TO_PDU_FAILED [] = 
    "Snmp Indication Handler failed to add SNMP Trap to the SNMP PDU: \"$0\".";
static const char _MSG_ADD_SNMP_TRAP_TO_PDU_FAILED_KEY [] = 
    "Handler.snmpIndicationHandler.snmpDeliverTrap_netsnmp._MSG_ADD_SNMP_TRAP_TO_PDU_FAILED";

static const char _MSG_PACK_CIM_PROPERTY_TO_PDU_FAILED [] = 
    "Snmp Indication Handler failed to pack a CIM Property into the SNMP PDU: \"$0\".";
static const char _MSG_PACK_CIM_PROPERTY_TO_PDU_FAILED_KEY [] = 
    "Handler.snmpIndicationHandler.snmpDeliverTrap_netsnmp._MSG_PACK_CIM_PROPERTY_TO_PDU_FAILED";

static const char _MSG_READ_OBJID_FAILED [] =
    "Snmp Indication Handler failed to convert trapOid \"$0\" from a "
    "numeric form to a list of subidentifiers.";
static const char _MSG_READ_OBJID_FAILED_KEY [] =
    "Handler.snmpIndicationHandler.snmpDeliverTrap_netsnmp._MSG_READ_OBJID_FAILED";

static const char _MSG_PARSE_CIM_PROPERTY_OID_FAILED [] =
    "Snmp Indication Handler failed to convert a CIM property OID \"$0\" from "
    "a numeric form to a list of subidentifiers.";
static const char _MSG_PARSE_CIM_PROPERTY_OID_FAILED_KEY [] =
    "Handler.snmpIndicationHandler.snmpDeliverTrap_netsnmp.__MSG_PARSE_CIM_PROPERTY_OID_FAILED";

static const char _MSG_READ_ENTOID_FAILED [] =
    "Snmp Indication Handler failed to convert SNMPV1 enterprise OID \"$0\" "
    "from a numeric form to a list of subidentifiers.";
static const char _MSG_READ_ENTOID_FAILED_KEY [] =
    "Handler.snmpIndicationHandler.snmpDeliverTrap_netsnmp._MSG_READ_ENTOID_FAILED";

static const char _MSG_UNSUPPORTED_SNMP_DATA_TYPE [] =
    "Type \"$0\" is an unsupported SNMP Data Type for the CIM property.";
static const char _MSG_UNSUPPORTED_SNMP_DATA_TYPE_KEY [] =
    "Handler.snmpIndicationHandler.snmpDeliverTrap_netsnmp._MSG_UNSUPPORTED_SNMP_DATA_TYPE";

static const char _MSG_ADD_VAR_TO_PDU_FAILED [] =
    "Snmp Indication Handler failed to add a CIM property \"$0\" to the SNMP PDU: \"$1\".";
static const char _MSG_ADD_VAR_TO_PDU_FAILED_KEY [] =
    "Handler.snmpIndicationHandler.snmpDeliverTrap_netsnmp._MSG_ADD_VAR_TO_PDU_FAILED";

class snmpDeliverTrap_netsnmp : public snmpDeliverTrap
{
public:

    void deliverTrap(
        const String& trapOid,
        const String& securityName, 
        const String& targetHost, 
        const Uint16& targetHostFormat, 
        const String& otherTargetHostFormat, 
        const Uint32& portNumber,
        const Uint16& snmpVersion, 
        const String& engineID,
        const Array<String>& vbOids,
        const Array<String>& vbTypes,
        const Array<String>& vbValues);

private:

    /**
        Creates a SNMP session.
      
        @param targetHost     the target system of a hostname or an IPv4 
                              address
                              to receive a trap
        @param portNumber     the port number to receive a trap
        @param securityName   the human readable community name
        @param snmpSession    the SNMP session
        @param sessionHandle  an opaque pointer of the SNMP session
        @param sessionPtr     the SNMP session pointer to its associated 
                              struct snmp_session
        
    */

    void _createSession(const String & targetHost,
                        Uint32 portNumber,
                        const String & securityName,
                        struct snmp_session & snmpSession,
                        void *&sessionHandle,
                        struct snmp_session *&sessionPtr);

    /**
        Creates a SNMP session.
      
        @param sessionHandle  an opaque pointer of the SNMP session
    */
    void _destroySession(void *sessionHandle);

    /**
        Creates a SNMP PDU.

        @param snmpVersion  the SNMP version
        @param trapOid      the trap OID
        @param sessionPtr   the SNMP session pointer
        @param snmpPdu      the SNMP PDU

    */

    void _createPdu(Uint16 snmpVersion,
                   const String& trapOid,
                   struct snmp_session *&sessionPtr,
                   struct snmp_pdu *& snmpPdu);

    /**
        Pack the trap information into the PDU.

        @param trapOid  the trap OID
        @param snmpPdu  the SNMP PDU
    */
    void _packTrapInfoIntoPdu(const String & trapOid,
                              snmp_pdu * snmpPdu);

    /**
        Pack CIM properties into PDU.

        @param vbOids    the array of CIM property OIDs
        @param vbTypes   the array of CIM property data types
        @param vbValues  the array of CIM property values
        @param snmpPdu   the SNMP PDU
    */
    void _packOidsIntoPdu(const Array<String>& vbOids,
                          const Array<String>& vbTypes,
                          const Array<String>& vbValues,
                          snmp_pdu * snmpPdu);

    enum SNMPVersion {_SNMPv1_TRAP = 2, _SNMPv2C_TRAP = 3,
        _SNMPv2C_INFORM = 4, _SNMPv3_TRAP = 5, _SNMPv3_INFORM = 6};

};

PEGASUS_NAMESPACE_END