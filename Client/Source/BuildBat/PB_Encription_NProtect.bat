
rem
rem 인크립터 파일의 경로를 설정한다.
rem

echo NProtect

set __EXE_ROOT=%1
set NPROTECT_EXE=%PB_ROOT%\Source_New\ClientSource\nProtect\NPGE\NPGEClient.exe

echo %__EXE_ROOT%

rem *****************************************
rem 주의
rem
rem 1. 비스타계열 부터는 NPGEClient.exe 를 배치파일에서 실행시에 실행파일명과 인자를 붙입니다.
rem ex) NPGEClient.exe/Point:Blank....   (o)
rem ex) NPGEClient.exe /Point:Blank....  (x)
rem
rem 2. NPGEClient.exe 를 FullPath를 지정지에는 exe패스에 쌍따옴표로 감쌉니다.
rem "C:\NPGEClient.exe"   (o)
rem C:\NPGEClient.exe     (x)
rem
rem *****************************************

rem
rem %EXEC_NAME%.i3Exec
rem
rem ECHO "%EXEC_NAME%.i3Exec Encrypt"
rem "%NPROTECT_EXE%"/Point:Blank!@#:2:"%__EXE_ROOT%\%EXEC_NAME%.i3Exec";"%__EXE_ROOT%\%EXE_NAME%.exe run"
rem IF ERRORLEVEL 1 exit /b



rem
rem %EXE_NAME%.exe
rem
ECHO "%EXE_NAME%.exe Encrypt"
"%NPROTECT_EXE%"/Point:Blank!@#:25:"%__EXE_ROOT%\%EXE_NAME%.exe"
IF ERRORLEVEL 1 exit /b


