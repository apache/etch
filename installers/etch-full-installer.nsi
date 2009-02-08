# Licensed to the Apache Software Foundation (ASF) under one   *
# or more contributor license agreements.  See the NOTICE file *
# distributed with this work for additional information        *
# regarding copyright ownership.  The ASF licenses this file   *
# to you under the Apache License, Version 2.0 (the            *
# "License"); you may not use this file except in compliance   *
# with the License.  You may obtain a copy of the License at   *
#                                                              *
#   http://www.apache.org/licenses/LICENSE-2.0                 *
#                                                              *
# Unless required by applicable law or agreed to in writing,   *
# software distributed under the License is distributed on an  *
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY       *
# KIND, either express or implied.  See the License for the    *
# specific language governing permissions and limitations      *
# under the License.                                           *
!define PRODUCT_NAME "Apache Etch"
!ifndef PRODUCT_VERSION
    !define PRODUCT_VERSION "1.0"
!endif
!define PRODUCT_PUBLISHER "Apache Software Foundation"
!define PRODUCT_WEB_SITE "http://incubator.apache.org/projects/etch.html"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}-${PRODUCT_VERSION}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"
!ifndef ROOT_DIRECTORY
    !define ROOT_DIRECTORY "..\target\Installers\dist"
!endif
!ifndef OUT_FILE
    !define OUT_FILE "apache-etch-${PRODUCT_VERSION}-win32-setup.exe"
!endif

!include WinMessages.nsh

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "${OUT_FILE}"
LoadLanguageFile "${NSISDIR}\Contrib\Language files\English.nlf"
InstallDir "$PROGRAMFILES\${PRODUCT_PUBLISHER}\apache-etch-${PRODUCT_VERSION}"
Icon "${NSISDIR}\Contrib\Graphics\Icons\pixel-install.ico"
UninstallIcon "${NSISDIR}\Contrib\Graphics\Icons\pixel-uninstall.ico"
DirText "Setup will install $(^Name) in the following folder.$\r$\n$\r$\nTo install in a different folder, click Browse and select another folder."
LicenseText "If you accept all the terms of the agreement, choose I Agree to continue. You must accept the agreement to install $(^Name)."
LicenseData "${ROOT_DIRECTORY}\LICENSE.txt"
#BrandingText "${PRODUCT_PUBLISHER}"
XPStyle on
ShowInstDetails show
ShowUnInstDetails show
#SetCompressor /SOLID zlib
LicenseBkColor ffffff
InstallColors 000000 ffffff

# Usage:
# Push "Substring"
# Push "Body"
# Call StrContains
# Pop 0/1
Function contains
  Exch $R1 ; Substring
  Exch 1
  Exch $R2 ; Body
  Push $R0 ; return value
  Exch 2
  Exch 1
  Push $R3 ; Length of Substring
  Push $R4 ; Length of Body
  Push $R5 ; i
  Push $R6 ; temp

  StrCpy $R0 ""
  StrLen $R3 $R1
  StrLen $R4 $R2
  StrCpy $R5 -1
  loop:
    IntOp $R5 $R5 + 1 ; ++i
    StrCpy $R6 $R2 $R3 $R5 ; temp = Body[i.. i + length of Substring]
    StrCmp $R6 $R1 found ; if temp = Substring -> found
    StrCmp $R5 $R4 done ; if i = length of body -> done
    GoTo loop
  found:
    StrCpy $R0 $R1
  done:
  Pop $R6
  Pop $R5
  Pop $R4
  Pop $R3
  Pop $R2
  Pop $R1
  Exch $R0
FunctionEnd

Section "CheckSection" SEC01
  ReadRegStr $R0 ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString"
  StrCmp $R0 "" +4 0 # Check if Etch is installed
    MessageBox MB_ICONINFORMATION|MB_OKCANCEL "${PRODUCT_NAME}-${PRODUCT_VERSION} is already installed. Do you wish to re-install?" /SD IDOK IDOK +3 IDCANCEL 0
      MessageBox MB_ICONINFORMATION|MB_OK "${PRODUCT_NAME} was not installed." /SD IDOK
      Quit
SectionEnd

Section "MainSection"
  SetOutPath $INSTDIR
  SetOverwrite try
  File /r "${ROOT_DIRECTORY}\*"
  WriteRegStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "ETCH_HOME" "$INSTDIR"
  ReadRegStr $R0 HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "Path"
  StrCpy $R1 "%ETCH_HOME%\bin"
  Push $R0
  Push $R1
  Call Contains
  Pop $R2
  StrCmp $R2 "" 0 +2
  WriteRegExpandStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "Path" "$R0;%ETCH_HOME%\bin"
  # This tells windows to refresh the environment variables
  SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000
SectionEnd

Section -AdditionalIcons
  SetOutPath $INSTDIR
#  WriteIniStr "$INSTDIR\etch-homepage.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
  CreateDirectory "$SMPROGRAMS\$(^Name)"
#  CreateShortCut "$SMPROGRAMS\$(^Name)\Apache Etch Homepage.lnk" "$INSTDIR\etch-homepage.url"
  CreateShortCut "$SMPROGRAMS\$(^Name)\Uninstall Apache Etch.lnk" "$INSTDIR\uninst.exe"
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd


Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully removed from your computer." /SD IDOK
FunctionEnd

Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name) and all of its components?" /SD IDOK IDYES +2
  Abort
FunctionEnd

# Usage:
# Push "String to cut"
# Push "Body text"
# Call Slice
# Pop $R0 ; "New String"
# Pop $R1 ; Cut done? 0/1
Function un.slice
  Exch $R0 ; input string
  Exch
  Exch $R1 ; to cut
  Push $R2
  Push $R3
  Push $R4
  Push $R5

  StrLen $R3 $R1
  StrCpy $R4 -1
  StrCpy $R5 0

  Loop:

    IntOp $R4 $R4 + 1
    StrCpy $R2 $R0 $R3 $R4
    StrCmp $R2 "" Done
    StrCmp $R2 $R1 0 Loop

    StrCpy $R5 1

    StrCmp $R4 0 0 +3
    StrCpy $R1 ""
    Goto +2
    StrCpy $R1 $R0 $R4
    StrLen $R2 $R0
    IntOp $R4 $R2 - $R4
    IntOp $R4 $R4 - $R3
    IntCmp $R4 0 0 0 +3
    StrCpy $R2 ""
    Goto +2
    StrCpy $R2 $R0 "" -$R4
    StrCpy $R0 $R1$R2

  Done:
  StrCpy $R1 $R5

  Pop $R5
  Pop $R4
  Pop $R3
  Pop $R2
  Exch $R1 ; slice? 0/1
  Exch
  Exch $R0 ; output string
FunctionEnd

Section Uninstall
  SetOutPath "$PROGRAMFILES\${PRODUCT_PUBLISHER}"
  RMDir /r $INSTDIR
  RMDir /r "$SMPROGRAMS\$(^Name)"
  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegValue HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "ETCH_HOME"
  # Replace Path with de-Etch'ed path
  ReadRegStr $R0 HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "Path"
  StrCpy $R1 ";%ETCH_HOME%\bin"
  Push $R1
  Push $R0
  Call un.slice
  Pop $R0
  WriteRegExpandStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "Path" "$R0"
  # This tells windows to refresh the environment variables
  SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000
  SetAutoClose true
SectionEnd
