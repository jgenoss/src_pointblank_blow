DECLARE
	@DBNamePostfix NVARCHAR(50),
	@DataFilePath NVARCHAR(MAX),
	@LogFilePath NVARCHAR(MAX),
	@sql	NVARCHAR(MAX)
	
SELECT
	@DBNamePostfix	= N'',
	@DataFilePath	= N'D:\Database\',
	@LogFilePath	= N'D:\Database\'

--IPList--
SELECT @sql = '
		CREATE DATABASE [IPList' + @DBNamePostfix + '] ON  PRIMARY ( NAME = N''IPList'+ @DBNamePostfix +''', FILENAME = N''' + @DataFilePath + 'IPList' + @DBNamePostfix + N'.mdf'' , SIZE = 10MB, MAXSIZE = UNLIMITED, FILEGROWTH = 128MB) 
		LOG ON ( NAME = N''IPList' + @DBNamePostfix + N'_log'', FILENAME = N''' + @LogFilePath + 'IPList' + @DBNamePostfix + '.ldf'', SIZE = 5MB , MAXSIZE = 2048GB , FILEGROWTH = 128MB) 
		COLLATE SQL_Latin1_General_CP1_CI_AS
	'
EXEC (@sql)