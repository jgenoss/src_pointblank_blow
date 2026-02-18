@ECHO off

echo --------------------------------------------------------
echo 빌드 카운트 추가
echo --------------------------------------------------------

call %cd%\\..\\ClientSource\\BuildCounter.exe

rem echo "%cd%\..\ClientSource\Source\BuildVerInfo.h"

rem echo svn commit "%cd%\..\ClientSource\Source\BuildVerInfo.h" -m "Build Count Auto commit"
rem svn commit "%cd%\..\ClientSource\Source\BuildVerInfo.h" -m "Build Count Auto commit"

rem if not %ERRORLEVEL% equ 0 (
rem  echo "ERR - SVN Commit에 실패했습니다. (%BUILD_COUNT_HEADER%)"
rem  goto fail
rem )

rem :fail

if %ERRORLEVEL% equ -1 (
  echo 빌드 카운팅 실패. 빌드 종료합니다.
  exit 1
)

set BUILD_COUNT=%ERRORLEVEL%

