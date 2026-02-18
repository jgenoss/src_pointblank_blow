@echo off
rem
rem   TIME_STAMP 환경변수에 yyyymmdd_hhmmss 형식으로 시간을 포맷팅한다.
rem


rem %date% 는 yyyy-mm-dd 형식으로 날짜를 반환한다.
rem %date:-=% 는 하이픈(-)을 제거한다. 결과는 yyyymmdd 이다.
set date2=%date:-=%

rem %time%은 hh:mm:ss.ss 형식으로 반환한다.
set time2=%time%

rem 시간을 hhmmss형식으로 만든다.
set time3=%time2:~0,2%%time2:~3,2%%time2:~6,2%

rem 빌드 카운트 버전을 세팅한다
call Build_Count_Version.bat

rem 환경 변수에 포맷팅한 시간을 저장한다.
set TIME_STAMP="%date2%_%time3%_%BUILD_COUNT_VERSION%"

