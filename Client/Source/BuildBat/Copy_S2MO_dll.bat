@echo off
rem
rem   S2MO_32bit.dll 파일을 복사한다.
rem
rem   실행전 설정이 필요한 환경변수 :
rem     - DEST_DIR : 복사될 파일의 경로
rem     - SRC_DIR  : 원본파일의 경로
rem

echo Start Copy_S2MO_dll.bat
echo SRC_DIR : %1
echo DEST_DIR : %2

set SRC_DIR=%1
set DEST_DIR=%2

MD %DEST_DIR%

copy "%SRC_DIR%\S2MO_32bit.dll" %DEST_DIR%
copy "%SRC_DIR%\S2MOd_32bit.dll" %DEST_DIR%

echo End Copy_S2MO_dll.bat