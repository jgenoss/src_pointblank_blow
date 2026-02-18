////////////////////////////////////////////////////////////////////////////////////////////////////
//	Module Name:
//		DB_Script.h
//	Author:
//		Ji-won Kang.
//	Description:
//		Define of DB Query Group
////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Stored Procedure & Query
// Comments were divided into features like ourselves.
//-----------------------------------------------------------------------------------
// Work Flow
//-----------------------------------------------------------------------------------
// 1. Message Architecture
// 2. Qurey Script
// 3. Stored Procedure
//
//-----------------------------------------------------------------------------------
// Naming
//-----------------------------------------------------------------------------------
// [Rule of QUREY Define Naming]
// QUERY_X00X
// QUERY : Qurey
// 00X  : PBClan Qurry Unique Number(X000)
//
// [Rule of SP Define Naming]
// PBUser     : USP_10X00
// PBInfo     : USP_20X00
// PBClan     : USP_30X00
// PBMess     : USP_40X00
// SIA	      : USP_50X00
// ZP_LOG     : USP_60X00
// PBOption	  : USP_70X00
// IPList	  : USP_80X00
// BillingWeb : USP_90X00
// Billing    : USP_100X00


// USP : user stored procedure
// X0X00 : PBClan usp Unique Number(X0000)
//
// [Rule of Procedure Naming]
//
// XX_YY_ZZ1
// XX : stored procedure ( ex : SP )
// YY : Access DBName ( ex : pbclan )
// ZZ : Work Description ( ex : RESET_FRIEND_LIST )
// 1  : Work Flow Number
// 
// (ex)
// usp_pbclan_loadclandata
// usp			: user stored procedure
// ItemDeliver	: Access DBName
// buy_item_add : Work Description
//
//-----------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
//SP를 사용할때 on 해주세요. 
#define USE_STORED_PROCEDURE //(게임서버는 모두 SP)
//////////////////////////////////////////////////////////////////////////

// [IPList] LoadPcCafeList_PcThread()
// in  : none
// out : vIPAddress, siPromotionID
#define USP_80100	"EXEC usp_80100_iplist_loadpccafelist;"
#define QUERY_8001	"SELECT vIPAddress, siPromotionID FROM IPList"
// [IPList] LoadThaiIDPlusThread
// in  : none
// out : iGlobalAccountNo, iLevel
#define USP_80200	"EXEC usp_80200_iplist_loadthaiidplus;"
#define QUERY_8002	"SELECT iGlobalAccountNo, iLevel FROM IDPlus ORDER BY iGlobalAccountNo ASC"
//////////////////////////////////////////////////////////////////////////

