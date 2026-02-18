USE [ShopInventoryAgent]
GO

------------------------------------------------------------------------------------------------------
-- [PB_BT_OPBF_DefaultItem]
------------------------------------------------------------------------------------------------------
DECLARE @i32Idx INT
SET @i32Idx = 0
WHILE( @i32Idx < 80 )
BEGIN
	INSERT INTO [PB_BT_OPBF_DefaultItem] (i32Idx, ui8Type, i32ItemID) VALUES (@i32Idx+1, 0, 0)
	SET @i32Idx = @i32Idx+1
END
GO

------------------------------------------------------------------------------------------------------
-- [PB_BT_SFieldShopCategory]
------------------------------------------------------------------------------------------------------
DECLARE @i32Idx INT
SET @i32Idx = 0
WHILE( @i32Idx < 6 )
BEGIN
	INSERT INTO [PB_BT_SFieldShopCategory] ( i8Idx, strName ) VALUES (@i32Idx+1, '')
	SET @i32Idx = @i32Idx+1
END
GO

------------------------------------------------------------------------------------------------------
-- [PB_BT_SOper_FieldShopVersionControl]
------------------------------------------------------------------------------------------------------
INSERT INTO [PB_BT_SOper_FieldShopVersionControl] (sdtDate,strID,strComment) VALUES(GETDATE(),'ZepettoQuery','FirstOpen')
GO

------------------------------------------------------------------------------------------------------
-- [PB_BT_SOper_VersionControl]
------------------------------------------------------------------------------------------------------
DECLARE @strToday NVARCHAR(32), @i32Today INT
SET @strToday = CONVERT( VARCHAR, GETDATE(), 20 )
SET @i32Today = SUBSTRING( @strToday, 3, 2 ) + SUBSTRING( @strToday, 6, 2 ) + SUBSTRING( @strToday, 9, 2 ) + SUBSTRING( @strToday, 12, 2 ) + SUBSTRING( @strToday, 15, 2 )
INSERT INTO [PB_BT_SOper_VersionControl] ([Date], [Command], [ID]) VALUES (@i32Today, 'FirstOpen', 'ZepettoQuery' )
GO


-- 1. 캐릭터 8종에 대한 상점 셋팅 변경
UPDATE [dbo].[PB_BT_SShopItem]
   SET [AuthType] =  3 
      ,[AuthExpireType]= 1
      ,[AutoStart] = 1 
   where itemid / 100000 = 6

GO


-- 2. 캐릭터 8종에 대한 상점 셋팅 변경
INSERT INTO PB_BT_SShopgoods ( GoodsID, Enable, SaleType, Name,Description ) VALUES 
	 ( 60100100, 1, 2, 'Red Bulls', '' ),
	( 60100300, 1, 2,'Tarantula', '' ),
	( 60100500, 1, 2,'Viper Red', '' ),
	( 60100700, 1, 2,'D-Fox', '' ),
	( 60200200, 1, 2,'Acid Pol', '' ),
	( 60200400, 1, 2,'Keen Eyes', '' ),
	( 60200800, 1, 2,'Leopard', '' ),
	( 60200900, 1, 2,'Hide', '' )

INSERT INTO PB_BT_SShopMatching ( GoodsID, ItemID, ItemArg, BonusArg ) VALUES 
	( 60100100, 601001, 0,0 ),
	( 60100300, 601003, 0,0 ),
	( 60100500, 601005, 0,0 ),
	( 60100700, 601007, 0,0 ),
	( 60200200, 602002, 0,0 ),
	( 60200400, 602004, 0,0 ),
	( 60200800, 602008, 0,0 ),
	( 60200900, 602009, 0,0 )

INSERT INTO [dbo].[PB_GT_SShopGoodsPrice] ([GoodsID] ,[PriceType] ,[Point] ,[Cash] ,[Mark] ,[Coin]) VALUES
( 60100100, 0, 500, 0, 0, 0 ),
( 60100300, 0, 500, 0, 0, 0 ),
( 60100500, 0, 500, 0, 0, 0 ),
( 60100700, 0, 500, 0, 0, 0 ),
( 60200200, 0, 500, 0, 0, 0 ),
( 60200400, 0, 500, 0, 0, 0 ),
( 60200800, 0, 500, 0, 0, 0 ),
( 60200900, 0, 500, 0, 0, 0 )



