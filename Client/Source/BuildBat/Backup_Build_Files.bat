@echo off
rem
rem   Build 결과물을 백업한다.
rem
rem   실행전 설정이 필요한 환경변수 :
rem     - BUILD_NAME : 빌드 이름
rem     - SRC_ROOT(1~3)   : 소스 파일의 경로
rem

rem 파일이 저장될 폴더 경로.
echo Start Backup_Build_Files

set SRC_DIR=%2
set DEST_DIR="%ROOT%\BuildFiles\%1\%TIME_STAMP%"

if not exist "%DEST_DIR%" mkdir "%DEST_DIR%"

call Copy_BIN_DBG.bat "%SRC_DIR%" "%DEST_DIR%"


