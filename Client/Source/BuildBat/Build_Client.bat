@echo off

set Locale_Val=%1
set BuildType_Val=%2

echo ----------------------------------------------------------
echo %Locale_Val% (%BuildType_Val%) 클라이언트 빌드를 합니다.
echo ----------------------------------------------------------

call Define_%Locale_Val%_%BuildType_Val%.bat

rem ----------------------------------------------------------
rem 클라이언트 업데이트
rem ----------------------------------------------------------

echo - Update %EXE_ROOT%
svn update %EXE_ROOT%

rem ----------------------------------------------------------
rem 클라이언트 빌드를 수행합니다.
rem ----------------------------------------------------------

call Build_%BuildType_Val%.bat
IF ERRORLEVEL 1 exit /b 1

exit /b 0
