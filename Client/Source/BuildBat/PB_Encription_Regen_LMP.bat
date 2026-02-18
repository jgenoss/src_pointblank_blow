@echo off
rem
rem %1 : PB 소스 경로
rem %2 : 인크립션할 파일의 경로
rem
rem 아래 스크립트중 설정파일을 교체하는 이유는 명령줄 모든 HSBGen의 옵션을 설정해줄 수 없기 때문이다.
rem 대표적인 문제로는 InitStep에 대한 옵션을 줄 수 없는 것이다.
rem


set ERROR_MSG=잘못 선택하셨습니다. 다시 선택하여주세요.
echo ************************************************
echo 빌드 옵션을 선택합니다.
echo ************************************************

:StartSelectCountry
echo ================================================
echo 국가를 선택하십시오.
echo 1 : Korea
echo 2 : Thailand
echo 3 : Indonesia
echo 4 : Russia
echo 5 : Turkey
echo 6 : PointBlank Dev
echo 7 : Brazil
echo 8 : Latin_America
echo ================================================

set /p Locale_Val=(1,2,3,4,5,6,7)?


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
	set Locale_Val=LAtin_America
	goto EndSelectCountry
)

echo %ERROR_MSG%
goto StartSelectCountry

:EndSelectCountry

set DEFINE_FILE_NAME=Define_%Locale_Val%_Live
echo Define File Name : %DEFINE_FILE_NAME%

call Define.bat
call %DEFINE_FILE_NAME%.bat

set __EXE_ROOT=%EXE_LIVE_FILE_DIR%

set NO_EXIST_FILE=0
rem if not exist %__EXE_ROOT%\%EXEC_NAME%_orig.i3Exec		set /a NO_EXIST_FILE=%NO_EXIST_FILE%+1
if not exist %__EXE_ROOT%\%EXE_NAME%_orig.exe			set /a NO_EXIST_FILE=%NO_EXIST_FILE%+1
if not exist %__EXE_ROOT%\WinLockDll_orig.dll			set /a NO_EXIST_FILE=%NO_EXIST_FILE%+1
rem if not exist %__EXE_ROOT%\%EXEC_NAME%_themida.i3Exec	set /a NO_EXIST_FILE=%NO_EXIST_FILE%+1
if not exist %__EXE_ROOT%\%EXE_NAME%_themida.exe	set /a NO_EXIST_FILE=%NO_EXIST_FILE%+1
if not exist %__EXE_ROOT%\WinLockDll_themida.dll		set /a NO_EXIST_FILE=%NO_EXIST_FILE%+1

if not "0" == "%NO_EXIST_FILE%" (
	echo 빌드 또는 더미다가 적용 되어 있지 않습니다.
	echo 빌드를 먼저 하시길 바랍니다.
	echo 이 스크립트는 빌드는 완료된 상태에서 LMP만 다시 적용하기 위한 빌드입니다.
	goto error_end
)


rem 더미다 파일을 다시 덮어씌워줍니다.
rem copy %__EXE_ROOT%\%EXEC_NAME%_themida.i3Exec %__EXE_ROOT%\%EXEC_NAME%.i3Exec
copy %__EXE_ROOT%\%EXE_NAME%_themida.exe %__EXE_ROOT%\%EXE_NAME%.exe
copy %__EXE_ROOT%\WinLockDll_themida.dll %__EXE_ROOT%\WinLockDll.dll


echo ============================================
echo HackShiled
echo ============================================
rem CD를 인크립션하는 파일의 폴더로 지정해야한다.
rem HSBGen에서 exe를 실행할 떄 시작폴더가 CD로 되기 때문이다.
set OLD_DIR=%CD%
cd %__EXE_ROOT%

if not exist HShield (
	echo exe에 LMP를 적용하려면 HShield 폴더가 있어야 합니다.
	goto error_end
)

set HS_UTIL_PATH=%PB_SRC_ROOT%\HackShield\Bin\AntiCrack
copy %HS_UTIL_PATH%\HSBGen.exe %__EXE_ROOT%\HSBGen.exe
copy %HS_UTIL_PATH%\HSBGen.dll %__EXE_ROOT%\HSBGen.dll

set HSBGEN_EXE=%__EXE_ROOT%\HSBGen.exe
echo HSBGEN_EXE : %__EXE_ROOT%\HSBGen.exe


rem
rem %EXEC_NAME%.i3Exec
rem
rem ECHO "%EXEC_NAME%.i3Exec Encrypt"

copy "%HS_UTIL_PATH%\HSBGen_for_dll.ini" %__EXE_ROOT%\HSBGen.ini

rem 인크립션합니다.
rem HSBGen.exe [원본게임경로] [패킹지원여부:1] [패킹게임경로] [패킹파일실행:0][HSB 저장 경로:0] [파일 타입:0] [LMP 적용 여부:1]
rem %HSBGEN_EXE% %__EXE_ROOT%\%EXEC_NAME%_orig.i3Exec,1,%__EXE_ROOT%\%EXEC_NAME%.i3Exec,0,0,0,1
rem IF ERRORLEVEL 1 goto error_end

rem
rem WinLockDll.dll
rem
ECHO "WinLockDll.dll Encrypt"

rem 설정파일을 교체합니다.
copy "%HS_UTIL_PATH%\HSBGen_for_dll.ini" %__EXE_ROOT%\HSBGen.ini

rem 인크립션합니다.
%HSBGEN_EXE% %__EXE_ROOT%\WinLockDll_orig.dll,1,%__EXE_ROOT%\WinLockDll.dll,0,0,0,1
IF ERRORLEVEL 1 goto error_end

rem
rem %EXE_NAME%.exe
rem
ECHO "%EXE_NAME%.exe Encrypt"

rem 설정파일을 교체합니다.
copy "%HS_UTIL_PATH%\HSBGen_for_exe.ini" %__EXE_ROOT%\HSBGen.ini

rem 인크립션합니다.
rem unused themida
rem %HSBGEN_EXE% %__EXE_ROOT%\%EXE_NAME%.exe,0,%HSB_OUTPUT_PATH%\AntiCpX.hsb,1,1
rem used themida
%HSBGEN_EXE% %__EXE_ROOT%\%EXE_NAME%_orig.exe,1,%__EXE_ROOT%\%EXE_NAME%.exe,1,1111,%HSB_OUTPUT_PATH%\AntiCpX.hsb,1,1
IF ERRORLEVEL 1 goto error_end





cd %OLD_DIR%
echo 성공하였습니다.
goto __end

:error_end
echo LMP 실패

:__end
pause