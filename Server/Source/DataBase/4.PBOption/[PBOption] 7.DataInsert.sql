USE [PBOption]
GO
------------------------------------------------------------------------------------------------------
-- [ControlContents]
------------------------------------------------------------------------------------------------------
INSERT [ControlContents] ([ContentsNum], [ContentsName], [ContentsVersion], [IsEnable]) VALUES 
	(0, N'Calculate Point', 0, 1),
	(1, N'Repair Durability', 1, 1),
	(2, N'Default Weapon', 0, 1),
	(3, N'Multi Slot', 0, 1),
	(4, N'IP Block', 0, 0),
	(5, N'Cash Recharge', 0, 1),
	(6, N'General Ranking', 0, 1),
	(7, N'Gift', 0, 1),
	(8, N'TS Event', 0, 1),
	(9, N'Attendance', 0, 1),
	(10, N'Mission Card', 0, 0),
	(11, N'Medal', 0, 1),
	(12, N'Clan', 0, 1),
	(13, N'Shop', 0, 1),
	(14, N'Gacha', 0, 0),
	(15, N'Shutdown', 0, 0),
	(16, N'Clan Match Season', 0, 0),
	(17, N'Account Trans', 0, 0),
	(18, N'Team Change',0,1),
	(19, N'Chara Buy Restricte',0,0),
	(20, N'Skill Use Restrict',0,0),
GO

------------------------------------------------------------------------------------------------------
-- [DBList]
------------------------------------------------------------------------------------------------------
INSERT [DBList] ([Idx], [strDBName], [strDataSource], [strCatalog], [strID], [strPW], [iUseServer]) VALUES 
	(1,		N'PBSIA',		N'127.0.0.1',	N'ShopInventoryAgent',	N'id',	N'pw', 65),
	(2,		N'PBSIA00',	N'127.0.0.1',	N'ShopInventoryAgent',	N'id',	N'pw', 1),
	(3,		N'PBSIA01',	N'127.0.0.1',	N'ShopInventoryAgent',	N'id',	N'pw', 1),
	(4,		N'PBSIA02',	N'127.0.0.1',	N'ShopInventoryAgent',	N'id',	N'pw', 1),
	(5,		N'PBSIA03',	N'127.0.0.1',	N'ShopInventoryAgent',	N'id',	N'pw', 1),
	(6,		N'PBSIA04',	N'127.0.0.1',	N'ShopInventoryAgent',	N'id',	N'pw', 1),
	(7,		N'PBSIA05',	N'127.0.0.1',	N'ShopInventoryAgent',	N'id',	N'pw', 1),
	(8,		N'PBSIA06',	N'127.0.0.1',	N'ShopInventoryAgent',	N'id',	N'pw', 1),
	(9,		N'PBSIA07',	N'127.0.0.1',	N'ShopInventoryAgent',	N'id',	N'pw', 1),
	(10,		N'PBSIA08',	N'127.0.0.1',	N'ShopInventoryAgent',	N'id',	N'pw', 1),
	(11,		N'PBSIA09',	N'127.0.0.1',	N'ShopInventoryAgent',	N'id',	N'pw', 1),
	(12,		N'ZP_Log',	N'127.0.0.1',	N'ZP_Log',		N'id',	N'pw', 1),
	(13,		N'PBClan',	N'127.0.0.1',	N'PBClan',		N'id',	N'pw', 2),
	(14,		N'IPList',		N'127.0.0.1',	N'IPList',			N'id',	N'pw', 1),
	(15,		N'PBUser',	N'127.0.0.1',	N'PBUser',		N'id',	N'pw', 1),
	(16,		N'PBUser00',	N'127.0.0.1',	N'PBUser',		N'id',	N'pw', 5),
	(17,		N'PBUser01',	N'127.0.0.1',	N'PBUser',		N'id',	N'pw', 5),
	(18,		N'PBUser02',	N'127.0.0.1',	N'PBUser',		N'id',	N'pw', 5),
	(19,		N'PBUser03',	N'127.0.0.1',	N'PBUser',		N'id',	N'pw', 5),
	(20,		N'PBUser04',	N'127.0.0.1',	N'PBUser',		N'id',	N'pw', 5),
	(21,		N'PBUser05',	N'127.0.0.1',	N'PBUser',		N'id',	N'pw', 5),
	(22,		N'PBUser06',	N'127.0.0.1',	N'PBUser',		N'id',	N'pw', 5),
	(23,		N'PBUser07',	N'127.0.0.1',	N'PBUser',		N'id',	N'pw', 5),
	(24,		N'PBUser08',	N'127.0.0.1',	N'PBUser',		N'id',	N'pw', 5),
	(25,		N'PBUser09',	N'127.0.0.1',	N'PBUser',		N'id',	N'pw', 5),
	(26,		N'ZLog',		N'127.0.0.1',	N'ZLog',			N'id',	N'pw', 128),
	(27,		N'PBInfo',	N'127.0.0.1',	N'PBInfo',		N'id',	N'pw', 1024),
	(28,		N'PBMess00',	N'127.0.0.1',	N'PBMess',		N'id',	N'pw', 4),
	(29,		N'PBMess01',	N'127.0.0.1',	N'PBMess',		N'id',	N'pw', 4),
	(30,		N'PBMess02',	N'127.0.0.1',	N'PBMess',		N'id',	N'pw', 4),
	(31,		N'PBMess03',	N'127.0.0.1',	N'PBMess',		N'id',	N'pw', 4),
	(32,		N'PBMess04',	N'127.0.0.1',	N'PBMess',		N'id',	N'pw', 4),
	(33,		N'PBMess05',	N'127.0.0.1',	N'PBMess',		N'id',	N'pw', 4),
	(34,		N'PBMess06',	N'127.0.0.1',	N'PBMess',		N'id',	N'pw', 4),
	(35,		N'PBMess07',	N'127.0.0.1',	N'PBMess',		N'id',	N'pw', 4),
	(36,		N'PBMess08',	N'127.0.0.1',	N'PBMess',		N'id',	N'pw', 4),
	(37,		N'PBMess09',	N'127.0.0.1',	N'PBMess',		N'id',	N'pw', 4),
	(38,		N'Login',		N'127.0.0.1',	N'PBAccount',		N'id'	N'pw', 512),
	(39,		N'Billing',		N'127.0.0.1',	N'PBBilling',		N'id',	N'pw', 512),
	(40,		N'AccountTrans',	N'127.0.0.1',	N'ZAccount',		N'id',	N'pw', 512)
