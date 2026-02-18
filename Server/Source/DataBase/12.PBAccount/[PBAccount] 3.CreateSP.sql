USE [PBAccount]
GO
/****** Object:  StoredProcedure [dbo].[usp_Account_ins]    Script Date: 2015-02-23 오전 10:30:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[usp_Account_ins]') AND type in (N'P', N'PC'))
BEGIN
EXEC dbo.sp_executesql @statement = N'
CREATE PROCEDURE [dbo].[usp_Account_ins]
/* ======================================================
// Interface Part
// ====================================================== */
    @strAccountID_  NVARCHAR(60)
,   @strPasswd_     NVARCHAR(24)
,   @iResult        SMALLINT = 0    OUTPUT
/* ======================================================
// Author       : mk8253.
// Create Date  : 
// Description  : 
// ====================================================== */

/* ======================================================
// Procedure Header
// ====================================================== */
--//WITH ENCRYPTION
AS

SET NOCOUNT         ON
SET XACT_ABORT      ON
SET LOCK_TIMEOUT    5000
SET TRANSACTION ISOLATION LEVEL READ UNCOMMITTED

DECLARE @strDBName   VARCHAR(50)
,       @strProcName VARCHAR(50)
,       @sdtNow      SMALLDATETIME
SELECT  @strDBName   = DB_NAME()
,       @strProcName = ''usp_Account_ins''
,       @sdtNow      = CURRENT_TIMESTAMP
,       @iResult     = 0
/* ======================================================
// Implementation Part
// ====================================================== */ BEGIN TRY
BEGIN DISTRIBUTED TRANSACTION

IF EXISTS
(
    SELECT 1
    FROM dbo.Account
    WHERE
        AccountID = @strAccountID_
) AND @iResult = 0
BEGIN    SET @iResult = -1    END

IF( @iResult = 0 )
BEGIN
    INSERT INTO dbo.Account
    (
        AccountID
    ,   Passwd
    ,   GameCash
    ,   UpdateDate
    ,   CreateDate
    )
    SELECT
        @strAccountID_
    ,   @strPasswd_
    ,   0
    ,   0
    ,   @sdtNow  
    
    COMMIT TRANSACTION
END
ELSE
BEGIN
    ROLLBACK TRANSACTION
    
    EXEC dbo.usp_zDataErrorLog_ins @strDBName, @strProcName, @iResult
END
/* ======================================================
// End of Procedure
// ====================================================== */ END TRY
BEGIN CATCH
    IF XACT_STATE() <> 0 ROLLBACK TRANSACTION
    
    EXEC dbo.usp_zDBErrorLog_ins @strDBName, @strProcName
    SET     @iResult = -9999
END CATCH
' 
END
GO
/****** Object:  StoredProcedure [dbo].[usp_Account_sel]    Script Date: 2015-02-23 오전 10:30:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[usp_Account_sel]') AND type in (N'P', N'PC'))
BEGIN
EXEC dbo.sp_executesql @statement = N'
CREATE PROCEDURE [dbo].[usp_Account_sel]
/* ======================================================
// Interface Part
// ====================================================== */
    @strAccountID_  NVARCHAR(60)
,   @strPasswd_     NVARCHAR(24)
,   @iResult        SMALLINT = 0    OUTPUT
/* ======================================================
// Author       : mk8253.
// Create Date  : 
// Description  : 
// ====================================================== */

/* ======================================================
// Procedure Header
// ====================================================== */
--//WITH ENCRYPTION
AS

SET NOCOUNT         ON
SET XACT_ABORT      ON
SET LOCK_TIMEOUT    5000
SET TRANSACTION ISOLATION LEVEL READ UNCOMMITTED

DECLARE @strDBName   VARCHAR(50)
,       @strProcName VARCHAR(50)
,       @sdtNow      SMALLDATETIME
SELECT  @strDBName   = DB_NAME()
,       @strProcName = ''usp_Account_sel''
,       @sdtNow      = CURRENT_TIMESTAMP
,       @iResult     = 0
/* ======================================================
// Implementation Part
// ====================================================== */
SELECT
    GameCash
