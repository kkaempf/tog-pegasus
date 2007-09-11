#//%2006////////////////////////////////////////////////////////////////////////
#//
#// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
#// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
#// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
#// IBM Corp.; EMC Corporation, The Open Group.
#// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
#// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
#// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
#// EMC Corporation; VERITAS Software Corporation; The Open Group.
#// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
#// EMC Corporation; Symantec Corporation; The Open Group.
#//
#// Permission is hereby granted, free of charge, to any person obtaining a copy
#// of this software and associated documentation files (the "Software"), to
#// deal in the Software without restriction, including without limitation the
#// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
#// sell copies of the Software, and to permit persons to whom the Software is
#// furnished to do so, subject to the following conditions:
#// 
#// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
#// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
#// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
#// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
#// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
#// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
#// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
#// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#//
#//==============================================================================
#%/////////////////////////////////////////////////////////////////////////////
#
# tog-pegasus.spec
#
#%/////////////////////////////////////////////////////////////////////////////

#
# WARNING: This file is automatically generated by executing 
#   make -f pegasus/Makefile.Release create_OpenPegasusRPMSpecFile
# Any changes made directly to this file in the OpenPegasus source will
# be lost the next time this file is regenerated and submitted to CVS.
#

Version: 2.7.0
Release: 1%{?LINUX_VERSION:.%{LINUX_VERSION}}
Epoch:   1

# Start of section pegasus/rpm/tog-specfiles/tog-pegasus-intro.spec
#
%{?!PEGASUS_BUILD_TEST_RPM:   %define PEGASUS_BUILD_TEST_RPM        0}
# do "rpmbuild --define 'PEGASUS_BUILD_TEST_RPM 1'" to build test RPM.
#
%{?!AUTOSTART:   %define AUTOSTART        0}
# Use "rpm -[iU]vh --define 'AUTOSTART 1'" in order to have cimserver enabled
# (chkconfig --level=345 tog-pegasus on) after installation.
#

%define srcRelease 1
Summary:   OpenPegasus WBEM Services for Linux
Name:      tog-pegasus
Group:     Systems Management/Base
License:   Open Group Pegasus Open Source
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root
URL:       http://www.openpegasus.org

Source:    %{name}-%{version}-%{srcRelease}.tar.gz
#
# End of section pegasus/rpm/tog-specfiles/tog-pegasus-intro.spec

# Start of section pegasus/rpm/tog-specfiles/tog-pegasus-buildRequires.spec
#
BuildRequires:      bash, sed, grep, coreutils, procps, gcc, gcc-c++
BuildRequires:      libstdc++, make, pam-devel
BuildRequires:      openssl-devel >= 0.9.6, e2fsprogs
BuildRequires:      net-snmp-devel
#
# End of section  pegasus/rpm/tog-specfiles/tog-pegasus-buildRequires.spec

# Start of section pegasus/rpm/tog-specfiles/tog-pegasus-requires.spec
#
Requires:           bash, sed, grep, coreutils, procps, openssl >= 0.9.6, pam
#Requires:          krb5-libs, redhat-lsb, chkconfig, SysVinit, bind-libs
Requires:           e2fsprogs, bind-utils, net-tools
Requires(post):     bash, sed, grep, coreutils, procps, openssl >= 0.9.6, pam
#Requires(post):    krb5-libs, redhat-lsb, chkconfig, SysVinit, bind-libs
Requires(post):     e2fsprogs, bind-utils, net-tools
Requires(pre):      bash, sed, grep, coreutils, procps, openssl >= 0.9.6, pam
#Requires(pre):     krb5-libs, redhat-lsb, chkconfig, SysVinit, bind-libs
Requires(pre):      e2fsprogs, bind-utils, net-tools
Requires(postun):   bash, sed, grep, coreutils, procps, openssl >= 0.9.6, pam
#Requires(postun):  krb5-libs, redhat-lsb, chkconfig, SysVinit, bind-libs
Requires(postun):   e2fsprogs, bind-utils, net-tools
Requires:           net-snmp
#
# End of section  pegasus/rpm/tog-specfiles/tog-pegasus-requires.spec

# Start of section pegasus/rpm/tog-specfiles/tog-pegasus-desc.spec
#
Conflicts: openwbem
Provides: tog-pegasus-cimserver
BuildConflicts: tog-pegasus

