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
//%////////////////////////////////////////////////////////////////////////////

#include "CertificateProvider.h"

#define OPENSSL_NO_KRB5 1
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <openssl/rand.h>

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/PegasusVersion.h>

#include <cctype>
#include <iostream>

#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/AutoPtr.h>
#include <Pegasus/Common/OperationContext.h>
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/XmlReader.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/XmlParser.h>

#ifdef PEGASUS_OS_OS400
#include <qycmutilu2.H>
#include "OS400ConvertChar.h"
#endif

#include <stdlib.h>

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

//PG_SSLCertificate property names
static const CIMName ISSUER_NAME_PROPERTY           = "IssuerName";
static const CIMName SERIAL_NUMBER_PROPERTY         = "SerialNumber";
static const CIMName SUBJECT_NAME_PROPERTY          = "SubjectName";
static const CIMName USER_NAME_PROPERTY             = "RegisteredUserName";
static const CIMName TRUSTSTORE_TYPE_PROPERTY       = "TruststoreType";
static const CIMName FILE_NAME_PROPERTY             = "TruststorePath";
static const CIMName NOT_BEFORE_PROPERTY            = "NotBefore";
static const CIMName NOT_AFTER_PROPERTY             = "NotAfter";
static const CIMName CERTIFICATE_TYPE_PROPERTY      = "CertificateType";

//PG_SSLCertificateRevocationList property names
//also has IssuerName
static const CIMName LAST_UPDATE_PROPERTY           = "LastUpdate";
static const CIMName NEXT_UPDATE_PROPERTY           = "NextUpdate";
static const CIMName REVOKED_SERIAL_NUMBERS_PROPERTY  = "RevokedSerialNumbers";
static const CIMName REVOCATION_DATES_PROPERTY      = "RevocationDates";

//method names for PG_SSLCertificate
static const CIMName METHOD_ADD_CERTIFICATE         = "addCertificate";
static const CIMName PARAMETER_CERT_CONTENTS        = "certificateContents";
static const CIMName PARAMETER_USERNAME             = "userName";
static const CIMName PARAMETER_TYPE                 = "certificateType";

static const String TYPE_AUTHORITY                  = "a";
static const String TYPE_AUTHORITY_END_ENTITY       = "e";
static const String TYPE_SELF_SIGNED_IDENTITY       = "s";

static const Uint16 CERT_TYPE_UNKNOWN                    = 0;

//method names for PG_SSLCertificateRevocationList
static const CIMName METHOD_ADD_CRL         = "addCertificateRevocationList";
static const CIMName PARAMETER_CRL_CONTENTS         = "CRLContents";

//truststore and crlstore directory mutexes
static Mutex _trustStoreMutex;
static Mutex _crlStoreMutex;

typedef struct Timestamp 
{
    char year[4];
    char month[2];
    char day[2];
    char hour[2];
    char minutes[2];
    char seconds[2];
    char dot;
    char microSeconds[6];
    char plusOrMinus;
    char utcOffset[3];
    char padding[3];
} Timestamp_t;

/** Convert ASN1_UTCTIME to CIMDateTime
 */
inline CIMDateTime getDateTime(const ASN1_UTCTIME *utcTime)
{
    struct tm time;
    int offset;
    Timestamp_t timeStamp;
    char tempString[80];
    char plusOrMinus = '+';
    unsigned char* utcTimeData = utcTime->data;

    memset(&time, '\0', sizeof(time));

#define g2(p) ( ( (p)[0] - '0' ) * 10 + (p)[1] - '0' )

    if (utcTime->type == V_ASN1_GENERALIZEDTIME)
    {
        time.tm_year = g2(utcTimeData) * 100;
        utcTimeData += 2;  // Remaining data is equivalent to ASN1_UTCTIME type
        time.tm_year += g2(utcTimeData);
    }
    else
    {
        time.tm_year = g2(utcTimeData);
        if (time.tm_year < 50)
        {
            time.tm_year += 2000;
        }
        else
        {
            time.tm_year += 1900;
        }
    }

    time.tm_mon = g2(utcTimeData + 2) - 1;
    time.tm_mday = g2(utcTimeData + 4);
    time.tm_hour = g2(utcTimeData + 6);
    time.tm_min = g2(utcTimeData + 8);
    time.tm_sec = g2(utcTimeData + 10);

    if (utcTimeData[12] == 'Z')
    {
        offset = 0;
    }
    else
    {
        offset = g2(utcTimeData + 13) * 60 + g2(utcTimeData + 15);
        if (utcTimeData[12] == '-')
        {
            plusOrMinus = '-';
        }
    }
#undef g2

    memset((void *)&timeStamp, 0, sizeof(Timestamp_t));

    // Format the date.
    sprintf((char *) &timeStamp,"%04d%02d%02d%02d%02d%02d.%06d%04d",
            time.tm_year,
            time.tm_mon + 1,  
            time.tm_mday,
            time.tm_hour,
            time.tm_min,
            time.tm_sec,
            0,
            offset);

    timeStamp.plusOrMinus = plusOrMinus;

    CIMDateTime dateTime;

    dateTime.clear();
    strcpy(tempString, (char *)&timeStamp);
    dateTime.set(tempString);

    return (dateTime);
}

/**
 * The issuer name should be in the format /type0=value0/type1=value1/type2=...
 * where characters may be escaped by \
 */
inline X509_NAME *getIssuerName(char *issuer, long chtype)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER, "CertificateProvider::getIssuerName");

    //allocate buffers for type-value pairs
    size_t buflen = strlen(issuer)+1; 
    size_t maxPairs = buflen / 2 + 1; 
    AutoArrayPtr<char> buf;
    AutoArrayPtr<char*> types;
    AutoArrayPtr<char*> values;

    try
    {
        buf.reset(new char[buflen]);
        types.reset(new char*[maxPairs]);
        values.reset(new char*[maxPairs]);
    }
    catch (...)
    {
        return NULL;
    }

    char *sp = issuer, *bp = buf.get();
    int count = 0;

    while (*sp)
    {
        PEG_TRACE_CSTRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
            "CertificateProvider::getIssuerName WHILE");

        if (*sp != '/') 
        {
            break;
        }
        sp++;

        types.get()[count] = bp;
        while (*sp)
        {
            if (*sp == '\\')
            {
                if (*++sp)
                {
                    *bp++ = *sp++;
                }
            }
            else if (*sp == '=')
            {
                sp++;
                *bp++ = '\0';
                break;
            }
            else
            {
                *bp++ = *sp++;
            }
        }

        values.get()[count] = bp;
        while (*sp)
        {
            if (*sp == '\\')
            {
                if (*++sp)
                {
                    *bp++ = *sp++;
                }
            }
            else if (*sp == '/')
            {
                break;
            }
            else
            {
                *bp++ = *sp++;
            }
        }
        *bp++ = '\0';
        count++;
    }


    PEG_TRACE_CSTRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
    "CertificateProvider::getIssuerName  WHILE EXIT");

    //create the issuername object and add each type/value pair
    X509_NAME* issuerNameNew = X509_NAME_new();
    int nid;

    for (int i = 0; i < count; i++)
    {
        nid = OBJ_txt2nid(types.get()[i]);

        //if we don't recognize the name element or there is no
        //corresponding value, continue to the next one
        if (nid == NID_undef || !*values.get()[i]) 
        {
            continue;
        }

        if (!X509_NAME_add_entry_by_NID(
                issuerNameNew, nid, chtype,
                (unsigned char*)values.get()[i], -1, -1, 0))
        {
            X509_NAME_free(issuerNameNew);
            issuerNameNew = NULL;
            break;
        }
    }

    PEG_TRACE_CSTRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
        "Got issuerName successfully");
    PEG_METHOD_EXIT();

    return issuerNameNew;
}

/** Determines whether the user has sufficient access to perform a 
    certificate operation.
  */
Boolean CertificateProvider::_verifyAuthorization(const String& userName)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
    "CertificateProvider::_verifyAuthorization");

    if (_enableAuthentication) 
    {
#if !defined(PEGASUS_OS_OS400)
        if (!System::isPrivilegedUser(userName)) 
#else
        CString user = userName.getCString();
        const char * tmp = (const char *)user;
        AtoE((char *)tmp);
        if (!ycmCheckUserSecurityAuthorities(tmp))
#endif
        {
            PEG_METHOD_EXIT();
            return false;
        }
    }

    PEG_METHOD_EXIT();
    return true;
}

/** Constructor
 */
CertificateProvider::CertificateProvider(CIMRepository* repository,
                                         SSLContextManager* sslContextMgr) :
