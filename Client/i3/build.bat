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
echo ================================================
echo 빌드 타입을 선택하십시오.
echo 1. Debug
echo 2. Release
echo 3. Distribute
echo ================================================

set /p BuildType=(1,2,3)?


if %BuildType% == 1 (
	set BuildType=Debug
	goto EndBuildType
)
if %BuildType% == 2 (
	set BuildType=Release
	goto EndBuildType
)
if %BuildType% == 3 (
	set BuildType=Distribute
	goto EndBuildType
)

echo %ERROR_MSG%
goto StartBuildType

:EndBuildType

:StartBuildStep
echo ================================================
echo 빌드 할 단계를 지정하십시오.
echo 1. 엔진만
echo 2. 전부
echo ================================================

set /p BuildStep=(1,2)?


if %BuildStep% == 1 (
	goto EndBuildStep
)
if %BuildStep% == 2 (
	goto EndBuildStep
)

echo %ERROR_MSG%
goto StartBuildStep

:EndBuildStep


rem     vorbis_ogg
rem
devenv.com vorbis_ogg\vorbis.sln %BuildConfig% %BuildType% /out %LOG_FILENAME%
IF ERRORLEVEL 1 (
	echo vorbis_ogg 빌드 실패
	goto end
)

rem     MovieBase
rem
devenv.com MovieBase\MovieBase.sln %BuildConfig% %BuildType% /out %LOG_FILENAME%
IF ERRORLEVEL 1 (
	echo MovieBase 빌드 실패
	goto end
)

rem     I3_DX
rem
devenv.com Project\I3_DX\I3_DX.sln %BuildConfig% %BuildType% /out %LOG_FILENAME%
IF ERRORLEVEL 1 (
	echo I3_DX 빌드 실패
	goto end
)

rem	BuildStep 1
rem
if %BuildStep% == 1 (
	echo 빌드를 종료합니다.
	goto end
)

rem     i3GIC
rem
if %BuildType% == Debug (
	set GICBuildType=Debug
) else (
	set GICBuildType=Distribute
)
devenv.com i3GIC\i3GIC.sln %BuildConfig% %GICBuildType% /out %LOG_FILENAME%
IF ERRORLEVEL 1 (
	echo i3GIC 빌드 실패
	goto end
)

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

:end
pause