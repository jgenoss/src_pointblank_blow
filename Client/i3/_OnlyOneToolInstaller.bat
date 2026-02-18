@echo off
setlocal enabledelayedexpansion

set _PBTOOL_SVN_COMMIT_=0
if "%~1"=="commit" set _PBTOOL_SVN_COMMIT_=1

rem if %_PBTOOL_SVN_COMMIT_% equ 1 (
rem   svn commit --targets PBToolSetupAddingFiles.txt -m "Auto commit ToolInstaller files.." 
rem   if not %ERRORLEVEL% equ 0 (
rem     echo "ERR - failed to svn commit for toolfiles"
rem     goto exitWithErr
rem   )
rem )

echo NSISNAME : %1
echo EXENAME : %2


set NSISNAME=%1
set EXENAME=%2


if %_PBTOOL_SVN_COMMIT_% equ 1 (

svnversion -c .\ > .\bin\toolpatchnote\_svnInfo_.txt
set BIN_SVNREV=0
for /f "tokens=*" %%a in ('svnversion.exe -c .\') do set BIN_SVNREV=%%a

svn commit --depth files ".\bin\toolpatchnote" -m "Auto commit ToolInstaller(ToolPatchInfo) "
  if not %ERRORLEVEL% equ 0 (
      echo "ERR - failed to svn commt ToolPatchInfo"
      goto exitWithErr
  )
)

set _PBTOOL_MAKEINSTALLER_="C:\Program Files\NSIS\makensis.exe"
if exist %_PBTOOL_MAKEINSTALLER_% goto DoMakeInstaller
set _PBTOOL_MAKEINSTALLER_="C:\Program Files (x86)\NSIS\makensis.exe"
if exist %_PBTOOL_MAKEINSTALLER_% goto DoMakeInstaller
echo.
echo "**************** ERROR **************"
echo "Err - failed to find installer maker ( %_PBTOOL_MAKEINSTALLER_% ) "
echo "Err - Please execute installer in i3\NsisToolsSetup\nsis-2.45-setup.exe"
echo "**************** ERROR **************"
endlocal
exit /b 1

:DoMakeInstaller
call %_PBTOOL_MAKEINSTALLER_% "%cd%\Client\i3\NsisToolsSetup\%NSISNAME%"


if not %ERRORLEVEL% equ 0 (
  echo "ERR - failed to make toolInstaller"
  goto exitWithErr
)

if %_PBTOOL_SVN_COMMIT_% equ 1 (
  svn commit "%cd%\NsisToolsSetup\%EXENAME%" -m "Auto commit %EXENAME%(SVNREV:%BIN_SVNREV%)"
  if not %ERRORLEVEL% equ 0 (
    echo "ERR - failed to svn commit %EXENAME%"
    goto exitWithErr
  )
)

endlocal
exit /b 0

:exitWithErr
endlocal
exit /b 1