_repository(repository),
_sslContextMgr(sslContextMgr)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
    "CertificateProvider::CertificateProvider");

    ConfigManager* configManager = ConfigManager::getInstance();

    //get config properties
    _enableAuthentication = ConfigManager::parseBooleanValue(
        configManager->getCurrentValue("enableAuthentication"));

    _sslTrustStore = 
        ConfigManager::getHomedPath(configManager->getCurrentValue(
        "sslTrustStore"));

#ifdef PEGASUS_ENABLE_SSL_CRL_VERIFICATION
    _crlStore = 
        ConfigManager::getHomedPath(configManager->getCurrentValue("crlStore"));
#else
    _crlStore = String::EMPTY;
#endif

    PEG_METHOD_EXIT();
}

/** Destructor
 */ 
CertificateProvider::~CertificateProvider(void)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
        "CertificateProvider::~CertificateProvider");
    PEG_METHOD_EXIT();
}

/** Delivers a single instance to the CIMOM
 */ 
void CertificateProvider::getInstance(
                              const OperationContext & context,
                              const CIMObjectPath & cimObjectPath,
                              const Boolean includeQualifiers,
                              const Boolean includeClassOrigin,
                              const CIMPropertyList & propertyList,
                              InstanceResponseHandler & handler)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER, "CertificateProvider::getInstance");

    //verify authorization
    const IdentityContainer container = context.get(IdentityContainer::NAME);
    if (!_verifyAuthorization(container.getUserName())) 
    {
        MessageLoaderParms parms(
            "ControlProviders.CertificateProvider.MUST_BE_PRIVILEGED_USER",
            "Superuser authority is required to run this CIM operation.");
        throw CIMException(CIM_ERR_ACCESS_DENIED, parms);
    }

    CIMName className(cimObjectPath.getClassName());

    //verify classname
    if (className == PEGASUS_CLASSNAME_CERTIFICATE)
    {
        // process request
        handler.processing();
    
        //verify the keys are set
        //ATTN: do we need to do this, or will the getInstance call handle it?
        Array<CIMKeyBinding> keyBindings = cimObjectPath.getKeyBindings();
        String keyName;
    
        for (Uint32 i=0; i < keyBindings.size(); i++)
        {
            keyName = keyBindings[i].getName().getString();
    
            if (!String::equal(keyName, ISSUER_NAME_PROPERTY.getString()) 
                && !String::equal(keyName, SERIAL_NUMBER_PROPERTY.getString())) 
            {
                throw CIMException(CIM_ERR_INVALID_PARAMETER, keyName);
            }
        }
    
        CIMInstance cimInstance = 
            _repository->getInstance(cimObjectPath.getNameSpace(), 
                    cimObjectPath);
    
        PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
            "Returning certificate COP " + cimInstance.getPath().toString());
    
        // deliver instance
        handler.deliver(cimInstance);
    
        // complete request
        handler.complete();

    } 
#ifdef PEGASUS_ENABLE_SSL_CRL_VERIFICATION
    else if (className == PEGASUS_CLASSNAME_CRL)
    {
        //ATTN: Fill in

    } 
#endif
    else
    {
        throw CIMException(CIM_ERR_INVALID_CLASS, className.getString());
    }

    PEG_METHOD_EXIT();
}

/** Builds and returns a PG_SSLCertificateRevocationList from an X509_CRL object
 */ 
inline CIMInstance _getCRLInstance(X509_CRL* xCrl, String host,
        CIMNamespaceName nameSpace)
{
    char issuerName[1024];
    STACK_OF(X509_REVOKED) *revoked = NULL;
    X509_REVOKED *r = NULL;
    int numRevoked = -1;
    long rawSerialNumber;
    char serial[1024];
    CIMDateTime revocationDate;

    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "CertificateProvider::_getCRLInstance");

    // build instance
    CIMInstance cimInstance(PEGASUS_CLASSNAME_CRL);

    // CA issuer name
    sprintf(issuerName, "%s",
            X509_NAME_oneline(X509_CRL_get_issuer(xCrl), NULL, 0));
    cimInstance.addProperty(CIMProperty(ISSUER_NAME_PROPERTY,
                CIMValue(String(issuerName))));

    // validity dates
    CIMDateTime lastUpdate = getDateTime(X509_CRL_get_lastUpdate(xCrl));
    cimInstance.addProperty(CIMProperty(LAST_UPDATE_PROPERTY,
                CIMValue(lastUpdate)));

    CIMDateTime nextUpdate = getDateTime(X509_CRL_get_nextUpdate(xCrl));
    cimInstance.addProperty(CIMProperty(NEXT_UPDATE_PROPERTY,
                CIMValue(nextUpdate)));

    Array<String> revokedSerialNumbers;
    Array<CIMDateTime> revocationDates;

    // get revoked certificate information
    revoked = X509_CRL_get_REVOKED(xCrl);
    numRevoked = sk_X509_REVOKED_num(revoked);

    for (int i = 0; i < numRevoked; i++) 
    {
        r = sk_X509_REVOKED_value(revoked, i);
        rawSerialNumber = ASN1_INTEGER_get(r->serialNumber);
        sprintf(serial, "%lu", rawSerialNumber);
        revokedSerialNumbers.append(String(serial));

        revocationDate = getDateTime(r->revocationDate);
        revocationDates.append(revocationDate);
    }

    cimInstance.addProperty(CIMProperty(REVOKED_SERIAL_NUMBERS_PROPERTY,
                CIMValue(revokedSerialNumbers)));
    cimInstance.addProperty(CIMProperty(REVOCATION_DATES_PROPERTY,
                CIMValue(revocationDates)));

     // set keys
    Array<CIMKeyBinding> keys;
    CIMKeyBinding key;
    key.setName(ISSUER_NAME_PROPERTY.getString());
    key.setValue(issuerName);
    key.setType(CIMKeyBinding::STRING);
    keys.append(key);

    // set object path for instance
    cimInstance.setPath(CIMObjectPath(host, nameSpace,
                PEGASUS_CLASSNAME_CRL, keys));

    PEG_METHOD_EXIT();

    return (cimInstance);
}

/** Delivers the complete collection of instances to the CIMOM
 */ 
void CertificateProvider::enumerateInstances(
                                     const OperationContext & context,
                                     const CIMObjectPath & cimObjectPath,
                                     const Boolean includeQualifiers,
                                     const Boolean includeClassOrigin,
                                     const CIMPropertyList & propertyList,
                                     InstanceResponseHandler & handler)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
    "CertificateProvider::enumerateInstances");

    //verify authorization
    const IdentityContainer container = context.get(IdentityContainer::NAME);
    if (!_verifyAuthorization(container.getUserName())) 
    {
        MessageLoaderParms parms( 
            "ControlProviders.CertificateProvider.MUST_BE_PRIVILEGED_USER", 
            "Superuser authority is required to run this CIM operation.");
        throw CIMException(CIM_ERR_ACCESS_DENIED, parms);
    }

    CIMName className(cimObjectPath.getClassName());

    //verify classname
    if (className == PEGASUS_CLASSNAME_CERTIFICATE)
    {
        // process request
        handler.processing();
    
        // get instances from the repository
        Array<CIMInstance> cimInstances;
        cimInstances = _repository->enumerateInstancesForClass(
            cimObjectPath.getNameSpace(), PEGASUS_CLASSNAME_CERTIFICATE, false);
    
        for (Uint32 i = 0, n = cimInstances.size(); i < n; i++)
        {
            Uint16 truststoreType = 0;
            Uint16 certType = 0;
          
            //
            // The truststore type key property is deprecated. To retain
            // backward compatibility, if there were instances of an earlier
            // version in the repository that specify a truststore type
            // other than cimserver, those instances will be ignored.
            // Also, if there are instances that do not specify a certificate
            // type, the type for such instances is set to unknown (1).
            // 
 
            //
            // Retrieve the truststore type
            //
            Uint32 pos = cimInstances[i].findProperty(
                            TRUSTSTORE_TYPE_PROPERTY);
            CIMProperty prop = cimInstances[i].getProperty(pos);
            prop.getValue().get(truststoreType);
    
            //
            // Filter instances whose truststore type is other than
            // server truststore.
            //

            if ( truststoreType == PG_SSLCERTIFICATE_TSTYPE_VALUE_SERVER )
            { 
                //
                // If the certificate type property does not have a value set, 
                // set its type to "Unknown"
                //

                Uint32 pos = cimInstances[i].findProperty(
                               CERTIFICATE_TYPE_PROPERTY);

                PEGASUS_ASSERT( pos != PEG_NOT_FOUND );

                CIMProperty prop = cimInstances[i].getProperty(pos);

                if ( prop.getValue().isNull())
                {
                    PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL3, 
                      "The instance does not have the certificate type set. " 
                      "Setting it to Unknown.");
                    prop.setValue(CERT_TYPE_UNKNOWN);
                }

                // deliver instance
                PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4, 
                    "Delivering CIMInstance " + cimInstances[i].getPath().toString());
                handler.deliver(cimInstances[i]);
            }
            else
            {
                PEG_TRACE_STRING(TRC_DISCARDED_DATA, Tracer::LEVEL3, 
                    "Ignoring CIMInstance " + 
                     cimInstances[i].getPath().toString());
            }
        }
    
        // complete request
        handler.complete();

    } 
