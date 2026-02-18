@echo off
rem 
rem 
rem 매개변수 옵션
rem %1 : 매개변수를 사용하는지 안하는지.
rem %2 : 국가
rem %3 : 빌드 타입 (QA or Live)
rem %4 : 빌드 구성 (Build or Rebuild)
rem %5 : SymbolBackup
rem %6 : AutoSvnCommit
rem %7 : 악산 사용 여부 



call Define.bat
IF ERRORLEVEL 1 goto failed


rem 빌드 심볼들을 백업할지 인자를 확인합니다.
set SymbolBackup=1
if "Symbol_NoBackup" == "%5" (
set SymbolBackup=0
)

set PB_AutoSvnCommit=0


if "Auto" == "%1" (
	echo 1: %1
	echo 2: %2
	echo 3: %3
	echo 4: %4
	echo 5: %5
	echo 6: %6
	echo 7: %7
	
if "%6"=="AutoSvnCommit" (
  set PB_AutoSvnCommit=1
)	
	
	call Command_Auto.bat %2 %3 %4 %7

) else (
	call Command_Page.bat

)

if ERRORLEVEL 1 goto failed

rem ------------------------------------
rem PointBlank 기본 셋팅을 해줍니다.
rem ------------------------------------

rem 빌드 타입 셋팅.
set BUILD_TYPE=%BuildConfig_Val%
set BUILD_PATH=%PB_SRC_ROOT%\ClientSource\ClientSource.sln


rem ------------------------------------
rem 국가 설정을 셋팅합니다.
rem ------------------------------------
set DEFINE_FILE_NAME=Define_%Locale_Val%_%BuildType_Val%
echo Define File Name : %DEFINE_FILE_NAME%

call %DEFINE_FILE_NAME%.bat





echo --------------------------------------------------------
echo 업데이트
echo --------------------------------------------------------
echo - Update %PB_SRC_ROOT%
svn update %PB_SRC_ROOT%

echo - Update %EXE_ROOT%
svn update %EXE_ROOT%





rem ----------------------------------------------------------
rem 빌드를 수행합니다.
rem ----------------------------------------------------------
if not %Engine_Build_Val% == "0" (

	echo --------------------------------------------------------
	echo 엔진을 업데이트합니다. %I3_ROOT%
	echo --------------------------------------------------------
	svn update %I3_ROOT%

	echo --------------------------------------------------------
	echo 엔진을 빌드합니다. %Engine_Build_Val%
	echo --------------------------------------------------------
	echo %ENGINE_PATH% %Engine_Build_Val% Release /out %LOG_PATH%\Log_Engine_%Engine_Build_Val:~1%.txt
	devenv.com %ENGINE_PATH% %Engine_Build_Val% Release /out %LOG_PATH%\Log_Engine_%Engine_Build_Val:~1%.txt
	if ERRORLEVEL 1 (
		echo 엔진 빌드 실패
		goto failed
	)
)

rem ----------------------------------------------------------
rem 클라이언트 빌드 전 빌드카운트를 먼저 올립니다.
rem ----------------------------------------------------------

call BuildCounter.bat

call Build_%BuildType_Val%.bat
IF ERRORLEVEL 1 goto failed


set _I3_REV=0
set _PBSRC_REV=0

for /f "tokens=*" %%a in ('svnversion.exe -c %I3_ROOT%') do set _I3_REV=%%a
for /f "tokens=*" %%b in ('svnversion.exe -c %PB_SRC_ROOT%') do set _PBSRC_REV=%%b



rem 로컬빌드시에 EXE_LIVE_FILE_DIR이 media폴더 내용만을 담고있지않을경우 다른것도 커밋되는문제로 인해
rem 빌드컴퓨터의 CruiseControl 에서 실행한경우에만 실행합니다.
if "%CCNetProject%"=="" goto skipAutoSvnCommit

if %PB_AutoSvnCommit% equ 1 (

echo *******************************************
echo ************ AutoSvnCommit ****************
echo *******************************************

echo svn commit "%EXE_ROOT%" -m "Auto commit PBSRCRev:%_PBSRC_REV% , i3Rev:%_I3_REV% - %CCNetProject% , BUILD_COUNT:%BUILD_COUNT%"
svn commit "%EXE_ROOT%" -m "Auto commit PBSRCRev:%_PBSRC_REV% , i3Rev:%_I3_REV% - %CCNetProject% , BUILD_COUNT:%BUILD_COUNT%"

if not %ERRORLEVEL% equ 0 (
  echo "ERR - SVN Commit에 실패했습니다. (%EXE_ROOT%)"
  goto failed
)

)


:skipAutoSvnCommit

echo 빌드가 성공적으로 끝났습니다.
echo.
echo.
echo ===========================     Svn Revision      ===========================
echo == pb : %_PBSRC_REV%   from %PB_SRC_ROOT%
echo == i3 : %_I3_REV%      from %I3_ROOT%
echo =============================================================================
echo.

goto end


:failed

echo 빌드가 실패하였습니다.
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
