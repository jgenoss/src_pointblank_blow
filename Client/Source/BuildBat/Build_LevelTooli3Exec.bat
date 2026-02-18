@echo off
@setLocal

set PBI3EXEC_COPYTO=%1
set PBI3LIB_COPYT1=%2

call Define.bat
IF ERRORLEVEL 1 goto failed

set PBCLIENT_SRC_PATH="%PB_ROOT%\Source_New\ClientSource\ClientSource.sln"
set PBI3EXEC_BIN="%PB_ROOT%\Source_New\ClientSource\Distribute_ForLevelTool\PointBlank_ForLevelTool.i3Exec"
set PBI3EXEC_LIB="%PB_ROOT%\Source_New\ClientSource\Distribute_ForLevelTool\PointBlank_ForLevelTool.lib"

echo devenv.com %PBCLIENT_SRC_PATH% /Build "Distribute_ForLevelTool"
rem devenv.com %PBCLIENT_SRC_PATH% /Build "Distribute_ForLevelTool"

if ERRORLEVEL 1 goto failed
if not "%PBI3EXEC_COPYTO%" == "" (
    echo copy /y %PBI3EXEC_BIN% %PBI3EXEC_COPYTO%
    copy /y %PBI3EXEC_BIN% %PBI3EXEC_COPYTO%
)
if ERRORLEVEL 1 goto failed
if not "%PBI3LIB_COPYT1%" == "" (
    echo copy /y %PBI3EXEC_LIB% %PBI3LIB_COPYT1%
    copy /y %PBI3EXEC_LIB% %PBI3LIB_COPYT1%
)

echo PointBlank_ForLevelTool.i3Exec ºôµå ¼º°ø!
goto endtag

:failed
echo *****************************************************
echo ERR! - PointBlank_ForLevelTool.i3Exec ºôµå ½ÇÆÐ!!!!!!
echo *****************************************************
@endLocal
exit /b 1

:endtag
@endLocal
exit /b 0


