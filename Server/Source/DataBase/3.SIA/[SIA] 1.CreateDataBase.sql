DECLARE
	@DBNamePostfix	NVARCHAR(50),
	@DataFilePath	NVARCHAR(MAX),
	@LogFilePath	NVARCHAR(MAX),
	@sql			NVARCHAR(MAX)
	
SELECT
	@DBNamePostfix	= N'',
	@DataFilePath	= N'D:\Database\',
	@LogFilePath	= N'D:\Database\'

--ShopInventoryAgent--
SELECT @sql = '
		CREATE DATABASE [ShopInventoryAgent' + @DBNamePostfix + '] ON
			PRIMARY ( NAME = N''ShopInventoryAgent'+ @DBNamePostfix +''', FILENAME = N''' + @DataFilePath + 'ShopInventoryAgent' + @DBNamePostfix + N'.mdf'' , SIZE = 1024MB, MAXSIZE = UNLIMITED, FILEGROWTH = 100MB)
			LOG ON ( NAME = N''ShopInventoryAgent' + @DBNamePostfix + N'_log'', FILENAME = N''' + @LogFilePath + 'ShopInventoryAgent' + @DBNamePostfix + '.ldf'', SIZE = 1024MB, MAXSIZE = 2048GB , FILEGROWTH = 100MB)
			COLLATE SQL_Latin1_General_CP1_CI_AS'
EXEC (@sql)