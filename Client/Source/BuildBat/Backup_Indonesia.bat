set BACKUP_ROOT=D:\PB_Backup_Indonesia\%date%

Define.bat

set PB_EXE_QA=%CLIENT_EXES%\IndonesiaQA
set PB_EXE_LIVE=%CLIENT_EXES%\IndonesiaLive
set PB_HACKSHIELD=%PB_ROOT%\HackShield
set PB_SOURCE_PATH=%PB_ROOT%

set PB_EXE_QA_NAME=IndonesiaQA
set PB_EXE_LIVE_NAME=IndonesiaLive

call Backup_PBSource.bat

if not "Auto" == "%1" (
	call Beep.exe
	pause
)