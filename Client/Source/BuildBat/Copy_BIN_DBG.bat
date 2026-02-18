@echo off
echo Start Copy_BIN_DBG.bat
call Copy_BIN.bat %1 %2
call Copy_DBG.bat %1 %2
echo End Copy_BIN_DBG.bat