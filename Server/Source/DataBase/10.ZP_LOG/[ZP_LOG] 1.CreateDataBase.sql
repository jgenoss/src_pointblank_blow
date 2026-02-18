DECLARE
	@DBNamePostfix NVARCHAR(50),
	@DataFilePath NVARCHAR(MAX),
	@LogFilePath NVARCHAR(MAX),
	@sql	NVARCHAR(MAX)
	
SELECT
	@DBNamePostfix	= N'',
	@DataFilePath	= N'D:\Database\',
	@LogFilePath	= N'D:\Database\'

--ZP_LOG--
SELECT @sql = '
			CREATE DATABASE [ZP_LOG' + @DBNamePostfix + '] ON
				PRIMARY ( NAME = N''ZP_LOG'+ @DBNamePostfix +''', FILENAME = N''' + @DataFilePath + 'ZP_LOG' + @DBNamePostfix + N'.mdf'', SIZE = 1024MB, MAXSIZE = UNLIMITED, FILEGROWTH = 100MB)
				LOG ON ( NAME = N''ZP_LOG' + @DBNamePostfix + N'_log'', FILENAME = N''' + @LogFilePath + 'ZP_LOG' + @DBNamePostfix + '.ldf'', SIZE = 1024MB , MAXSIZE = 2048GB , FILEGROWTH = 100MB)
				COLLATE SQL_Latin1_General_CP1_CI_AS'
EXEC (@sql)