// [ZP_LOG] _GetStatsData
// in  : ui32StMonthDate,ui32UVMonthType,ui32StWeekDate,ui32UVWEEKType,ui32StDayDate,ui32UVDayType,ui32StHourDate,ui8BUDayType,ui8BUMonthType
// out : ui32StUVMonth,ui32StUVWeek,ui32StUVDay,ui64StTSDay,ui32StCCU,ui32NewGRUDay,ui64TotalGRUDay,ui32DBDay,ui32DBMonth
#define USP_60600	"EXEC usp_60600_zplog_getstatesdata %d,%d,%d,%d,%d,%d,N'%s',%d,%d,%d;"
#define QUERY_1023	"SELECT ui32UV FROM TPBUV WHERE ui32Date=%d AND i8Type=%d"
#define QUERY_1024	"SELECT ui32UV FROM TPBUV WHERE ui32Date=%d AND i8Type=%d"
#define QUERY_1025	"SELECT ui32UV FROM TPBUV WHERE ui32Date=%d AND i8Type=%d"
#define QUERY_1026	"SELECT ui64TS FROM TPBTS WHERE ui32Date=%d AND i8Type=%d"
#define QUERY_1027	"SELECT CCU FROM TPbCCU WHERE Date=%d"
#define QUERY_1029	"SELECT ui32GRU FROM TPBNewGRU WHERE ui32Date=%d"
#define QUERY_1030	"SELECT ui64GRU FROM TPBTotalGRU order by ui32Date desc"
#define QUERY_1031	"SELECT MAX(ui32Date) FROM TPBBU WHERE i8Type=%d"
#define QUERY_1032	"SELECT MAX(ui32Date) FROM TPBBU WHERE i8Type=%d"
// [ZP_LOG] _SetDBUV
// in  : ui32Date,i8Type,ui32Value
// out : result
#define USP_60700	"EXEC usp_60700_zplog_set_db_uv %d,%d,%d;"
#define QUERY_1033	"SELECT ui32UV FROM TPBUV WHERE ui32Date=%d AND i8Type=%d"
#define QUERY_1034	"INSERT INTO TPBUV (ui32Date,i8Type,ui32UV) VALUES (%d,%d,%d)"
#define QUERY_1035	"UPDATE TPBUV SET ui32UV=%d WHERE ui32Date=%d AND i8Type=%d"
// [ZP_LOG] _SetDBTS
// in  : ui32Date,i8Type,ui64Value
// out : result
#define USP_60800	"EXEC usp_60800_zplog_set_db_ts %d,%d,%I64d;"
#define QUERY_1036	"SELECT ui64TS FROM TPBTS WHERE ui32Date=%d AND i8Type=%d"
#define QUERY_1037	"INSERT INTO TPBTS (ui32Date,i8Type,ui64TS) VALUES (%d,%d,%I64d)"
#define QUERY_1038	"UPDATE TPBTS SET ui64TS=%I64d WHERE ui32Date=%d AND i8Type=%d"
// [ZP_LOG] _SetDBCCU
// in  : ui32Date,ui32Value
// out : result
#define USP_60900	"EXEC usp_60900_zplog_set_db_ccu N'%s',%d;"
#define QUERY_1039	"SELECT CCU FROM TPbCCU WHERE Date=%d"
#define QUERY_1040	"INSERT INTO TPbCCU (Date,CCU) VALUES (%d,%d)"
#define QUERY_1041	"UPDATE TPbCCU SET CCU=%d WHERE Date=%d"
// [ZP_LOG] _SetDBNewGRU
// in  : ui32Date,ui32Value
// out : result
#define USP_61000	"EXEC usp_61000_zplog_set_db_newgru %d,%d;"
#define QUERY_1042	"SELECT ui32GRU FROM TPBNewGRU WHERE ui32Date=%d"
#define QUERY_1043	"INSERT INTO TPBNewGRU (ui32Date,ui32GRU) VALUES (%d,%d)"
#define QUERY_1044	"UPDATE TPBNewGRU SET ui32GRU=%d WHERE ui32Date=%d"
// [ZP_LOG] _SetDBTotalGRU
// in  : ui32Date,ui64Value
// out : result
#define USP_61100	"EXEC usp_61100_zplog_set_db_totalgru %d,%I64d;"
#define QUERY_1045	"SELECT ui64GRU FROM TPBTotalGRU WHERE ui32Date=%d"
#define QUERY_1046	"INSERT INTO TPBTotalGRU (ui32Date,ui64GRU) VALUES (%d,%I64d)"
#define QUERY_1047	"UPDATE TPBTotalGRU SET ui64GRU=%I64d WHERE ui32Date=%d"
// [ZP_LOG] _SetDBBUUID
// in  : ui64Value
// out : result
#define USP_61200	"EXEC usp_61200_zplog_set_db_uuid %I64d;"
#define QUERY_1048	"INSERT INTO TPBBUUID (UID, i8Type) VALUES (%I64d, 0)"
#define QUERY_1049	"INSERT INTO TPBBUUID (UID, i8Type) VALUES (%I64d, 1)"
#define QUERY_1050	"INSERT INTO TPBBUUID (UID, i8Type) VALUES (%I64d, 2)"
// [ZP_LOG] _SetDBBU
// in  : ui32Date,i8Type,i8idx
// out : result
#define USP_61300	"EXEC usp_61300_zplog_set_db_bu %d,%d,%d;"
#define QUERY_1051	"SELECT COUNT(DISTINCT UID) FROM TPBBUUID WHERE i8Type = 0"
#define QUERY_1052	"SELECT COUNT(DISTINCT UID) FROM TPBBUUID WHERE i8Type = 1"
#define QUERY_1053	"SELECT COUNT(DISTINCT UID) FROM TPBBUUID WHERE i8Type = 2"
#define QUERY_1054	"INSERT INTO TPBBU ( ui32Date, i8Type, ui32BU ) VALUES ( %d, %d, %d )"
#define QUERY_1055	"DELETE FROM TPBBUUID WHERE i8Type = 0"
#define QUERY_1056	"DELETE FROM TPBBUUID WHERE i8Type = 1"
#define QUERY_1057	"DELETE FROM TPBBUUID WHERE i8Type = 2"
//////////////////////////////////////////////////////////////////////////