FROM dbo.Account
WHERE
    AccountID = @strAccountID_
AND Passwd    = @strPasswd_
OPTION (MAXDOP 1)
' 
END
GO
/****** Object:  StoredProcedure [dbo].[usp_Account_upd]    Script Date: 2015-02-23 오전 10:30:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[usp_Account_upd]') AND type in (N'P', N'PC'))
BEGIN
EXEC dbo.sp_executesql @statement = N'
CREATE PROCEDURE [dbo].[usp_Account_upd]
/* ======================================================
// Interface Part
// ====================================================== */
    @strAccountID_  NVARCHAR(60)
,   @strOldPasswd_  NVARCHAR(24)
,   @strNewPasswd_  NVARCHAR(24)
,   @iResult        SMALLINT = 0    OUTPUT
/* ======================================================
// Author       : mk8253.
// Create Date  : 
// Description  : 
// ====================================================== */

/* ======================================================
// Procedure Header
// ====================================================== */
--//WITH ENCRYPTION
AS

SET NOCOUNT         ON
SET XACT_ABORT      ON
SET LOCK_TIMEOUT    5000
SET TRANSACTION ISOLATION LEVEL READ UNCOMMITTED

DECLARE @strDBName   VARCHAR(50)
,       @strProcName VARCHAR(50)
,       @sdtNow      SMALLDATETIME
SELECT  @strDBName   = DB_NAME()
,       @strProcName = ''usp_Account_upd''
,       @sdtNow      = CURRENT_TIMESTAMP
,       @iResult     = 0
/* ======================================================
// Implementation Part
// ====================================================== */ BEGIN TRY
BEGIN DISTRIBUTED TRANSACTION

IF NOT EXISTS
(
    SELECT 1
    FROM dbo.Account
    WHERE
        AccountID = @strAccountID_
    AND Passwd    = @strOldPasswd_
) AND @iResult = 0
BEGIN    SET @iResult = -1    END

IF( @iResult = 0 )
BEGIN
    UPDATE dbo.Account
    SET Passwd = @strNewPasswd_
    WHERE
        AccountID = @strAccountID_
    
    COMMIT TRANSACTION
END
ELSE
BEGIN
    ROLLBACK TRANSACTION
    
    EXEC dbo.usp_zDataErrorLog_ins @strDBName, @strProcName, @iResult
END
/* ======================================================
// End of Procedure
// ====================================================== */ END TRY
BEGIN CATCH
    IF XACT_STATE() <> 0 ROLLBACK TRANSACTION
    
    EXEC dbo.usp_zDBErrorLog_ins @strDBName, @strProcName
    SET     @iResult = -9999
END CATCH
' 
END
GO
/****** Object:  StoredProcedure [dbo].[usp_GameCash_mer]    Script Date: 2015-02-23 오전 10:30:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[usp_GameCash_mer]') AND type in (N'P', N'PC'))
BEGIN
EXEC dbo.sp_executesql @statement = N'
CREATE PROCEDURE [dbo].[usp_GameCash_mer]
/* ======================================================
// Interface Part
// ====================================================== */
    @strAccountID_  NVARCHAR(60)
,   @iGameCash_     INT
,   @iResult        SMALLINT = 0    OUTPUT
/* ======================================================
// Author       : mk8253.
// Create Date  : 20130731.
// Description  : QA GameCash 충전.
// ====================================================== */

/* ======================================================
// Procedure Header
// ====================================================== */
--//WITH ENCRYPTION
AS

SET NOCOUNT         ON
SET XACT_ABORT      ON
SET LOCK_TIMEOUT    5000
SET TRANSACTION ISOLATION LEVEL READ UNCOMMITTED

DECLARE @strDBName   VARCHAR(50)
,       @strProcName VARCHAR(50)
,       @sdtNow      SMALLDATETIME
SELECT  @strDBName   = DB_NAME()
,       @strProcName = ''usp_GameCash_mer''
,       @sdtNow      = CURRENT_TIMESTAMP
,       @iResult     = 0
/* ======================================================
// Implementation Part
// ====================================================== */ BEGIN TRY
BEGIN DISTRIBUTED TRANSACTION


