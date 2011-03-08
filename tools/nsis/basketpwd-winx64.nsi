!include "MUI.nsh"
!include x64.nsh

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

Function .onInit

        ${If} ${RunningX64}
        	${EnableX64FSRedirection}
        ${else}
        	MessageBox MB_OK "Sorry this application runs only on x64 machines"
        	Abort
        ${EndIf}
        StrCpy '$INSTDIR' '$PROGRAMFILES64\$(TITLE_SecMain)'
FunctionEnd

Function CheckVCRedist
	Push $R0
	ClearErrors
	ReadRegDword $R0 HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{DA5E371C-6333-3D8A-93A4-6FD5B20BCC6E}" "Version"
	
	; if VS 2005+ redist SP1 not installed, install it
	IfErrors 0 VSRedistInstalled
	StrCpy $R0 "-1"
	VSRedistInstalled:
		Pop $R0
		Exch $R0
FunctionEnd

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
	File "/media/share-disk/devel/bins/basketpwd/shared64/basketpwd.exe"
	File "/media/share-disk/devel/bins/basketpwd/shared64/README"
	File "/media/share-disk/devel/bins/basketpwd/shared64/ChangeLog.txt"
	File "/media/share-disk/devel/bins/basketpwd/shared64/basketpwd.exe.manifest"
	; File "/media/share-disk/devel/bins/basketpwd/shared64/vcredist_x64.exe"
	WriteUninstaller "$INSTDIR\Uninstall.exe"
	WriteRegStr HKCU "Software\$(CompanyName)\$(TITLE_SecMain)" "" $INSTDIR
	; ExecWait '"$INSTDIR\${DLLMSVC10}" /q:a /c:"vcredist_x64.exe /q:a /c:""msiexec /i vcredist.msi /qb!"" "'
SectionEnd

Section $(TITLE_SecQt) SecQt
	SetOutPath "$INSTDIR"
	File "/media/share-disk/devel/bins/basketpwd/shared64/QtCore4.dll"
	File "/media/share-disk/devel/bins/basketpwd/shared64/QtGui4.dll"
	File "/media/share-disk/devel/bins/basketpwd/shared64/QtXml4.dll"
SectionEnd

Section "Microsoft Visual C++ 2010 Redist (required)"
	SectionIn RO
	SetOutPath $TEMP
	File "/media/share-disk/devel/bins/basketpwd/shared64/vcredist_x64.exe"
	Call CheckVCRedist
	StrCmp $R0 "-1" installRedist
		DetailPrint "Visual C++ 2010 Redistributable already installed, skipping..."
		Goto skipRedist
	installRedist:
		ExecWait "$TEMP\vcredist_x64.exe"
	SkipRedist:
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
;	Delete "$SMPROGRAMS\$(TITLE_SecMain)\$(TITLE_SecMain).lnk"
;	Delete "$SMPROGRAMS\$(TITLE_SecMain)\$(UninstallShortCut).lnk"
	RMDir "$SMPROGRAMS\$(TITLE_SecMain)"
SectionEnd