-- 3. 노말 퀘스트 구매제한 설정.
UPDATE [dbo].[PB_BT_SShopItem] SET [Restriction] = 4 ,[RestrictionType] = 1 ,[Condition] = 50 WHERE ItemID = 105031
UPDATE [dbo].[PB_BT_SShopItem] SET [Restriction] = 4 ,[RestrictionType] = 1 ,[Condition] = 51 WHERE ItemID = 106005
UPDATE [dbo].[PB_BT_SShopItem] SET [Restriction] = 4 ,[RestrictionType] = 1 ,[Condition] = 52 WHERE ItemID = 104011
UPDATE [dbo].[PB_BT_SShopItem] SET [Restriction] = 4 ,[RestrictionType] = 1 ,[Condition] = 53 WHERE ItemID = 106003
UPDATE [dbo].[PB_BT_SShopItem] SET [Restriction] = 4 ,[RestrictionType] = 1 ,[Condition] = 54 WHERE ItemID = 202011
UPDATE [dbo].[PB_BT_SShopItem] SET [Restriction] = 4 ,[RestrictionType] = 1 ,[Condition] = 55 WHERE ItemID = 103015
UPDATE [dbo].[PB_BT_SShopItem] SET [Restriction] = 4 ,[RestrictionType] = 1 ,[Condition] = 56 WHERE ItemID = 103036
UPDATE [dbo].[PB_BT_SShopItem] SET [Restriction] = 4 ,[RestrictionType] = 1 ,[Condition] = 57 WHERE ItemID = 105005
UPDATE [dbo].[PB_BT_SShopItem] SET [Restriction] = 4 ,[RestrictionType] = 1 ,[Condition] = 58 WHERE ItemID = 104013
UPDATE [dbo].[PB_BT_SShopItem] SET [Restriction] = 4 ,[RestrictionType] = 1 ,[Condition] = 59 WHERE ItemID = 214002
UPDATE [dbo].[PB_BT_SShopItem] SET [Restriction] = 4 ,[RestrictionType] = 1 ,[Condition] = 60 WHERE ItemID = 202007
UPDATE [dbo].[PB_BT_SShopItem] SET [Restriction] = 4 ,[RestrictionType] = 1 ,[Condition] = 61 WHERE ItemID = 103153
UPDATE [dbo].[PB_BT_SShopItem] SET [Restriction] = 4 ,[RestrictionType] = 1 ,[Condition] = 62 WHERE ItemID = 104134

 
-- 5. 기본지급아이템
UPDATE dbo.PB_BT_OPBF_DefaultItem SET i32ItemID = 103004 WHERE i32Idx = 1
UPDATE dbo.PB_BT_OPBF_DefaultItem SET i32ItemID = 202003 WHERE i32Idx = 2
UPDATE dbo.PB_BT_OPBF_DefaultItem SET i32ItemID = 301001 WHERE i32Idx = 3
UPDATE dbo.PB_BT_OPBF_DefaultItem SET i32ItemID = 407001 WHERE i32Idx = 4
UPDATE dbo.PB_BT_OPBF_DefaultItem SET i32ItemID = 508001 WHERE i32Idx = 5
UPDATE dbo.PB_BT_OPBF_DefaultItem SET i32ItemID = 110009 WHERE i32Idx = 6
UPDATE dbo.PB_BT_OPBF_DefaultItem SET i32ItemID = 104006 WHERE i32Idx = 7
UPDATE dbo.PB_BT_OPBF_DefaultItem SET i32ItemID = 105003 WHERE i32Idx = 8
UPDATE dbo.PB_BT_OPBF_DefaultItem SET i32ItemID = 106001 WHERE i32Idx = 9
UPDATE dbo.PB_BT_OPBF_DefaultItem SET i32ItemID = 1500511 WHERE i32Idx = 10
UPDATE dbo.PB_BT_OPBF_DefaultItem SET i32ItemID = 1500512 WHERE i32Idx = 11
UPDATE dbo.PB_BT_OPBF_DefaultItem SET i32ItemID = 1500513 WHERE i32Idx = 12