GO


------------------------------------------------------------------------------------------------------
-- [PB_BT_ODailyGiftInfo]
------------------------------------------------------------------------------------------------------
INSERT [dbo].[PB_BT_ODailyGiftInfo] ([Active], [ResetTime], [FirTime], [SecTime], [ThiTime], [CreateDate]) VALUES (1, 0, 5, 10, 15, GETDATE())
GO
DECLARE @I32Idx INT, @i32Idx2 INT
SET @I32Idx = 0
WHILE ( @I32Idx < 3 )
BEGIN
	SET @i32Idx2 = 0
	WHILE ( @i32Idx2 < 20 )
	BEGIN
		INSERT [dbo].[PB_BT_ODailyGiftGoods] ([GiftType], [GoodsID], [Rate], [CreateDate]) VALUES (@I32Idx, 0, 0, GETDATE())
		SET @i32Idx2 = @i32Idx2 + 1
	END
	SET @I32Idx = @I32Idx + 1
END
GO

------------------------------------------------------------------------------------------------------
-- [PB_BT_ToolMenu]
------------------------------------------------------------------------------------------------------
INSERT INTO [PB_BT_ToolMenu]([MenuIDX],[ParentIDX],[Name],[Title],[Path],[IsHidden],[MenuInfo01],[MenuInfo02],[MenuInfo03],[OrderNo])VALUES
	(1,0,'WebToolRoot','WebToolRoot','/',0,NULL,NULL,NULL,0),
	(2,1,'main','main','/main.do',0,NULL,NULL,NULL,0),
	(3,1,'GM menu','GM menu','/gmMenu',0,NULL,NULL,NULL,0),
	(4,1,'Shop','Shop','/shop',0,NULL,NULL,NULL,0),
	(5,1,'Game Management','Game Management','/gameManagement',0,NULL,NULL,NULL,0),
	(6,1,'Event','Event','/event',0,NULL,NULL,NULL,0),
	(7,1,'Web Tool Admin','Web Tool Admin','/webToolAdmin',0,NULL,NULL,NULL,0),
	(8,1,'Log','Log','/log',0,NULL,NULL,NULL,0),
	(9,1,'Lottery Shop','Lottery Shop','/lotteryShop',1,NULL,NULL,NULL,0),
	(10,3,'User Mgt.','User Mgt.','/gmMenu/defaultInfo.do',0,NULL,NULL,NULL,0),
	(11,3,'Clans','Clans','/gmMenu/clans.do',0,NULL,NULL,NULL,0),
	(12,3,'Real-time Notices','Real-time Notices','/gmMenu/realTimeNotices.do',0,NULL,NULL,NULL,0),
	(13,3,'Regular Notices','Regular Notices','/gmMenu/regularNotices.do',0,NULL,NULL,NULL,0),
	(14,3,'To-be-blocked List','To-be-blocked List','/gmMenu/toBeBlockedList.do',0,NULL,NULL,NULL,0),
	(15,3,'blocked User List','blocked User List','/gmMenu/blockedUserList.do',0,NULL,NULL,NULL,0),
	(16,4,'Goods Mgt.','Goods Mgt.','/shop/goodsMgt',0,NULL,NULL,NULL,0),
	(17,4,'Field Shop','Field Shop','/shop/fieldShop.do',0,NULL,NULL,NULL,0),
	(18,4,'Create coupon','Create coupon','/shop/createCoupon.do',0,NULL,NULL,NULL,0),
	(19,16,'One Goods','One Goods','/shop/goodsMgt/oneGoods.do',0,NULL,NULL,NULL,0),
	(20,16,'Set Goods','Set Goods','/shop/goodsMgt/setGoods.do',0,NULL,NULL,NULL,0),
	(21,5,'Basic Setting','Basic Setting','/gameManagement/basicSetting.do',0,NULL,NULL,NULL,0),
	(22,5,'Map Settings','Map Settings','/gameManagement/mapSetting.do',0,NULL,NULL,NULL,0),
	(23,5,'MissionCard Settings','MissionCard Settings','/gameManagement/missionCardSetting.do',1,NULL,NULL,NULL,0),
	(24,5,'Hign Rank','Hign Rank','/gameManagement/highRank.do',1,NULL,NULL,NULL,0),
	(25,6,'Bonus Events','Bonus Events','/event/bonusEvents.do',0,NULL,NULL,NULL,1),
	(26,6,'New User Event','New User Event','/event/newUserEvent.do',0,NULL,NULL,NULL,3),
	(27,6,'Playtime Events','Playtime Events','/event/playtimeEvents.do',1,NULL,NULL,NULL,4),
	(28,6,'Attendance Check','Attendance Check','/event/attendanceCheckForCalendar.do',0,NULL,NULL,NULL,5),
	(29,6,'Rank-UP Bonus','Rank-UP Bonus','/event/rankUpBonus.do',0,NULL,NULL,NULL,6),
	(30,6,'Send Gift','Send Gift','/event/sendGift.do',0,NULL,NULL,NULL,7),
	(31,7,'Account Settings','Account Settings','/webToolAdmin/accountSettings.do',0,NULL,NULL,NULL,0),
	(32,7,'Mac Address','Mac Address','/webToolAdmin/macAddress.do',0,NULL,NULL,NULL,0),
	(33,8,'KPI','KPI','/log/KPI.do',0,NULL,NULL,NULL,0),
	(34,8,'Sale Log','Sale Log','/log/saleLog.do',0,NULL,NULL,NULL,0),
	(35,8,'Query','Query','/log/pbWriteQuery.do',0,NULL,NULL,NULL,0),
	(36,8,'Map Stat','Map Stat','/log/mapStat.do',0,NULL,NULL,NULL,0),
	(37,9,'Notice','Notice','/lotteryShop/notice.do',1,NULL,NULL,NULL,0),
	(38,9,'Setting','Setting','/lotteryShop/setting.do',1,NULL,NULL,NULL,0),
	(39,9,'Registration','Registration','/lotteryShop/registration.do',1,NULL,NULL,NULL,0),
	(40,9,'Goods List','Goods List','/lotteryShop/goodsList.do',1,NULL,NULL,NULL,0),
	(41,9,'Hidden List','Hidden List','/lotteryShop/hiddenList.do',1,NULL,NULL,NULL,0),
	(42,9,'Event Time','Event Time','/lotteryShop/eventTime.do',1,NULL,NULL,NULL,0),
	(43,6,'Item Event (old)','Item Event (old)','/event/itemEventOld.do',1,NULL,NULL,NULL,2),
	(44,8,'CCU','CCU','/log/ccu.do',0,NULL,NULL,NULL,0),
	(45,5,'Default Item Setting','Default Item Setting','/gameManagement/defaultItemSetting.do',0,NULL,NULL,NULL,0),
	(46,5,'Default Parts Setting','Default Parts Setting','/gameManagement/defaultPartsSetting.do',1,NULL,NULL,NULL,0),
	(47,5,'Domi Reward Setting','Domi Reward Setting','/gameManagement/domiRewardItem.do',1,NULL,NULL,NULL,0),
	(48,6,'Daily Gift','Daily Gift','/event/dailyGift.do',0,NULL,NULL,NULL,0),
	(49,8,'Statistics','Statistics','/log/statistics.do',1,NULL,NULL,NULL,0),
	(50,8,'Sale Cash','Sale Cash','/log/saleCash.do',1,NULL,NULL,NULL,0),
	(51,1,'Lottery Shop','Lottery Shop','/lucky',0,NULL,NULL,NULL,7),
	(52,51,'Notice','Notice','/lucky/notice.do',0,NULL,NULL,NULL,1),
	(53,51,'Setting','Setting','/lucky/setting.do',0,NULL,NULL,NULL,2),
	(54,5,'Change Service','Change Service','/gameManagement/changeService.do',0,NULL,NULL,NULL,0),
	(60,6,'Login Event','Login Event','/event/loginEvent.do',0,NULL,NULL,NULL,0),
	(61,5,'IP Block','IP Block','/gameManagement/ipBlock.do',0,NULL,NULL,NULL,0),
	(62,5,'Channel Setting','Channel Setting','/gameManagement/channelSetting.do',0,NULL,NULL,NULL,0),
	(65,5,'Season','Season','/gameManagement/season.do',0,NULL,NULL,NULL,4),
	(66,5,'Anti-Addition System','Anti-Addition System','/gameManagement/antiAdditionSystem.do',0,NULL,NULL,NULL,4)
	(67,8,'Real-time CCU','Real-time CCU','/log/realTimeCCU.do',0,NULL,NULL,NULL,7)
