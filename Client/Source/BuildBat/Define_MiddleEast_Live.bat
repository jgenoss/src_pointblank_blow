@echo off

set LOCALE=MiddleEast
set EXE_NAME=PointBlank
set EXEC_NAME=PointBlank
set LOG_PATH=.
set EXE_ROOT=%CLIENT_EXES%\MiddleEastLive
set EXE_LIVE_FILE_DIR=%EXE_ROOT%
set BUILD_NAME=%LOCALE%_Live
set DIST_TYPE=Default
set SECURITY_TYPE=HackShield
set SECURITY_PATH=HackShield_R.5.6.32.1(build428)
set HSB_OUTPUT_PATH=%PB_SRC_ROOT%\HackShield\Server\HackShield_MiddleEast
set BUILD_CONFIG_QA=%CONFIG_QA%_MiddleEast
set BUILD_CONFIG_LIVE=%CONFIG_Live%_MiddleEast
set BUILD_RESULT_QA=%PB_SRC_ROOT%\ClientSource\%BUILD_CONFIG_QA%
set BUILD_RESULT_LIVE=%PB_SRC_ROOT%\ClientSource\%BUILD_CONFIG_LIVE%