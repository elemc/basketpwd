Name:			basketpwd
Version:		0.4.3
Release:		2%{?dist}
Summary:		Basket of passwords
Summary(ru):		Корзинка паролей
Group:			Applications/System
License:		GPLv2
Source0:		http://cloud.github.com/downloads/elemc/%{name}/%{name}-%{version}.tar.bz2
BuildRoot:		%{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
URL:			http://github.com/elemc/basketpwd

Requires:		qt openssl
BuildRequires:		qt-devel openssl-devel gcc-c++ desktop-file-utils cmake

%description 
Basket of passwords
The program for storage and information management about passwords.

%description -l ru
Корзинка паролей
Программа для хранения и управления информацией о паролях.

%prep
%setup -q

%build
%cmake .
make VERBOSE=1 %{?_smp_mflags}

%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT
desktop-file-install --dir=${RPM_BUILD_ROOT}%{_datadir}/applications tools/%{name}.desktop
desktop-file-validate %{buildroot}/%{_datadir}/applications/%{name}.desktop

%check
ctest

%files
%defattr(-,root,root)
%{_bindir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/pixmaps/%{name}.png
%doc ChangeLog.txt README

%clean
make clean
rm -rf $RPM_BUILD_ROOT

%changelog
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
