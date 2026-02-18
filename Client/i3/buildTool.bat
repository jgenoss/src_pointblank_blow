@echo off

rem VS 2008
rem
if not defined VS90COMNTOOLS (
	echo In order to build this project you need MS Visual Studio 2008 to be installed.
	echo http://msdn.microsoft.com/vstudio/express/downloads/default.aspx
	exit
)
rem VS 2015
if not defined VS140COMNTOOLS (
	echo In order to build this project you need MS Visual Studio 2015 to be installed.
	endlocal
	exit /b 1
)

call "%VS140COMNTOOLS%\vsvars32.bat" || exit /b 1
rem EnginTool
set EnginTool="Distribute\i3Engine2 Tools.sln"
set I3_ROOT=%CD%
echo -
echo I3_ROOT : %I3_ROOT%
echo - 

set ERROR_MSG=잘못 선택하셨습니다. 다시 선택하여주세요.
set LOG_FILENAME=build_log.txt
del %LOG_FILENAME%

:StartBuildConfig
echo ================================================
echo 빌드 구성을 선택하십시오.
echo 1 : Build
echo 2 : Rebuild
echo ================================================

set /p BuildConfig=(1,2)?


if %BuildConfig% == 1 (
	set BuildConfig=/Build
	goto EndBuildConfig
)
if %BuildConfig% == 2 (
	set BuildConfig=/Rebuild
	goto EndBuildConfig
)

echo %ERROR_MSG%
goto StartBuildConfig

:EndBuildConfig

:StartBuildType

set BuildType=Distribute
goto EndBuildType

echo %ERROR_MSG%
goto StartBuildType

:EndBuildType

:StartBuildStep

goto EndBuildStep

echo %ERROR_MSG%
goto StartBuildStep

:EndBuildStep

rem     I3_DX
rem
devenv.com Project\I3_DX\I3_DX.sln %BuildConfig% %BuildType% /out %LOG_FILENAME%
IF ERRORLEVEL 1 (
	echo I3_DX 빌드 실패
	goto end
)

rem     i3GIC
rem
rem if %BuildType% == Debug (
rem 	set GICBuildType=Debug
rem ) else (
rem 	set GICBuildType=Distribute
rem )
rem devenv.com i3GIC\i3GIC.sln %BuildConfig% %GICBuildType% /out %LOG_FILENAME%
rem IF ERRORLEVEL 1 (
rem 	echo i3GIC 빌드 실패
rem 	goto end
rem )

rem     Tools
rem
if %BuildType% == Debug (
	set ToolBuildType=Debug
) else (
	set ToolBuildType=Distribute
)
devenv.com Tools\Tools.sln %BuildConfig% %ToolBuildType% /out %LOG_FILENAME%
IF ERRORLEVEL 1 (
	echo Tools 빌드 실패
	goto end
)

devenv.com Tools\i3Export2\i3Export2.sln %BuildConfig% Distribute_MAX2011 /out %LOG_FILENAME%
IF ERRORLEVEL 1 (
	echo Distribute_MAX2011 빌드 실패
	goto end
)
rem     Distribute
rem

devenv.com %EnginTool% %BuildConfig% Debug /out %LOG_FILENAME%
IF ERRORLEVEL 1 (
	echo i3Engine2 Tools 빌드 실패
	goto end
)
:end
pause