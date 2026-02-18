@echo off


set ERROR_MSG=잘못 선택하셨습니다. 다시 선택하여주세요.
echo ************************************************
echo 빌드 옵션을 선택합니다.
echo ************************************************

:StartSelectCountry
echo ================================================
echo 국가를 선택하십시오.
echo 0 : UI2_Korea
echo 1 : Korea
echo 2 : Thailand
echo 3 : Indonesia
echo 4 : Russia
echo 5 : Turkey
echo 6 : PointBlank Dev
echo 7 : Brazil
echo 8 : Latin_America
echo 9 : China
echo 10 : NorthAmerica
echo 11 : Italy
echo 12 : English
echo 13 : MiddleEast
echo 14 : Philippines
echo 15 : Singmal
echo 16 : Vietnam
echo 17 : UI2_Korea_Uni
echo 18 : ModeScript
echo 19 : Steam
echo 20 : Japan
echo ================================================

set /p Locale_Val=(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20)?

if %Locale_Val% == 0 (
	set Locale_Val=UI2_Korea
	goto EndSelectCountry
)
if %Locale_Val% == 1 (
	set Locale_Val=Korea
	goto EndSelectCountry
)
if %Locale_Val% == 2 (
	set Locale_Val=Thailand
	goto EndSelectCountry
)
if %Locale_Val% == 3 (
	set Locale_Val=Indonesia
	goto EndSelectCountry
)
if %Locale_Val% == 4 (
	set Locale_Val=Russia
	goto EndSelectCountry
)
if %Locale_Val% == 5 (
	set Locale_Val=Turkey
	goto EndSelectCountry
)
if %Locale_Val% == 6 (
	set Locale_Val=Development
	goto EndSelectCountry
)
if %Locale_Val% == 7 (
	set Locale_Val=Brazil
	goto EndSelectCountry
)
if %Locale_Val% == 8 (
	set Locale_Val=Latin_America
	goto EndSelectCountry
)
if %Locale_Val% == 9 (
	set Locale_Val=China
	goto EndSelectCountry
)
if %Locale_Val% == 10 (
	set Locale_Val=NorthAmerica
	goto EndSelectCountry
)
if %Locale_Val% == 11 (
	set Locale_Val=Italy
	goto EndSelectCountry
)
if %Locale_Val% == 12 (
	set Locale_Val=English
	goto EndSelectCountry
)
if %Locale_Val% == 13 (
	set Locale_Val=MiddleEast
	goto EndSelectCountry
)
if %Locale_Val% == 14 (
	set Locale_Val=Philippines
	goto EndSelectCountry
)
if %Locale_Val% == 15 (
	set Locale_Val=Singmal
	goto EndSelectCountry
)
if %Locale_Val% == 16 (
	set Locale_Val=Vietnam
	goto EndSelectCountry
)
if %Locale_Val% == 17 (
	set Locale_Val=UI2_Korea_Uni
	goto EndSelectCountry
)
if %Locale_Val% == 18 (
	set Locale_Val=ModeScript
	goto EndSelectCountry
)
if %Locale_Val% == 19 (
	set Locale_Val=Steam
	goto EndSelectCountry
)
if %Locale_Val% == 20 (
	set Locale_Val=Japan
	goto EndSelectCountry
)


echo %ERROR_MSG%
goto StartSelectCountry

:EndSelectCountry


:StartBuildType
echo ================================================
echo 빌드 타입을 선택하십시오.
echo 1. QA
echo 2. Live
echo ================================================

set /p BuildType_Val=(1,2)?


if %BuildType_Val% == 1 (
	set BuildType_Val=QA
	goto EndBuildType
)
if %BuildType_Val% == 2 (
	set BuildType_Val=Live
	goto EndBuildType
)

echo %ERROR_MSG%
goto StartBuildType

:EndBuildType


:StartBuildConfig
echo ================================================
echo 빌드 구성을 선택하십시오.
echo 1 : Build
echo 2 : Rebuild
echo ================================================

set /p BuildConfig_Val=(1,2)?


if %BuildConfig_Val% == 1 (
	set BuildConfig_Val=/Build
	goto EndBuildConfig
)
if %BuildConfig_Val% == 2 (
	set BuildConfig_Val=/Rebuild
	goto EndBuildConfig
)

echo %ERROR_MSG%
goto StartBuildConfig

:EndBuildConfig



:StartIsEngineBuild
echo ================================================
echo 엔진을 빌드합니까?
echo 1 : 안함
echo 8 : Build
echo 9 : Rebuild
echo ================================================

set Engine_Build_Val="0"
set /p Engine_Build_Val=(1,8,9)?

if "%Engine_Build_Val%" == "1" (
	set Engine_Build_Val="0"
	goto EndIsEngineBuild
)
if "%Engine_Build_Val%" == "8" (
	set Engine_Build_Val=/Build
	goto EndIsEngineBuild
)
if "%Engine_Build_Val%" == "9" (
	set Engine_Build_Val=/Rebuild
	goto EndIsEngineBuild
)

echo %ERROR_MSG%
goto StartIsEngineBuild

:EndIsEngineBuild


rem 로컬빌드시에 EXE_LIVE_FILE_DIR이 media폴더 내용만을 담고있지않을경우 다른것도 커밋되는문제로 인해
rem 문제가 해결될때까지 주석처리
rem :StartAutoSvnCommit
rem 
rem echo ================================================
rem echo ClientExes의 결과물을 자동커밋하시겠습니까?
rem echo 0 : 안함
rem echo 1 : 자동커밋
rem echo ================================================
rem 
rem set /p PB_AutoSvnCommit=(0,1)?
rem 
rem if %PB_AutoSvnCommit% equ 0 (
rem 	goto EndAutoSvnCommit
rem )
rem if %PB_AutoSvnCommit% equ 1 (
rem 	goto EndAutoSvnCommit
rem )
rem 
rem  goto StartAutoSvnCommit
rem 
rem :EndAutoSvnCommit