// [PBUser] WebInfoGetUidForNick
// in  : nick
// out : UID
//#define USP_10100	"EXEC usp_pbuser_webinfogetuidfornick N'%s';"
#define QUERY_1001	"SELECT UID FROM TNickInfo WHERE nick =N'%s'  "
// [PBUser] WebInfoGetUidForId
// in  : UserID
// out : idx(uid)
//#define USP_10200	"EXEC usp_pbuser_webinfogetuidforid N'%s';"
#define QUERY_1002	"SELECT idx FROM TUserIdx WHERE id =N'%s'  "
// [PBUser] WebInfoGetUserInfo
// in  : tablenum,uid
// out : result,
//		 nick, exp, rank, money, cash,
//       match,win,lose,killcount,headshot,death,dis,draw,smatch,swin,slose,skillcount,sheadshot,sdeath,sdis,sdraw,
//		 nowpprim,nowpsub,nowpmelee,nowpthrow,nowpitem,nocharred,nocharblue,nocharhair,nocharitem, nochardino
//#define USP_10300	"EXEC usp_pbuser_webinfogetuserinfo %d,%I64d;"
#define QUERY_1003	"SELECT nick, exp, rank, money, cash FROM TBasicInfo%d WHERE UID ='%I64d'"
#define QUERY_1004	"SELECT match,win,lose,killcount,headshot,death,dis,draw,smatch,swin,slose,skillcount,sheadshot,sdeath,sdis,sdraw FROM TUserRecord%d WHERE UID ='%I64d'"
#define QUERY_1005	"SELECT nowpprim,nowpsub,nowpmelee,nowpthrow,nowpitem,nocharred,nocharblue,nocharhair,nocharitem, nochardino FROM TUserEquipment%d WHERE UID ='%I64d'"
// [PBUser] WebInfoGetMoney
// in  : tablenum,uid
// out : point
//#define USP_10400	"EXEC usp_pbuser_webinfogetmoney %d,%I64d;"
#define QUERY_1006	"SELECT point FROM TBasicInfo%d WHERE UID ='%I64d'"
// [PBUser] WebInfoGetRecord
// in  : tablenum,uid
// out : win, lose, draw, dis
//#define USP_10500	"EXEC usp_pbuser_webinfogetrecord %d,%I64d;"
#define QUERY_1007	"SELECT win, lose, draw, dis FROM TUserRecord%d WHERE UID ='%I64d'"
// [PBUser] WebInfoChangeNickName
// in  : tablenum,nick,uid
// out : result
//#define USP_10600	"EXEC usp_pbuser_webinfochangenickname %d,N'%s',%I64d;"
#define QUERY_1008	"UPDATE TBasicInfo%d SET nick=N'%s' WHERE UID ='%I64d'"
// [PBUser] WebInfoChangeGameMoney
// in  : tablenum,point,uid
// out : result
//#define USP_10700	"EXEC usp_pbuser_webinfochangegamemoney %d,%d,%I64d;"
#define QUERY_1009	"UPDATE TBasicInfo%d SET point='%d' WHERE UID='%I64d'"
// [PBUser] WebInfoChangeExp
// in  : tablenum,exp,rank,uid
// out : result
//#define USP_10800	"EXEC usp_pbuser_webinfochangeexp %d,%d,%d,%I64d;"
#define QUERY_1010	"UPDATE TBasicInfo%d SET exp='%d', rank='%d' WHERE UID='%d'"
// [PBUser] WebInfoChangeSRecord
// in  : tablenum,smatch,swin,slose,sdraw,uid
// out : result
//#define USP_10900	"EXEC usp_pbuser_webinfochangesrecord %d,%d,%d,%d,%d,%d;"
#define QUERY_1011	"UPDATE TUserRecord%d SET smatch='%d',swin='%d', slose='%d', sdraw='%d' WHERE no ='%d'"
// [PBUser] WebInfoChangeRecord
// in  : tablenum,match,win,lose,draw,dis,uid
// out : result
//#define USP_11000	"EXEC usp_pbuser_webinfochangerecord %d,%d,%d,%d,%d,%d,%d;"
#define QUERY_1012	"UPDATE TUserRecord%d SET match='%d', win='%d', lose='%d', draw='%d', dis='%d' WHERE no ='%d'"
// [PBUser] WebInfoIsValidClanname
// in  : clanName
// out : clanNo
//#define USP_11100	"EXEC usp_pbuser_webinfoisvalidclanname N'%s';"
#define QUERY_1013	"SELECT no FROM TClan WHERE name = N'%s'"
// [PBUser] WebInfoChangeClanName
// in  : clanName,muid
// out : result
//#define USP_11200	"EXEC usp_pbuser_webinfochangeclanname N'%s',%d;"
#define QUERY_1014	"UPDATE TClan SET name = N'%s' WHERE muid = %d"
// [PBUser] WebInfoClanInfo
// in  : clanNo
// out : match,win,lose
//#define USP_11300	"EXEC usp_pbuser_webinfoclaninfo %d;"
#define QUERY_1015	"SELECT name, mnick, builddate, curper, exprank, exp, azitURL FROM TClan WHERE no = '%d'"
#define QUERY_1016	"SELECT match, win, lose FROM TClan WHERE no = '%d'"
// [PBUser] WebInfoGetFriendList
// in  : tablenum,uid,state,
// out : nick, clanidx, clanstate, state
//#define USP_11400	"EXEC usp_pbuser_webinfogetfriendlist %d,%d,%d,%d;"
#define QUERY_1017	"SELECT nick, clanidx, clanstate, state FROM TBasicInfo%d, TCmnFr%d WHERE uid = %d AND state >= %d AND TUserInfo.no = fuid"

// [PBUser] GetRankData
// in  : top_cnt,tablenum,min_exp,max_rank
// out : UID,nick,exp,rank,keep_rank
#define USP_11500	"EXEC usp_11500_pbuser_getrankdata %d,%d,%d,%d;"
#define QUERY_1018	"SELECT TOP %d A.[UID],A.[nick],A.[exp],A.[rank],A.[keep_rank] FROM [TBasicInfo%d] AS A JOIN (SELECT [idx] FROM [TUserIdx] WHERE %d = ([idx]%%%d) AND [blockdate] < GETDATE()) AS B ON A.UID = B.idx WHERE A.[exp] >= %d AND A.[rank] < %d ORDER BY A.[exp] DESC"
// [PBUser] UpdateRankAll46
// in  : tablenum,init_rank,min_rank,max_rank,min_exp,max_exp
// out : result
#define USP_11600	"EXEC usp_11600_pbuser_updaterankall46 %d,%d,%d,%d,%d,%d;"
#define QUERY_1019	"UPDATE [TBasicInfo%d] SET [rank] = %d WHERE [rank] > %d AND [rank] < %d AND [exp] >= %d AND  [exp] < %d AND [logindate] <> 0;"
// [PBUser] UpdateRankUp
// in  : tablenum,rank,uid
// out : result
#define USP_11700	"EXEC usp_11700_pbuser_updaterankup %d,%d,%I64d;"
#define QUERY_1020	"UPDATE [TBasicInfo%d] SET [rank] = %d WHERE [UID] = %I64d"

