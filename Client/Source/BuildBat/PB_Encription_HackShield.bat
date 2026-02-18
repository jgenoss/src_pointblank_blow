
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
copy %__EXE_ROOT%\i3GfxDx.dll %__EXE_ROOT%\i3GfxDx_orig.dll

copy %__EXE_ROOT%\NxCharacter.2.8.1.dll %__EXE_ROOT%\NxCharacter.2.8.1_orig.dll
copy %__EXE_ROOT%\i3BaseDx.dll %__EXE_ROOT%\i3BaseDx_orig.dll
copy %__EXE_ROOT%\i3FrameworkDx.dll %__EXE_ROOT%\i3FrameworkDx_orig.dll


if defined Enc_WinLockDLL (
	copy %__EXE_ROOT%\WinLockDll.dll %__EXE_ROOT%\WinLockDll_orig.dll
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

if defined Enc_WinLockDLL (
	echo Themida protection WinLockDll.dll
	Themida.exe /protect winlock.tmd /inputfile %__EXE_ROOT%\WinLockDll.dll /outputfile %__EXE_ROOT%\WinLockDll.dll
	IF NOT ERRORLEVEL == 0 exit /b 1
)

cd %OLD_DIR%





echo ============================================
echo HackShiled
echo ============================================
rem CD를 인크립션하는 파일의 폴더로 지정해야한다.
rem HSBGen에서 exe를 실행할 떄 시작폴더가 CD로 되기 때문이다.
set OLD_DIR=%CD%
f:
cd %__EXE_ROOT%

if not exist HShield (
	echo __EXE_ROOT : %__EXE_ROOT%
	echo exe에 LMP를 적용하려면 HShield 폴더가 있어야 합니다.
	exit /b 1
)

set HS_UTIL_PATH=%PB_SRC_ROOT%\HackShield\Bin\AntiCrack
set HS_GEN_PATH=%PB_SRC_ROOT%\%SECURITY_PATH%\Bin\AntiCrack

copy %HS_GEN_PATH%\HSBGen.exe %__EXE_ROOT%\HSBGen.exe
copy %HS_GEN_PATH%\HSBGen.dll %__EXE_ROOT%\HSBGen.dll

set HSBGEN_EXE=%__EXE_ROOT%\HSBGen.exe
echo HSBGEN_EXE : %__EXE_ROOT%\HSBGen.exe


rem 원본 파일을 백업합니다.
rem copy %__EXE_ROOT%\%EXEC_NAME%.i3Exec %__EXE_ROOT%\%EXEC_NAME%_themida.i3Exec
copy %__EXE_ROOT%\%EXE_NAME%.exe %__EXE_ROOT%\%EXE_NAME%_themida.exe
copy %__EXE_ROOT%\i3GfxDx.dll	 %__EXE_ROOT%\i3GfxDx_themida.dll

if defined Enc_WinLockDLL (
	copy %__EXE_ROOT%\WinLockDll.dll %__EXE_ROOT%\WinLockDll_themida.dll
)



rem
rem %EXEC_NAME%.i3Exec
rem
rem ECHO "%EXEC_NAME%.i3Exec Encrypt"

rem copy "%HS_UTIL_PATH%\HSBGen_for_dll.ini" %__EXE_ROOT%\HSBGen.ini

rem 인크립션합니다.
rem HSBGen.exe [원본게임경로] [패킹지원여부:1] [패킹게임경로] [패킹파일실행:0][HSB 저장 경로:0] [파일 타입:0] [LMP 적용 여부:1]
rem %HSBGEN_EXE% %__EXE_ROOT%\%EXEC_NAME%_orig.i3Exec,1,%__EXE_ROOT%\%EXEC_NAME%.i3Exec,0,0,0,1
rem IF ERRORLEVEL 1 exit /b 1



rem 설정파일을 교체합니다.
copy "%HS_UTIL_PATH%\HSBGen_for_dll.ini" %__EXE_ROOT%\HSBGen.ini

if defined Enc_WinLockDLL (
	rem
	rem WinLockDll.dll
	rem
	ECHO "WinLockDll.dll Encrypt"

	rem 인크립션합니다.
	%HSBGEN_EXE% %__EXE_ROOT%\WinLockDll_orig.dll,1,%__EXE_ROOT%\WinLockDll.dll,0,0,0,1
	IF ERRORLEVEL 1 exit /b 1
)

rem i3GfxDx인크립션합니다.
%HSBGEN_EXE% %__EXE_ROOT%\i3GfxDx_orig.dll,1,%__EXE_ROOT%\i3GfxDx.dll,0,0,0,1
IF ERRORLEVEL 1 exit /b 1

%HSBGEN_EXE% %__EXE_ROOT%\NxCharacter.2.8.1_orig.dll,1,%__EXE_ROOT%\NxCharacter.2.8.1.dll,0,0,0,1
IF ERRORLEVEL 1 exit /b 1
%HSBGEN_EXE% %__EXE_ROOT%\i3BaseDx_orig.dll,1,%__EXE_ROOT%\i3BaseDx.dll,0,0,0,1
IF ERRORLEVEL 1 exit /b 1
%HSBGEN_EXE% %__EXE_ROOT%\i3FrameworkDx_orig.dll,1,%__EXE_ROOT%\i3FrameworkDx.dll,0,0,0,1
IF ERRORLEVEL 1 exit /b 1



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
IF ERRORLEVEL 1 exit /b 1

E:
cd %OLD_DIR%





rem ////////////////////////////////////////////////////////////////////
echo revert %__EXE_ROOT%HShield
svn revert --depth=infinity %__EXE_ROOT%\HShield

echo hsb commit  %HSB_OUTPUT_PATH%
set OLD_DIR=%CD%
cd %HSB_OUTPUT_PATH%

svn update --accept mine-full
svn commit -m "hsb commit" --username pbbuild --password build1234
cd %OLD_DIR%
rem ////////////////////////////////////////////////////////////////////



echo 인크립션 종료
