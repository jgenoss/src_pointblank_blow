@echo off


set LOCALE=Thailand
set EXE_NAME=PointBlank
set EXEC_NAME=PointBlank
set LOG_PATH=.
set EXE_ROOT=%CLIENT_EXES%\ThailandLive\Apps\PBTH
set EXE_QA_FILE_DIR=%EXE_ROOT%\QABuild
set EXE_LIVE_FILE_DIR=%EXE_ROOT%
set BUILD_NAME=%LOCALE%_Live
set DIST_TYPE=NC_Distribute
set SECURITY_TYPE=HackShield
set SECURITY_PATH=HackShield_R.5.7.3.1(Build467)
set HSB_OUTPUT_PATH=%PB_SRC_ROOT%\HackShield\Server\HackShield_Thailand
set BUILD_CONFIG_QA=%CONFIG_QA%_Thai
set BUILD_CONFIG_LIVE=%CONFIG_Live%_Thai
set BUILD_RESULT_QA=%PB_SRC_ROOT%\ClientSource\%BUILD_CONFIG_QA%
set BUILD_RESULT_LIVE=%PB_SRC_ROOT%\ClientSource\%BUILD_CONFIG_LIVE%

