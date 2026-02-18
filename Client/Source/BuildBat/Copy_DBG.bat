@echo off
rem
rem   디버깅 파일들을 복사한다.
rem
rem   실행전 설정이 필요한 환경변수 :
rem     - DEST_DIR : 복사될 파일의 경로
rem     - SRC_DIR  : 원본파일의 경로
rem

echo Start Copy_DBG.bat
echo SRC_DIR : %1
echo DEST_DIR : %2

set SRC_DIR=%1
set DEST_DIR=%2

MD %DEST_DIR%

copy "%SRC_DIR%\*.pdb" %DEST_DIR%
copy "%SRC_DIR%\*.map" %DEST_DIR%

echo End Copy_DBG.bat