%description
OpenPegasus WBEM Services for Linux enables management solutions that deliver
increased control of enterprise resources. WBEM is a platform and resource
independent DMTF standard that defines a common information model and
communication protocol for monitoring and controlling resources from diverse
sources.
#
# End of section pegasus/rpm/tog-specfiles/tog-pegasus-desc.spec

# Start of section pegasus/rpm/tog-specfiles/tog-pegasus-arch.spec
#
%ifarch ia64
%global PEGASUS_HARDWARE_PLATFORM LINUX_IA64_GNU
%else
%ifarch x86_64
%global PEGASUS_HARDWARE_PLATFORM LINUX_X86_64_GNU
%else
%ifarch ppc
%global PEGASUS_HARDWARE_PLATFORM LINUX_PPC_GNU
%else
%ifarch ppc64 pseries
%global PEGASUS_HARDWARE_PLATFORM LINUX_PPC64_GNU
%else
%ifarch s390
%global PEGASUS_HARDWARE_PLATFORM LINUX_ZSERIES_GNU
%else
%ifarch s390x zseries
%global PEGASUS_HARDWARE_PLATFORM LINUX_ZSERIES64_GNU
%else
%global PEGASUS_HARDWARE_PLATFORM LINUX_IX86_GNU
%endif 
%endif 
%endif 
%endif 
%endif
%endif
#
# End of section pegasus/rpm/tog-specfiles/tog-pegasus-arch.spec

%global PEGASUS_ARCH_LIB %{_lib}
%global OPENSSL_HOME /usr
%global OPENSSL_BIN /usr/bin
%global PEGASUS_PEM_DIR /etc/Pegasus
%global PEGASUS_SSL_CERT_FILE server.pem
%global PEGASUS_SSL_KEY_FILE file.pem
%global PEGASUS_SSL_TRUSTSTORE client.pem
%global PAM_CONFIG_DIR /etc/pam.d
%global PEGASUS_CONFIG_DIR /etc/Pegasus
%global PEGASUS_VARDATA_DIR /var/lib/Pegasus
%global PEGASUS_VARDATA_CACHE_DIR /var/lib/Pegasus/cache
%global PEGASUS_LOCAL_DOMAIN_SOCKET_PATH  /var/run/tog-pegasus/socket/cimxml.socket
%global PEGASUS_CIMSERVER_START_FILE /var/run/tog-pegasus/cimserver.pid
%global PEGASUS_TRACE_FILE_PATH /var/lib/Pegasus/cache/trace/cimserver.trc
%global PEGASUS_CIMSERVER_START_LOCK_FILE /var/run/tog-pegasus/cimserver_start.lock
%global PEGASUS_REPOSITORY_DIR /var/lib/Pegasus/repository
%global PEGASUS_PREV_REPOSITORY_DIR_NAME prev_repository
%global PEGASUS_REPOSITORY_PARENT_DIR /var/lib/Pegasus
%global PEGASUS_PREV_REPOSITORY_DIR /var/lib/Pegasus/prev_repository
%global PEGASUS_SBIN_DIR /usr/sbin
%global PEGASUS_DOC_DIR /usr/share/doc/tog-pegasus-2.7

%global PEGASUS_RPM_ROOT  $RPM_BUILD_DIR/$RPM_PACKAGE_NAME-$RPM_PACKAGE_VERSION
%global PEGASUS_RPM_HOME %PEGASUS_RPM_ROOT/build
%global PEGASUS_INSTALL_LOG /var/lib/Pegasus/log/install.log

# Start of section pegasus/rpm/tog-specfiles/tog-pegasus-packages.spec
#
%package devel
Summary: The OpenPegasus Software Development Kit
Group: Systems Management/Base
Requires: tog-pegasus >= %{version}
Obsoletes: tog-pegasus-sdk

%description devel
The OpenPegasus WBEM Services for Linux SDK is the developer's kit for the
OpenPegasus WBEM Services for Linux release. It provides Linux C++ developers
with the WBEM files required to build WBEM Clients and Providers. It also
supports C provider developers via the CMPI interface.

%if %{PEGASUS_BUILD_TEST_RPM}
%package test
Summary: The OpenPegasus Tests
Group: Systems Management/Base
Requires: tog-pegasus >= %{version} 

%description test
The OpenPegasus WBEM tests for the OpenPegasus %{version} Linux rpm.
%endif
#
#End of section pegasus/rpm/tog-specfiles/tog-pegasus-packages.spec

%prep
%setup -q -n %{name}-%{version}

