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
// XX_YY_ZZ
// XX : stored procedure ( ex : SP )
// YY : Thread Location ( ex : FRIEND )
// ZZ : Work Description ( ex : RESET_FRIEND_LIST )
// 
// usp_web_buy_item_add
// usp			: user stored procedure
// ItemDeliver	: Access Table
// buy_item_add : Work Description
//
//-----------------------------------------------------------------------------------

//#define PROGRAM_CRASH_TEST
//#define DB_QUERY_TEST		// Used : Qurey Test

//////////////////////////////////////////////////////////////////////////
//SP를 사용할때 on 해주세요. 
#define USE_STORED_PROCEDURE //(게임서버는 모두 SP)
//////////////////////////////////////////////////////////////////////////

#define ACTIVATE_FRIEND_DATABASE
#define ACTIVATE_NOTE_DATABASE 
#define USE_CHANGE_NICK
#define USE_CHANGE_RANK
//#define USE_HASH_NICK -- 사용안함

//#define USE_MESS_ALIVE_USER
//#define USE_BUFFER_TEST


static char	s_NoteTable[DB_THREAD_COUNT][SQL_STRING_COUNT] = { "TNote0", "TNote1", "TNote2", "TNote3", "TNote4","TNote5", "TNote6", "TNote7", "TNote8","TNote9"};

//-----------------------------------------------------------------------------------
//	DataBase_Thread_friend.cpp
//-----------------------------------------------------------------------------------

