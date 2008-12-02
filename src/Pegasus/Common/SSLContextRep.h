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

#ifndef Pegasus_SSLContextRep_h
#define Pegasus_SSLContextRep_h

#ifdef PEGASUS_HAS_SSL
# define OPENSSL_NO_KRB5 1
# include <openssl/err.h>
# include <openssl/ssl.h>
# include <openssl/rand.h>
#else
# define SSL_CTX void
#endif

#include <Pegasus/Common/SSLContext.h>
#include <Pegasus/Common/Mutex.h>
#include <Pegasus/Common/Threads.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/AutoPtr.h>
#include <Pegasus/Common/SharedPtr.h>

//
// Typedef's for OpenSSL callback functions.
//
extern "C"
{
    typedef void (* CRYPTO_SET_LOCKING_CALLBACK)(int, int, const char *, int);
    typedef unsigned long (* CRYPTO_SET_ID_CALLBACK)(void);
}

PEGASUS_NAMESPACE_BEGIN

struct FreeX509STOREPtr
{
    void operator()(X509_STORE* ptr)
    {
#ifdef PEGASUS_HAS_SSL
        X509_STORE_free(ptr);
#endif
    }
};

#ifdef PEGASUS_HAS_SSL

class SSLEnvironmentInitializer
{
public:

    SSLEnvironmentInitializer()
    {
        AutoMutex autoMut(_instanceCountMutex);

        PEG_TRACE((TRC_SSL, Tracer::LEVEL4,
            "In SSLEnvironmentInitializer(), _instanceCount is %d",
            _instanceCount));

        if (_instanceCount == 0)
        {
            _initializeCallbacks();
            SSL_load_error_strings();
            SSL_library_init();
        }

        _instanceCount++;
    }

    ~SSLEnvironmentInitializer()
    {
        AutoMutex autoMut(_instanceCountMutex);
        _instanceCount--;

        PEG_TRACE((TRC_SSL, Tracer::LEVEL4,
            "In ~SSLEnvironmentInitializer(), _instanceCount is %d",
            _instanceCount));

        if (_instanceCount == 0)
        {
            ERR_free_strings();
            _uninitializeCallbacks();
        }
    }

private:

    SSLEnvironmentInitializer(const SSLEnvironmentInitializer&);
    SSLEnvironmentInitializer& operator=(const SSLEnvironmentInitializer&);

    /*
        Initialize the SSL locking and ID callbacks.
    */
    static void _initializeCallbacks()
    {
        PEG_TRACE_CSTRING(TRC_SSL, Tracer::LEVEL4,
            "Initializing SSL callbacks.");

        // Allocate Memory for _sslLocks. SSL locks needs to be able to handle
        // up to CRYPTO_num_locks() different mutex locks.

        _sslLocks.reset(new Mutex[CRYPTO_num_locks()]);

# if defined(PEGASUS_HAVE_PTHREADS) && !defined(PEGASUS_OS_VMS)
        // Set the ID callback. The ID callback returns a thread ID.
        CRYPTO_set_id_callback((CRYPTO_SET_ID_CALLBACK) pthread_self);
# endif

        // Set the locking callback.

        CRYPTO_set_locking_callback(
            (CRYPTO_SET_LOCKING_CALLBACK) _lockingCallback);
    }

    /*
        Reset the SSL locking and ID callbacks.
    */
    static void _uninitializeCallbacks()
    {
        PEG_TRACE_CSTRING(TRC_SSL, Tracer::LEVEL4, "Resetting SSL callbacks.");
        CRYPTO_set_locking_callback(NULL);
        CRYPTO_set_id_callback(NULL);
        _sslLocks.reset();
    }

    static void _lockingCallback(
        int mode,
        int type,
        const char* file,
        int line)
    {
        if (mode & CRYPTO_LOCK)
        {
            _sslLocks.get()[type].lock();
        }
        else
        {
            _sslLocks.get()[type].unlock();
        }
    }

    /**
        Locks to be used by SSL.
    */
    static AutoArrayPtr<Mutex> _sslLocks;

    /**
        Count of the instances of this class.  The SSL environment must be
        initialized when the first SSLEnvironmentInitializer is constructed.
        It must be uninitialized when the last SSLEnvironmentInitializer is
        destructed.
    */
    static int _instanceCount;

    /**
        Mutex for controlling access to _instanceCount.
    */
    static Mutex _instanceCountMutex;
};

#endif

class SSLCallbackInfoRep
{
public:
    SSLCertificateVerifyFunction* verifyCertificateCallback;
    Array<SSLCertificateInfo*> peerCertificate;
    X509_STORE* crlStore;

    String ipAddress;

    friend class SSLCallback;

    friend class SSLCallbackInfo;
};

class SSLContextRep
{
public:

    /** Constructor for a SSLContextRep object.
    @param trustStore  trust store file path
    @param certPath  server certificate file path
    @param keyPath  server key file path
    @param verifyCert  function pointer to a certificate verification
    call back function.
    @param randomFile  file path of a random file that is used as a seed
    for random number generation by OpenSSL.

    @exception SSLException  exception indicating failure to create a context.
    */
    SSLContextRep(
        const String& trustStore,
        const String& certPath = String::EMPTY,
        const String& keyPath = String::EMPTY,
        const String& crlPath = String::EMPTY,
        SSLCertificateVerifyFunction* verifyCert = NULL,
        const String& randomFile = String::EMPTY);

    SSLContextRep(const SSLContextRep& sslContextRep);

    ~SSLContextRep();

    SSL_CTX * getContext() const;

    String getTrustStore() const;

    String getCertPath() const;

    String getKeyPath() const;

#ifdef PEGASUS_USE_DEPRECATED_INTERFACES
    String getTrustStoreUserName() const;
#endif

    String getCRLPath() const;

    SharedPtr<X509_STORE, FreeX509STOREPtr> getCRLStore() const;

    void setCRLStore(X509_STORE* store);

    Boolean isPeerVerificationEnabled() const;

    SSLCertificateVerifyFunction* getSSLCertificateVerifyFunction() const;

    /**
        Checks if the certificate associated with this SSL context has expired
        or is not yet valid.
        @exception SSLException if the certificate is determined to be invalid.
    */
    void validateCertificate();

private:

#ifdef PEGASUS_HAS_SSL
    /**
        Ensures that the SSL environment remains initialized for the lifetime
        of the SSLContextRep object.
    */
    SSLEnvironmentInitializer _env;
#endif

    SSL_CTX * _makeSSLContext();
    void _randomInit(const String& randomFile);
    Boolean _verifyPrivateKey(SSL_CTX *ctx, const String& keyPath);

    String _trustStore;
    String _certPath;
    String _keyPath;
    String _crlPath;
    String _randomFile;
    SSL_CTX * _sslContext;

    Boolean _verifyPeer;

    SSLCertificateVerifyFunction* _certificateVerifyFunction;

    SharedPtr<X509_STORE, FreeX509STOREPtr> _crlStore;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_SSLContextRep_h */
