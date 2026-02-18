@echo off
rem
rem   실행파일, dll의 결과물을 CodeSign 폴더에 넣는다.
rem
rem   - SRC_DIR  : 원본파일의 경로
rem   - CODE_SIGN_DIR : Define.bat에 정의되어 있음.
rem

echo Start Code Signing

set EXE_LIVE_DIR=%1
set SIGN_FILE=www.zepetto.com.pfx

copy "%EXE_LIVE_DIR%\%EXE_NAME%.exe" %CODE_SIGN_DIR%

echo - 실행파일과 Exec파일을 서명합니다.
echo - 복사 경로 : %CODE_SIGN_DIR%
echo - 실행 명령 : %CODE_SIGN_DIR%\signtool.exe
echo - 인증 파일명 : %SIGN_FILE%

%CODE_SIGN_DIR%\SignTool.exe sign /v /f %CODE_SIGN_DIR%\www.zepetto.com.pfx /p zepetto12#$ /d "Point Blank" /t http://timestamp.verisign.com/scripts/timestamp %CODE_SIGN_DIR%\%EXE_NAME%.exe

echo - 디지털 서명 파일들을 기존 경로로 복사합니다.
copy "%CODE_SIGN_DIR%\%EXE_NAME%.exe" %EXE_LIVE_DIR%

echo - 디지털 서명을 완료했습니다.