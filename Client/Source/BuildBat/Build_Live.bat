rem @echo off

rem 지정되어야 하는 환경 변수
rem
rem 1. BUILD_PATH=
rem 2. BUILD_TYPE= 빌드 타입 /Rebuild, /Build
rem 3. LOCALE= 국가
rem 4. LOG_PATH= 로그파일 경로
rem 5. EXE_ROOT=
rem 6. EXE_LIVE_FILE_DIR=
rem 7. BUILD_RESULT_QA=
rem 8. BUILD_RESULT_LIVE=
rem 9. BUILD_NAME=
rem 10. DIST_TYPE= Default, NC_Distribute
rem 11. SECURITY_TYPE= NProtect, Indonesia, Themida, Null
rem 12. BUILD_CONFIG_QA=
rem 13. BUILD_CONFIG_LIVE=


set BUILD_RESULT_FILE_NAME_QA=%LOG_PATH%\Log_%LOCALE%_LiveQA_%BUILD_TYPE:~1%.txt
set BUILD_RESULT_FILE_NAME_LIVE=%LOG_PATH%\Log_%LOCALE%_Live_%BUILD_TYPE:~1%.txt

if exist %BUILD_RESULT_FILE_NAME_QA% del %BUILD_RESULT_FILE_NAME_QA%
if exist %BUILD_RESULT_FILE_NAME_LIVE% del %BUILD_RESULT_FILE_NAME_LIVE%

echo LogFile Name (LiveQA): %BUILD_RESULT_FILE_NAME_QA%
echo LogFile Name (Live): %BUILD_RESULT_FILE_NAME_LIVE%


if not %DIST_TYPE% == NC_Distribute goto NO_QA_BUILD
echo --------------------------------------------------------
echo QA 빌드 ( DIST_TYPE 가 NC_Distribute 일때만 QA를 빌드합니다. )
echo --------------------------------------------------------
echo devenv.com %BUILD_PATH% %BUILD_TYPE% %BUILD_CONFIG_QA% /out %BUILD_RESULT_FILE_NAME_QA%
_buildIB.bat ib %BUILD_PATH% %BUILD_TYPE% %BUILD_CONFIG_QA% %BUILD_RESULT_FILE_NAME_QA%
:NO_QA_BUILD

echo --------------------------------------------------------
echo Live 빌드
echo --------------------------------------------------------
echo "%BUILD_PATH% %BUILD_TYPE% %BUILD_CONFIG_LIVE%"

echo devenv.com %BUILD_PATH% %BUILD_TYPE% %BUILD_CONFIG_LIVE% /out %BUILD_RESULT_FILE_NAME_LIVE%
_buildIBLive.bat ib %BUILD_PATH% %BUILD_TYPE% %BUILD_CONFIG_LIVE% %BUILD_RESULT_FILE_NAME_LIVE%


