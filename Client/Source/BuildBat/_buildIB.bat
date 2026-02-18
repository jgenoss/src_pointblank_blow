setlocal enabledelayedexpansion

set USAGE_MSG="usage) _buildIB.bat [ib|vc] [solution path] [build | rebuild] [configuration] [logfilename]"
set Builder="%ProgramFiles%\Xoreax\IncrediBuild\BuildConsole.exe"
set BuildSln=%2
set IBLogFileName=%~5

if "%~1"=="vc" (
   set Builder="devenv.com"
   goto DoBuild
)   

rem resolve location of incredibuild
if exist %Builder% goto DoBuild

rem try 32bit version of incredibuild
set Builder="%ProgramFiles(x86)%\Xoreax\IncrediBuild\BuildConsole.exe"
if exist %Builder% goto DoBuild
echo "Err - There is no incredibuild on this system, check install location.."
endlocal
exit /b 1

:DoBuild
if "%~1"=="vc" set BuildParam=%BuildSln% %3 %4 /out "%~5"
if "%~1"=="ib" set BuildParam=%BuildSln% %3 "%~4|win32" /log="%~5"

echo %Builder% %BuildParam%
call %Builder% %BuildParam%

if not %ERRORLEVEL% equ 0 (
  rem failed to build
  endlocal
  exit /b 1
)

:BuildSuccess
endlocal
echo --------------------------------------------------------
echo 복사
echo --------------------------------------------------------
rem SRC_DIR : %1
rem DEST_DIR : %2

rem 1) 엔진 파일 복사
call Copy_BIN_DBG.bat %I3_RELEASE_DLL_ROOT% %EXE_ROOT%
call Copy_BIN_DBG.bat %PHYS_X_RELEASE_DLL% %EXE_ROOT%
call Copy_BIN_DBG.bat %BUILD_RESULT_QA% %EXE_ROOT%


if "%SymbolBackup%" == "0" goto skip_backup

echo --------------------------------------------------------
echo 빌드 파일 백업
echo --------------------------------------------------------
rem BUILD_NAME : %1
rem SRC_DIR : %2

call TimeStamp.bat

call Backup_Build_Files.bat %BUILD_NAME% %I3_RELEASE_DLL_ROOT%
call Backup_Build_Files.bat %BUILD_NAME% %PHYS_X_RELEASE_DLL%
call Backup_Build_Files.bat %BUILD_NAME% %BUILD_RESULT_QA%

:skip_backup

exit /b 0

:ParamErrorExit
echo.
echo ============***************** ERROR *****************============
echo "ERR - Bad IncrediBuild Parameter.."
echo %1 %2 %3 %4
echo %USAGE_MSG%
echo ============***************** ERROR *****************============echo.
endlocal
exit /b 1

