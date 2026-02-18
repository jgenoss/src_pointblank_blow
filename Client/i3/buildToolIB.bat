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

set PBTOOL_SVN=svn.exe 
set PBTOOL_SVN_CMD_UPDATE= update --depth infinity 

if exist "C:\Program Files\TortoiseSVN\bin\TortoiseProc.exe" (  
  set PBTOOL_SVN=C:\Program Files\TortoiseSVN\bin\TortoiseProc.exe 
  set PBTOOL_SVN_CMD_UPDATE= /command:update /closeonend:1 /includeexternals /path:
)
if exist "C:\Program Files (x86)\TortoiseSVN\bin\TortoiseProc.exe" (  
  set PBTOOL_SVN=C:\Program Files ^(x86^)\TortoiseSVN\bin\TortoiseProc.exe 
  set PBTOOL_SVN_CMD_UPDATE= /command:update /closeonend:1 /includeexternals /path:
)

echo Info - Using "%PBTOOL_SVN%" to operate subversion stuff

set I3_ROOT=%CD%
echo I3_ROOT : %I3_ROOT%
set USAGE_MSG="usage) BuildToolIB.bat [auto] [ib | vc] [build | rebuild] [makeToolInstaller | makeToolInstallerAndSvn]"
set AutoBuild=0
set BuilderType=ib
set MakePBSetupTool=0
set LOG_FILENAME=buildib_log.txt
set EnginTool="Distribute\i3Engine2 Tools.sln"
del %LOG_FILENAME% > NUL
set SlnPath=""
set BuildCfg="distribute"

if "%3"=="makeToolInstaller" set MakePBSetupTool=1
if "%3"=="makeToolInstallerAndSvn" set MakePBSetupTool=2


if "%1"=="auto" (
   set AutoBuild=1
   set BuilderType=%2
   set BuildType=%3
   
   if "%~4"=="makeToolInstaller" set MakePBSetupTool=1
   if "%~4"=="makeToolInstallerAndSvn" set MakePBSetupTool=2

   goto BuildEntry
   
) else (
   set /p BuilderType=(Use IncrediBuild=0 or VC=1)
   set /p BuildType=(Select BuildType build=0, rebuild=1)
   rem set /p BuildCfg=(Select BuildConfiguration debug=1, release=2, distribute=3)
   set /p MakePBSetupTool=(Make PBSetupTool.exe no=0, makeonly=1, makeandcommit=2)
  
   if %BuilderType% equ 0 (
      set BuilderType=ib
      goto BuildType
   )
 
  if %BuilderType% equ 1 (
      set BuilderType=vc
      goto BuildType
  )

  goto ParamErrorExitManual


:BuildType
  
   if %BuildType% equ 0 (
      set BuildType=build
      goto BuildEntry
   )
 
  if %BuildType% equ 1 (
      set BuildType=rebuild
      goto BuildEntry
  )
  
  goto ParamErrorExitManual
)


:BuildEntry
rem == i3 svn update ==
echo.
echo ===============================================================
echo Updating i3 engine ^& tool source from svn..
echo ===============================================================
echo.

echo "%PBTOOL_SVN%" %PBTOOL_SVN_CMD_UPDATE%"\*.*"
call "%PBTOOL_SVN%" %PBTOOL_SVN_CMD_UPDATE%"\*.*"
if not %ERRORLEVEL% equ 0 (
  echo "ERR - svn update(/.) failed.. %cd%"
  goto BuildErrorExit
)

rem == svn update - pointblank source - for avataviewer tool ==
echo "Updating pointblank source from svn.. (for avatarviewer tool)"
echo "%PBTOOL_SVN%" %PBTOOL_SVN_CMD_UPDATE%"../PointBlank/Source_New/*.*"
call "%PBTOOL_SVN%" %PBTOOL_SVN_CMD_UPDATE%"../PointBlank/Source_New/*.*"
if not %ERRORLEVEL% equ 0 (
  echo "ERR - svn update(/../PointBlank/Source_New/) failed.. %cd%"
  goto BuildErrorExit
)

echo.

rem ==============================
rem i3Engine
rem ==============================
set SlnPath="project\i3_dx\I3_DX.sln"


call _buildIB.bat %BuilderType% %SlnPath% %BuildType% %BuildCfg% %LOG_FILENAME%
if not %ERRORLEVEL% equ 0 goto BuildErrorExit
echo "i3Engine is successfully build..(distribute)"

if %BuildCfg%=="distribute|win32" (
  xcopy /D /Y "./lib/DirectX/DistributeDLL/*_DISTRIBUTE.dll" "./Bin/*.*"
)

