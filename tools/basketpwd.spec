# if configure_method is 0 - using qmake or
#    configure_method is 1 - using cmake

%define configure_method 1

%if 0%{?fedora} == 0
%define configure_method 0
%endif

%define qmake /usr/bin/qmake-qt4

%if (0%{?fedora} == 0) || (0%{?rhel} == 0)
%define qmake %{_libdir}/qt4/bin/qmake -spec linux-g++
%endif

Name:			basketpwd
Version:		0.4.5
Release:		1%{?dist}
Summary:		Basket of passwords
Summary(ru):		Корзинка паролей

%if (0%{?fedora} >0) && (0%{?rhel} > 0)
Group:			Applications/System
%else
Group:			Productivity/Security
%endif

License:		GPLv2
Source0:		http://cloud.github.com/downloads/elemc/%{name}/%{name}-%{version}.tar.bz2
BuildRoot:		%{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
URL:			http://github.com/elemc/basketpwd

Requires:		qt openssl

BuildRequires:		openssl-devel 
BuildRequires:		gcc-c++ 
BuildRequires:		desktop-file-utils

%if 0%{?rhel} == 6
BuildRequires:		qt4-devel
%endif

%if (0%{?fedora} < 14) && (0%{?fedora} > 0)
BuildRequires:		qt4-devel
%endif

%if 0%{?fedora} >= 14
BuildRequires:		qt-devel
%endif

%if (0%{?fedora} == 0) && (0%{?rhel} == 0)
BuildRequires:	     	qt-devel
%endif

%if 0%{?fedora} > 0
BuildRequires:		cmake
%endif

%description 
Basket of passwords
The program for storage and information management about passwords.

%description -l ru
Корзинка паролей
Программа для хранения и управления информацией о паролях.

%prep
%setup -q

%build
mkdir build-cmake
pushd build-cmake
%if 0%{?configure_method} > 0
%cmake ..
%else
%{qmake} %{_builddir}/%{name}-%{version}/basketpwd.pro
%endif
make %{?_smp_mflags}
popd

%install
rm -rf $RPM_BUILD_ROOT

pushd build-cmake
%if 0%{?configure_method} > 0
make install DESTDIR=$RPM_BUILD_ROOT
%else
make install INSTALL_ROOT=$RPM_BUILD_ROOT
%endif
popd
desktop-file-install --dir=${RPM_BUILD_ROOT}%{_datadir}/applications tools/%{name}.desktop
desktop-file-validate %{buildroot}/%{_datadir}/applications/%{name}.desktop

%files
%defattr(-,root,root)
%{_bindir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/pixmaps/%{name}.png
%doc ChangeLog.txt README

%clean
pushd build-cmake
make clean
popd
rm -rf $RPM_BUILD_ROOT

%changelog
* Thu Sep  8 2011 Alexei Panov <elemc AT atisserv DOT ru> - 0.4.5-1
- New version 0.4.5 (see ChangeLog.txt)

* Thu Aug 11 2011 Alexei Panov <elemc AT atisserv DOT ru> - 0.4.4-2
- Change build dir for cmake

* Sun Jul 17 2011 Alexei Panov <avpanov@atisserv.ru> - 0.4.4-1
- new release
* Mon Mar 21 2011 Alexei Panov <avpanov@atisserv.ru> - 0.4.3-2
- fix source in spec to url for koji
- move package to russianfedora repository
* Thu Dec 23 2010 Alexei Panov <avpanov@atisserv.ru> - 0.4.3-1
- fix checkbox in settings dialog
- changed default folder for open/save file dialog
* Sun Dec  5 2010 Alexei Panov <avpanov@atisserv.ru> - 0.4.2-2
- fix crashing qstrcpy
* Fri Dec  3 2010 Alexei Panov <avpanov@atisserv.ru> - 0.4.2-1
- change crypt library from libgcrypt to openssl
- little GUI changes
* Sat Nov 20 2010 Alexei Panov <avpanov@atisserv.ru> - 0.4.1-2
- add cmake rules
- change spec for use cmake
* Mon Nov  1 2010 Alexei Panov <avpanov@atisserv.ru> - 0.4.1-1
- Litle changes in code
- add style change in menu
* Fri Sep 24 2010 Alexei Panov <avpanov@atisserv.ru> - 0.4.0-1
- Very big changes, application now use model/view
* Sun Aug  8 2010 Alexei Panov <avpanov@atisserv.ru> - 0.3.6-3
- Litle changes of spec-file
* Wed Jul 21 2010 Alexei Panov <avpanov@atisserv.ru> - 0.3.6-1
- Fixup version number on GUI
* Wed Jul 21 2010 Alexei Panov <avpanov@atisserv.ru> - 0.3.6-1
- Fixup add folder if cursor stay on top level folder
* Mon Mar 29 2010 Alexei Panov <avpanov@atisserv.ru> - 0.3.5-1
- Application translation on English language is changed
- Add thankfulness to About
* Wed Mar 10 2010 Alexei Panov <avpanov@atisserv.ru> - 0.3.4-3
- Rebuild for f13
* Wed Mar  3 2010 Alexei Panov <avpanov@atisserv.ru> - 0.3.4-2
- Fix SPEC file
* Sun Feb 28 2010 Alexei Panov <avpanov@atisserv.ru> - 0.3.4-1
- New version 0.3.4
- Fixed SPEC file
* Thu Jan 21 2010 Alexei Panov <avpanov@atisserv.ru>
- Fixed a phrase in "Settings", a spelling mistake
* Mon Jan 18 2010 Alexei Panov <avpanov@atisserv.ru>
- Works correctly with folders / directories in the list of passwords.
- Added a new item in the "Options" - "When you close the program is minimized to tray", which does not allow to close the program, except through the event "Close"
* Thu Dec 10 2009 Alexei Panov <avpanov@atisserv.ru>
- Fixed nedorobotka in the menu "Copy name (login)"
* Wed Dec 9 2009 Alexei Panov <avpanov@atisserv.ru>
- Added a new item in the Edit menu and context menu "Copy name (login)", which performs the backup name (login) to the clipboard
* Sat Nov 28 2009 Alexei Panov <avpanov@atisserv.ru>
- Added ability to run the program minimized in system tray this requires to add to the list of arguments run the program - tray-icon or-ti
* Wed Nov 11 2009 Alexei Panov <avpanov@atisserv.ru>
- Modified SPEC file.
- Rebuilt SRPMS package that was "broken".
- Changed file basketpwd.desktop, for the correct location references in the application menu.
- Created a static build for Enterprise Linux 5
* Fri Nov 6 2009 Alexei Panov <avpanov@atisserv.ru>
- Modified SPEC file to better reflect the standards