DECLARE @iOldGameCash INT
,       @iNewGameCash INT
,       @iActionType  INT

SELECT  @iOldGameCash = 0
,       @iNewGameCash = @iGameCash_

SELECT  @iOldGameCash = GameCash
,       @iNewGameCash = GameCash + @iGameCash_
FROM dbo.Account
WHERE
    AccountID = @strAccountID_

IF @iGameCash_ > 0
BEGIN
    SELECT @iActionType = 10
END
ELSE
BEGIN
    SELECT @iActionType = 20
END

IF @iNewGameCash < 0
BEGIN    SET @iResult = -1    END

IF( @iResult = 0 )
BEGIN
    UPDATE dbo.Account
    SET GameCash   = @iNewGameCash
    ,   UpdateDate = @sdtNow
    WHERE
        AccountID = @strAccountID_
    
    INSERT INTO dbo.GameCashLog
    (
        RegDate
    ,   AccountID
    ,   GameCash
    ,   ActionType
    ,   OldGameCash
    ,   NewGameCash
    )
    SELECT
        @sdtNow
    ,   @strAccountID_
    ,   @iGameCash_
    ,   @iActionType
    ,   @iOldGameCash
    ,   @iNewGameCash

    COMMIT TRANSACTION
END
ELSE
BEGIN
    ROLLBACK TRANSACTION
    
    EXEC dbo.usp_zDataErrorLog_ins @strDBName, @strProcName, @iResult
END
/* ======================================================
// End of Procedure
// ====================================================== */ END TRY
BEGIN CATCH
    IF XACT_STATE() <> 0 ROLLBACK TRANSACTION
    
    EXEC dbo.usp_zDBErrorLog_ins @strDBName, @strProcName
    SET     @iResult = -9999
END CATCH

SELECT
    @iResult
,   @iNewGameCash
' 
END
GO
/****** Object:  StoredProcedure [dbo].[usp_GameCash_sel]    Script Date: 2015-02-23 오전 10:30:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[usp_GameCash_sel]') AND type in (N'P', N'PC'))
BEGIN
EXEC dbo.sp_executesql @statement = N'
CREATE PROCEDURE [dbo].[usp_GameCash_sel]
/* ======================================================
// Interface Part
// ====================================================== */
    @strAccountID_      NVARCHAR(64)
/* ======================================================
// Author       : mk8253.
// Create Date  : 20130731.
// Description  : QA Game Cash 조회.
// ====================================================== */

/* ======================================================
// Procedure Header
// ====================================================== */
--//WITH ENCRYPTION
AS

SET NOCOUNT         ON
SET XACT_ABORT      ON
SET LOCK_TIMEOUT    5000
SET TRANSACTION ISOLATION LEVEL READ UNCOMMITTED

DECLARE @strDBName   VARCHAR(50)
,       @strProcName VARCHAR(50)
,       @sdtNow      SMALLDATETIME
SELECT  @strDBName   = DB_NAME()
,       @strProcName = ''usp_GameCash_sel''
,       @sdtNow      = CURRENT_TIMESTAMP
/* ======================================================
// Implementation Part
// ====================================================== */

DECLARE @iGameCash INT

SELECT
    @iGameCash = GameCash
FROM dbo.Account
WHERE
    AccountID = @strAccountID_
OPTION (MAXDOP 1)