// [PBUser] CLoginWorker_Italy::_WorkProcess
// in  : strID, strPW
// out : 
#define USP_10001	"EXECUTE GAME_LOGIN_PB_IT N'%s', N'%s' "
// [] CLoginWorker_NorthA::_GetExtensionInfoNorth
// in  : UserID
// out : 
#define USP_10002	"EXECUTE prc_uwlt_chk_tot_byPB N'%s' "
// [PBUser] CModuleDBNick::Working
// in  : strNick,i64UID
// out : 
#define USP_11800	"EXEC usp_11800_pbuser_nick_find N'%s',%I64d;"
// in  : strNick,i64UID
// out : 
#define USP_11810	"EXEC usp_11810_pbuser_nick_insert N'%s',%I64d;"
// in  : strNick,strOldNick,i64UID
// out : 
#define USP_11820	"EXEC usp_11820_pbuser_nick_change N'%s',N'%s',%I64d;"
// in  : strNick
// out : UID
//#define USP_11830	"EXEC usp_11830_pbuser_SEND_GIFT_find N'%s';"
//#define QUERY_1021	"SELECT UID FROM TNickInfo WHERE nick =N'%s' "
// [PBUser] _GetUID
// in  : strNick
// out : UID, DeleteDate
//#define USP_11900	"EXEC usp_11900_pbuser_get_uid N'%s';"
//#define QUERY_1022	"SELECT UID, DeleteDate FROM TNickInfo WHERE nick=N'%s' "
// [PBUser] CModuleDBUID::OnWorking
// in  : strData,i64UID,ui32ItemID
// out : result
#define USP_12000	"EXEC usp_12000_pbuser_onworking0 %s,%I64d,%d;"
#define QUERY_1058	"UPDATE TInvItemData SET data = %s WHERE UID=%I64d AND ItemID=%d "
// in  : i64UID,ui32ItemID,strData
// out : result
#define USP_12010	"EXEC usp_12010_pbuser_onworking1 %I64d,%d,%s;"
#define QUERY_1059	"INSERT INTO TInvItemData (UID, ItemID, data) VALUES(%I64d,%d, %s)"
// in  : i64UID, ui32ItemID
// out : result
#define USP_12020	"EXEC usp_12020_pbuser_onworking2 %I64d,%d;"
#define QUERY_1060	"DELETE FROM TInvItemData WHERE UID=%I64d AND ItemID=%d"
// in  : i64UID
// out : ItemID, data
#define USP_12030	"EXEC usp_12030_pbuser_onworking3 %I64d;"
#define QUERY_1061	"SELECT ItemID, data FROM TInvItemData WHERE UID='%I64d'"
// in  : i8Isblock(1), strTempComment, strTempId
// out : result
#define USP_12040	"EXEC usp_12040_pbuser_onworking4 %d,N'%S',N'%s';"
#define QUERY_1062	"UPDATE TUserIdx SET isblock=%d, comment=N'%s' WHERE id=N'%s'"
// [PBUser] _GetUID_UIDTHREAD
// in  : UserID(50)
// out : result(1:old,2:new),i64UID,ui32BlockDate
#define USP_12100	"EXEC usp_12100_pbuser_getuid0 N'%s';"
#define QUERY_1063	"SELECT idx, blockdate FROM TUserIdx WHERE id=N'%s' "
#define QUERY_1064	"INSERT INTO TUserIdx (id) VALUES(N'%s')"
#define QUERY_1065	"SELECT idx FROM TUserIdx WHERE id=N'%s' "
// in  : UserID(50),pass(21)
// out : idx(UID), blockdate
#define USP_12110	"EXEC usp_12110_pbuser_getuid1 N'%s',N'%s';"
#define QUERY_1066	"SELECT idx, blockdate FROM TUserIdx WHERE id=N'%s' AND pass=N'%s' "
// in  : i64UID,UserID(50)
// out : idx(UID)
#define USP_12120	"EXEC usp_12120_pbuser_getuid2 %I64d,N'%s';"
#define QUERY_1067	"SELECT idx FROM TUserIdx WHERE id=N'%s' and idx=%I64d "
#define QUERY_1068	"INSERT INTO TUserIdx (id, idx) VALUES(N'%s', %I64d)"
#define QUERY_1069	"SELECT idx FROM TUserIdx WHERE id=N'%s' and idx=%I64d "
// in  : i64UID,UserID(50)
// out : idx(UID), blockdate
#define USP_12130	"EXEC usp_12130_pbuser_getuid3 %I64d,N'%s';"
#define QUERY_1070	"SELECT id, blockdate FROM TUserIdx WHERE idx = %I64d "
#define QUERY_1071	"INSERT INTO TUserIdx (id, idx ) VALUES(N'%s', %I64d )"
#define QUERY_1072	"SELECT idx FROM TUserIdx WHERE id=N'%s' and idx=%I64d "
// in : i64UID, i32Birthday
// out : result(Success 1, Fail -1/-2)
#define USP_12140	"EXEC usp_12140_pbuser_setbirthday %I64d, %d;"
// [PBUser] CModuleDBUserSave::_SaveUserInfo
// in  : tablenum,ui32ConnectTime,ui32LogInTime,ui32InvneTime,ui32Exp,ui32Point,ui32Keep_rank,ui32AbusingCount,ui32EventItemDate,ui32PresentedDate,ui8PresentedSupplyBox,ui8NickColor,ui8TutorialIng,i64UID
// out : result
#define USP_12200	"EXEC usp_12200_pbuser_saveuserinfo0 %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,N'%s',%I64d;"
#define QUERY_1073	"UPDATE TBasicInfo%d SET connecttime=connecttime+%d,connectcount=connectcount+1,logindate=%d,logoutdate=%d,exp=%d,point=%d,keep_rank=%d,AbusingCount=%d,EventItemDate=%d,SupplyBoxPresentedTime=%d, SupplyBoxPresentedCount=%d,ui8NickColor=%d,TutorialIng=%d WHERE UID=%I64d"
// in  : tablenum,ui32ConnectTime,ui32LogInTime,ui32InvneTime,ui32Exp,ui32Point,ui32Rank,ui32Keep_rank,ui32AbusingCount,ui32EventItemDate,ui32PresentedDate,ui8PresentedSupplyBox,ui8NickColor,ui8TutorialIng,i64UID
// out : result
#define USP_12210	"EXEC usp_12210_pbuser_saveuserinfo1 %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,N'%s',%I64d;"
#define QUERY_1074	"UPDATE TBasicInfo%d SET connecttime=connecttime+%d,connectcount=connectcount+1,logindate=%d,logoutdate=%d,exp=%d,point=%d,rank=%d,keep_rank=%d,AbusingCount=%d,EventItemDate=%d,SupplyBoxPresentedTime=%d, SupplyBoxPresentedCount=%d,ui8NickColor=%d,TutorialIng=%d WHERE UID=%I64d"
// in  : tablenum,ui32ConnectTime,ui32LogInTime,ui32InvneTime,i64UID
// out : result
#define USP_12220	"EXEC usp_12220_pbuser_saveuserinfo2 %d,%d,%d,%d,%I64d;"
#define QUERY_1075	"UPDATE TBasicInfo%d SET connecttime=connecttime+%d,connectcount=connectcount+1,logindate=%d,logoutdate=%d WHERE UID=%I64d"
// in  : tablenum,ui32smatch,ui32swin,ui32slose,ui32skillcount,ui32sheadshot,ui32sdeath,ui32sdraw,ui32sdis,ui32sdmatch,ui32sdkillcount,i64UID
// out : result
#define USP_12230	"EXEC usp_12230_pbuser_saveuserinfo3 %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%I64d;"
#define QUERY_1076	"UPDATE TUserRecord%d SET smatch='%d',swin='%d',slose='%d',skillcount='%d',sheadshot='%d',sdeath='%d',sdraw='%d',sdis='%d',sdmatch='%d',sdkillcount='%d' WHERE UID ='%I64d'"
// in  : tablenum,ui32match,ui32win,ui32lose,ui32killcount,ui32headshot,ui32death,ui32draw,ui32dis,ui32dmatch,ui32dkillcount,i64UID
// out : result
#define USP_12240	"EXEC usp_12240_pbuser_saveuserinfo4 %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%I64d;"
#define QUERY_1077	"UPDATE TUserRecord%d SET match=%d,win=%d,lose=%d,killcount=%d,headshot=%d,death=%d,draw=%d,dis=%d,dmatch=%d,dkillcount=%d WHERE UID=%I64d"
// in  : tablenum,ui32RedNum,ui32BlueNum,ui32HairNum,ui32ItemNum,ui32DinoNum,ui32noprim,ui32nosub,ui32nomelee,ui32nothrow,ui32noitem,i64UID
// out : result
#define USP_12250	"EXEC usp_12250_pbuser_saveuserinfo5 %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%I64d;"
#define QUERY_1078	"UPDATE TUserEquipment%d SET nocharred='%d',nocharblue='%d',nocharhair='%d',nocharitem='%d', nochardino='%d', nowpprim='%d',nowpsub='%d',nowpmelee='%d',nowpthrow='%d',nowpitem='%d' WHERE UID ='%I64d'"
// in  : tablenum,ui32Miniature,ui32Insignia,ui32Order,ui32Master,ui64GainUserTitle,strMission0,strMission1,strMission2,strMission3,strMissionIng,ui32iQuestEventDate,ui8EquipUserTitle0,ui8EquipUserTitle1,ui8EquipUserTitle2,i64UID
// out : result
#define USP_12260	"EXEC usp_12260_pbuser_saveuserinfo6 %d,%d,%d,%d,%d,%I64d,%s,%s,%s,%s,%s,%d,%d,%d,%d,%I64d;"
#define QUERY_1079	"UPDATE TUserCh_Medal%d SET iMiniature=%d,iInsignia=%d,iMedal=%d,iMaster=%d,iUserTitle=%I64u,Mission00=%s,Mission01=%s,Mission02=%s,Mission03=%s,MissionIng=%s,eventdate=%d,userTitle1=%d,userTitle2=%d,userTitle3=%d WHERE UID=%I64d"
// [PBUser] CModuleDBWorker::OnWorking
// in  : tablenum,strNick,i64UID
// out : result
#define USP_12300	"EXEC usp_12300_pbuser_nick_change_update %d,N'%s',%I64d;"
#define QUERY_1080	"UPDATE TBasicInfo%d SET nick=N'%s' WHERE UID=%I64d"
// [PBUser] CModuleDBWorker::_LoadUserInfo
// in  : tablenum,i64UID
// out : nick,createdate,connectcount,connecttime,exp,point,rank,keep_rank,AbusingCount,ui8NickColor,EventItemDate,SupplyBoxPresentedTime,SupplyBoxPresentedCount,logindate,TutorialIng,ESportLevel
#define USP_12400	"EXEC usp_12400_pbuser_loaduserinfo0 %d,%I64d;"
#define QUERY_1081	"SELECT nick,createdate,connectcount,connecttime,exp,point,rank,keep_rank,AbusingCount,ui8NickColor,EventItemDate,SupplyBoxPresentedTime,SupplyBoxPresentedCount,logindate,TutorialIng,TotalBattleTime,LastBuyCash,ESportLevel FROM TBasicInfo%d WHERE UID=%I64d"
// in  : tablenum,i64UID
// out : nick,createdate,connectcount,connecttime,exp,point,rank,keep_rank,AbusingCount,ui8NickColor,EventItemDate,SupplyBoxPresentedTime,SupplyBoxPresentedCount,logindate,TutorialIng
#define USP_12410	"EXEC usp_12410_pbuser_loaduserinfo1 %d,%I64d;"
#define QUERY_1082	"SELECT nick,createdate,connectcount,connecttime,exp,point,rank,keep_rank,AbusingCount,ui8NickColor,EventItemDate,SupplyBoxPresentedTime,SupplyBoxPresentedCount,logindate,TutorialIng,TotalBattleTime,LastBuyCash FROM TBasicInfo%d WHERE UID=%I64d"
// in  : tablenum,i64UID,createdate,point
// out : result
#define USP_12420	"EXEC usp_12420_pbuser_loaduserinfo2 %d,%I64d,%d,%d;"
#define QUERY_1083	"INSERT INTO TBasicInfo%d (UID,createdate,point) VALUES (%I64d,%d,%d)"
// in  : tablenum,i64UID,
// out : result(1,2),match,win,lose,killcount,headshot,death,dis,draw,dmatch,dkillcount,smatch,swin,slose,skillcount,sheadshot,sdeath,sdis,sdraw,sdmatch,sdkillcount
#define USP_12430	"EXEC usp_12430_pbuser_loaduserinfo3 %d,%I64d;"
#define QUERY_1084	"SELECT match,win,lose,killcount,headshot,death,dis,draw,dmatch,dkillcount,smatch,swin,slose,skillcount,sheadshot,sdeath,sdis,sdraw,sdmatch,sdkillcount FROM TUserRecord%d WHERE UID=%I64d"
#define QUERY_1085	"INSERT INTO TUserRecord%d (UID) VALUES (%I64d)"
// in  : tablenum,i64UID,
// out : result(1,2),nowpprim,nowpsub,nowpmelee,nowpthrow,nowpitem,nocharred,nocharblue,nocharhair,nocharitem, nochardino
#define USP_12440	"EXEC usp_12440_pbuser_loaduserinfo4 %d,%I64d;"
#define QUERY_1086	"SELECT nowpprim,nowpsub,nowpmelee,nowpthrow,nowpitem,nocharred,nocharblue,nocharhair,nocharitem, nochardino FROM TUserEquipment%d WHERE UID=%I64d"
#define QUERY_1087	"INSERT INTO TUserEquipment%d (UID) VALUES(%I64d)"
// in  : tablenum,i64UID,
// out : result(1,2),Mission00,Mission01,Mission02,Mission03,MissionIng,iMiniature,iInsignia,iMedal,iMaster,eventdate,iUserTitle,userTitle1,userTitle2,userTitle3
#define USP_12450	"EXEC usp_12450_pbuser_loaduserinfo5 %d,%I64d;"
#define QUERY_1088	"SELECT Mission00,Mission01,Mission02,Mission03,MissionIng,iMiniature,iInsignia,iMedal,iMaster,eventdate,iUserTitle,userTitle1,userTitle2,userTitle3 FROM TUserCh_Medal%d WHERE UID=%I64d"
#define QUERY_1089	"INSERT INTO TUserCh_Medal%d (UID) VALUES(%I64d)"

