
Function GetParameterValue
  Exch $R0  ; get the top of the stack(default parameter) into R0
  Exch      ; exchange the top of the stack(default) with
            ; the second in the stack(parameter to search for)
  Exch $R1  ; get the top of the stack(search parameter) into $R1

  ;Preserve on the stack the registers used in this function
  Push $R2
  Push $R3
  Push $R4
  Push $R5

  Strlen $R2 $R1+2    ; store the length of the search string into R2

  Call GetParameters  ; get the command line parameters
  Pop $R3             ; store the command line string in R3

  # search for quoted search string
  StrCpy $R5 '"'      ; later on we want to search for a open quote
  Push $R3            ; push the 'search in' string onto the stack
  Push '"/$R1='       ; push the 'search for'
  Call StrStr         ; search for the quoted parameter value
  Pop $R4
  StrCpy $R4 $R4 "" 1   ; skip over open quote character, "" means no maxlen
  StrCmp $R4 "" "" next ; if we didn't find an empty string go to next

  # search for non-quoted search string
  StrCpy $R5 ' '      ; later on we want to search for a space since we
                      ; didn't start with an open quote '"' we shouldn't
                      ; look for a close quote '"'
  Push $R3            ; push the command line back on the stack for searching
  Push '/$R1='        ; search for the non-quoted search string
  Call StrStr
  Pop $R4

  ; $R4 now contains the parameter string starting at the search string,
  ; if it was found
next:
  StrCmp $R4 "" check_for_switch ; if we didn't find anything then look for
                                 ; usage as a command line switch
  # copy the value after /$R1= by using StrCpy with an offset of $R2,
  # the length of '/OUTPUT='
  StrCpy $R0 $R4 "" $R2  ; copy commandline text beyond parameter into $R0
  # search for the next parameter so we can trim this extra text off
  Push $R0
  Push $R5            ; search for either the first space ' ', or the first
                      ; quote '"'
                      ; if we found '"/output' then we want to find the
                      ; ending ", as in '"/output=somevalue"'
                      ; if we found '/output' then we want to find the first
                      ; space after '/output=somevalue'
  Call StrStr         ; search for the next parameter
  Pop $R4
  StrCmp $R4 "" done  ; if 'somevalue' is missing, we are done
  StrLen $R4 $R4      ; get the length of 'somevalue' so we can copy this
                      ; text into our output buffer
  StrCpy $R0 $R0 -$R4 ; using the length of the string beyond the value,
                      ; copy only the value into $R0
  goto done           ; if we are in the parameter retrieval path skip over
                      ; the check for a command line switch

; See if the parameter was specified as a command line switch, like '/output'
check_for_switch:
  Push $R3            ; push the command line back on the stack for searching
  Push '/$R1'         ; search for the non-quoted search string
  Call StrStr
  Pop $R4
  StrCmp $R4 "" done  ; if we didn't find anything then use the default
  StrCpy $R0 ""       ; otherwise copy in an empty string since we found the
                      ; parameter, just didn't find a value

done:
  Pop $R5
  Pop $R4
  Pop $R3
  Pop $R2
  Pop $R1
  Exch $R0 ; put the value in $R0 at the top of the stack
FunctionEnd



Push "OUTPUT"         ; push the search string onto the stack
Push "nekoray_setup"   ; push a default value onto the stack
Call GetParameterValue
Pop $2


Name "nekoray"
OutFile "$2.exe"
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

!macro AbortOnRunningApp EXEName
  killModule:
  FindProcDLL::FindProc ${EXEName}
  Pop $R0
  IntCmp $R0 1 0 notRunning
    FindProcDLL::KillProc ${EXEName}
    Sleep 1000
    Goto killModule
  notRunning:
!macroend

Section "Install"
  SetOutPath "$INSTDIR"
  SetOverwrite on

  !insertmacro AbortOnRunningApp "$INSTDIR\nekoray.exe"


  Push "DIRECTORY"
  Push "windows64"
  Call GetParameterValue
  Pop $2


  File /r ".\deployment\$2\*"

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

  !insertmacro AbortOnRunningApp "$INSTDIR\nekoray.exe"

  Delete "$SMPROGRAMS\nekoray.lnk"
  Delete "$desktop\nekoray.lnk"
  RMDir "$SMPROGRAMS\nekoray"

  RMDir /r "$INSTDIR"

  Delete "$INSTDIR\uninstall.exe"

  DeleteRegKey HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\nekoray"
SectionEnd