GO

------------------------------------------------------------------------------------------------------
-- [TPBC_Basic]
------------------------------------------------------------------------------------------------------
INSERT INTO [TPBC_Basic] ([NewUserPoint],[ClanCreateRank],[ClanCreatePoint],[sdtRSEventStartTime],[sdtRSEventEndTime],[strAdMessage1],[strAdMessage2],[strAdMessage3],[i8RSAdMessage1Active],[i8RSAdMessage2Active],[i8RSAdMessage3Active],[FieldShopFlag],[ClanDefalutMemberCnt])
	VALUES ( 10000, 0, 100, '2010-01-01 01:01:01', '2010-01-01 01:01:02', '', '', '', 0, 0, 0, 0, 50 )
GO

------------------------------------------------------------------------------------------------------
-- [TPBC_GeneralRankup]
------------------------------------------------------------------------------------------------------
INSERT INTO [TPBC_GeneralRankup]([active],[rank_47],[rank_48],[rank_49],[rank_50],[start_time],[offset_Day])
     VALUES ( 0, 100, 90, 80, 70, 3, 1 )
GO

------------------------------------------------------------------------------------------------------
-- [TPBC_Mac]
------------------------------------------------------------------------------------------------------
DECLARE @I32Idx INT
SET @I32Idx = 1
WHILE ( @I32Idx < 31 )
BEGIN
	INSERT INTO [TPBC_Mac] ([Idx],[MacAddress],[Description]) VALUES (@I32Idx,'00-00-00-00-00-00','')	
	SET @I32Idx = @I32Idx + 1