#define USP_12600	"EXEC usp_12600_pbuser_TUserDailyRecord_set %I64d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d"

#define USP_12610	"EXEC usp_12610_pbuser_TUserDailyRecord_sel %I64d;"

#define USP_12620	"EXEC usp_12620_pbuser_TUserDailyRecord_init"

#define USP_12710	"EXEC usp_12710_pbuser_UAttendanceUser_sel %d,%I64d"
// [PBUser] 

#define USP_12800	"EXEC usp_12800_pbuser_nick_new_insert N'%s',%I64d"

#define USP_12900	"EXEC usp_12900_pbuser_nick_new_change N'%s',N'%s',%I64d"

//
#define USP_14070	"EXEC usp_14070_pbuser_saveuserinfo0 %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,N'%s',%d,%I64d;"
#define QUERY_1090	"UPDATE TBasicInfo%d SET connecttime=connecttime+%d,connectcount=connectcount+1,logindate=%d,logoutdate=%d,exp=%d,point=%d,keep_rank=%d,AbusingCount=%d,EventItemDate=%d,SupplyBoxPresentedTime=%d, SupplyBoxPresentedCount=%d,ui8NickColor=%d,TutorialIng=%d,TotalBattleTime=%d,LastBuyCash=%s,ReportCount=%d WHERE UID=%I64d"

