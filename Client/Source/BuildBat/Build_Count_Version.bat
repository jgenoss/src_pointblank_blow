@echo off
rem 빌드 카운트 추가

rem 빌드 카운트 파일
set BUILD_VERSION=%cd%\..\ClientSource\Source\BuildVerInfo.h
set N1="BUILD_VER_GAME"
set N2="BUILD_VER_BUILD"
set N3="BUILD_VER_SC_PROTOCOL"
set N4="BUILD_COUNT"

for /f "tokens=3" %%i in ('findstr %N1% %BUILD_VERSION%') do (set N1=%%i)
for /f "tokens=3" %%i in ('findstr %N2% %BUILD_VERSION%') do (set N2=%%i)
for /f "tokens=3" %%i in ('findstr %N3% %BUILD_VERSION%') do (set N3=%%i)
for /f "tokens=3" %%i in ('findstr %N4% %BUILD_VERSION%') do (set N4=%%i)

Set BUILD_COUNT_VERSION=%N1%.%N2%.%N3%.%N4%

echo %BUILD_COUNT_VERSION%