set AlterBuildCfg_Engine=%BuildCfg%
if %BuildCfg%=="distribute" set AlterBuildCfg_Engine="release"

call _buildIB.bat %BuilderType% %SlnPath% %BuildType% %AlterBuildCfg_Engine% %LOG_FILENAME%
if not %ERRORLEVEL% equ 0 goto BuildErrorExit
echo "i3Engine is successfully build..(release)"

if %AlterBuildCfg_Engine%=="release" (
  xcopy /D /Y "\lib\DirectX\ReleaseDLL\*.dll" "Bin\*.*"
)

rem ==============================
rem i3GIC은 i3Engine프로젝트에 있음
rem ==============================


rem ==============================
rem Tools
rem ==============================
set SlnPath="Tools\Tools.sln"

call _buildIB.bat %BuilderType% %SlnPath% %BuildType% %BuildCfg% %LOG_FILENAME%
if not %ERRORLEVEL% equ 0 goto BuildErrorExit
echo "i3Tool is successfully build.."


rem ==============================
rem Avataviewer
rem ==============================
set SlnPath="..\PointBlank\Source_New\AvatarViewer\AvatarViewer.sln"

set AlterBuildCfg=%BuildCfg%
if %BuildCfg%=="release" set AlterBuildCfg="distribute"

call _buildIB.bat %BuilderType% %SlnPath% %BuildType% %AlterBuildCfg% %LOG_FILENAME%

rem if not %ERRORLEVEL% equ 0 goto BuildErrorExit
if not %ERRORLEVEL% equ 0 goto skip_avatarviewer

if %AlterBuildCfg%=="distribute" (
  xcopy /d /y "..\PointBlank\Source_New\AvatarViewer\release\AvatarViewer.exe" "bin\"
)
echo "Avataviewer is successfully build.."

:skip_avatarviewer

rem ==============================
rem RSCTool
rem ==============================
set SlnPath="..\PointBlank\Source_New\ClientSource\RscTool\RscTool.sln"

set AlterBuildCfg_RscTool=%BuildCfg%
if %BuildCfg%=="release" set AlterBuildCfg="distribute"
rem if %BuildCfg%=="distribute" set AlterBuildCfg_RscTool="release"

call _buildIB.bat %BuilderType% %SlnPath% %BuildType% %AlterBuildCfg_RscTool% %LOG_FILENAME%

rem if not %ERRORLEVEL% equ 0 goto BuildErrorExit
if not %ERRORLEVEL% equ 0 goto skip_RscTool

if %AlterBuildCfg_RscTool%=="distribute" (
  xcopy /d /y "..\PointBlank\Source_New\ClientSource\RscTool\distribute\RscTool.exe" "bin\"
)
echo "RscTool is successfully build.."

:skip_RscTool

rem ==============================
rem I3Export for 2011
rem ==============================

set SlnPath="Tools\i3Export2\i3Export2.sln"

if not %BuildCfg%=="distribute" goto skipBuildMax2011Exporter
set AlterBuildCfg="Distribute_MAX2011"

call _buildIB.bat %BuilderType% %SlnPath% %BuildType% %AlterBuildCfg% %LOG_FILENAME%
if not %ERRORLEVEL% equ 0 goto BuildErrorExit
echo "i3Export for 2011 is successfully build.."

:skipBuildMax2011Exporter

rem ==============================
rem EngineTool
rem ==============================
rem set SlnPath=%EnginTool%
rem set BuildCfg="Debug|Any CPU"
rem call _buildIB.bat %SlnPath% %BuildType% %BuildCfg% %LOG_FILENAME%
rem if not %ERRORLEVEL% equ 0 goto BuildErrorExit
rem echo "EngineTool is successfully build.."


:BuildSucceed
echo "Build Succeed"

if %MakePBSetupTool% equ 1 (
call "_BuildToolInstaller.bat"
)

if %MakePBSetupTool% equ 2 (
call "_BuildToolInstaller.bat" commit
)

endlocal
exit /b 0


:BuildErrorExit
echo.
echo ============*********** ERROR ***********============
echo Err - Failed to Build solution= %SlnPath%
echo ============*********** ERROR ***********============
echo .

if %AutoBuild% equ 0 pause
endlocal
exit /b 1

:ParamErrorExit
echo USAGE_MSG

:ParamErrorExitManual
echo "Err - Bad Parameter.."
if %AutoBuild% equ 0 pause
endlocal
exit /b 1
