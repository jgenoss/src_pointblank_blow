
mkdir %BACKUP_ROOT%
mkdir %BACKUP_ROOT%\PointBlank
mkdir %BACKUP_ROOT%\PointBlank\Media
mkdir %BACKUP_ROOT%\ClientExes

svn export %I3_ROOT% %BACKUP_ROOT%\i3 --force

svn export %PB_SOURCE_PATH%\Source_New %BACKUP_ROOT%\PointBlank\Source_New --force

svn export %PB_SOURCE_PATH%\GuiWorking %BACKUP_ROOT%\PointBlank\GuiWorking --force

svn export %PB_SOURCE_PATH%\HackShield %BACKUP_ROOT%\PointBlank\HackShield --force

svn export %PB_SOURCE_PATH%\BuildBat %BACKUP_ROOT%\PointBlank\BuildBat --force

svn export %PB_EXE_QA% %BACKUP_ROOT%\ClientExes\%PB_EXE_QA_NAME% --force

svn export %PB_EXE_LIVE% %BACKUP_ROOT%\ClientExes\%PB_EXE_LIVE_NAME% --force

rem 백업시에 엔진 빌드된 DLL들을 복사해둔다.
copy /y %I3_ROOT%\lib\DirectX\ReleaseDLL %BACKUP_ROOT%\i3\lib\DirectX\ReleaseDLL
copy /y %I3_ROOT%\PhysX\bin\win32\Release %BACKUP_ROOT%\i3\PhysX\bin\win32\Release

