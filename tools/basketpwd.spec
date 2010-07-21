Name: basketpwd
Version: 0.3.6
Release: 1%{?dist}
Summary: Basket of passwords
Summary(ru): Корзинка паролей
Group: Applications/Security
License: GPLv2
Source0: %{name}-%{version}.tar.bz2
BuildRoot: %{_tmppath}/%{name}-%{version}
Vendor: Atis Service Ltd.
URL: http://www.atisserv.ru/program.php
Packager: Alexei V. Panov <avpanov@atisserv.ru>

%if %{defined fedora}
Requires: qt >= 4.4.1 libgcrypt >= 1.4.0
BuildRequires: qt-devel >= 4.4.1 libgcrypt-devel >= 1.4.0 gcc-c++
%endif
%if %{defined suse_version}
Source1: %{name}.desktop
Requires: libqt4 >= 4.4.1 libgcrypt11 >= 1.4.0
BuildRequires: libqt4-devel >= 4.4.1 libgcrypt-devel >= 1.4.0 update-desktop-files
%endif

%description 
Basket of passwords
The program for storage and information management about passwords.

%description -l ru
Корзинка паролей
Программа для хранения и управления информацией о паролях.

%prep
%setup -q -n %{name}-%{version}

%build
%{_libdir}/qt4/bin/qmake -spec linux-g++
make release

%install
mkdir -p $RPM_BUILD_ROOT/%{_bindir}
mkdir -p $RPM_BUILD_ROOT/%{_datadir}
mkdir -p $RPM_BUILD_ROOT/%{_datadir}/doc/%{name}-%{version}
mkdir -p $RPM_BUILD_ROOT/%{_datadir}/applications
mkdir -p $RPM_BUILD_ROOT/%{_datadir}/pixmaps
install bin/basketpwd $RPM_BUILD_ROOT/%{_bindir}/
install -m 644 ChangeLog.txt $RPM_BUILD_ROOT/%{_datadir}/doc/%{name}-%{version}/ChangeLog-ru.txt
install -m 644 images/prog.png $RPM_BUILD_ROOT/%{_datadir}/pixmaps/%{name}.png

%if %{defined fedora}
install -m 644 tools/%{name}.desktop $RPM_BUILD_ROOT/%{_datadir}/applications
%endif
%if %{defined suse_version}
%suse_update_desktop_file -i %{name} Utility
%endif

%files
%defattr(-,root,root)
%{_bindir}/basketpwd
%dir %{_datadir}/doc/%{name}-%{version}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/pixmaps/basketpwd.png
%{_datadir}/doc/%{name}-%{version}/ChangeLog-ru.txt

%clean
make clean
rm -rf $RPM_BUILD_ROOT

%changelog
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