// in  : tablenum,ui32ConnectTime,ui32LogInTime,ui32InvneTime,ui32Exp,ui32Point,ui32Rank,ui32Keep_rank,ui32AbusingCount,ui32EventItemDate,ui32PresentedDate,ui8PresentedSupplyBox,ui8NickColor,ui8TutorialIng,i64UID
// out : result
#define USP_14080	"EXEC usp_14080_pbuser_saveuserinfo1 %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,N'%s',%d,%I64d;"
#define QUERY_1091	"UPDATE TBasicInfo%d SET connecttime=connecttime+%d,connectcount=connectcount+1,logindate=%d,logoutdate=%d,exp=%d,point=%d,rank=%d,keep_rank=%d,AbusingCount=%d,EventItemDate=%d,SupplyBoxPresentedTime=%d, SupplyBoxPresentedCount=%d,ui8NickColor=%d,TutorialIng=%d,TotalBattleTime=%d,LastBuyCash=%s,ReportCount=%d WHERE UID=%I64d"

//
#define USP_12401	"EXEC usp_12401_pbuser_loaduserinfo0_new %d,%I64d;"
#define QUERY_1092	"SELECT nick,createdate,connectcount,connecttime,exp,point,rank,keep_rank,AbusingCount,ui8NickColor,EventItemDate,SupplyBoxPresentedTime,SupplyBoxPresentedCount,logindate,TutorialIng,TotalBattleTime,LastBuyCash,ReportCount,ESportLevel FROM TBasicInfo%d WHERE UID=%I64d"