END
GO

------------------------------------------------------------------------------------------------------
-- [TPBC_Notice]
------------------------------------------------------------------------------------------------------
DECLARE @I32Idx INT
SET @I32Idx = 0
WHILE ( @I32Idx < 5 )
BEGIN
	INSERT INTO [TPBC_Notice] ([Active],[RepeatTime],[Text],[BeginTime],[FinishTime]) VALUES (0,0,'',GETDATE(),GETDATE())
	SET @I32Idx = @I32Idx + 1
END
GO

------------------------------------------------------------------------------------------------------
-- [TPBF_Basic]
------------------------------------------------------------------------------------------------------
INSERT INTO [TPBF_Basic]
	([QuestEvent],[ClanMaxCount],
	[ui8Version],[ui8AASEnableFlag],[ui16AASProhibitionFlag],[ui16AASDailyUsageMaxMin],[ui16AASDailyUsageResetTime],[ui64AASAppointedTimeTable],[LocalCode])
    VALUES
	( 0, 10000, 1, 0, 0, 0, 0, 0, 0 )
GO

------------------------------------------------------------------------------------------------------
-- [TPBF_Medal]
------------------------------------------------------------------------------------------------------
-- Event Medal
INSERT INTO [dbo].[TPBF_Medal] ([ui8MedalEnable],[sdtEventMedalSetStartTime],[sdtEventMedalSetEndTime],[i16EventMedalSetIdx])
     VALUES( 1,'2014-01-01','2025-01-01',0)
