@echo off
setlocal enabledelayedexpansion

rem VS 2008
if not defined VS90COMNTOOLS (
	echo In order to build this project you need MS Visual Studio 2008 to be installed.
	echo http://msdn.microsoft.com/vstudio/express/downloads/default.aspx
	endlocal
	exit /b 1
)

rem VS 2015
if not defined VS140COMNTOOLS (
	echo In order to build this project you need MS Visual Studio 2015 to be installed.
	endlocal
	exit /b 1
)

call "%VS140COMNTOOLS%\vsvars32.bat" || exit /b 1

set I3_ROOT=%CD%
echo I3_ROOT : %I3_ROOT%
set USAGE_MSG="usage) BuildIB.bat [auto] [build | rebuild] [debug|release|distribute]"
set LOG_FILENAME=build_log.txt

if "%~1"=="auto" (
  set BuildType=%~2
  set BuildCfg=%~3
  
  goto BuildEntry

) else (
   set /p BuildType=(Select BuildType build=0, rebuild=1)
   set /p BuildCfg=(Select BuildConfiguration debug=0, release=1, distribute=2)
  
   if %BuildType% equ 0 (
      set BuildType=build
      goto ConvertBuildCfg
   )
 
  if %BuildType% equ 1 (
      set BuildType=rebuild
      goto ConvertBuildCfg
  )
   
  goto ParamErrorExitManual

:ConvertBuildCfg

   if %BuildCfg% equ 0 (
      set BuildCfg="debug"
      goto BuildEntry
   )
 
  if %BuildCfg% equ 1 (
      set BuildCfg="release"
      goto BuildEntry
  )
  
  if %BuildCfg% equ 2 (
      set BuildCfg="distribute"
      goto BuildEntry
  )
  
  goto ParamErrorExitManual

)


:BuildEntry

rem ==============================
rem i3Engine
rem ==============================
set SlnPath="project\i3_dx\i3_dx.sln"

echo _buildIB.bat ib %SlnPath% %BuildType% %BuildCfg% %LOG_FILENAME%
call _buildIB.bat ib %SlnPath% %BuildType% %BuildCfg% %LOG_FILENAME%
if not %ERRORLEVEL% equ 0 goto BuildErrorExit
echo "i3Engine is successfully build.."

if %BuildCfg%=="distribute|win32" (
  xcopy /D /Y "./lib/DirectX/DistributeDLL/*_DISTRIBUTE.dll" "./Bin/*.*"
)

:BuildSucceed
echo "Build Succeed"

endlocal
exit /b 0


:BuildErrorExit
echo.
echo ============*********** ERROR ***********============
echo Err - Failed to Build solution= %SlnPath%
echo ============*********** ERROR ***********============
echo .

if not "%~1"=="auto" pause
endlocal
exit /b 1

:ParamErrorExit
echo USAGE_MSG

:ParamErrorExitManual
echo "Err - Bad Parameter.."
if %AutoBuild% equ 0 pause
endlocal
exit /b 1