%build
# Start of section pegasus/rpm/tog-specfiles/tog-pegasus-build.spec
#
export PEGASUS_ROOT=%PEGASUS_RPM_ROOT
export PEGASUS_HOME=%PEGASUS_RPM_HOME
export PEGASUS_PLATFORM=%PEGASUS_HARDWARE_PLATFORM
export PEGASUS_ARCH_LIB=%PEGASUS_ARCH_LIB
export PEGASUS_ENVVAR_FILE=$PEGASUS_ROOT/env_var_Linux.status

export OPENSSL_HOME=%OPENSSL_HOME
export OPENSSL_BIN=%OPENSSL_BIN
export LD_LIBRARY_PATH=$PEGASUS_HOME/lib
export PATH=$PEGASUS_HOME/bin:$PATH

export PEGASUS_EXTRA_C_FLAGS="$RPM_OPT_FLAGS -g -Wno-unused"
export PEGASUS_EXTRA_CXX_FLAGS="$PEGASUS_EXTRA_C_FLAGS"
export PEGASUS_EXTRA_LINK_FLAGS="$RPM_OPT_FLAGS"
#export PEGASUS_EXTRA_LINK_FLAGS="$RPM_OPT_FLAGS -g -pie -Wl,-z,relro,-z,now,-z,nodlopen,-z,noexecstack"

%if %{PEGASUS_BUILD_TEST_RPM}
export PEGASUS_DISPLAYCONSUMER_DIR=%PEGASUS_VARDATA_DIR/log
%endif

make -f $PEGASUS_ROOT/Makefile.Release create_ProductVersionFile
make -f $PEGASUS_ROOT/Makefile.Release create_CommonProductDirectoriesInclude
make -f $PEGASUS_ROOT/Makefile.Release create_ConfigProductDirectoriesInclude
make %{?_smp_mflags} -f $PEGASUS_ROOT/Makefile.Release all
chmod 777 %PEGASUS_RPM_HOME
make -f $PEGASUS_ROOT/Makefile.Release repository
#
# End of section pegasus/rpm/tog-specfiles/tog-pegasus-build.spec

%install
# Start of section pegasus/rpm/tog-specfiles/tog-pegasus-install.spec
#
export PEGASUS_ROOT=%PEGASUS_RPM_ROOT
export PEGASUS_HOME=%PEGASUS_RPM_HOME
export PEGASUS_PLATFORM=%PEGASUS_HARDWARE_PLATFORM
export PEGASUS_ARCH_LIB=%PEGASUS_ARCH_LIB
export PEGASUS_ENVVAR_FILE=$PEGASUS_ROOT/env_var_Linux.status

export OPENSSL_BIN=%OPENSSL_BIN
export LD_LIBRARY_PATH=$PEGASUS_HOME/lib
export PATH=$PEGASUS_HOME/bin:$PATH

export PEGASUS_STAGING_DIR=$RPM_BUILD_ROOT

%if %{PEGASUS_BUILD_TEST_RPM}
make -f $PEGASUS_ROOT/Makefile.Release stage \
    PEGASUS_STAGING_DIR=$PEGASUS_STAGING_DIR \
    PEGASUS_BUILD_TEST_RPM=%{PEGASUS_BUILD_TEST_RPM}
%else
make -f $PEGASUS_ROOT/Makefile.Release stage \
    PEGASUS_STAGING_DIR=$PEGASUS_STAGING_DIR
%endif

[ "$PEGASUS_HOME" != "/" ] && [ -d $PEGASUS_HOME ] && rm -rf $PEGASUS_HOME;
#
# End of section pegasus/rpm/tog-specfiles/tog-pegasus-install.spec

%clean
# Start of section pegasus/rpm/tog-specfiles/tog-pegasus-clean.spec
#
[ "$RPM_BUILD_ROOT" != "/" ] && rm -rf $RPM_BUILD_ROOT;
[ "${RPM_BUILD_DIR}" != "/" ] && rm -rf ${RPM_BUILD_DIR}/%{name}-%{version};
#
# End of section pegasus/rpm/tog-specfiles/tog-pegasus-clean.spec

%pre
# Start of section pegasus/rpm/tog-specfiles/tog-pegasus-pre.spec
#           install   remove   upgrade  reinstall
# pre          1        -         2         2
#
# If it exists, the active OpenPegasus Repository
#    directory, %PEGASUS_REPOSITORY_DIR is renamed
#    to %PEGASUS_PREV_REPOSITORY_DIR.  If
#    %PEGASUS_PREV_REPOSITORY_DIR already exists,
#    it is saved as an archive file.