GO

------------------------------------------------------------------------------------------------------
-- [TPBF_RankUpItem]
------------------------------------------------------------------------------------------------------
DECLARE @i32Idx INT
SET @i32Idx = 1;
WHILE( @i32Idx < 52 ) 
BEGIN
	INSERT INTO TPBF_RankUpItem([iRank] ,[iGoodsCount] ,[iItemGoodsID1] ,[iItemGoodsID2] ,[iItemGoodsID3] ,[iItemGoodsID4])
		VALUES ( @i32Idx , 0, 0, 0, 0 ,0)
	SET @i32Idx = @i32Idx + 1;
END
GO

------------------------------------------------------------------------------------------------------
-- [PB_BT_LuckyItem]
------------------------------------------------------------------------------------------------------
INSERT INTO [dbo].[PB_BT_LuckyItem]
           ([PriceType]
           ,[Enable]
           ,[LuckyGoodsID]
           ,[LuckyDropRate])
     VALUES
           ( 2
           ,1
           ,0
           ,10000)
GO

INSERT INTO [dbo].[PB_BT_LuckyItem]
           ([PriceType]
           ,[Enable]
           ,[LuckyGoodsID]
           ,[LuckyDropRate])
     VALUES
           ( 1
           ,1
           ,0
           ,10000)
GO
INSERT INTO [dbo].[PB_BT_LuckyItem]
           ([PriceType]
           ,[Enable]
           ,[LuckyGoodsID]
           ,[LuckyDropRate])
     VALUES
           ( 3
           ,1
           ,0
           ,10000)
GO

INSERT INTO [dbo].[PB_BT_ConfigRevision]
           ([Item]
           ,[CodeInfo]
           ,[Medal])
     VALUES
           (0
           ,0
           ,0)
GO


-- Default Stage
INSERT INTO [dbo].[MapDefault] ([i32Mode] ,[i32Stage])
VALUES
(1,	7 ),
(2,	25),
(3,	35),
(4,	1),
(5,	39),
(6,	0),
(7,	40),
(8,	0),
(9,	0),
(10,	44),
(11,	0),
(12,	51),
(13	,93),
(14,69)

-- ChangeLog
INSERT INTO TPBC_ChangeLog ( ID, Comment, Date) VALUES ( N'Zepetto', '1.5 Create', GetDate() )

--ShopTagPriority
INSERT INTO [dbo].[ShopTagPriority]	( [TagNo], [Priority], [TagName] )	VALUES	( 0,  5, 'None'	) 

INSERT INTO [dbo].[ShopTagPriority]	( [TagNo], [Priority], [TagName] )	VALUES	( 1,  0, 'New'	) 

INSERT INTO [dbo].[ShopTagPriority]	( [TagNo], [Priority], [TagName] )	VALUES	( 2,  2, 'Hot'	) 

INSERT INTO [dbo].[ShopTagPriority]	( [TagNo], [Priority], [TagName] )	VALUES	( 3,  4, 'Event') 

INSERT INTO [dbo].[ShopTagPriority]	( [TagNo], [Priority], [TagName] )	VALUES  ( 4,  1, 'PC Room') 

INSERT INTO [dbo].[ShopTagPriority]	( [TagNo], [Priority], [TagName] )	VALUES	( 5,  3, 'Sale'	) 