// in  : tablenum,i64UID
// out : nick,createdate,connectcount,connecttime,exp,point,rank,keep_rank,AbusingCount,ui8NickColor,EventItemDate,SupplyBoxPresentedTime,SupplyBoxPresentedCount,logindate,TutorialIng
#define USP_12411	"EXEC usp_12411_pbuser_loaduserinfo1_new %d,%I64d;"
#define QUERY_1093	"SELECT nick,createdate,connectcount,connecttime,exp,point,rank,keep_rank,AbusingCount,ui8NickColor,EventItemDate,SupplyBoxPresentedTime,SupplyBoxPresentedCount,logindate,TutorialIng,TotalBattleTime,LastBuyCash,ReportCount FROM TBasicInfo%d WHERE UID=%I64d"

//-----------------------------------------------------------------------------------

// Taiwan Billing Cash Check
// in	: id
// out	: result, Current cash
#define USP_TAIWAN_CASH_CHECK	"EXEC sp_getGameCash N'%s' "
#define USP_THAI_GARENA_CASH_CHECK		"EXEC usp_GameCash_sel_PB %I64d"

//////////////////////////////////////////////////////////////
// v1.5 Medal
#define USP_UMEDAL_BASICINFO_SEL		"EXEC usp_UMedalBasicInfo_sel %I64d"	// 메달 기본 정보 조회.
// Input Parameter
//		@iUSN_				BIGINT
// Return Value
//		i16NotifyMedalIDX1	SMALLINT
//		i16NotifyMedalIDX1	SMALLINT
//		i16NotifyMedalIDX1	SMALLINT
//		i16NotifyMedalIDX1	SMALLINT


#define USP_UMEDAL_BASICINFO_MER		"EXEC usp_UMedalBasicInfo_mer %I64d, %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d"	// 메달 기본 정보 기록.
// Input Parameter
//		@iUSN_               BIGINT
//		@i16NotifyMedalType1_ TINYINT
//		@i16NotifyMedalType2_ TINYINT
//		@i16NotifyMedalType3_ TINYINT
//		@i16NotifyMedalType4_ TINYINT
//		@i16NotifyMedalIDX1_ SMALLINT
//		@i16NotifyMedalIDX2_ SMALLINT
//		@i16NotifyMedalIDX3_ SMALLINT
//		@i16NotifyMedalIDX4_ SMALLINT
//		@i16NotifyMedalCount1_ SMALLINT
//		@i16NotifyMedalCount2_ SMALLINT
//		@i16NotifyMedalCount3_ SMALLINT
//		@i16NotifyMedalCount4_ SMALLINT

// Return Value
//		@iResult             SMALLINT = 0   


#define USP_UMEDAL_SEL		"EXEC usp_UMedal_sel %I64d"	// 메달 조회
// Input Parameter
//		@iUSN_			BIGINT
// Return Value
//		i16MedalIDX			SMALLINT
//		i16Count			SMALLINT
//		ui8GetReward		TINYINT


#define USP_UMEDAL_MER		"EXEC usp_UMedal_mer %I64d, %d, %d, %d"	// 메달 기록
// Input Parameter
//		@iUSN_			BIGINT
//		@i16MedalIDX_		SMALLINT
//		@i16Count_		SMALLINT
//		@ui8GetReward		TINYINT
// Return Value
//		 @iResult			SMALLINT



#define USP_UCURRENT_MEDALSET_MER		"EXEC usp_UCurrentMedalSet_mer %I64d, %d,%s,%d,%d,%d,%d,%d,%d,%d,%d"	// 진행중인 메달 셋트 기록
// Input Parameter
//		@iUSN_              BIGINT
//		@i8Type_            TINYINT
//		@i16MedalSetIDX_    SMALLINT
//		@i16Medal1Count_    SMALLINT
//		@i16Medal2Count_    SMALLINT
//		@i16Medal3Count_    SMALLINT
//		@i16Medal4Count_    SMALLINT
//		@i16Medal5Count_    SMALLINT
//		@i16Medal6Count_    SMALLINT
//		@ui8GetReward		TINYINT
// Return Value
//		@iResult            SMALLINT = 0   



