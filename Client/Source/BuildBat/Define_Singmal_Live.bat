@echo off


set LOCALE=Singmal
set EXE_NAME=PointBlank
set EXEC_NAME=PointBlank
set LOG_PATH=.
set EXE_ROOT=%CLIENT_EXES%\SingmalLive
set EXE_LIVE_FILE_DIR=%EXE_ROOT%
set BUILD_NAME=%LOCALE%_Live
set DIST_TYPE=Default
set SECURITY_TYPE=HackShield
set SECURITY_PATH=HackShield

set BUILD_CONFIG_QA=%CONFIG_QA%_Singmal
set HSB_OUTPUT_PATH=%PB_SRC_ROOT%\HackShield\Server\HackShield_Singmal
set BUILD_CONFIG_LIVE=%CONFIG_Live%_Singmal
set BUILD_RESULT_QA=%PB_SRC_ROOT%\ClientSource\%BUILD_CONFIG_QA%
set BUILD_RESULT_LIVE=%PB_SRC_ROOT%\ClientSource\%BUILD_CONFIG_LIVE%