#ifdef PEGASUS_ENABLE_SSL_CRL_VERIFICATION
    else if (className == PEGASUS_CLASSNAME_CRL)
    {
        // process request
        handler.processing();

        FileSystem::translateSlashes(_crlStore); 

        if (FileSystem::isDirectory(_crlStore)
                && FileSystem::canWrite(_crlStore))
        {
            Array<String> crlFiles;
            if (FileSystem::getDirectoryContents(_crlStore, crlFiles))
            {
                Uint32 count = crlFiles.size();
                for (Uint32 i = 0; i < count; i++)
                {
                    String filename = crlFiles[i];

                    PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
                                     "Filename " + filename);

                    //ATTN: Is this a two-way hash?  If so, I don't need to
                    //read in the CRL just to determine the issuer name
                    BIO* inFile = BIO_new(BIO_s_file());
                    X509_CRL* xCrl = NULL;
                    char fullPathName[1024];

                    sprintf(fullPathName, "%s/%s",
                            (const char*)_crlStore.getCString(),
                            (const char*)filename.getCString());

                    if (BIO_read_filename(inFile, fullPathName))
                    {
                        PEG_TRACE_CSTRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
                                         "Successfully read filename");

                         if (PEM_read_bio_X509_CRL(inFile, &xCrl, NULL, NULL))
                         {
                            // build instance
                            CIMInstance cimInstance = _getCRLInstance(xCrl,
                                    cimObjectPath.getHost(),
                                    cimObjectPath.getNameSpace());
                            
                            PEG_TRACE_STRING(TRC_CONTROLPROVIDER,Tracer::LEVEL4,
                                "Delivering CIMInstance: " + 
                    cimInstance.getPath().toString());

                            // deliver instance
                            handler.deliver(cimInstance);
                         }

                    } else
                    {
                        //error
                        PEG_TRACE_CSTRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3,
                                         "Error reading CRL file");
                    }

                    BIO_free_all(inFile);

                } //end for
            
                // complete request
                handler.complete();

            } else
            {
                PEG_TRACE_CSTRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3,
                         "Error: Could not read sslCRLStore directory.");
                MessageLoaderParms parms(
                        "ControlProviders.CertificateProvider."
                            "COULD_NOT_READ_DIRECTORY",
                        "Cannot read directory $0.", _crlStore);
                throw CIMException(CIM_ERR_FAILED, parms);
            }
        } else
        {
            PEG_TRACE_CSTRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3,
                             "Error: sslCRLStore is not a valid directory.");
            MessageLoaderParms parms(
                    "ControlProviders.CertificateProvider."
                        "INVALID_DIRECTORY",
                    "Invalid directory $0.", _crlStore);
            throw CIMException(CIM_ERR_FAILED, parms);
        }
    } 
#endif
    else
    {
        throw CIMException(CIM_ERR_INVALID_CLASS, className.getString());
    }

    PEG_METHOD_EXIT();
}

/** Delivers the complete collection of instance names (CIMObjectPaths) to
    the CIMOM
 */ 
void CertificateProvider::enumerateInstanceNames(
                                         const OperationContext & context,
                                         const CIMObjectPath & cimObjectPath,
                                         ObjectPathResponseHandler & handler)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "CertificateProvider::enumerateInstanceNames");

    //verify authorization
    const IdentityContainer container = context.get(IdentityContainer::NAME);
    if (!_verifyAuthorization(container.getUserName())) 
    {
        MessageLoaderParms parms(
                "ControlProviders.CertificateProvider."
                    "MUST_BE_PRIVILEGED_USER",
                "Superuser authority is required to run this CIM operation.");
        throw CIMException(CIM_ERR_ACCESS_DENIED, parms);
    }

    CIMName className(cimObjectPath.getClassName());

    //verify classname
    if (className == PEGASUS_CLASSNAME_CERTIFICATE)
    {
        // process request
        handler.processing();
    
        Array<CIMObjectPath> instanceNames =
            _repository->enumerateInstanceNamesForClass(
                cimObjectPath.getNameSpace(), PEGASUS_CLASSNAME_CERTIFICATE);
    
        for (Uint32 i = 0, n = instanceNames.size(); i < n; i++)
        {
            String truststoreType;
            Array<CIMKeyBinding> kb;
            Uint16 certType = 0;

            //
            // The truststore type key property is deprecated. To retain
            // backward compatibility, if there were instances of an earlier
            // version in the repository that specify a truststore type
            // other than cimserver, those instances will be ignored.
            // 
            kb = instanceNames[i].getKeyBindings();
            Uint32 count = kb.size();

            for (Uint32 j = 0; j < count; j++)
            {
                //
                // Retrieve the truststore type
                //
                PEG_TRACE_STRING ( TRC_CONTROLPROVIDER, Tracer::LEVEL4,
                    "Property name : " + kb[j].getName().getString());
                if ( kb[j].getName() == TRUSTSTORE_TYPE_PROPERTY )
                {
                    truststoreType = kb[j].getValue();
                    break;
                }
            }

            //
            // Filter instances whose truststore type is other than
            // server truststore.
            //

            if ( truststoreType == PG_SSLCERTIFICATE_TSTYPE_VALUE_SERVER)
            {
                PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4, 
                   "Delivering CIMObjectPath: " + instanceNames[i].toString());
    
                // deliver object path
                handler.deliver(instanceNames[i]);
            }
            else
            {
                PEG_TRACE_STRING(TRC_DISCARDED_DATA, Tracer::LEVEL3, 
                   "Ignoring CIMObjectPath: " + 
                   instanceNames[i].toString());
            }
        }
    
        // complete request
        handler.complete();

    } 
#ifdef PEGASUS_ENABLE_SSL_CRL_VERIFICATION
    else if (className == PEGASUS_CLASSNAME_CRL)
    {
         // process request
        handler.processing();

        FileSystem::translateSlashes(_crlStore); 
        if (FileSystem::isDirectory(_crlStore) 
                && FileSystem::canWrite(_crlStore))
        {
            Array<String> crlFiles;
            if (FileSystem::getDirectoryContents(_crlStore, crlFiles))
            {
                Uint32 count = crlFiles.size();
                for (Uint32 i = 0; i < count; i++)
                {
                    String filename = crlFiles[i];

                    PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3,
                                     "Filename " + filename);

                    CIMObjectPath cimObjectPath;

                    //ATTN: Is this a two-way hash?  If so, I don't need
                    //to read in the CRL just to determine the issuer name
                    BIO* inFile = BIO_new(BIO_s_file());
                    X509_CRL* xCrl = NULL;
                    char issuerName[1024];
                    char fullPathName[1024];

                    sprintf(fullPathName, "%s/%s",
                            (const char*)_crlStore.getCString(),
                            (const char*)filename.getCString());

                    if (BIO_read_filename(inFile, fullPathName))
                    {
                        PEG_TRACE_CSTRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3,
                                         "Successfully read filename");

                         if (PEM_read_bio_X509_CRL(inFile, &xCrl, NULL, NULL))
                         {
                            PEG_TRACE_CSTRING(TRC_CONTROLPROVIDER,Tracer::LEVEL3,
                                "Successfully read CRL file");
                            sprintf(issuerName, "%s",
                                X509_NAME_oneline(X509_CRL_get_issuer(xCrl),
                                NULL, 0));
    
                            // build object path
                            Array<CIMKeyBinding> keys;
                            CIMKeyBinding key;
                            key.setName(ISSUER_NAME_PROPERTY.getString());
                            key.setValue(issuerName);
                            key.setType(CIMKeyBinding::STRING);
                            keys.append(key);
                        
                            // set object path for instance
                            CIMObjectPath instanceName(cimObjectPath.getHost(),
                                cimObjectPath.getNameSpace(),
                                PEGASUS_CLASSNAME_CRL, keys);
                    
                            PEG_TRACE_STRING(TRC_CONTROLPROVIDER,Tracer::LEVEL4,
                                "Instance Name: " + instanceName.toString());

                            handler.deliver(instanceName);
                         }

                    } else
                    {
                        //error
                        PEG_TRACE_CSTRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3,
                                         "Error reading CRL file");
                    }

                    BIO_free_all(inFile);
                } //end for
            
                // complete request
                handler.complete();

            } else
            {
                PEG_TRACE_CSTRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3,
                    "Error: Could not read sslCRLStore directory.");
                MessageLoaderParms parms(
                    "ControlProviders.CertificateProvider."
                        "COULD_NOT_READ_DIRECTORY",
                    "Cannot read directory $0.", _crlStore);
                throw CIMException(CIM_ERR_FAILED, parms);
            }
        } else
        {
            PEG_TRACE_CSTRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3,
                             "Error: sslCRLStore is not a valid directory.");
            MessageLoaderParms parms(
                "ControlProviders.CertificateProvider.INVALID_DIRECTORY",
                "Invalid directory $0.", _crlStore);
            throw CIMException(CIM_ERR_FAILED, parms);
        }
    } 
