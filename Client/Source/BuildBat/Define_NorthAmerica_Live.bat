@echo off


set LOCALE=NorthAmerica
set EXE_NAME=PBlackout
set EXEC_NAME=PBlackout
set LOG_PATH=.
set EXE_ROOT=%CLIENT_EXES%\NorthAmericaLive
set EXE_LIVE_FILE_DIR=%EXE_ROOT%
set BUILD_NAME=%LOCALE%_Live
set DIST_TYPE=Default
set SECURITY_TYPE=HackShield
set SECURITY_PATH=HackShield_R.5.7.3.1(Build467)

set BUILD_CONFIG_QA=%CONFIG_QA%_NorthAmerica
set HSB_OUTPUT_PATH=%PB_SRC_ROOT%\HackShield\Server\HackShield_NorthAmerica
set BUILD_CONFIG_LIVE=%CONFIG_Live%_NorthAmerica
set BUILD_RESULT_QA=%PB_SRC_ROOT%\ClientSource\%BUILD_CONFIG_QA%
set BUILD_RESULT_LIVE=%PB_SRC_ROOT%\ClientSource\%BUILD_CONFIG_LIVE%

