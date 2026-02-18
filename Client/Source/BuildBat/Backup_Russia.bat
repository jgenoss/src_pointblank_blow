set BACKUP_ROOT=D:\PB_Backup_Russia\%date%

set PB_EXE_QA=%CLIENT_EXES%\RussiaQA
set PB_EXE_LIVE=%CLIENT_EXES%\RussiaLive
set PB_HACKSHIELD=%PB_ROOT%\HackShield
set PB_SOURCE_PATH=%PB_ROOT%

set PB_EXE_QA_NAME=RussiaQA
set PB_EXE_LIVE_NAME=RussiaLive

call Backup_PBSource.bat

if not "Auto" == "%1" (
	call Beep.exe
	pause
)