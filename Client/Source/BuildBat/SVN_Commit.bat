@echo off
if %CCNetIntegrationStatus% == Success (
	svn commit -m "Auto commit %CCNetProject%"
)