#endif
    else
    {
        throw CIMException(CIM_ERR_INVALID_CLASS, className.getString());
    } 

    PEG_METHOD_EXIT();
}

/** Not supported.  Use invokeMethod to create a certificate or CRL
*/ 
void CertificateProvider::createInstance(
                                 const OperationContext & context,
                                 const CIMObjectPath & cimObjectPath,
                                 const CIMInstance & cimInstance,
                                 ObjectPathResponseHandler & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED,
            "CertificateProvider::createInstance");
}

/** Not supported.
  */ 
void CertificateProvider::modifyInstance(
                                 const OperationContext & context,
                                 const CIMObjectPath & cimObjectPath,
                                 const CIMInstance & cimInstance,
                                 const Boolean includeQualifiers,
                                 const CIMPropertyList & propertyList,
                                 ResponseHandler & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED,
            "CertificateProvider::modifyInstance");
}

/** Deletes the internal object denoted by the specified CIMObjectPath
 */ 
void CertificateProvider::deleteInstance(
                                 const OperationContext & context,
                                 const CIMObjectPath & cimObjectPath,
                                 ResponseHandler & handler)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER, 
        "CertificateProvider::deleteInstance");

    //verify authorization
    const IdentityContainer container = context.get(IdentityContainer::NAME);
    if (!_verifyAuthorization(container.getUserName())) 
    {
        MessageLoaderParms parms(
           "ControlProviders.CertificateProvider.MUST_BE_PRIVILEGED_USER",
           "Superuser authority is required to run this CIM operation.");
        throw CIMException(CIM_ERR_ACCESS_DENIED, parms);
    }

    CIMName className(cimObjectPath.getClassName());

    //verify classname
    if (className == PEGASUS_CLASSNAME_CERTIFICATE)
    {
        // process request
        handler.processing();
    
        //
        // Check if the subjectName is passed.
        //
        Array<CIMInstance> cimInstances;
        Array<CIMKeyBinding> keys;
        CIMKeyBinding key;
        String certIssuer;
        String certSubject;
        String certSerialNum;
        Boolean subjectSet = true;
        Boolean issuerSet = true;
        Boolean serialNumSet = true;

        keys = cimObjectPath.getKeyBindings();

        if (keys.size() && String::equal(keys[0].getName().getString(), 
            ISSUER_NAME_PROPERTY.getString()))
        {
            certIssuer = keys[0].getValue();
        }
        else
        {
            issuerSet = false;
        }

        if (keys.size() && String::equal(keys[1].getName().getString(), 
            SUBJECT_NAME_PROPERTY.getString()))
        {
            certSubject = keys[1].getValue();
        }
        else
        {
            subjectSet = false;
        }

        if (keys.size() && String::equal(keys[1].getName().getString(), 
            SERIAL_NUMBER_PROPERTY.getString()))
        {
            certSerialNum = keys[1].getValue();
        }
        else
        {
            serialNumSet = false;
        }

        PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3, 
            "issuerName :" + certIssuer);

        //
        // Check if the subject and issuer were specified.
        //
        if (subjectSet && issuerSet)
        {
            PEG_TRACE_CSTRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3, 
               "Subject and issuer specified.");
            PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3, 
                "subjectName :" + certSubject);

            Array<CIMInstance> certificateNamedInstances;

            //
            // get all the instances of class PG_SSLCertificate
            //
            certificateNamedInstances =
                _repository->enumerateInstancesForClass(
                    PEGASUS_NAMESPACENAME_CERTIFICATE,
                    PEGASUS_CLASSNAME_CERTIFICATE, false);

            //
            // Retrieve the instances for the specified subject & issuer
            // 
            Uint32 num = certificateNamedInstances.size();

            for (Uint32 i = 0; i < num; i++)
            {
                String issuer;
                String subject;
                Uint16 truststoreType = 0;

                CIMInstance& certificateInstance =
                    certificateNamedInstances[i];

                PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4, 
                   "Comparing instance : " 
                    + certificateInstance.getPath().toString());
                
                //
                // Retrieve the truststore type
                //
                Uint32 pos = certificateInstance.findProperty(
                                TRUSTSTORE_TYPE_PROPERTY);
                CIMProperty prop = certificateInstance.getProperty(pos);
                prop.getValue().get(truststoreType);

                //
                // Filter instances whose truststore type is 
                // other than server truststore.
                //

                if ( truststoreType == PG_SSLCERTIFICATE_TSTYPE_VALUE_SERVER )
                {
                    //
                    // Check if issuer name and subject are specified
                    // and they match
                    //
                    Uint32 pos = certificateInstance.findProperty(
                                    ISSUER_NAME_PROPERTY);
                    CIMProperty prop = certificateInstance.getProperty(pos);
                    prop.getValue().get(issuer);

                    pos = 
                        certificateInstance.findProperty(SUBJECT_NAME_PROPERTY);
                    prop = certificateInstance.getProperty(pos);
                    prop.getValue().get(subject);

                   if ( issuer == certIssuer && subject == certSubject)
                   {
                       PEG_TRACE_CSTRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3, 
                          "Found a matching instance.");
                       cimInstances.append(certificateInstance);
                   }
               }
               else
               {
                    PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3, 
                        "Ignoring instance : " + 
                        certificateInstance.getPath().toString());
               }
           }

           // Check if the certificate was found
           if (cimInstances.size() == 0)
           {
               // Certificate does not exist, throw exception
               PEG_TRACE_CSTRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3, 
                   "The certificate does not exist.");
               MessageLoaderParms parms(
                   "ControlProviders.CertificateProvider.CERT_DNE",
                   "The certificate does not exist.");
               throw CIMException(CIM_ERR_NOT_FOUND, parms);
           }
        } 
        else if (issuerSet && serialNumSet)
        {
            PEG_TRACE_CSTRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3, 
                "issuer and serial number specified.");

            PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3, 
                "serial number :" + certSerialNum);

            CIMObjectPath tmpPath = cimObjectPath;

            try
            {
                Array<CIMKeyBinding> keys;

                //
                // The truststore type key property is deprecated. To retain
                // backward compatibility, add the truststore type property
                // to the key bindings and set it to cimserver truststore.
                //
                CIMKeyBinding kb (TRUSTSTORE_TYPE_PROPERTY, 
                    PG_SSLCERTIFICATE_TSTYPE_VALUE_SERVER);
                keys.append (kb);

                keys = cimObjectPath.getKeyBindings();
                
                keys.append (kb);
 
                tmpPath.setKeyBindings(keys);

                cimInstances.append(_repository->getInstance(
                   cimObjectPath.getNameSpace(), tmpPath));

            } 
            catch (Exception& ex)
            {
                PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3, 
                   "The certificate does not exist. " + tmpPath.toString() );
                MessageLoaderParms parms(
                    "ControlProviders.CertificateProvider.CERT_DNE",
                    "The certificate does not exist.");
                throw CIMException(CIM_ERR_NOT_FOUND, parms);
            }
        }
        else
        {
            throw CIMException(CIM_ERR_INVALID_PARAMETER, 
                cimObjectPath.toString());
        }

        // Check if there were certificates to be deleted.
        if (cimInstances.size() > 0)
        {
            // Delete the certificates
            _removeCert(cimInstances);
        }

        // complete request
        handler.complete();

    } 