if [ -d %PEGASUS_REPOSITORY_DIR"/root#PG_Internal" ]; then
   #
   # Save the current repository to prev_repository
   #
   if [ -d %PEGASUS_PREV_REPOSITORY_DIR ]; then
        /bin/tar -C %PEGASUS_REPOSITORY_PARENT_DIR -cf \
            %PEGASUS_PREV_REPOSITORY_DIR`date '+%Y-%m-%d-%s.%N'`.tar \
            %PEGASUS_PREV_REPOSITORY_DIR_NAME
        rm -rf %PEGASUS_PREV_REPOSITORY_DIR
   fi
   /bin/mv %PEGASUS_REPOSITORY_DIR %PEGASUS_PREV_REPOSITORY_DIR
fi
if [ $1 -gt 0 ]; then
   #  Create the 'pegasus' user and group:
   /usr/sbin/groupadd pegasus > /dev/null 2>&1 || :;
   /usr/sbin/useradd -c "tog-pegasus OpenPegasus WBEM/CIM services" \
        -g pegasus -s /sbin/nologin -r -d %PEGASUS_VARDATA_DIR pegasus \
         > /dev/null 2>&1 || :;
fi
if [ $1 -eq 2 ]; then
    rm -f %PEGASUS_LOCAL_DOMAIN_SOCKET_PATH;
    rm -f %PEGASUS_CIMSERVER_START_FILE;
    rm -f %CIMSERVER_LOCK_FILE;
fi
#
# End of section pegasus/rpm/tog-specfiles/tog-pegasus-pre.spec

# When Privilege Separation is enabled, create the 'cimsrvr' user and 
# 'cimsrvr' group which are used as the context of the cimservermain process
if [ $1 -gt 0 ]; then
    /usr/sbin/groupadd cimsrvr > /dev/null 2>&1 || :;
    /usr/sbin/useradd -c "tog-pegasus OpenPegasus WBEM/CIM services" \
        -g cimsrvr -s /sbin/nologin -r -d %PEGASUS_VARDATA_DIR cimsrvr \
        > /dev/null 2>&1 || :;
fi

%post
if [ $1 -eq 1 ]; then
   echo `date` > %PEGASUS_INSTALL_LOG 2>&1
   %define PEGASUS_CONFIG_DIR /etc/Pegasus
   %define PEGASUS_PEM_DIR /etc/Pegasus
   %define PEGASUS_SSL_CERT_FILE server.pem
   %define PEGASUS_SSL_KEY_FILE file.pem
   %define PEGASUS_SSL_TRUSTSTORE client.pem

   # Create Symbolic Links for SDK Libraries
   #
   ln -sf libpegclient.so.1 /usr/%PEGASUS_ARCH_LIB/libpegclient.so
   ln -sf libpegcommon.so.1 /usr/%PEGASUS_ARCH_LIB/libpegcommon.so
   ln -sf libpegprovider.so.1 /usr/%PEGASUS_ARCH_LIB/libpegprovider.so
   ln -sf libDefaultProviderManager.so.1 /usr/%PEGASUS_ARCH_LIB/libDefaultProviderManager.so
   ln -sf libCIMxmlIndicationHandler.so.1 /usr/%PEGASUS_ARCH_LIB/libCIMxmlIndicationHandler.so
   ln -sf libCMPIProviderManager.so.1 /usr/%PEGASUS_ARCH_LIB/libCMPIProviderManager.so
   ln -sf libsnmpIndicationHandler.so.1 /usr/%PEGASUS_ARCH_LIB/libsnmpIndicationHandler.so

   # Create Symbolic Links for Packaged Provider Libraries
   #
   ln -sf libComputerSystemProvider.so.1 /usr/%PEGASUS_ARCH_LIB/Pegasus/providers/libComputerSystemProvider.so
   ln -sf libOSProvider.so.1 /usr/%PEGASUS_ARCH_LIB/Pegasus/providers/libOSProvider.so
   ln -sf libProcessProvider.so.1 /usr/%PEGASUS_ARCH_LIB/Pegasus/providers/libProcessProvider.so


