Name:           ulxmlrpcpp
Version:        1.5.0
Release:        0
Epoch:          0
Summary:        Ultra Lightweight Xml Rpc Library for C++
Group:          System Environment/Libraries
License:        LGPL
URL:            http://ulxmlrpcpp.sourceforge.net/
Source:         http://prdownloads.sourceforge.net/ulxmlrpcpp/%{name}-%{version}.tar.gz
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildRequires:  expat-devel openssl-devel
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description
ulxmlrpcpp is a library to perform calls via XML-RPC in an object oriented approach.
Unlike other implementations, it does not insist on HTTP for transportation but might
also accept other ways (e.g., a simple serial line with a user-defined protocol).

%package        devel
Summary:        Development tools for ulxmlrpcpp library
Group:          Development/Libraries
Requires:       %{name} = %{epoch}:%{version}-%{release}

%description    devel
ulxmlrpcpp is a library to perform calls via XML-RPC in an object oriented approach.
Unlike other implementations, it does not insist on HTTP for transportation but might
also accept other ways (e.g., a simple serial line with a user-defined protocol).

If you want to develop programs which will use ulxmlrpcpp library,
you should install ulxmlrpcpp-devel. You'll also need to install the ulxmlrpcpp package.

%package        utils
Summary:        Utilities for the ulxmlrpcpp library
Group:          Applications/System
Requires:       %{name} = %{epoch}:%{version}-%{release}

%description    utils
ulxmlrpcpp is a library to perform calls via XML-RPC in an object oriented approach.
Unlike other implementations, it does not insist on HTTP for transportation but might
also accept other ways (e.g., a simple serial line with a user-defined protocol).

This package contains utilities from ulxmlrpcpp library.


%prep
%setup -q

%build
%configure
make %{?_smp_mflags}


%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT
find $RPM_BUILD_ROOT -type f -name "*.la" -exec rm -f {} ';'


%clean
rm -rf $RPM_BUILD_ROOT


%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig


%files
%defattr(-,root,root,-)
%doc AUTHORS COPYING ChangeLog README TODO
%{_libdir}/*.so.*

%files devel
%defattr(-,root,root,-)
%{_includedir}/%{name}
%{_libdir}/*.a
%{_libdir}/*.so

%files utils
%defattr(-,root,root,-)
%{_bindir}/*
%{_datadir}/%{name}


%changelog
* Wed Jun 29 2005 Olexander Shtepa <isk@ecommerce.com> - 0:1.5.0
- New upstream version.

* Tue Jun 21 2005 Olexander Shtepa <isk@ecommerce.com> - 0:1.5.0-rc4.fc3.ghcp
- New upstream version.
- Remove html docs

* Mon Jan 31 2005 Olexander Shtepa <isk@ecommerce.com> - 0:1.4.7-0.fc3.ghcp
- Initial RPM release.
