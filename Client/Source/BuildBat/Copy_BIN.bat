@echo off
rem
rem   실행파일, dll 등 빌드 결과물을 복사한다.
rem
rem   실행전 설정이 필요한 환경변수 :
rem     - DEST_DIR : 복사될 파일의 경로
rem     - SRC_DIR  : 원본파일의 경로
rem

echo Start Copy_BIN.bat
echo SRC_DIR : %1
echo DEST_DIR : %2

set SRC_DIR=%1
set DEST_DIR=%2

MD %DEST_DIR%

copy "%SRC_DIR%\*.dll" %DEST_DIR%
copy "%SRC_DIR%\*.exe" %DEST_DIR%
rem copy "%SRC_DIR%\*.i3Exec" %DEST_DIR%

echo End Copy_BIN.bat