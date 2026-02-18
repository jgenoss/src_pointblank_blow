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
// QUERY_300X
// QUERY : Qurey
// 300X  : PBClan Qurry Unique Number(3000)
//
// [Rule of SP Define Naming]
// USP_30X00
// USP : user stored procedure
// 30X00 : PBClan usp Unique Number(30000)
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

//LoadClanData()
// in  : 
// out : no, name,builddate,mark,exp,curper,maxper,smatch,swin,slose,match,win,lose,ranklock,lagelock,hagelock,muid,mnick,mrank,intro,announce,azitURL,authstaff,statedate,point
#define USP_30100			"EXEC usp_30100_pbclan_loadclandata;"
#define QUERY_3001			"SELECT no, name,builddate,mark,exp,curper,maxper,smatch,swin,slose,match,win,lose,ranklock,lagelock,hagelock,muid,mnick,mrank,intro,announce,azitURL,authstaff,statedate,point FROM TClan"
//LoadClanItem()
// in  : 
// out : idx,no,ItemID,date,Arg0
#define USP_30200			"EXEC usp_30200_pbclan_loadclanitem0;"
#define	QUERY_3002			"SELECT idx,no,ItemID,date,Arg0 FROM TClanItem"
// in  : idx
// out : 
#define USP_30210			"EXEC usp_30210_pbclan_loadclanitem1 %d;"
#define	QUERY_3003			"DELETE TClanItem WHERE idx=%d"
#define QUERY_3088			"DELETE TClanItem WHERE idx=%d"
//LoadMemberData()
// in  : uid
// out : no,clanNo,level,joindate,nick,rank
#define USP_30300			"EXEC usp_30300_pbclan_loadmemberdata %I64d;"
#define	QUERY_3004			"SELECT TOP(1000) no,clanNo,level,joindate,nick,rank FROM TClanMem WHERE no > %I64d ORDER BY no"
//LoadRequestData()
// in  : 
// out : no,clanNo,reqdate,nick,rank
#define USP_30400			"EXEC usp_30400_pbclan_loadrequestdata;"
#define	QUERY_3005			"SELECT no,clanNo,reqdate,nick,rank FROM TClanReq"
//_UserLogin()
// in  : uid(Frist Check uid)
// out : clanNo,level
#define USP_30500			"EXEC usp_30500_pbclan_userlogin %I64d;"
#define	QUERY_3006			"SELECT clanNo,level FROM TClanMem WHERE no=%I64d"
#define	QUERY_3007			"SELECT clanNo,0 AS level FROM TClanReq WHERE no=%I64d"
//_ClanCreate()
// in  : bAzitActive,i64uid,ui32Date,ui32Rank,wMark,strNick,strClanName,strAzit,strIntro
// out : result, clanNo(create clanNo)
// result(0:success,-1:Invalid clanmem,-2:Invalid clanreq,-3:Invalid clan name,-4:Invalid azitURL,-5:Fail Create Clan,-6:Invalid clanNo,-7:Fail Insert ClanMem)
#define USP_30600			"EXEC usp_30600_pbclan_clancreate %d,%I64d,%d,%d,%d,N'%s',N'%s',N'%s',N'%s';"
#define	QUERY_3008			"SELECT clanNo FROM TClanMem WHERE no=%I64d"
#define	QUERY_3009			"SELECT clanNo FROM TClanReq WHERE no=%I64d"
#define	QUERY_3010			"SELECT no FROM TClan WHERE name=N'%s'"
#define	QUERY_3011			"SELECT no FROM TClan WHERE azitURL=N'%s'"
#define	QUERY_3012			"INSERT INTO TClan (name,maxper,builddate,muid,mnick,mrank,intro,azitURL,mark,authstaff,point) VALUES(N'%s',%d,%d,%I64d,N'%s',%d,N'%s',N'%s',%d,%d,%f)"
#define QUERY_3013			"SELECT SCOPE_IDENTITY()"
#define	QUERY_3014			"INSERT INTO TClanMem (no,clanNo,[level],auth,joinDate,nick,rank) VALUES(%I64d,%d,%d,255,%d,N'%s',%d)"
//_ClanDestory()
// in  : clanNo, muid
// out : muid, clanNo, level
// clanNo( -1:Not Master, -2: Not Find ClanNo, -3:Empty member, -4:Fail del clan, -5:Fail del clanItem, -6:Fail del clanMem, -7:Fail del clanReq)
#define USP_30700			"EXEC usp_30700_pbclan_clandestroy %d, %I64d;"
#define	QUERY_3015			"SELECT muid FROM TClan WHERE no=%d"
#define	QUERY_3016			"SELECT clanNo,level FROM TClanMem WHERE no=%I64d"
#define	QUERY_3017			"SELECT no FROM TClanMem WHERE clanNo=%d"
#define	QUERY_3018			"DELETE FROM TClan WHERE no=%d"
#define	QUERY_3019			"DELETE FROM TClanItem WHERE no=%d"
#define	QUERY_3020			"DELETE FROM TClanMem WHERE clanNo=%d"
#define	QUERY_3021			"DELETE FROM TClanReq WHERE clanNo=%d"
//ClanRequestJoin()
// in  : uid,clanNo,Date,Rank,Killcount,Death,Match,Win,Lose,strNick,strTemp
// out : count 
// (0 : Fail, 1 : success, 100 : FULL_RequestJoin_User)
#define USP_30800			"EXEC usp_30800_pbclan_clanrequestjoin %I64d,%d,%d,%d,%d,%d,%d,%d,%d,N'%S',N'%s';"
#define QUERY_3022			"SELECT clanNo FROM TClanMem WHERE no=%I64d"
#define	QUERY_3023			"SELECT clanNo FROM TClanReq WHERE no=%I64d"
#define	QUERY_3024			"SELECT no FROM TClanReq WHERE clanNo=%d"
#define	QUERY_3025			"INSERT INTO TClanReq (no,clanNo,comment,reqdate,nick,rank,killcount,death,match,win,lose) VALUES (%I64d,%d,N'%s',%d,N'%s',%d,%d,%d,%d,%d,%d)"
//ClanRequestAccept()
// in  : clanNo
// out : curper, maxper
#define USP_30900			"EXEC usp_30900_pbclan_clanrequestaccept0 %d;"
#define	QUERY_3026			"SELECT curper, maxper FROM TClan WHERE no=%d"
// in  : uid,clanNo
// out : result,nick,rank (result : 1:success,2:Invalid clan user,3:Invalid clan req,4:Invalid clanNo)
#define USP_30910			"EXEC usp_30910_pbclan_clanrequestaccept1 %I64d, %d;"
#define	QUERY_3027			"SELECT clanNo FROM TClanMem WHERE no=%I64d"
#define	QUERY_3028			"SELECT clanNo,nick,rank FROM TClanReq WHERE no=%I64d"
// in  : uid,clanNo
// out : result( 1:success,-1:Fail Insert ClanMem,-2:Fail Del ClanReq)
#define USP_30920			"EXEC usp_30920_pbclan_clanrequestaccept2 %I64d,%d,%d,%d,N'%s';"
#define	QUERY_3029			"INSERT INTO TClanMem(no,clanNo,[level],auth,joinDate,nick,rank)VALUES(%I64d,%d,%d,%d,%d,N'%s',%d)"
#define	QUERY_3030			"DELETE FROM TClanReq WHERE no=%I64d"
// in  : uiCount, clanNo
// out : result(1:success,NoLine:Fail)
#define USP_30930			"EXEC usp_30930_pbclan_clanrequestaccept3 %d,%d;"
#define	QUERY_3031			"UPDATE TClan SET curper=curper+%d WHERE no=%d"
//ClanRequestCancel()
// in  : uid
// out : result(1:success,-1:Invalid clanreq,0:Fail del clanreq)
#define USP_31000			"EXEC usp_31000_pbclan_clanrequestcancel %I64d;"
#define	QUERY_3032			"SELECT clanNo FROM TClanReq WHERE no=%I64d"
#define QUERY_3033			"DELETE FROM TClanReq WHERE no=%I64d"
//ClanRequestDenial()
// in  : ReqCnt,clanNo,(char*)uid[100]
// out : result (1:success,-1:Invalid clanreq,-2:Invalid Clan,-3:Fail Del ClanReq,-4:Invalid ReqCnt)
#define USP_31100			"EXEC usp_31100_pbclan_clanrequestdenial %d,%d,N'%s';"
#define	QUERY_3034			"SELECT clanNo FROM TClanReq WHERE no=%I64d"
#define	QUERY_3035			"DELETE FROM TClanReq WHERE no=%I64d"
//ClanMemberSecession()
// in  : uid, clanNo
// out : result (1:success,-1:Invalid Clan,-2:Fail Del clanmem,-3:Fail update clanInfo)
#define USP_31200			"EXEC usp_31200_pbclan_clanmembersecession %I64d, %d;"
#define	QUERY_3036			"SELECT clanNo FROM TClanMem WHERE no=%I64d"
#define	QUERY_3037			"DELETE FROM TClanMem WHERE no=%I64d"
#define	QUERY_3038			"UPDATE TClan SET curper=curper-%d WHERE no=%d"
//ClanMemberDeportatio()
// in  : ReqCnt,clanNo,(char*)uid[250]
// out : result (1:success,-1:Invalid clanmem,-2:Invalid Clan,-3:Fail Del ClanReq,-4:Invalid ReqCnt,-5:Fail Update clanInfo)
#define USP_31300			"EXEC usp_31300_pbclan_clanmemberdeportatio %d,%d,N'%s';"
#define	QUERY_3039			"SELECT clanNo FROM TClanMem WHERE no=%I64d"
#define	QUERY_3040			"DELETE FROM TClanMem WHERE no=%I64d"
#define	QUERY_3041			"UPDATE TClan SET curper=curper-%d WHERE no=%d"
//ClanInvite()
// in  : uid
// out : result (1:success,-1:Invalid clanmem)
#define USP_31400			"EXEC usp_31400_pbclan_claninvite %I64d;"
#define	QUERY_3042			"SELECT clanNo FROM TClanMem WHERE no=%I64d"
//ClanInviteAccept()
// in  : uid,clanNo,date,rank,nick
// out : result (1:success,-1:Invalid clan,-2:full clanmem,-3:Invalid clanMem,-4:Invalid clanReq,-5:Fail Insert clanMem,-6:Fail Update clanInfo)
#define USP_31500			"EXEC usp_31500_pbclan_claninviteaccept %I64d,%d,%d,%d,N'%S';"
#define	QUERY_3043			"SELECT curper, maxper FROM TClan WHERE no=%d"
#define QUERY_3044			"SELECT clanNo FROM TClanMem WHERE no=%I64d"
#define	QUERY_3045			"SELECT clanNo FROM TClanReq WHERE no=%I64d"
#define	QUERY_3046			"INSERT INTO TClanMem(no,clanNo,[level],auth,joinDate,nick,rank)VALUES(%I64d,%d,%d,%d,%d,N'%s',%d)"
#define	QUERY_3047			"UPDATE TClan SET curper=curper+%d WHERE no=%d"
//ClanCommissionMaster()
// in  : masterUid,uid,clanNo
// out : result (1:success,0:fail,-1:noncount clanMem,-2:Invalid level,-3:Fail update clanMem,-4:Fail update masterlevel clanMem,-5:Fail update clanInfo)
#define USP_31600			"EXEC usp_31600_pbclan_clancommissionmaster %I64d,%I64d,%d;"
#define	QUERY_3048			"SELECT level,nick,rank FROM TClanMem WHERE no=%I64d"			
#define	QUERY_3049			"UPDATE TClanMem SET level=%d,auth=%d WHERE no=%I64d"
#define	QUERY_3050			"UPDATE TClanMem SET level=%d,auth=%d WHERE no=%I64d"
#define	QUERY_3051			"UPDATE TClan SET muid=%I64d, mnick=N'%s',mrank=%d WHERE no=%d"			
//ClanCommissionStaff()
// in  : ui8count,uid[250]
// out : result (1:success,0:fail,-1:noncount clanMem,-2:Invalid level(Master(1)),-3:fail update clanMen,-4:Invalid reqcnt)
#define USP_31700			"EXEC usp_31700_pbclan_clancomissionstaff %d,N'%s';"
#define	QUERY_3052			"SELECT level FROM TClanMem WHERE no=%I64d"
#define	QUERY_3053			"UPDATE TClanMem SET level=%d WHERE no=%I64d"
//ClanCommissionRegular()
// in  : ui8count,uid[250]
// out : result (1:success,0:fail,-1:noncount clanMem,-2:Invalid level(Master(1)),-3:fail update clanMen,-4:Invalid reqcnt)
#define USP_31800			"EXEC usp_31800_pbclan_clancommissionreqular %d,N'%s';"
#define	QUERY_3054			"SELECT level FROM TClanMem WHERE no=%I64d"
#define QUERY_3055			"UPDATE TClanMem SET level=%d WHERE no=%I64d"
//ClanGetMemberInfo()
// in  : uid
// out : swin,slose,[skill],sdeath,sheadshot,sconnect,win,lose,[kill],death,headshot,connect
#define USP_31900			"EXEC usp_31900_pbclan_clangetmemberInfo %I64d;"
#define	QUERY_3056			"SELECT swin,slose,[skill],sdeath,sheadshot,sconnect,win,lose,[kill],death,headshot,connect FROM TClanMem WHERE no=%I64d"
//ClanGetRequestInfo()
// in  : uid
// out : nick,rank,killcount,death,match,win,lose,comment
#define USP_32000			"EXEC usp_32000_pbclan_clangetrequestinfo %I64d;"
#define	QUERY_3057			"SELECT nick,rank,killcount,death,match,win,lose,comment FROM TClanReq WHERE no=%I64d"
//ClanReplaceNotice()
// in  : clanNo,notice(255)
// out : result (1:success,0:fail,-1:Fail Update)
#define USP_32100			"EXEC usp_32100_pbclan_clanreplacenotice %d,N'%S';"
#define	QUERY_3058			"UPDATE TClan SET announce=N'%s' WHERE no=%d"
//ClanReplaceIntro()
// in  : clanNo,Intro(255)
// out : result (1:success,0:fail,-1:Fail Update)
#define USP_32200			"EXEC usp_32200_pbclan_clanreplaceintro %d,N'%S';"
#define	QUERY_3059			"UPDATE TClan SET intro=N'%s' WHERE no=%d"
//ClanReplaceClanName()
// in  : clanNo,NewClanName
// out : result (1:success,0:fail,-1:Fail Update)
#define USP_32300			"EXEC usp_32300_pbclan_clanreplaceclanname %d,N'%s';"
#define	QUERY_3060			"UPDATE TClan SET name=N'%s' WHERE no=%d"
//ClanReplaceMark()
// in  : clanNo,NewMark
// out : result (1:success,0:fail,-1:Fail Update)
#define USP_32400			"EXEC usp_32400_pbclan_clanreplacemark %d,%d;"
#define	QUERY_3061			"UPDATE TClan SET mark=%d WHERE no=%d"
//ClanReplacePersonMax()
// in  : clanNo,IncreaseMemberCount
// out : result (1:success,0:fail,-1:Fail Update)
#define USP_32500			"EXEC usp_32500_pbclan_clanreplacepresonmax %d,%d;"
#define	QUERY_3062			"UPDATE TClan SET maxper=maxper+%d WHERE no=%d"
//ClanReplaceManagement()
// in  : clanNo,authstaff,ranklock,lagelock,hagelock
// out : result (1:success,0:fail,-1:Fail Update)
#define USP_32600			"EXEC usp_32600_pbclan_clanreplacemanagement %d,%d,%d,%d,%d;"
#define	QUERY_3063			"UPDATE TClan SET authstaff=%d,ranklock=%d,lagelock=%d,hagelock=%d WHERE no=%d"
//ClanReplaceMemberNick()
// in  : uid,clanNo,level,strNick
// out : result (1:success,0:fail,-1:Fail Update(clanMem),-2:Fail Update(clan),-3:Fail Update(clanReq))
#define USP_32700			"EXEC usp_32700_pbclan_clanreplacemembernick %I64d,%d,%d,N'%S';"
#define	QUERY_3064			"UPDATE TClanMem SET nick=N'%s' WHERE no=%I64d"
#define	QUERY_3065			"UPDATE TClan SET mnick=N'%s' WHERE no=%d"
#define QUERY_3066			"UPDATE TClanReq SET nick=N'%s' WHERE no=%I64d"
//ClanReplaceMemberRank()
// in  : uid,clanNo,level,rank
// out : result (1:success,0:fail,-1:Fail Update(clanMem),-2:Fail Update(clan),-3:Fail Update(clanReq))
#define USP_32800			"EXEC usp_32800_pbclan_clanreplacememberrank %I64d,%d,%d,%d;"
#define	QUERY_3067			"UPDATE TClanMem SET rank=%d WHERE no=%I64d"
#define	QUERY_3068			"UPDATE TClan SET mrank=%d WHERE no=%d"
#define	QUERY_3069			"UPDATE TClanReq SET rank=%d WHERE no=%I64d"
//ClanCheckName()
// in  : strNick(17)
// out : no (no count:success,anybody:fail)
#define USP_32900			"EXEC usp_32900_pbclan_clancheckname N'%S';"
#define	QUERY_3070			"SELECT no FROM TClan WHERE name=N'%s'"
//ClanCheckAzit()
// in  : strAzit(21)
// out : no (no count:success,anybody:fail)
#define USP_33000			"EXEC usp_33000_pbclan_clancheckazit N'%S';"
#define	QUERY_3071			"SELECT no FROM TClan WHERE azitURL=N'%s'"
//ClanItemPeriod()
// in  : ui32ClanNo,ui32ItemID,ui32Date,ui32Arg
// out : result (1:success,0:fail,-1:Fail Insert,-2:Fail update)
#define USP_33100			"EXEC usp_33100_pbclan_clanitemperiod %d,%d,%d,%d;"
#define	QUERY_3072			"SELECT idx FROM TClanItem WHERE no=%d AND ItemID=%d"
#define	QUERY_3073			"INSERT INTO TClanItem(no,ItemID,date,Arg0) VALUES(%d,%d,%d,%d)"
#define	QUERY_3074			"UPDATE TClanItem SET date=%d,Arg0=%d WHERE idx=%d"
//ClanItemResetRecord()
// in  : ui32ClanNo
// out : result (1:success,0:fail,-1:Fail update)
#define USP_33200			"EXEC usp_33200_pbclan_clanitemresetrecord %d;"
#define	QUERY_3075			"UPDATE TClan SET match=0,win=0,lose=0,smatch=0,swin=0,slose=0 WHERE no=%d"
//ClanItemResetPoint()
// in  : ui32ClanNo,r32ClanBasicPoint
// out : result (1:success,0:fail,-1:Fail update)
#define USP_33300			"EXEC usp_33300_pbclan_clanitemresetpoint %d,%d,%f;"
#define	QUERY_3076			"UPDATE TClan SET point=%f WHERE no=%d"
//ClanSetActiveState()
// in  : ui32ClanNo,ui32NowDate
// out : result (1:success,0:fail,-1:Fail update)
#define USP_33400			"EXEC usp_33400_pbclan_clansetactivestate %d,%d;"
#define QUERY_3077			"UPDATE TClan SET statedate=%d WHERE no=%d"
//ClanInfoRankUpdate()
// in  : i64UID,ui8Rank
// out : result, ui32ClanNo
// (1:success,0:fail,-1:Invalid clanMem,-2:Invalid clan,-3:Fail update clan,-4:Invalid clanReq,-5:Fail update clanReq)
#define USP_33500			"EXEC usp_33500_pbclan_claninforankupdate %I64d,%d;"
#define	QUERY_3078			"SELECT clanNo,level,rank FROM TClanMem WHERE no=%I64d"
#define	QUERY_3079			"UPDATE TClanMem SET rank = %d WHERE no = %I64d"
#define	QUERY_3080			"SELECT mrank, muid FROM TClan WHERE no=%d"
#define	QUERY_3081			"UPDATE TClan SET mrank = %d WHERE no=%d"
#define	QUERY_3082			"SELECT clanNo,rank FROM TClanReq WHERE no=%I64d"
#define	QUERY_3083			"UPDATE TClanReq SET rank = %d WHERE no = %I64d"
//ClanBattleResult()
// in  : ui32ClanNo,ui32Exp,ui32sMatch,ui32sWin,ui32sLose
// out : result (1:success,0:fail,-1:Fail update)
#define USP_33600			"EXEC usp_33600_pbclan_clanbattleresult %d,%d,%d,%d,%d;"
#define	QUERY_3084			"UPDATE TClan SET exp=%d,smatch=%d,swin=%d,slose=%d WHERE no=%d"
//ClanBattleMatchResult()
// in  : ui32ClanNo,r32Point
// out : result (1:success,0:fail,-1:Fail update)
#define USP_33700			"EXEC usp_33700_pbclan_clanbattlematchresult %d,%f;"
#define	QUERY_3085			"UPDATE TClan SET point=%f WHERE no=%d"
//ClanBattleResultUser()
// in  : i64UID,i32TableIdx,ui32Exp,ui32Win,ui32Lose,ui16Kill,ui16Death,ui16Headshot,ui32Scount
// out : result (1:success,0:fail,-1:Fail update)
#define USP_33800			"EXEC usp_33800_pbclan_clanbattleresultuser %I64d,%d,%d,%d,%d,%d,%d,%d,%d;"
#define	QUERY_3086			"UPDATE %s SET sexp=sexp+%d,swin=swin+%d,slose=slose+%d,skill=skill+%d,sdeath=sdeath+%d,sheadshot=sheadshot+%d,sconnect=sconnect+%d WHERE no=%I64d"
//ClanMemberOrder()
// in  : ui32ClanNo
// out : no,sexp,swin,skill,sheadshot,sconnect,exp,win,[kill],headshot,connect
// (no count:success,anybody:fail)
#define USP_33900			"EXEC usp_33900_pbclan_clanmemberorder %d;"
#define	QUERY_3087			"SELECT no,sexp,swin,skill,sheadshot,sconnect,exp,win,[kill],headshot,connect FROM TClanMem WHERE clanNo=%d"

#define USP_34000			"EXEC usp_34000_pbclan_clanitemdelete %d, %d;"


#define USP_CLAN_FINDINFO	"EXEC usp_CClan_FindInfo %I64d;"

// 
// [PBClan] 
// 300000
// in :  
// out : 
//#define USP_300001			"EXEC usp_TClan_check_subscribe_clan_azit N'%s';"
//#define QUERY_30XX			"SELECT no FROM TClan WHERE azitURL=N'%s';"

// Clan Match
#define USP_CLAN_MATCH_RESULT_SEL				"EXEC usp_CClanMatchResult_sel %d;"
#define USP_CLAN_MATCH_RESULT_INS				"EXEC usp_CClanMatchResult_ins %d,%d,N'%s',%d,N'%s',%d,%d,%d,%d,%d,%d,%I64d,%I64d,%I64d,%I64d,%I64d,%I64d,%I64d,%I64d;"
#define USP_CLAN_MATCH_RESULT_EMOTION_UPD		"EXEC usp_CClanMatchResult_Emotion_upd %d,%d,N'%s',%I64d,N'%s',%d;"

//-----------------------------------------------------------------------------------