#ifdef PEGASUS_ENABLE_SSL_CRL_VERIFICATION
    else if (className == PEGASUS_CLASSNAME_CRL)
    {
        Array<CIMKeyBinding> keys;
        CIMKeyBinding key;
        String issuerName;

        keys = cimObjectPath.getKeyBindings();
        if (keys.size() && String::equal(keys[0].getName().getString(),
                                          ISSUER_NAME_PROPERTY.getString()))
        {
            issuerName = keys[0].getValue();
        }

        PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
                         "CRL COP" + cimObjectPath.toString());
        PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
                         "Issuer Name " + issuerName);

        //ATTN: it would nice to be able to do this by getting the hash
        //directly from the issuerName
        //unfortunately, there does not seem to be an easy way to achieve this
        //the closest I can get is to add the individual DN components using
        //X509_NAME_add_entry_by_NID
        //which involves a lot of tedious parsing.
        //look in the do_subject method of apps.h for how this is done
        //X509_NAME* name = X509_name_new();

        char issuerChar[1024];
        sprintf(issuerChar, "%s", (const char*) issuerName.getCString());

        X509_NAME* name = getIssuerName(issuerChar, MBSTRING_ASC);

        AutoMutex lock(_crlStoreMutex);

        String crlFileName = _getCRLFileName(_crlStore, X509_NAME_hash(name));
        if (FileSystem::exists(crlFileName)) 
        {
            if (FileSystem::removeFile(crlFileName)) 
            {
                PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3,
                             "Successfully deleted CRL file " + crlFileName);

                //
                // reload the CRL store to refresh the cache
                //
                _sslContextMgr->reloadCRLStore();

                Logger::put(Logger::STANDARD_LOG, 
                        System::CIMSERVER, Logger::TRACE,
                        "The CRL from issuer $0 has been deleted.",
                        issuerName);

            } else
            {
                PEG_TRACE_CSTRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3,
                                 "Could not delete file.");
                 MessageLoaderParms parms(
                     "ControlProviders.CertificateProvider.DELETE_FAILED",
                     "Could not delete file $0.",
                     FileSystem::extractFileName(crlFileName));
                 throw CIMException(CIM_ERR_FAILED, parms);
            }
        } else
        {
             PEG_TRACE_CSTRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3,
                              "File does not exist.");
             MessageLoaderParms parms(
                     "ControlProviders.CertificateProvider.FILE_DNE",
                     "File does not exist $0.",
                     FileSystem::extractFileName(crlFileName));
             throw CIMException(CIM_ERR_NOT_FOUND, parms);
        }
   
        X509_NAME_free(name);

    } 
#endif
    else
    {
        throw CIMException(CIM_ERR_INVALID_CLASS, className.getString());
    }

    PEG_METHOD_EXIT();
}


void CertificateProvider::_removeCert (Array<CIMInstance> cimInstances)
{
    Uint32 num = cimInstances.size();
    PEG_TRACE((TRC_CONTROLPROVIDER, Tracer::LEVEL4, 
        "Number of certificate instances to be removed : %d " , num));


    for ( Uint32 i = 0; i < num ; i++)
    {
        String issuerName;
        String userName;
        String certificateFileName;
        String serialNumber;
        CIMProperty cimProperty;

        CIMInstance& certificateInstance = cimInstances[i];

        //certificate file name
        cimProperty = certificateInstance.getProperty(
                         certificateInstance.findProperty(FILE_NAME_PROPERTY));
        cimProperty.getValue().get(certificateFileName);

        PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4, 
           "Certificate file name " + certificateFileName);

        //issuer name
        cimProperty = certificateInstance.getProperty(
                     certificateInstance.findProperty(ISSUER_NAME_PROPERTY));
        cimProperty.getValue().get(issuerName);

        //user name
        cimProperty = certificateInstance.getProperty(
                         certificateInstance.findProperty(USER_NAME_PROPERTY));
        cimProperty.getValue().get(userName);

        //serial number
        cimProperty = certificateInstance.getProperty(
                      certificateInstance.findProperty(SERIAL_NUMBER_PROPERTY));
        cimProperty.getValue().get(serialNumber);

        PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4, 
           "Issuer name " + issuerName);
        
        PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4, 
           "serial number " + serialNumber);

        if (userName == String::EMPTY)
        {
            PEG_TRACE_CSTRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4, 
               "The certificate does not have a username associated with it");
        }
        else
        {
            PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4, 
                "User name " + userName);
        }

        AutoMutex lock(_trustStoreMutex);

        if (!FileSystem::exists(certificateFileName)) 
        {
             //
             // In rare cases a certificate may have been
             // manually removed from the truststore, but the repositoty
             // entry still exists. Delete the Repository instance so that
             // the certificate can be re-added again if required.
             //
             // This is also valid for end-entity certificates as they
             // would not exist in the truststore.
             //

             PEG_TRACE_CSTRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3, 
                "WARNING: Certificate file does not exist," 
                 "remove entry from repository anyway.");
        }
        else if (!FileSystem::removeFile(certificateFileName)) 
        {
            PEG_TRACE_CSTRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3, 
                 "Could not delete file.");
            MessageLoaderParms parms(
                "ControlProviders.CertificateProvider.DELETE_FAILED",
                 "Could not delete file $0.", certificateFileName);
            throw CIMException(CIM_ERR_FAILED, parms);
        }

        PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3, 
          "Successfully deleted certificate file " + certificateFileName);
                
        Array<CIMKeyBinding> kbArray;
        CIMKeyBinding        kb;

        kb.setName(ISSUER_NAME_PROPERTY);
        kb.setValue(issuerName);
        kb.setType(CIMKeyBinding::STRING);
        kbArray.append(kb);

        kb.setName(SERIAL_NUMBER_PROPERTY);
        kb.setValue(serialNumber);
        kb.setType(CIMKeyBinding::STRING);
        kbArray.append(kb);

        //
        // The truststore type key property is deprecated. To retain
        // backward compatibility, add the truststore type property
        // to the key bindings and set it to cimserver truststore.
        // 
        CIMKeyBinding key (TRUSTSTORE_TYPE_PROPERTY,
                PG_SSLCERTIFICATE_TSTYPE_VALUE_SERVER);
        kbArray.append (key);

        CIMObjectPath reference(
            String::EMPTY, PEGASUS_NAMESPACENAME_CERTIFICATE,
            PEGASUS_CLASSNAME_CERTIFICATE, kbArray);

        PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3, 
          "keys are: " + reference.toString());

        // Delete from repository.
        _repository->deleteInstance(
            PEGASUS_NAMESPACENAME_CERTIFICATE,
            reference);

        if (userName == String::EMPTY)
        {
            Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, 
               Logger::TRACE,
              "The certificate without an associated user" 
              "name from issuer $0 "
              "has been deleted from the truststore.",
               issuerName);
        }
        else
        {
            Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
               "The certificate registered to $0 from issuer $1 "
               "has been deleted from the truststore.",
               userName,
               issuerName);
        }
    }

    //
    // Request SSLContextManager to delete the certificate from the cache
    //
    try
    {
        _sslContextMgr->reloadTrustStore();
    }
    catch (SSLException& ex)
    {
        PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3, 
           "Trust store reload failed, " + ex.getMessage());

        MessageLoaderParms parms(
            "ControlProviders.CertificateProvider.TRUSTSTORE_RELOAD_FAILED",
            "Trust store reload failed, certificate deletion will" 
            " not be effective until cimserver restart.");
                throw CIMException(CIM_ERR_FAILED, parms);
    }
}

/** Returns the CRL filename associated with the hashvalue that
 *  represents the issuer name.  
 *  There is only one CRL per issuer so the file name will always end in .r0
 */ 
String CertificateProvider::_getCRLFileName(String crlStore,
        unsigned long hashVal)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
                     "CertificateProvider::_getCRLFileName");

    Uint32 index = 0;

    //The files are looked up by the CA issuer name hash value. 
    //Since only one CRL should exist for a given CA, the extension .r0
    //is appended to the CA hash
    char hashBuffer[32];
    sprintf(hashBuffer, "%08lx", hashVal);

    String hashString = "";
    for (int j = 0; j < 32; j++)
    {
        if (hashBuffer[j] != '\0')
        {
            hashString.append(hashBuffer[j]);
        } else
        {
            break; // end of hash string
        }
    }

    char filename[1024];
    sprintf(filename, "%s/%s.r0", 
            (const char*)crlStore.getCString(),
            (const char*)hashString.getCString());

    PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
        "Searching for files like " + hashString + "in " + crlStore);

    FileSystem::translateSlashes(crlStore); 
    if (FileSystem::isDirectory(crlStore) && FileSystem::canWrite(crlStore))
    {
        if (FileSystem::exists(filename)) 
        {
            //overwrite
            PEG_TRACE_CSTRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3,
                             "CRL already exists, overwriting");

        } else
        {
            //create
            PEG_TRACE_CSTRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3,
                             "CRL does not exist, creating");
        }
    } else
    {
        PEG_TRACE_CSTRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3,
            "Cannot add CRL to CRL store : CRL directory DNE or does not"
            " have write privileges");
        MessageLoaderParms parms(
                "ControlProviders.CertificateProvider.INVALID_DIRECTORY",
                "Invalid directory $0.", crlStore);
        throw CIMException(CIM_ERR_FAILED, parms);
    }

    PEG_METHOD_EXIT();

    return (String(filename));
}

