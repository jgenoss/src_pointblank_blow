rem @echo off

rem VS 2003
rem
rem if not defined VS71COMNTOOLS (
rem 	echo In order to build this project you need MS Visual Studio 2003 to be installed.
rem 	echo http://msdn.microsoft.com/vstudio/express/downloads/default.aspx
rem 	exit /b
rem )
rem call "%VS71COMNTOOLS%\vsvars32.bat" || exit /b 1

rem VS 2008
rem
rem if not defined VS90COMNTOOLS (
rem 	echo In order to build this project you need MS Visual Studio 2003 to be installed.
rem	echo http://msdn.microsoft.com/vstudio/express/downloads/default.aspx
rem	exit
rem )
rem call "%VS90COMNTOOLS%\vsvars32.bat" || exit /b 1

rem VS 2014

if not defined VS140COMNTOOLS (
	echo In order to build this project you need MS Visual Studio 2003 to be installed.
	echo http://msdn.microsoft.com/vstudio/express/downloads/default.aspx
	exit
)
call "%VS140COMNTOOLS%\vsvars32.bat" || exit /b 1

rem
rem ROOT 경로를 저장한다.
rem
set CUR_DIR=%CD%
cd ../../../
set ROOT=%CD%


set I3_ROOT=%ROOT%\i3

set ENGINE_PATH=%I3_ROOT%\Project\I3_DX\I3_DX.sln

set I3_RELEASE_DLL_ROOT=%I3_ROOT%\lib\DirectX\ReleaseDLL
set PHYS_X_RELEASE_DLL=%I3_ROOT%\PhysX\bin\win32\Release


rem 포인트 블랭크 Path
set PB_ROOT=%ROOT%\PointBlank
set PB_SRC_ROOT=%PB_ROOT%\Source_New


rem ClientExes Path
set CLIENT_EXES=F:\PB_Backup_All\Trunk\ClientExes


rem 빌드 구성
set CONFIG_QA=Release(BugTrap)
set CONFIG_LIVE=Distribute
set CONFIG_DEBUG=Debug(BugTrap)

rem 코드 사이닝
set CODE_SIGN_DIR=%PB_SRC_ROOT%\CodeSign
set PASSWORD="1234"

echo **********************************
echo define setting
echo ROOT : %ROOT%
echo I3_ROOT : %I3_ROOT%
echo PB_ROOT : %PB_ROOT%
echo CLIENT_EXES : %CLIENT_EXES%
echo **********************************

cd %CUR_DIR%