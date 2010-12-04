!include "MUI.nsh"

Name $(TITLE_SecMain)
OutFile "basketpwd.exe"
InstallDir "$PROGRAMFILES\$(TITLE_SecMain)"
SetCompressor lzma

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_DIRECTORY
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "Russian"
!insertmacro MUI_LANGUAGE "English"

LangString TITLE_SecMain ${LANG_RUSSIAN} "Корзинка паролей"
LangString TITLE_SecMain ${LANG_ENGLISH} "Basket of Passwords"
LangString TITLE_SecQt   ${LANG_RUSSIAN} "Библиотеки Qt4"
LangString TITLE_SecQt   ${LANG_ENGLISH} "Qt4 libraries"
LangString CompanyName   ${LANG_RUSSIAN} "Атис Сервис"
LangString CompanyName   ${LANG_ENGLISH} "Atis Service"
LangString UninstallShortCut ${LANG_RUSSIAN} "Удаление программы"
LangString UninstallShortCut ${LANG_ENGLISH} "Uninstall"
LangString StartMenuShortCuts ${LANG_RUSSIAN} "Ярлыки в меню 'Пуск'"
LangString StartMenuShortCuts ${LANG_ENGLISH} "Start Menu Shortcuts"

Section $(TITLE_SecMain) SecMain
	SetOutPath "$INSTDIR"
	File "/home/alex/workspace/basketpwd/bin/basketpwd.exe"
	File "/home/alex/workspace/basketpwd/README"
	File "/home/alex/workspace/basketpwd/ChangeLog.txt"
	File "/home/alex/workspace/basketpwd/tools/basketpwd.exe.manifest"
        File "/usr/i686-pc-mingw32/sys-root/mingw/bin/libcrypto-10.dll"
        File "/usr/i686-pc-mingw32/sys-root/mingw/bin/libpng14-14.dll"
        File "/usr/i686-pc-mingw32/sys-root/mingw/bin/libgcc_s_sjlj-1.dll"
	File "/usr/i686-pc-mingw32/sys-root/mingw/bin/libstdc++-6.dll"
	File "/usr/i686-pc-mingw32/sys-root/mingw/bin/zlib1.dll"
	WriteUninstaller "$INSTDIR\Uninstall.exe"
	WriteRegStr HKCU "Software\$(CompanyName)\$(TITLE_SecMain)" "" $INSTDIR
SectionEnd

Section $(TITLE_SecQt) SecQt
	SetOutPath "$SYSDIR"
	File "/usr/i686-pc-mingw32/sys-root/mingw/bin/QtCore4.dll"
	File "/usr/i686-pc-mingw32/sys-root/mingw/bin/QtGui4.dll"
	File "/usr/i686-pc-mingw32/sys-root/mingw/bin/QtXml4.dll"
SectionEnd

Section $(StartMenuShortCuts) SecShortcuts
	CreateDirectory "$SMPROGRAMS\$(TITLE_SecMain)"
	CreateShortCut "$SMPROGRAMS\$(TITLE_SecMain)\$(UninstallShortCut).lnk" "$INSTDIR\Uninstall.exe" "" "$INSTDIR\Uninstall.exe" 0
	CreateShortCut "$SMPROGRAMS\$(TITLE_SecMain)\$(TITLE_SecMain).lnk" "$INSTDIR\basketpwd.exe" "" "$INSTDIR\basketpwd.exe" 0
SectionEnd

LangString DESC_SecMain ${LANG_RUSSIAN} "Основная программа"
LangString DESC_SecMain ${LANG_ENGLISH} "Main application"
LangString DESC_SecQt   ${LANG_RUSSIAN} "Библиотеки Qt4 - пакет необходимый для нормальной работы программы"
LangString DESC_SecQt   ${LANG_ENGLISH} "Qt4 libraries - the package which is needed for normal application work"
LangString DESC_SecShortcuts ${LANG_RUSSIAN} "Создать ярлыки в меню 'Пуск'"
LangString DESC_SecShortcuts ${LANG_ENGLISH} "Create Start Menu shortcuts"

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
	!insertmacro MUI_DESCRIPTION_TEXT ${SecMain} $(DESC_SecMain)
	!insertmacro MUI_DESCRIPTION_TEXT ${SecQt} $(DESC_SecQt)
	!insertmacro MUI_DESCRIPTION_TEXT ${SecShortcuts} $(DESC_SecShortcuts)
!insertmacro MUI_FUNCTION_DESCRIPTION_END
Section "Uninstall"
	Delete "$INSTDIR\*.*"
	RMDir "$INSTDIR"
	DeleteRegKey /ifempty HKCU "Software\$(CompanyName)\$(TITLE_SecMain)"
	Delete "$SMPROGRAMS\$(TITLE_SecMain)\$(TITLE_SecMain).lnk"
	Delete "$SMPROGRAMS\$(TITLE_SecMain)\$(UninstallShortCut).lnk"
	RMDir "$SMPROGRAMS\$(TITLE_SecMain)"
SectionEnd

