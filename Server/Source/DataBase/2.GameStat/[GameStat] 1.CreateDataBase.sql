DECLARE
	@DBNamePostfix	NVARCHAR(50),
	@DataFilePath	NVARCHAR(MAX),
	@LogFilePath	NVARCHAR(MAX),
	@sql			NVARCHAR(MAX)
	
SELECT
	@DBNamePostfix	= N'',
	@DataFilePath	= N'D:\Database\',
	@LogFilePath	= N'D:\Database\'

--GameStat--
SELECT @sql = '
		CREATE DATABASE [GameStat' + @DBNamePostfix + '] ON
			PRIMARY ( NAME = N''GameStat'+ @DBNamePostfix +''', FILENAME = N''' + @DataFilePath + 'GameStat' + @DBNamePostfix + N'.mdf'' , SIZE = 1024MB, MAXSIZE = UNLIMITED, FILEGROWTH = 100MB)
			LOG ON ( NAME = N''GameStat' + @DBNamePostfix + N'_log'', FILENAME = N''' + @LogFilePath + 'GameStat' + @DBNamePostfix + '.ldf'', SIZE = 1024MB, MAXSIZE = 2048GB , FILEGROWTH = 100MB)
			COLLATE SQL_Latin1_General_CP1_CI_AS'
EXEC (@sql)