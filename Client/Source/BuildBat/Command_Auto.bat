@echo off

set Locale_Val=%1
set BuildType_Val=%2
set BuildConfig_Val=%3
set Arxan_Build_Val=%4

set ERROR_MSG=매개변수가 잘못되었습니다.


rem ================================================
rem            %1
rem 국가를 선택하십시오.
rem 1 : Korea
rem 2 : Thailand
rem 3 : Indonesia
rem 4 : Russia
rem 5 : Turkey
rem 6 : PointBlank Dev
rem 6 : Brazil
rem 14 : Philippines
rem 18 : Mode Script
rem 19 : Steam
rem 20 : Japan
rem ================================================

if %Locale_Val% == UI2_Korea (
	goto EndSelectCountry
)
if %Locale_Val% == Korea (
	goto EndSelectCountry
)
if %Locale_Val% == Thailand (
	goto EndSelectCountry
)
if %Locale_Val% == Indonesia (
	goto EndSelectCountry
)
if %Locale_Val% == Russia (
	goto EndSelectCountry
)
if %Locale_Val% == Turkey (
	goto EndSelectCountry
)
if %Locale_Val% == Development (
	goto EndSelectCountry
)
if %Locale_Val% == Brazil (
	goto EndSelectCountry
)
if %Locale_Val% == NorthAmerica (
	goto EndSelectCountry
)
if %Locale_Val% == China (
	goto EndSelectCountry
)
if %Locale_Val% == Latin_America (
	goto EndSelectCountry
)
if %Locale_Val% == Italy (
	goto EndSelectCountry
)
if %Locale_Val% == English (
	goto EndSelectCountry
)
if %Locale_Val% == MiddleEast (
	goto EndSelectCountry
)
if %Locale_Val% == Philippines (
	goto EndSelectCountry
)
if %Locale_Val% == Singmal (
	goto EndSelectCountry
)
if %Locale_Val% == Vietnam (
	goto EndSelectCountry
)
if %Locale_Val% == UI2_Korea_Uni (
	goto EndSelectCountry
)
if %Locale_Val% == ModeScript (
	goto EndSelectCountry
)
if %Locale_Val% == Steam (
	goto EndSelectCountry
)
if %Locale_Val% == Japan (
	goto EndSelectCountry
)

goto failed
:EndSelectCountry



rem ================================================
rem            %2
rem 빌드 타입을 선택하십시오.
rem 1. QA
rem 2. Live
rem ================================================

if %BuildType_Val% == QA (
	set BuildType_Val=QA
	goto EndBuildType
)
if %BuildType_Val% == Live (
	set BuildType_Val=Live
	goto EndBuildType
)

goto failed
:EndBuildType



rem ================================================
rem            %3
rem 빌드 구성을 선택하십시오.
rem 1 : Build
rem 2 : Rebuild
rem ================================================


if %BuildConfig_Val% == Build (
	set BuildConfig_Val=/Build
	goto EndBuildConfig
)
if %BuildConfig_Val% == Rebuild (
	set BuildConfig_Val=/Rebuild
	goto EndBuildConfig
)

goto failed
:EndBuildConfig

:StartIsArxanBuild

if %Arxan_Build_Val% == Arxan (
	set Arxan_Build_Val=1
	goto EndIsArxanBuild
)
if %Arxan_Build_Val% == NoArxan (
	set Arxan_Build_Val=0
	goto EndIsArxanBuild
)

:EndIsArxanBuild


rem ================================================
rem 엔진을 빌드합니다.
rem ================================================
set Engine_Build_Val=/Build
set Symbol_Backup="0"


goto :EOF


:failed
set ERRORLEVEL=1
echo %ERROR_MSG%