/** Returns the new certificate filename for the hashvalue that
 *  represents the subject name.
 */ 
String CertificateProvider::_getNewCertificateFileName(String trustStore,
                                                       unsigned long hashVal) 
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
                     "CertificateProvider::_getNewCertificateFileName");

    //The files are looked up by the CA subject name hash value. 
    //If more than one CA certificate with the same name hash value exists, 
    //the extension must be different (e.g. 9d66eef0.0, 9d66eef0.1 etc)
    char hashBuffer[32];
    sprintf(hashBuffer, "%08lx", hashVal);

    String hashString = "";
    for (int j = 0; j < 32; j++)
    {
        if (hashBuffer[j] != '\0')
        {
            hashString.append(hashBuffer[j]);
        } else
        {
            break; // end of hash string
        }
    }

    PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
                     "Searching for files like " + hashString);

    Uint32 index = 0;
    FileSystem::translateSlashes(trustStore); 
    if (FileSystem::isDirectory(trustStore) && FileSystem::canWrite(trustStore))
    {
        Array<String> trustedCerts;
        if (FileSystem::getDirectoryContents(trustStore, trustedCerts))
        {
            for (Uint32 i = 0; i < trustedCerts.size(); i++)
            {
                //
                // Check if another certificate with the same
                // subject name already exists. If yes, error out.
                //
                if (String::compare(trustedCerts[i], 
                    hashString, hashString.size()) == 0)
                {
                    PEG_TRACE_STRING(TRC_CONTROLPROVIDER, 
                        Tracer::LEVEL3, 
                        "Error: Certificate with the same subject"
                        " already exists.");
                    MessageLoaderParms parms( "ControlProviders."
                        "CertificateProvider.CERT_WITH_SAME_SUBJECT",
                        "Another certificate with the"
                        " same subject name already exists.");
                    throw CIMException(CIM_ERR_ALREADY_EXISTS, parms);
                }
            }
        } else
        {
            PEG_TRACE_CSTRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3,
                             "Error: Could not read truststore directory.");
            MessageLoaderParms parms(
                "ControlProviders.CertificateProvider."
                    "COULD_NOT_READ_DIRECTORY",
                "Cannot read directory $0.", trustStore);
            throw CIMException(CIM_ERR_FAILED, parms);
        }
    } else
    {
        PEG_TRACE_CSTRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3,
                         "Error: sslCRLStore is not a valid directory.");
        MessageLoaderParms parms(
            "ControlProviders.CertificateProvider.INVALID_DIRECTORY",
            "Invalid directory $0.", trustStore);
        throw CIMException(CIM_ERR_FAILED, parms);
    }

    char filename[1024];
    sprintf(filename, "%s/%s.%d", 
            (const char*)trustStore.getCString(),
            (const char*)hashString.getCString(), 
            index);

    PEG_METHOD_EXIT();

    return (String(filename));
}

/** Calls an extrinsic method on the class.
 */ 