# Start of section pegasus/rpm/tog-specfiles/tog-pegasus-post.spec
#
#           install   remove   upgrade  reinstall
# %post        1        -         2         2
#
   export PEGASUS_ARCH_LIB=%PEGASUS_ARCH_LIB

   if [ -d %PEGASUS_PREV_REPOSITORY_DIR ]; then
       # Running Repository Upgrade utility
       %PEGASUS_SBIN_DIR/repupgrade %PEGASUS_PREV_REPOSITORY_DIR \
           %PEGASUS_REPOSITORY_DIR 2>>%PEGASUS_INSTALL_LOG
       /bin/tar -C %PEGASUS_REPOSITORY_PARENT_DIR -cf \
           %PEGASUS_PREV_REPOSITORY_DIR`date '+%Y-%m-%d-%s.%N'`.tar \
           %PEGASUS_PREV_REPOSITORY_DIR_NAME
       rm -rf %PEGASUS_PREV_REPOSITORY_DIR
   fi
   if [ $1 -eq 1 ]; then
%if %{AUTOSTART}
       /sbin/chkconfig --add tog-pegasus
%endif
   :;
   elif [ $1 -gt 0 ]; then
       /etc/init.d/tog-pegasus condrestart
   :;
   fi
#
# End of section pegasus/rpm/tog-specfiles/tog-pegasus-post.spec

elif [ $1 -gt 1 ]; then
   if [ -d %PEGASUS_PREV_REPOSITORY_DIR ]; then
     # Running Repository Upgrade utility
     %PEGASUS_SBIN_DIR/repupgrade 2>>%PEGASUS_INSTALL_LOG
   fi
   # Check if the cimserver is running
   isRunning=`ps -el | grep cimserver | grep -v "grep cimserver"`
   if [ "$isRunning" ]; then
       /etc/init.d/tog-pegasus stop
   fi
   if [ -f %PEGASUS_TRACE_FILE_PATH ]; then
     /bin/mv %PEGASUS_TRACE_FILE_PATH %PEGASUS_TRACE_FILE_PATH-`date '+%Y-%m-%d-%R'`
   fi
   if [ "$isRunning" ]; then
       /etc/init.d/tog-pegasus start
   fi
fi

%preun
# Start of section pegasus/rpm/tog-specfiles/tog-pegasus-preun.spec
#           install   remove   upgrade  reinstall
#  preun       -        0         1         -
if [ $1 -eq 0 ]; then
   # Check if the cimserver is running
   isRunning=`ps -el | grep cimserver |  grep -v "grep cimserver"`
   if [ "$isRunning" ]; then
      %PEGASUS_SBIN_DIR/cimserver -s
   fi 
   /sbin/chkconfig --del tog-pegasus;
   rm -f %PEGASUS_VARDATA_DIR/cimserver_current.conf;
   rm -f %PEGASUS_INSTALL_LOG;
   [ -d %PEGASUS_REPOSITORY_DIR ]  && rm -rf %PEGASUS_REPOSITORY_DIR;
   [ -d %PEGASUS_VARDATA_CACHE_DIR ]  && rm -rf %PEGASUS_VARDATA_CACHE_DIR;
   rm -f %PEGASUS_LOCAL_DOMAIN_SOCKET_PATH;
   rm -f %PEGASUS_CIMSERVER_START_FILE;
   rm -f %PEGASUS_CIMSERVER_START_LOCK_FILE;
fi
#
# End of section pegasus/rpm/tog-specfiles/tog-pegasus-preun.spec

%preun devel
make --directory /usr/share/Pegasus/samples -s clean

%if %{PEGASUS_BUILD_TEST_RPM}
%preun test
make --directory /usr/share/Pegasus/test -s unsetupTEST
[ -d /var/lib/Pegasus/testrepository ] &&  rm -rf /var/lib/Pegasus/testrepository;

%endif
%postun
if [ $1 -eq 0 ]; then
   export LC_ALL=C
fi
# Start of section pegasus/rpm/tog-specfiles/tog-pegasus-postun.spec
#
#           install   remove   upgrade  reinstall
#  postun      -        0         1         -
if [ $1 -eq 0 ]; then
   /usr/sbin/userdel pegasus > /dev/null 2>&1 || :;
   /usr/sbin/groupdel pegasus > /dev/null 2>&1 || :;
fi;
#
# End of section pegasus/rpm/tog-specfiles/tog-pegasus-postun.spec

# When Privilege Separation is enabled, delete the 'cimsrvr' user and 
# 'cimsrvr' group which are used as the context of the cimservermain process
if [ $1 -eq 0 ]; then
    /usr/sbin/userdel cimsrvr > /dev/null 2>&1 || :;
    /usr/sbin/groupdel cimsrvr > /dev/null 2>&1 || :;
