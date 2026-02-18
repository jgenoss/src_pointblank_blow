@echo off

rem
rem %1 : PB 소스 경로
rem

set __EXE_ROOT=%1

echo ============================================
echo XTrap CS3 파일 생성...
echo ============================================
set OLD_DIR=%CD%
set XTRAP_ROOT=%PB_SRC_ROOT%\Xtrap
set XPROTECT_UTIL_PATH=%XTRAP_ROOT%\Tool

set XPROTECT_EXE=%XPROTECT_UTIL_PATH%\XProtect
echo XProtect Path : %XPROTECT_UTIL_PATH%\XProtect.exe

set CS3_TARGET_FILE=%EXE_LIVE_FILE_DIR%\%EXE_NAME%.exe
echo Target Path : %CS3_TARGET_FILE%

set CS3_OUTPUT_FILE=%EXE_ROOT%\PointBlank.CS3
echo CS3 Path: %CS3_OUTPUT_FILE%

if not exist %XPROTECT_UTIL_PATH%\XProtect.exe (
	echo !! XProtect : %XPROTECT_UTIL_PATH%\XProtect.exe
	echo !! CS3를 생성하기 위해 XProtect 툴파일이 있어야 합니다.
	exit /b 1
)

if not exist %EXE_ROOT%\Xtrap (
	echo !! Xtarp 보안 모듈 : %EXE_ROOT%\Xtrap
	echo !! 클라이언트를 실행하려면 Xtrap 모듈 파일이 있어야 합니다.
	exit /b 1
)


rem ////////////////////////////////////////////////////////////////////
echo ============================================
echo Themida
echo ============================================
rem
rem		One of the following codes will be returned:
rem		0 protection was successful.
rem		1 Project file does not exist or invalid.
rem		2 File to protect cannot be opened.
rem		3 File already protected.
rem		4 Error in inserted SecureEngine macros.
rem		5 Fatal error while protecting file.
rem		6 Cannot write protected file to disk.
rem

rem		themida /protect YourProjectFile.tmd

set OLD_DIR=%CD%
cd %PB_SRC_ROOT%\Themida

rem echo Themida protection %EXEC_NAME%.i3Exec
rem Themida.exe /protect PBi3exec.tmd /inputfile %__EXE_ROOT%\%EXEC_NAME%.i3Exec /outputfile %__EXE_ROOT%\%EXEC_NAME%.i3Exec
rem IF NOT ERRORLEVEL == 0 exit /b 1

rem PointBlank.exe Themida 패킹 시 /inifile Themida.ini 있을 경우 
rem 핵쉴드에서 클라이언트 강제 종료 시키기 때문에 지웠습니다.

echo Themida protection %EXE_NAME%.exe
Themida.exe /protect PB_exe.tmd /inputfile %__EXE_ROOT%\%EXE_NAME%.exe /outputfile %__EXE_ROOT%\%EXE_NAME%.exe
IF NOT ERRORLEVEL == 0 exit /b 1

rem echo Themida protection i3GfxDx.dll
rem Themida.exe /protect winlock.tmd /inputfile %__EXE_ROOT%\i3GfxDx.dll /outputfile %__EXE_ROOT%\i3GfxDx.dll
rem IF NOT ERRORLEVEL == 0 exit /b 1
echo ////////////////////////////////////////////////////////////////////////////

rem
rem %EXE_NAME%.exe
rem
ECHO "CS3 file creating from %EXE_NAME%.exe"

rem --------------------------------------------------------------------
rem XProtect 툴을 이용하여 CS3 파일을 생성합니다.
rem --------------------------------------------------------------------
%XPROTECT_EXE% -t "%CS3_TARGET_FILE%" -s "%CS3_OUTPUT_FILE%" -l "%CS3_TARGET_FILE%" -a "1234"
IF ERRORLEVEL 1 exit /b 1

rem --------------------------------------------------------------------
rem CS3 파일을 복사합니다.
rem --------------------------------------------------------------------
copy %CS3_OUTPUT_FILE% %CS3_OUTPUT_PATH%

cd %OLD_DIR%

rem --------------------------------------------------------------------
rem CS3 파일을 커밋합니다.
rem --------------------------------------------------------------------
echo CS3 commit  %CS3_OUTPUT_FILE%
set OLD_DIR=%CD%
cd %CS3_OUTPUT_PATH%

svn update --accept mine-full
svn commit -m "XTrap CS3 commit"
cd %OLD_DIR%
rem ////////////////////////////////////////////////////////////////////



echo CS3 생성 종료
