set BACKUP_ROOT=D:\PB_Backup\%date%

set PB_EXE_QA=%CLIENT_EXES%\PointBlank
set PB_EXE_LIVE=%CLIENT_EXES%\NCBuild
set PB_HACKSHIELD=%PB_ROOT%\HackShield
set PB_SOURCE_PATH=%PB_ROOT%

set PB_EXE_QA_NAME=KoreaQA_Old
set PB_EXE_LIVE_NAME=KoreaLive_Old

call Backup_PBSource.bat

if not "Auto" == "%1" (
	call Beep.exe
	pause
)