rem @echo off

rem 지정되어야 하는 환경 변수
rem
rem 1. BUILD_PATH=
rem 2. BUILD_TYPE= 빌드 타입 /Rebuild, /Build
rem 3. LOCALE= 국가
rem 4. LOG_PATH= 로그파일 경로
rem 5. EXE_ROOT=
rem 6. BUILD_RESULT_QA=
rem 7. BUILD_NAME=
rem 8. BUILD_CONFIG_QA=


set BUILD_RESULT_FILE_NAME_QA=%LOG_PATH%\Log_%LOCALE%_QA_%BUILD_TYPE:~1%.txt

del %BUILD_RESULT_FILE_NAME_QA%


echo --------------------------------------------------------
echo QA 빌드
echo --------------------------------------------------------
echo devenv.com %BUILD_PATH% %BUILD_TYPE% %BUILD_CONFIG_QA% /out %BUILD_RESULT_FILE_NAME_QA%
_buildIB.bat ib %BUILD_PATH% %BUILD_TYPE% %BUILD_CONFIG_QA% %BUILD_RESULT_FILE_NAME_QA%
IF ERRORLEVEL 1 echo QA 빌드 실패
IF ERRORLEVEL 1 exit /b 1






