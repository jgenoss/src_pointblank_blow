@echo off

call Define.bat
IF ERRORLEVEL 1 goto failed

rem ------------------------------------
rem PointBlank 기본 셋팅을 해줍니다.
rem ------------------------------------

set BuildConfig_Val=/Build

rem 빌드 타입 셋팅.
set BUILD_TYPE=%BuildConfig_Val%
set BUILD_PATH=%PB_SRC_ROOT%\ClientSource\ClientSource.sln
set SymbolBackup=1


rem ----------------------------------------------------------
rem 엔진 빌드를 수행합니다.
rem ----------------------------------------------------------

set Engine_Build_Val=/Build

if not %Engine_Build_Val% == "0" (

	echo --------------------------------------------------------
	echo 엔진을 업데이트합니다. %I3_ROOT%
	echo --------------------------------------------------------
	svn update %I3_ROOT%

	echo --------------------------------------------------------
	echo 엔진을 빌드합니다. %Engine_Build_Val%
	echo --------------------------------------------------------
	echo %ENGINE_PATH% %Engine_Build_Val% Release /out .\Log_Engine_%Engine_Build_Val:~1%.txt
	devenv.com %ENGINE_PATH% %Engine_Build_Val% Release /out .\Log_Engine_%Engine_Build_Val:~1%.txt
	if ERRORLEVEL 1 (
		echo 엔진 빌드 실패
		goto failed
	)
)

echo --------------------------------------------------------
echo 소스 업데이트
echo --------------------------------------------------------
echo - Update %PB_SRC_ROOT%
svn update %PB_SRC_ROOT%

rem --------------------------------------------------------
rem 태국 클라이언트 빌드
rem --------------------------------------------------------

set Locale_Val=Thailand
set BuildType_Val=QA

call Build_Client.bat %Locale_Val% %BuildType_Val%
IF ERRORLEVEL 1 goto failed

set BuildType_Val=Live

call Build_Client.bat %Locale_Val% %BuildType_Val%
IF ERRORLEVEL 1 goto failed

rem --------------------------------------------------------
rem 인도네시아 클라이언트 빌드
rem --------------------------------------------------------

set Locale_Val=Indonesia
set BuildType_Val=QA

call Build_Client.bat %Locale_Val% %BuildType_Val%
IF ERRORLEVEL 1 goto failed

set BuildType_Val=Live

call Build_Client.bat %Locale_Val% %BuildType_Val%
IF ERRORLEVEL 1 goto failed

rem --------------------------------------------------------
rem 러시아 클라이언트 빌드
rem --------------------------------------------------------

set Locale_Val=Russia
set BuildType_Val=QA

call Build_Client.bat %Locale_Val% %BuildType_Val%
IF ERRORLEVEL 1 goto failed

set BuildType_Val=Live

call Build_Client.bat %Locale_Val% %BuildType_Val%
IF ERRORLEVEL 1 goto failed

rem --------------------------------------------------------
rem 터키 클라이언트 빌드
rem --------------------------------------------------------

set Locale_Val=Turkey
set BuildType_Val=QA

call Build_Client.bat %Locale_Val% %BuildType_Val%
IF ERRORLEVEL 1 goto failed

set BuildType_Val=Live

call Build_Client.bat %Locale_Val% %BuildType_Val%
IF ERRORLEVEL 1 goto failed

rem --------------------------------------------------------
rem 브라질 클라이언트 빌드
rem --------------------------------------------------------

set Locale_Val=Brazil
set BuildType_Val=QA

call Build_Client.bat %Locale_Val% %BuildType_Val%
IF ERRORLEVEL 1 goto failed

set BuildType_Val=Live

call Build_Client.bat %Locale_Val% %BuildType_Val%
IF ERRORLEVEL 1 goto failed

rem --------------------------------------------------------
rem 중남미 클라이언트 빌드
rem --------------------------------------------------------

set Locale_Val=Latin_America
set BuildType_Val=QA

call Build_Client.bat %Locale_Val% %BuildType_Val%
IF ERRORLEVEL 1 goto failed

set BuildType_Val=Live

call Build_Client.bat %Locale_Val% %BuildType_Val%
IF ERRORLEVEL 1 goto failed

rem --------------------------------------------------------
rem 북미 클라이언트 빌드
rem --------------------------------------------------------

set Locale_Val=NorthAmerica
set BuildType_Val=QA

call Build_Client.bat %Locale_Val% %BuildType_Val%
IF ERRORLEVEL 1 goto failed

set BuildType_Val=Live

call Build_Client.bat %Locale_Val% %BuildType_Val%
IF ERRORLEVEL 1 goto failed

rem --------------------------------------------------------
rem 필리핀 클라이언트 빌드
rem --------------------------------------------------------

set Locale_Val=Philippines
set BuildType_Val=QA

call Build_Client.bat %Locale_Val% %BuildType_Val%
IF ERRORLEVEL 1 goto failed

set BuildType_Val=Live

call Build_Client.bat %Locale_Val% %BuildType_Val%
IF ERRORLEVEL 1 goto failed


echo --------------------------------------------------------
echo ** 모든 빌드가 성공적으로 끝났습니다.
echo --------------------------------------------------------
goto end


:failed
echo --------------------------------------------------------------
echo !!! %Locale_Val% (%BuildType_Val%) 빌드가 실패하였습니다. !!!
echo --------------------------------------------------------------
if "Auto" == "%1" (
	rem exit /b 1 로 하게되면 크루즈컨트롤에서 에러값 리턴을 감지 못한다.
	rem 그래서 exit 1로 해줘야한다.
	exit 1
)

:end

rem 자동 빌드는 알림음을 내지 않습니다.
if not "Auto" == "%1" (
	call Beep.exe
	pause
)
