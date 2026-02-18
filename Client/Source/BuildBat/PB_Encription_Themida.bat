
rem
rem %1 : PB 소스 경로
rem %2 : 인크립션할 파일의 경로
rem
rem 아래 스크립트중 설정파일을 교체하는 이유는 명령줄 모든 HSBGen의 옵션을 설정해줄 수 없기 때문이다.
rem 대표적인 문제로는 InitStep에 대한 옵션을 줄 수 없는 것이다.
rem

set __EXE_ROOT=%1

rem ////////////////////////////////////////////////////////////////////
echo 원본 복사
rem copy %__EXE_ROOT%\%EXEC_NAME%.i3Exec %__EXE_ROOT%\%EXEC_NAME%_orig.i3Exec
copy %__EXE_ROOT%\%EXE_NAME%.exe %__EXE_ROOT%\%EXE_NAME%_orig.exe
copy %__EXE_ROOT%\WinLockDll.dll %__EXE_ROOT%\WinLockDll_orig.dll
copy %__EXE_ROOT%\i3GfxDx.dll %__EXE_ROOT%\i3GfxDx_orig.dll

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

echo Themida protection %EXE_NAME%.exe
Themida.exe /protect PB_exe.tmd /inputfile %__EXE_ROOT%\%EXE_NAME%.exe /outputfile %__EXE_ROOT%\%EXE_NAME%.exe /inifile Themida.ini
IF NOT ERRORLEVEL == 0 exit /b 1

echo Themida protection WinLockDll.dll
Themida.exe /protect winlock.tmd /inputfile %__EXE_ROOT%\WinLockDll.dll /outputfile %__EXE_ROOT%\WinLockDll.dll
IF NOT ERRORLEVEL == 0 exit /b 1

rem echo Themida protection i3GfxDx.dll
rem Themida.exe /protect winlock.tmd /inputfile %__EXE_ROOT%\i3GfxDx.dll /outputfile %__EXE_ROOT%\i3GfxDx.dll
rem IF NOT ERRORLEVEL == 0 exit /b 1


cd %OLD_DIR%

echo 인크립션 종료