#define USP_UCURRENT_MEDALSET_SEL		"EXEC usp_UCurrentMedalSet_sel %I64d"	// 진행중인 메달 셋트 조회.
// Input Parameter
//		@iUSN_				BIGINT
// Return Value
//    i8Type				TINYINT
//,   i16MedalSetIDX		SMALLINT
//,   sdtRecvDate			SMALLDATETIME
//,   i16Medal1Count		SMALLINT
//,   i16Medal2Count		SMALLINT
//,   i16Medal3Count		SMALLINT
//,   i16Medal4Count		SMALLINT
//,   i16Medal5Count		SMALLINT
//,   i16Medal6Count		SMALLINT
//,   ui8GetReward			TINYINT


#define USP_UCURRENT_MEDALSET_COM		"EXEC usp_UCurrentMedalSet_complete %I64d, %d,%d"	// 진행중인 메달 셋트 완료.
// Input Parameter
//		@iUSN_              BIGINT
//		@i8Type_            TINYINT
//		@i16MedalSetIDX_    SMALLINT
// Return Value
//		@iResult            SMALLINT = 0   



#define USP_UCOMPLETE_MEDALSET_SEL		"EXEC usp_UCompleteMedalSet_sel %I64d"	// 완료된 메달 셋트 조회.
// Input Parameter
//		@iUSN_				BIGINT
// Return Value
//    i8Type
//,   i16MedalSetIDX
//,   sdtRecvDate
//,   sdtCompleteDate
//,   i16Medal1Count
//,   i16Medal2Count
//,   i16Medal3Count
//,   i16Medal4Count
//,   i16Medal5Count
//,   i16Medal6Count
//,   ui8GetReward		

#define USP_UCOMPLETE_MEDALSET_UPD		"EXEC usp_UCompleteMedalSet_upd %I64d, %d,%d,%d"	// 완료된 메달 셋트 기록. 보상 횟득 여부.
// Input Parameter
//		@iUSN_           BIGINT
//		@i8Type_         TINYINT
//		@i16MedalSetIDX_ SMALLINT
//		@ui8GetReward	TINYINT
// Return Value
//		@iResult         SMALLINT 


#define USP_UMEDAL_UPD		"EXEC usp_UMedal_upd %I64d, %d,%d"	// 일반 메달 보상 지급 여부 기록.
// Input Parameter
//  @iUSN_          BIGINT
//  @i16MedalIDX_   SMALLINT 
// @ui8GetReward_   TINYINT  
// Return Value
//		@iResult    SMALLINT 



#define USP_UCURMEDALSET_UPD		"EXEC usp_UCurrentMedalSet_upd %I64d, %d,%d,%d"	// 진행중인 메달 셋트 기록. 보상 획득여부.
// Input Parameter
//		@iUSN_           BIGINT
//		@i8Type_         TINYINT
//		@i16MedalSetIDX_ SMALLINT
//		@ui8GetReward	TINYINT
// Return Value
//		@iResult         SMALLINT 


#define USP_QUICKJOIN_SEL			"EXEC usp_QuickJoin_sel %I64d"	// 빠른입장 설정 정보 조회
// Input Parameter
//		@USN			BIGINT
// Return Value
//    StageType1		TINYINT
//,   StageOption1		TINYINT
//,   Stage1			TINYINT
//,   StageType2		TINYINT
//,   StageOption2		TINYINT
//,   Stage2			TINYINT
//,   StageType3		TINYINT
//,   StageOption3		TINYINT
//,   Stage3			TINYINT

#define USP_QUICKJOIN_MER			"EXEC usp_QuickJoin_mer %I64d, %d,%d,%d,%d,%d,%d,%d,%d,%d"	// 빠른입장 설정 정보 기록.
// Input Parameter
//		@USN			BIGINT
//    StageType1		TINYINT
//,   StageOption1		TINYINT
//,   Stage1			TINYINT
//,   StageType2		TINYINT
//,   StageOption2		TINYINT
//,   Stage2			TINYINT
//,   StageType3		TINYINT
//,   StageOption3		TINYINT
//,   Stage3			TINYINT

// Return Value
//	  Result			INT

#define USP_AAS_MER					"EXEC usp_UUserInfo_AAS_upd %I64d, %d"	// 하루플레이시간(분) 정보 기록
// Input Parameter
//		@USN			BIGINT
//,	  ui16AASAccMin		SMALLINT

#define USP_GMCHAT_DIRECT_BLOCK		"EXEC usp_TUserIdx_Block_upd %I64d, %d"		//GM-유저 1:1창에서 유저 즉시 블럭
//		@Idx      AS BIGINT - 블럭계정 UID
//,		@BlockDay AS INT	- 블럭일수

#define USP_GMCHAT_CHAT_BLOCK		"EXEC usp_UUserInfo_Chat_Block %I64d, %d"	//GM-유저 1:1창에서 유저 채팅 블럭
//		@Idx      AS BIGINT - 채팅금지계정 UID
//,		@BlockDay AS INT	- 채팅금지시간(초)

// AccountItemData Load
#define USP_ACCOUNTITEM_LOAD			"EXEC usp_AccountItemData_sel %I64d"	//계정 아이템 정보 로드
//		@UID_     AS BIGINT - 로그할 계정 UID

// AccountItemData Save
#define USP_ACCOUNTITEM_SAVE			"EXEC usp_AccountItemData_mer %I64d, %d, %d, %d, '%s', %d, %d"	//계정 아이템 정보 저장
//		@iUSN_						BIGINT
//		@iNicknameColor_			INT
//		@iDisguiseRank_				INT
//		@iDisguiseRank_AprilFool_   INT
//		@iFakeNick_					NVARCHAR(50)
//		@iCrossHairColor_			INT
//		@iChattingColor_			INT