void CertificateProvider::invokeMethod(
    const OperationContext & context,
    const CIMObjectPath & cimObjectPath,
    const CIMName & methodName,
    const Array<CIMParamValue> & inParams,
    MethodResultResponseHandler & handler)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,"CertificateProvider::invokeMethod");

    //verify authorization
    const IdentityContainer container = context.get(IdentityContainer::NAME);
    if (!_verifyAuthorization(container.getUserName())) 
    {
        MessageLoaderParms parms(
            "ControlProviders.CertificateProvider.MUST_BE_PRIVILEGED_USER",
            "Superuser authority is required to run this CIM operation.");
        throw CIMException(CIM_ERR_ACCESS_DENIED, parms);
    }

    CIMName className(cimObjectPath.getClassName());

    //verify classname
    if (className == PEGASUS_CLASSNAME_CERTIFICATE)
    {
        // process request
        handler.processing();
    
        if (methodName == METHOD_ADD_CERTIFICATE)
        {
            PEG_TRACE_CSTRING(TRC_CONTROLPROVIDER,Tracer::LEVEL4,
                             "CertificateProvider::addCertificate()");
    
            String certificateContents = String::EMPTY;
            String userName = String::EMPTY;
            Uint16 certType = 0;
            CIMValue cimValue;
            
            cimValue = inParams[0].getValue();
            cimValue.get(certificateContents);
            
            cimValue = inParams[1].getValue();
            cimValue.get(userName);
            
            cimValue = inParams[2].getValue();
            cimValue.get(certType);

            PEG_TRACE_CSTRING(TRC_CONTROLPROVIDER,Tracer::LEVEL4,
                             "Certificate parameters:\n");
            PEG_TRACE_STRING(TRC_CONTROLPROVIDER,Tracer::LEVEL4,
                             "\tcertificateContents:" + certificateContents);
            PEG_TRACE_STRING(TRC_CONTROLPROVIDER,Tracer::LEVEL4,
                             "\tcertificateType:" + certType);
            if (userName == String::EMPTY)
            {
                PEG_TRACE_CSTRING(TRC_CONTROLPROVIDER,Tracer::LEVEL4,
                                 "\tDoes not have an associated username");
            }
            else
            {
                PEG_TRACE_STRING(TRC_CONTROLPROVIDER,Tracer::LEVEL4,
                                 "\tuserName:" + userName);
            }
            
            //check for a valid username if one is specified
            if (userName == String::EMPTY)
            { 
                PEG_TRACE_CSTRING(TRC_CONTROLPROVIDER,Tracer::LEVEL4,
                    "The certificate does not have an associated user name");
            }
            else if ( !System::isSystemUser(userName.getCString()))
            {
                throw CIMException(CIM_ERR_INVALID_PARAMETER, 
                 "The user specified by userName is not a valid system user.");
            }
    
            //read in the certificate contents
            BIO *mem = BIO_new(BIO_s_mem());
            BIO_puts(mem, (const char*)certificateContents.getCString());
    
            //
            // Read the buffer until no more certificates found.
            //
            Uint32 certCount = 0;
            while (PEM_read_bio_X509(mem, NULL , 0, NULL))
            {
                certCount++;
            }
 
            //
            // If more than one certificate was found, error out.
            //
            if (certCount > 1)
            {
                PEG_TRACE((TRC_CONTROLPROVIDER, Tracer::LEVEL3,
                        "Error: More than one cert in file : %d", certCount));
                BIO_free(mem);
                MessageLoaderParms parms(
                    "ControlProviders.CertificateProvider."
                        "MULTIPLE_CERT_IN_FILE",
                    "Specified certificate file contains more than one"
                        " certificate.");
                throw CIMException(CIM_ERR_FAILED, parms);
            }

            BIO_free(mem);

            //read in the certificate contents
            BIO *memCert = BIO_new(BIO_s_mem());
            BIO_puts(memCert, (const char*)certificateContents.getCString());

            //
            // Read the certificate from buffer.
            //
            X509* xCert = PEM_read_bio_X509(memCert, NULL , 0, NULL);
            if (xCert == NULL)
            {
                BIO_free(memCert);

                PEG_TRACE_CSTRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3,
                                 "Error: Could not read x509 PEM format.");
                MessageLoaderParms parms(
                        "ControlProviders.CertificateProvider.BAD_X509_FORMAT",
                        "Could not read x509 PEM format.");
                throw CIMException(CIM_ERR_FAILED, parms);
            }
            BIO_free(memCert);
            
            PEG_TRACE_CSTRING(TRC_CONTROLPROVIDER,Tracer::LEVEL4,
                             "Read x509 certificate...");
            
            char buf[256];
            String issuerName = String::EMPTY;
            String serialNumber = String::EMPTY;
            String subjectName = String::EMPTY;
            CIMDateTime notBefore;
            CIMDateTime notAfter;
            
            //issuer name
            X509_NAME_oneline(X509_get_issuer_name(xCert), buf, 256);
            issuerName = String(buf);
            
            //serial number
            long rawSerialNumber = 
                ASN1_INTEGER_get(X509_get_serialNumber(xCert));
            char serial[256];
            sprintf(serial, "%lu", rawSerialNumber);
            serialNumber = String(serial);
    
            //subject name
            X509_NAME_oneline(X509_get_subject_name(xCert), buf, 256);
            subjectName = String(buf);

            //validity dates
            notBefore = getDateTime(X509_get_notBefore(xCert));
            notAfter = getDateTime(X509_get_notAfter(xCert));

            PEG_TRACE_STRING(TRC_CONTROLPROVIDER,Tracer::LEVEL4,
                             "IssuerName:" + issuerName);
            PEG_TRACE_STRING(TRC_CONTROLPROVIDER,Tracer::LEVEL4,
                             "SerialNumber:" + serialNumber);
            PEG_TRACE_STRING(TRC_CONTROLPROVIDER,Tracer::LEVEL4,
                             "SubjectName:" + subjectName);
            PEG_TRACE_STRING(TRC_CONTROLPROVIDER,Tracer::LEVEL4,
                             "NotBefore:" + notBefore.toString());
            PEG_TRACE_STRING(TRC_CONTROLPROVIDER,Tracer::LEVEL4,
                             "NotAfter:" + notAfter.toString());
            
            //check validity with current datetime.
            //openssl will reject the certificate if it's not valid even if
            //we add it to the truststore
            try
            {
                if ((CIMDateTime::getDifference(
                            CIMDateTime::getCurrentDateTime(), notBefore) > 0))
                {
                    PEG_TRACE_CSTRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3,
                        "Certificate or CRL is not valid yet."
                        "  Check the timestamps on your machine.");
                    MessageLoaderParms parms(
                            "ControlProviders.CertificateProvider."
                                "CERT_NOT_VALID_YET",
                            "The certificate is not valid yet."
                                "  Check the timestamps on your machine.");
                    throw CIMException(CIM_ERR_FAILED, parms);
                }
                if (CIMDateTime::getDifference(notAfter,
                            CIMDateTime::getCurrentDateTime()) > 0) 
                {
                    PEG_TRACE_CSTRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3,
                                     "Certificate or CRL is expired.");
                    MessageLoaderParms parms(
                            "ControlProviders.CertificateProvider.CERT_EXPIRED",
                            "The certificate has expired.");
                    throw CIMException(CIM_ERR_FAILED, parms);
                }

            } catch (DateTimeOutOfRangeException& ex)
            {
                    PEG_TRACE_CSTRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3,
                        "Certificate or CRL dates are out of range.");
                    MessageLoaderParms parms(
                        "ControlProviders.CertificateProvider.BAD_DATE_FORMAT",
                        "The validity dates are out of range.");
                    throw CIMException(CIM_ERR_FAILED, parms);
            }

            AutoMutex lock(_trustStoreMutex);

            String certificateFileName = _getNewCertificateFileName(
                                             _sslTrustStore, 
                                             X509_subject_name_hash(xCert));
            if (userName != String::EMPTY)
            {
                PEG_TRACE_STRING(TRC_CONTROLPROVIDER,Tracer::LEVEL4,
                   "Certificate " + certificateFileName + 
                    " registered to " + userName + "\n");
            }
            else
            {
                PEG_TRACE_STRING(TRC_CONTROLPROVIDER,Tracer::LEVEL4,
                   "Certificate " + certificateFileName + 
                    " does not have a user name associated with it");
            }
    
            // build instance
            CIMInstance cimInstance(PEGASUS_CLASSNAME_CERTIFICATE);
        
            cimInstance.addProperty(CIMProperty(ISSUER_NAME_PROPERTY,
                        CIMValue(issuerName)));
            cimInstance.addProperty(CIMProperty(SERIAL_NUMBER_PROPERTY,
                        CIMValue(serialNumber)));
            cimInstance.addProperty(CIMProperty(SUBJECT_NAME_PROPERTY,
                        CIMValue(subjectName)));
            cimInstance.addProperty(CIMProperty(USER_NAME_PROPERTY,
                        CIMValue(userName)));
            cimInstance.addProperty(CIMProperty(FILE_NAME_PROPERTY,
                        CIMValue(certificateFileName)));
            cimInstance.addProperty(CIMProperty(TRUSTSTORE_TYPE_PROPERTY,
                        CIMValue(PG_SSLCERTIFICATE_TSTYPE_VALUE_SERVER)));
            cimInstance.addProperty(CIMProperty(NOT_BEFORE_PROPERTY,
                        CIMValue(notBefore)));
            cimInstance.addProperty(CIMProperty(NOT_AFTER_PROPERTY,
                        CIMValue(notAfter)));
            cimInstance.addProperty(CIMProperty(CERTIFICATE_TYPE_PROPERTY,
                        CIMValue(certType)));
        
     // set keys
    Array<CIMKeyBinding> keys;
    CIMKeyBinding key;
    key.setName(ISSUER_NAME_PROPERTY.getString());
    key.setValue(issuerName);
    key.setType(CIMKeyBinding::STRING);
    keys.append(key);

    key.setName(SERIAL_NUMBER_PROPERTY.getString());
                key.setType(CIMKeyBinding::STRING);
    key.setValue(String(serialNumber));
    keys.append(key);

    CIMKeyBinding kb (TRUSTSTORE_TYPE_PROPERTY,
                      PG_SSLCERTIFICATE_TSTYPE_VALUE_SERVER);
    keys.append(key);

            // set object path for instance
            cimInstance.setPath(CIMObjectPath(cimObjectPath.getHost(),
                              cimObjectPath.getNameSpace(),
                              PEGASUS_CLASSNAME_CERTIFICATE, keys));
    
            PEG_TRACE_STRING(TRC_CONTROLPROVIDER,Tracer::LEVEL4,
               "New certificate COP: " + cimInstance.getPath().toString());
    
            //attempt to add the instance to the repository first; that way if
            //this instance already exist it will take care of throwing 
            //an error before we add the file to the truststore
            _repository->createInstance("root/PG_Internal", cimInstance);
    
            //
            // Check if the type of certificate is authority issued end entity. 
            // If true, the certificate is not added to the truststore. 
            // A username will be associated with the certificate in the
            // repository.
            //
            if ( ! (certType == TYPE_AUTHORITY_END_ENTITY ))
            {
                //ATTN: Take care of this conversion
                char newFileName[256];
                sprintf(newFileName, "%s",
                        (const char*) certificateFileName.getCString());
        
                //use the ssl functions to write out the client x509 certificate
                BIO* outFile = BIO_new(BIO_s_file());
       
                if (outFile == NULL)
                {
                    PEG_TRACE_CSTRING(TRC_CONTROLPROVIDER, Tracer::LEVEL2, 
                        "Unable to add certificate to truststore. " 
                        "Error while trying to write certificate,"
                        " BIO_new returned error");
                    MessageLoaderParms parms(
                        "ControlProviders.CertificateProvider."
                            "ERROR_WRITING_CERT",
                        "Unable to add certificate to truststore."
                            " Error while trying to write certificate.");
                    throw CIMException(CIM_ERR_FAILED, parms);
                }

                if (!BIO_write_filename(outFile, newFileName))
                {
                    BIO_free_all(outFile);
                    PEG_TRACE_CSTRING(TRC_CONTROLPROVIDER, Tracer::LEVEL2, 
                        "Unable to add certificate to truststore."
                        " Error while trying to write certificate, " 
                        "BIO_write_filename returned error");
                    MessageLoaderParms parms(
                        "ControlProviders.CertificateProvider."
                            "ERROR_WRITING_CERT",
                        "Unable to add certificate to truststore."
                            " Error while trying to write certificate.");
                    throw CIMException(CIM_ERR_FAILED, parms);
                }
                if (!PEM_write_bio_X509(outFile, xCert))
                {
                    BIO_free_all(outFile);
                    PEG_TRACE_CSTRING(TRC_CONTROLPROVIDER, Tracer::LEVEL2, 
                        "Unable to add certificate to truststore. " 
                        "Error while trying to write certificate,"
                        " PEM_write_bio_X509 returned error");
                    MessageLoaderParms parms(
                        "ControlProviders.CertificateProvider."
                            "ERROR_WRITING_CERT",
                        "Unable to add certificate to truststore."
                        " Error while trying to write certificate.");
                    throw CIMException(CIM_ERR_FAILED, parms);
                }

                BIO_free_all(outFile);
            
                if (userName == String::EMPTY)
                {
                    Logger::put(Logger::STANDARD_LOG,
                            System::CIMSERVER, Logger::TRACE, 
                            "The certificate without an associated user name"
                                " from issuer $0 has been added to the server"
                                " truststore.", 
                            issuerName);
                }
                else
                {
                    Logger::put(Logger::STANDARD_LOG,
                            System::CIMSERVER, Logger::TRACE, 
                            "The certificate registered to $0 from issuer"
                                " $1 has been added to the server truststore.", 
                            userName,
                            issuerName);
                }
            }

            CIMValue returnValue(Boolean(true));
    
            handler.deliver(returnValue);
    
            handler.complete();
    
        } else
        {
            throw CIMException(CIM_ERR_METHOD_NOT_FOUND,
                               methodName.getString());
        }

    } 