fi

%files
%defattr(600, cimsrvr, cimsrvr, 700)
/var/lib/Pegasus/repository
%defattr(600, root, pegasus, 755)
%dir /usr/share/doc/tog-pegasus-2.7
%dir /usr/share/Pegasus
%dir /usr/share/Pegasus/scripts
%dir /usr/share/Pegasus/mof
%dir /usr/share/Pegasus/mof/CIM29
%dir /usr/share/Pegasus/mof/Pegasus
%dir /var/lib/Pegasus
%dir /var/lib/Pegasus/cache
%dir /var/lib/Pegasus/log
%dir /var/lib/Pegasus/cache/localauth
%dir /usr/%PEGASUS_ARCH_LIB/Pegasus 
%dir /usr/%PEGASUS_ARCH_LIB/Pegasus/providers 

%dir %attr(755, root, pegasus) /etc/Pegasus
%dir %attr(755, cimsrvr, cimsrvr) /var/run/tog-pegasus
%dir %attr(1755,cimsrvr,cimsrvr) /var/run/tog-pegasus/socket
%dir %attr(1777,root,pegasus) /var/lib/Pegasus/cache/trace

/usr/share/Pegasus/mof/CIM29/*
/usr/share/Pegasus/mof/Pegasus/*

%config %attr(750,root,pegasus) /etc/init.d/tog-pegasus
%config(noreplace) %attr(644,  root, pegasus) /var/lib/Pegasus/cimserver_planned.conf
%config(noreplace) /etc/Pegasus/access.conf
%config(noreplace) /etc/pam.d/wbem

%ghost %config(noreplace) /etc/Pegasus/ssl.cnf
%ghost %config(noreplace) /etc/Pegasus/client.pem
%ghost %config(noreplace) /etc/Pegasus/server.pem
%ghost %config(noreplace) /etc/Pegasus/file.pem
%ghost /var/lib/Pegasus/log/install.log

%attr(755,root,pegasus) /usr/sbin/*
%attr(755,root,pegasus) /usr/bin/*
%attr(755,root,pegasus) /usr/%PEGASUS_ARCH_LIB/*.so.1
%attr(755,root,pegasus) /usr/%PEGASUS_ARCH_LIB/Pegasus/providers/*.so.1
%attr(750,root,pegasus) /usr/share/Pegasus/scripts/*
%attr(644,root,pegasus) /usr/share/man/man1/*
%attr(644,root,pegasus) /usr/share/man/man8/*

%doc %attr(444,root,pegasus) /usr/share/doc/tog-pegasus-2.7/Admin_Guide_Release.pdf
%doc %attr(444,root,pegasus) /usr/share/doc/tog-pegasus-2.7/PegasusSSLGuidelines.htm
%doc %attr(444,root,pegasus) /usr/share/doc/tog-pegasus-2.7/license.txt
/usr/%PEGASUS_ARCH_LIB/libpegclient.so
/usr/%PEGASUS_ARCH_LIB/libpegcommon.so
/usr/%PEGASUS_ARCH_LIB/libpegprovider.so
/usr/%PEGASUS_ARCH_LIB/libDefaultProviderManager.so
/usr/%PEGASUS_ARCH_LIB/libCIMxmlIndicationHandler.so
/usr/%PEGASUS_ARCH_LIB/libCMPIProviderManager.so
/usr/%PEGASUS_ARCH_LIB/libsnmpIndicationHandler.so
/usr/%PEGASUS_ARCH_LIB/Pegasus/providers/libComputerSystemProvider.so
/usr/%PEGASUS_ARCH_LIB/Pegasus/providers/libOSProvider.so
/usr/%PEGASUS_ARCH_LIB/Pegasus/providers/libProcessProvider.so

%files devel
%defattr(644,root,pegasus,755)
/usr/share/Pegasus/samples
/usr/include/Pegasus
/usr/share/doc/tog-pegasus-2.7/*
/usr/share/Pegasus/html
%attr(755,root,pegasus) /usr/%PEGASUS_ARCH_LIB/Pegasus/providers/*.so

%if %{PEGASUS_BUILD_TEST_RPM}
%files test
%defattr(-,root,pegasus,-)
/usr/share/Pegasus/test
%defattr(600,cimsrvr, cimsrvr,700)
/var/lib/Pegasus/testrepository
%endif
