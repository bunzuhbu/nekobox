Name "nekoray"

!ifdef OUTFILE
    OutFile "$OUTFILE.exe"
!else
    OutFile "nekoray_setup.exe"
!endif
InstallDir $APPDATA\nekoray
RequestExecutionLevel user

!include MUI2.nsh
!define MUI_ICON "res\nekoray.ico"
!define MUI_ABORTWARNING
!define MUI_WELCOMEPAGE_TITLE "Welcome to nekoray Installer"
!define MUI_WELCOMEPAGE_TEXT "This wizard will guide you through the installation of nekoray."
!define MUI_FINISHPAGE_RUN "$INSTDIR\nekoray.exe"
!define MUI_FINISHPAGE_RUN_TEXT "Launch nekoray"
!addplugindir .\script\

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_LANGUAGE "English"

UninstallText "This will uninstall nekoray. Do you wish to continue?"
UninstallIcon "res\nekoray_del.ico"

Section "Install"
  SetOutPath "$INSTDIR"
  SetOverwrite on

  !ifdef DIRECTORY
    File /r ".\deployment\$DIRECTORY\*"
  !else
    File /r ".\deployment\windows64\*"
  !endif



  CreateShortcut "$desktop\nekoray.lnk" "$instdir\nekoray.exe"
  CreateShortcut "$SMPROGRAMS\nekoray.lnk" "$INSTDIR\nekoray.exe" "" "$INSTDIR\nekoray.exe" 0
  
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\nekoray" "DisplayName" "nekoray"
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\nekoray" "UninstallString" "$INSTDIR\uninstall.exe"
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\nekoray" "InstallLocation" "$INSTDIR"
  WriteRegDWORD HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\nekoray" "NoModify" 1
  WriteRegDWORD HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\nekoray" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
SectionEnd

Section "Uninstall"

  Delete "$SMPROGRAMS\nekoray.lnk"
  Delete "$desktop\nekoray.lnk"
  RMDir "$SMPROGRAMS\nekoray"

  RMDir /r "$INSTDIR"

  Delete "$INSTDIR\uninstall.exe"

  DeleteRegKey HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\nekoray"
SectionEnd