#ifdef PEGASUS_ENABLE_SSL_CRL_VERIFICATION
    else if (className == PEGASUS_CLASSNAME_CRL)
    {
        if (methodName == METHOD_ADD_CRL)
        {
            PEG_TRACE_CSTRING(TRC_CONTROLPROVIDER,Tracer::LEVEL4,
                "CertificateProvider::addCertificateRevocationList");
    
            String crlContents = String::EMPTY;
            CIMValue cimValue;
            
            cimValue = inParams[0].getValue();
            cimValue.get(crlContents);
           
            PEG_TRACE_STRING(TRC_CONTROLPROVIDER,Tracer::LEVEL4,
                             "inparam CRL contents:" + crlContents);
        
            //check for a valid CRL
            //read in the CRL contents
            BIO *mem = BIO_new(BIO_s_mem());
            BIO_puts(mem, (const char*)crlContents.getCString());
    
            X509_CRL *xCrl = NULL;
            if (!PEM_read_bio_X509_CRL(mem, &xCrl, NULL, NULL))
            {
                BIO_free(mem);
                
                PEG_TRACE_CSTRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3,
                                 "Error: Could not read x509 PEM format.");
                MessageLoaderParms parms(
                    "ControlProviders.CertificateProvider.BAD_X509_FORMAT",
                    "Could not read x509 PEM format.");
                throw CIMException(CIM_ERR_FAILED, parms);
            }
            BIO_free(mem);
            
            PEG_TRACE_CSTRING(TRC_CONTROLPROVIDER,Tracer::LEVEL4,
                             "Successfully read x509 CRL...");
            
            char buf[256];
            String issuerName = String::EMPTY;
            CIMDateTime lastUpdate;
            CIMDateTime nextUpdate;
            Array<String> revokedSerialNumbers;
            Array<CIMDateTime> revocationDates;

            //issuer name
            X509_NAME_oneline(X509_CRL_get_issuer(xCrl), buf, 256);
            issuerName = String(buf);

            //check validity of CRL
            //openssl will only issue a warning if the CRL is expired
            //However, we still don't want to let them register an expired
            //or invalid CRL
            lastUpdate = getDateTime(X509_CRL_get_lastUpdate(xCrl));
            nextUpdate = getDateTime(X509_CRL_get_nextUpdate(xCrl));
            try
            {
                if ((CIMDateTime::getDifference(
                                CIMDateTime::getCurrentDateTime(),
                                                lastUpdate) > 0))
                {
                    PEG_TRACE_CSTRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3,
                       "The CRL is not valid yet. "
                           "Check timestamps on your machine.");
                    MessageLoaderParms parms(
                        "ControlProviders.CertificateProvider."
                            "CRL_NOT_VALID_YET",
                        "The CRL is not valid yet."
                            "  Check the timestamps on your machine.");
                    throw CIMException(CIM_ERR_FAILED, parms);
                }
                if (CIMDateTime::getDifference(nextUpdate,
                            CIMDateTime::getCurrentDateTime()) > 0) 
                {
                    PEG_TRACE_CSTRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3,
                       "This CRL is not up-to-date."
                       "  Check CA for the latest one.");
                    MessageLoaderParms parms(
                        "ControlProviders.CertificateProvider.CRL_EXPIRED",
                        "The CRL is not up-to-date."
                            "  Check with the issuing CA for the latest one.");
                    throw CIMException(CIM_ERR_FAILED, parms);
                }

            } catch (DateTimeOutOfRangeException& ex)
            {
                PEG_TRACE_CSTRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3,
                                 "Certificate or CRL dates are out of range.");
                MessageLoaderParms parms(
                    "ControlProviders.CertificateProvider.BAD_DATE_FORMAT",
                    "Certificate or CRL dates are out of range.");
                throw CIMException(CIM_ERR_FAILED, parms);
            }
            
            STACK_OF(X509_REVOKED)* revokedCertificates = NULL;
            X509_REVOKED* revokedCertificate = NULL;
            int revokedCount = -1;

            revokedCertificates = X509_CRL_get_REVOKED(xCrl);
            revokedCount = sk_X509_REVOKED_num(revokedCertificates);
            
            char countStr[3];
            sprintf(countStr, "%d", revokedCount);

            if (revokedCount > 0)
            {
                PEG_TRACE_CSTRING(TRC_CONTROLPROVIDER,Tracer::LEVEL4,
                                 "CRL contains revoked certificate entries ");
                PEG_TRACE_CSTRING(TRC_CONTROLPROVIDER,Tracer::LEVEL4, countStr);
            } else
            {
                PEG_TRACE_CSTRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3,
                                 "Error: CRL is empty.");
                MessageLoaderParms parms(
                    "ControlProviders.CertificateProvider.EMPTY_CRL",
                    "The CRL is empty.");
                throw CIMException(CIM_ERR_FAILED, parms);
            }

            AutoMutex lock(_crlStoreMutex);

            String crlFileName = 
                _getCRLFileName(_crlStore,
                        X509_NAME_hash(X509_CRL_get_issuer(xCrl)));

            PEG_TRACE_STRING(TRC_CONTROLPROVIDER,Tracer::LEVEL4,
                             "IssuerName:" + issuerName);
            PEG_TRACE_STRING(TRC_CONTROLPROVIDER,Tracer::LEVEL4,
                             "FileName: " + crlFileName);

            //ATTN: Take care of this conversion
            //For some reason i cannot do this in the BIO_write_filename call
            char newFileName[256];
            sprintf(newFileName, "%s", (const char*) crlFileName.getCString());
        
            //use the ssl functions to write out the client x509 certificate
            BIO* outFile = BIO_new(BIO_s_file());
            if (outFile == NULL)
            {
                PEG_TRACE_CSTRING(TRC_CONTROLPROVIDER, Tracer::LEVEL2, 
                    "Unable to add CRL to truststore. " 
                    "Error while trying to write CRL, BIO_new returned error");
                MessageLoaderParms parms(
                    "ControlProviders.CertificateProvider.ERROR_WRITING_CRL",
                    "Unable to add CRL to truststore."
                        " Error while trying to write CRL.");
                throw CIMException(CIM_ERR_FAILED, parms);
            }

            if (!BIO_write_filename(outFile, newFileName))
            {
                BIO_free_all(outFile);
                PEG_TRACE_CSTRING(TRC_CONTROLPROVIDER, Tracer::LEVEL2, 
                    "Unable to add CRL to truststore. " 
                    "Error trying to write CRL,"
                    " BIO_write_filename returned error");
                PEG_TRACE_CSTRING(TRC_CONTROLPROVIDER, Tracer::LEVEL2,
                                 "Error: Unable to store CRL");
                MessageLoaderParms parms(
                    "ControlProviders.CertificateProvider.ERROR_WRITING_CRL",
                    "Unable to add CRL to truststore."
                        " Error while trying to write CRL.");
                throw CIMException(CIM_ERR_FAILED, parms);
            }

            if (!PEM_write_bio_X509_CRL(outFile, xCrl))
            {
                BIO_free_all(outFile);
                PEG_TRACE_CSTRING(TRC_CONTROLPROVIDER, Tracer::LEVEL2, 
                    "Unable to add CRL to truststore. " 
                    "Error trying to write CRL,"
                    " PEM_write_bio_X509_CRL returned error");
                MessageLoaderParms parms(
                    "ControlProviders.CertificateProvider.ERROR_WRITING_CRL",
                    "Unable to add CRL to truststore."
                        " Error while trying to write CRL.");
                throw CIMException(CIM_ERR_FAILED, parms);
            }

            BIO_free_all(outFile);

            Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE, 
                        "The CRL for issuer $1 has been updated.", 
                        issuerName);

            //reload the CRL store
            PEG_TRACE_CSTRING(TRC_SSL, Tracer::LEVEL4,
                             "Loading CRL store after an update");
            _sslContextMgr->reloadCRLStore();

            CIMValue returnValue(Boolean(true));
    
            handler.deliver(returnValue);
    
            handler.complete();
        } 
         else
        {
            throw CIMException(CIM_ERR_METHOD_NOT_FOUND,
                    methodName.getString());
        }
    } 
#endif
    else
    {
        throw CIMException(CIM_ERR_INVALID_CLASS, className.getString());
    }
    
}


PEGASUS_NAMESPACE_END


