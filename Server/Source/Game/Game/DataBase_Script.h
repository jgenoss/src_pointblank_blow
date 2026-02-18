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

// SetOption()
// in  : i32ThreadIdx,i64UID,i8Envdefault,strEnvBasic(none),strEnvkey(none),strMacrochat0(none),strMacrochat1(none),strMacrochat2(none),strMacrochat3(none),strMacrochat4(none),
// out : result (1:success)
#define USP_20110	"EXEC usp_20100_pbinfo_setoptiondefault %d,%I64d,%d,N'none',N'none',N'none',N'none',N'none',N'none',N'none';"	// ENV_SETTING_DEFAULT
#define QUERY_2011	"UPDATE TEnv%d SET Envdefault=%dWHERE UID=%I64d"
#define USP_20100	"EXEC usp_20100_pbinfo_setoptiondefault %d,%I64d,%d"							
//#define USP_20100	"EXEC usp_20100_pbinfo_setoptiondefault %d,%I64d,%d,%d,%d,'%s','%s','%s','%s';"	// ENV_SETTING_CHANGE
#define QUERY_2001	"UPDATE TEnv%d SET Envdefault=%d,EnvBasic=%s,Envkey=%s,macrochat0='%s',macrochat1='%s',macrochat2='%s',macrochat3='%s',macrochat4='%s' WHERE UID=%I64d"
// GetOption()
// in  : i32tablenum, i64UID
// out : Envdefault
#define USP_20200	"EXEC usp_20200_pbinfo_getoption0 %d,%I64d;"
#define QUERY_2002	"SELECT Envdefault FROM TEnv%d WHERE UID=%I64d"
// in  : i32tablenum, i64UID
// out : EnvBasic, Envkey, macrochat0, macrochat1, macrochat2, macrochat3, macrochat4
#define USP_20210	"EXEC usp_20210_pbinfo_getoption1 %d,%I64d;"
#define QUERY_2003	"SELECT EnvBasic, Envkey, macrochat0, macrochat1, macrochat2, macrochat3, macrochat4 FROM TEnv%d WHERE UID=%I64d"
// in  : i32tablenum, i64UID
// out : result (1:success)
#define USP_20220	"EXEC usp_20220_pbinfo_getoption2 %d,%I64d;"
#define QUERY_2004	"INSERT INTO TEnv%d(UID)VALUES(%I64d)"
// in  : i32tablenum, i64UID
// out : Idx
#define USP_20230	"EXEC usp_20230_pbinfo_getoption3 %I64d;"
#define QUERY_2005	"SELECT Idx FROM TEvent WHERE UID=%I64d"
// in  : i32tablenum, i64UID
// out : result (1:success)
#define USP_20240	"EXEC usp_20240_pbinfo_getoption4 %I64d;"
#define QUERY_2006	"INSERT INTO TEvent(UID) VALUES (%I64d)"

//-----------------------------------------------------------------------------------