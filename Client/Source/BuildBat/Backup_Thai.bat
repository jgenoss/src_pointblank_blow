set BACKUP_ROOT=D:\PB_Backup_Thai\%date%

set PB_EXE_QA=%CLIENT_EXES%\ThailandQA
set PB_EXE_LIVE=%CLIENT_EXES%\ThailandLive
set PB_HACKSHIELD=%PB_ROOT%\HackShield
set PB_SOURCE_PATH=%PB_ROOT%

set PB_EXE_QA_NAME=ThailandQA
set PB_EXE_LIVE_NAME=ThailandLive

call Backup_PBSource.bat

if not "Auto" == "%1" (
	call Beep.exe
	pause
)