SELECT ISNULL(@iGameCash, 0) AS GameCash
' 
END
GO
/****** Object:  StoredProcedure [dbo].[usp_zDataErrorLog_ins]    Script Date: 2015-02-23 오전 10:30:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[usp_zDataErrorLog_ins]') AND type in (N'P', N'PC'))
BEGIN
EXEC dbo.sp_executesql @statement = N'
CREATE PROCEDURE [dbo].[usp_zDataErrorLog_ins]
-- =============================================
-- Author      : hongseok.jeong
-- Create date : 20130708
-- Description : zDataErrorLog 기록
-- =============================================

-- =============================================
-- Parameter
-- =============================================
	@strErrorDB_        NVARCHAR(50)
,	@strErrorProcedure_ NVARCHAR(100)
,	@iErrorCode_        INT
-- WITH ENCRYPTION
AS

-- =============================================
-- Procedure Header
-- =============================================
SET NOCOUNT ON
SET XACT_ABORT ON
SET LOCK_TIMEOUT 5000
SET TRANSACTION ISOLATION LEVEL READ UNCOMMITTED

-- =============================================
-- Execution Part
-- =============================================
BEGIN TRY
BEGIN TRANSACTION

INSERT INTO dbo.zDataErrorLog
(
	ErrorDB
,	ErrorProcedure
,	ErrorCode
,	CreateDate
)
SELECT
	@strErrorDB_
,	@strErrorProcedure_
,	@iErrorCode_
,	CURRENT_TIMESTAMP

COMMIT TRANSACTION
END TRY

-- =============================================
-- End of Procedure
-- =============================================
BEGIN CATCH
	IF (XACT_STATE() <> 0) ROLLBACK TRANSACTION
	EXEC dbo.usp_zDBErrorLog_ins @strErrorDB_, ''usp_zDataErrorLog_ins''
	SET @iErrorCode_ = -9999
END CATCH
' 
END
GO
/****** Object:  StoredProcedure [dbo].[usp_zDBErrorLog_ins]    Script Date: 2015-02-23 오전 10:30:16 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
IF NOT EXISTS (SELECT * FROM sys.objects WHERE object_id = OBJECT_ID(N'[dbo].[usp_zDBErrorLog_ins]') AND type in (N'P', N'PC'))
BEGIN
EXEC dbo.sp_executesql @statement = N'
CREATE PROCEDURE [dbo].[usp_zDBErrorLog_ins]
-- =============================================
-- Author      : hongseok.jeong
-- Create date : 20130708
-- Description : zDBErrorLog 기록
-- =============================================

-- =============================================
-- Parameter
-- =============================================
	@strErrorDB_        NVARCHAR(50)
,	@strErrorProcedure_ NVARCHAR(100)
-- WITH ENCRYPTION
AS

-- =============================================
-- Procedure Header
-- =============================================
SET NOCOUNT ON
SET XACT_ABORT ON
SET LOCK_TIMEOUT 5000
SET TRANSACTION ISOLATION LEVEL READ UNCOMMITTED

-- =============================================
-- Execution Part
-- =============================================
BEGIN TRY
IF (ERROR_NUMBER() IS NULL) RETURN

SELECT
	@strErrorDB_        AS ErrorDB
,	ERROR_NUMBER()	    AS ErrorNumber
,	ERROR_SEVERITY()    AS ErrorSeverity
,	ERROR_STATE()	    AS ErrorState
,	@strErrorProcedure_ AS ErrorProcedure
,	ERROR_LINE()	    AS ErrorLine
,	ERROR_MESSAGE()	    AS ErrorMessage
,	CURRENT_TIMESTAMP   AS CreateDate

INSERT INTO dbo.zDBErrorLog
(
	ErrorDB
,	ErrorNumber
,	ErrorSeverity
,	ErrorState
,	ErrorProcedure
,	ErrorLine
,	ErrorMessage
,	CreateDate
)
SELECT
	@strErrorDB_
,	ERROR_NUMBER()
,	ERROR_SEVERITY()
,	ERROR_STATE()
,	@strErrorProcedure_
,	ERROR_LINE()
,	ERROR_MESSAGE()
,	CURRENT_TIMESTAMP
END TRY
-- =============================================
-- End of Procedure
-- =============================================
BEGIN CATCH

	DECLARE
		@strErrorMessage NVARCHAR(4000)
	,	@iErrorSeverity  INT
	,	@iErrorState	 INT

	SELECT
		@strErrorMessage = ERROR_MESSAGE()
	,	@iErrorSeverity  = ERROR_SEVERITY()
	,	@iErrorState	 = ERROR_STATE()

	RAISERROR
	(
		@strErrorMessage
	,	@iErrorSeverity
	,	@iErrorState
	)
END CATCH
' 
END
GO
