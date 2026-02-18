#include "pch.h"
#include "ModuleDBUserSave.h"

BOOL CModuleDBUserSave::_WorkingExitSave( INT32 i32ThreadIdx )
{
	if( 0 == m_ppExitSaveIn[ i32ThreadIdx ]->GetBufferCount() )	return FALSE;

	RING_EXIT_SAVE_IN* pRingIn		= (RING_EXIT_SAVE_IN *)m_ppExitSaveIn[ i32ThreadIdx ]->Pop();
	RING_EXIT_SAVE_OUT* pRingOut	= (RING_EXIT_SAVE_OUT *)m_ppExitSaveOut[ i32ThreadIdx ]->PushPointer();
	if( NULL == pRingOut )
	{
		g_pLog->WriteLog( L"Exit Save Out Buffer Full" );
		return FALSE;
	}
	else if( NULL == pRingIn )
	{
		g_pLog->WriteLog( L"Exit Save In Buffer Not Find" );
		return FALSE;
	}

	pRingOut->pNode = pRingIn->pNode; 

	LARGE_INTEGER stQP_Freq_Begin, stQP_Count_Begin, stQP_Count_End, stQP_Result;
	QueryPerformanceFrequency(&stQP_Freq_Begin);
	QueryPerformanceCounter(&stQP_Count_Begin);

	if( FALSE == _SaveExitSave( i32ThreadIdx, pRingIn ) )
	{
		g_pLog->WriteLog( L"[Error]SaveUserInfo Not Save UID : %I64d", pRingIn->pNode->_i64UID);
	}

	QueryPerformanceCounter(&stQP_Count_End);
	stQP_Result.QuadPart = (LONGLONG)stQP_Count_End.QuadPart - stQP_Count_Begin.QuadPart;

	REAL64 r64ElepseTime = (REAL64)stQP_Result.QuadPart / (REAL64)stQP_Freq_Begin.QuadPart;

	g_pStatistics->AddStat(STAT_TYPE_DB_LOGOUT, i32ThreadIdx, r64ElepseTime);

	if (r64ElepseTime > CHECK_TIMEOUT )
	{	// DB TIMEOVER
		g_pLog->WriteLog(L"[ModuleDBUserSave] DB Time Over (Sec) : %.6lf / UID : %I64d ", r64ElepseTime, pRingIn->pNode->_i64UID);
	}

	m_ppExitSaveIn[ i32ThreadIdx ]->PopIdx();
	m_ppExitSaveOut[ i32ThreadIdx ]->PushPointerIdx(); 

	return TRUE;
}

BOOL CModuleDBUserSave::_SaveExitSave( INT32 i32ThreadIdx, RING_EXIT_SAVE_IN * pIn )
{
	//NODE_SAVE_TYPE eSaveType = (NODE_SAVE_TYPE)pIn->i8Type;
	wchar_t wstrSQL[SERVER_STRING_COUNT];
	CADODatabase * pADO = m_ppSaveDB[ i32ThreadIdx ];
	BOOL bRv = TRUE;
	UINT32	ui32UpdateInfo = 0;

	USER_NODE * pNode = pIn->pNode; 
	if( USER_STATE_INFO_SAVE != pNode->_ui32State )
	{
		g_pLog->WriteLog( L"[ERROR]_SaveExitSave STATE Error Thread=%d, iUID=%I64d, stage=%d", i32ThreadIdx, pNode->_i64UID, pNode->_ui32State );
		return FALSE;
	}

	ui32UpdateInfo = pNode->_ui32UpdateInfo; 

	//기본정보
	wchar_t wstrLoginDate[ SMALLDATE_STRING_SIZE ];
	pNode->m_dt32LoginDate.GetSmallDate( wstrLoginDate, SMALLDATE_STRING_SIZE );

	if( SAVE_DATABASE_BASEINFO & ui32UpdateInfo )
	{	
		wchar_t wstrLastBuyCash[SMALLDATE_STRING_SIZE];
		S2DB::ConvertUINT32ToSmallDate( wstrLastBuyCash, SMALLDATE_STRING_SIZE, pNode->m_stUserInfoServer.m_ui32LastBuyCash );
		wchar_t wstrChatBlockDate[SMALLDATE_STRING_SIZE];
		S2DB::ConvertUINT32ToSmallDate( wstrChatBlockDate, SMALLDATE_STRING_SIZE, pNode->m_stUserInfoServer.m_ui32ChatBlockDate );
		wchar_t wstrDate[ SMALLDATE_STRING_SIZE ];
		i3ThreadTimer::LocalTime().GetSmallDate( wstrDate, SMALLDATE_STRING_SIZE );
		wchar_t wstrHackChatBlockDate[ SMALLDATE_STRING_SIZE ];
		//S2DB::ConvertUINT32ToSmallDate( wstrHackChatBlockDate, SMALLDATE_STRING_SIZE, pNode->m_stUserInfoServer.m_ui32HackChatBlockDate );
		pNode->m_stUserInfoServer.m_dt32HackChatBlockDate.GetSmallDate(wstrHackChatBlockDate, SMALLDATE_STRING_SIZE);
		S2DB::MakeQueryW( wstrSQL, L"usp_Basic_upd", 
			DB_DATA_TYPE_BIGINT,		pNode->_i64UID,										//    @i64UserUID_                BIGINT
			DB_DATA_TYPE_TTCHAR,		pNode->_sBasic.m_strNick,							//,   @strUNickname_              NVARCHAR(50)
			DB_DATA_TYPE_INT,			pNode->_sBasic.m_ui32Exp,							//,   @i32EXP_                    INT
			DB_DATA_TYPE_INT,			pNode->_sBasic.m_ui32Point,							//,   @i32Point_                  INT
			DB_DATA_TYPE_INT,			pNode->_sBasic.m_ui32Rank,							//,   @i32RANK_                   INT
			DB_DATA_TYPE_INT,			pNode->_ui32TotalConnectTime,						//,   @i32ConnectTime_            INT
			DB_DATA_TYPE_SMALLDATETIME,	wstrLoginDate,										//,   @sdtLogin_                  SMALLDATETIME
			DB_DATA_TYPE_SMALLDATETIME,	wstrDate,											//,   @sdtLogout_                 SMALLDATETIME
			DB_DATA_TYPE_INT,			pNode->_sBasic.m_ui32KeepRank,						//,   @i32KeepRank_               INT
			DB_DATA_TYPE_TINYINT,		pNode->_ui8ESportIDFlag,							//,   @ui8ESportLevel_            TINYINT
			DB_DATA_TYPE_INT,			pNode->_sBasic.m_ui32AbusingCount,					//,   @i32AbusingCNT_             INT
			DB_DATA_TYPE_SMALLDATETIME,	wstrHackChatBlockDate,	// 채팅 도배 방지로 사용		//,   @sdtEventItem_              SMALLDATETIME
			DB_DATA_TYPE_TINYINT,		pNode->_sBasic.m_ui8TutorialIng,					//,   @ui8Tutorialing_            TINYINT
			DB_DATA_TYPE_INT,			pNode->m_stUserInfoServer.m_ui32TotalBattleTime,	//,   @i32TotalBattleTime_        INT
			DB_DATA_TYPE_SMALLDATETIME,	wstrLastBuyCash,									//,   @sdtLastBuyCash_            SMALLDATETIME
			DB_DATA_TYPE_BIGINT,		pNode->_sBasic.m_ui64GuideComplete,					//,   @ui64GuideComplete_		  BIGINT
			DB_DATA_TYPE_SMALLDATETIME,	wstrChatBlockDate,			 						//,   @sdtChatBlockDate           SMALLDATETIME
			DB_DATA_TYPE_INT,			pNode->_sBasic.m_ui32Coin,							//,   @ui32Coin_                  INT
			DB_DATA_TYPE_TINYINT,		pNode->m_stUserInfoServer.m_ui8VoteCount,			//,   @VoteCount_				  TINYINT
			DB_DATA_TYPE_END );
		//pNode->_ui8ReportCount, 
	}
	else
	{
		wchar_t wstrDate[ SMALLDATE_STRING_SIZE ];
		i3ThreadTimer::LocalTime().GetSmallDate( wstrDate, SMALLDATE_STRING_SIZE );

		S2DB::MakeQueryW( wstrSQL, L"usp_Basic_upd_Login", 
			DB_DATA_TYPE_BIGINT,		pNode->_i64UID,										//	  @i64UserUID_                BIGINT
			DB_DATA_TYPE_INT,			pNode->_ui32TotalConnectTime,						//,   @i32ConnectTime_            INT
			DB_DATA_TYPE_SMALLDATETIME,	wstrLoginDate,										//,   @sdtLogin_                  SMALLDATETIME
			DB_DATA_TYPE_SMALLDATETIME,	wstrDate,											//,   @sdtLogout_                 SMALLDATETIME
			DB_DATA_TYPE_END );
	}
	if( FALSE == pADO->ExecuteSP(wstrSQL) )
	{
		ErrorDBUserSave( i32ThreadIdx, wstrSQL );
		pADO = m_ppSaveDB[ i32ThreadIdx ];
		bRv = FALSE;
	}
	else
	{
		if( 0 < pADO->GetRecordCount() )
		{
			if( 0 != pADO->GetINT16Data(0) )
			{
				_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
				bRv = FALSE;
			}
		}
		else
		{
			_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
			bRv = FALSE;
		}
	}	

	if( SAVE_DATABASE_CLAN & ui32UpdateInfo )
	{	 
		wchar_t wstrClanRankingDate[SMALLDATE_STRING_SIZE];
		S2DB::ConvertUINT32ToSmallDate( wstrClanRankingDate, SMALLDATE_STRING_SIZE, pNode->_sClan._ui32ClanRankingDate );
		
		S2DB::MakeQueryW( wstrSQL, L"usp_Basic_upd_Clan", 
			DB_DATA_TYPE_BIGINT,		pNode->_i64UID,										//    @i64UserUID_                BIGINT 
			DB_DATA_TYPE_INT,			pNode->_sClan._ui32Mark,							//,   @ui32ClanMark		  		  INT
			DB_DATA_TYPE_TTCHAR,		pNode->_sClan._strClanName,							//,   @strClanName		  		  NVARCHAR(50) 
			DB_DATA_TYPE_SMALLDATETIME,	wstrClanRankingDate,			 					//,   @strClanRankingDate          SMALLDATETIME
			DB_DATA_TYPE_END );

		if( FALSE == pADO->ExecuteSP( wstrSQL ) )
		{
			ErrorDBUserSave( i32ThreadIdx, wstrSQL );
			pADO = m_ppSaveDB[ i32ThreadIdx ];
			bRv = FALSE;
		}
		else
		{
			if( 0 < pADO->GetRecordCount() )
			{
				if( 0 != pADO->GetINT16Data(0) )
				{
					_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
					bRv = FALSE;
				}
			}
			else
			{
				_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
				bRv = FALSE;
			}
		}	
	}

	if(SAVE_DATABASE_ATTENDANCE & ui32UpdateInfo )
	{
		for( INT32 i32Type = 0; i32Type < ATTENDANCE_TYPE_MAX; i32Type++ )
		{
			if( -1 == g_pConfig->m_i32ActiveAttendanceIdx[ i32Type ] )	continue;
			
			ATTENDANCE_USER_SVR *pAttendanceUserInfo = &pNode->_Attendance[i32Type];
			S2DB::MakeQueryW( wstrSQL, L"usp_UAttendanceUser_mer",
				DB_DATA_TYPE_BIGINT,	pNode->_i64UID,
				DB_DATA_TYPE_INT,		pAttendanceUserInfo->m_ui32Version,			// 1 // 이 4개의 데이터는 DB 데이터 형식이 INT임
				DB_DATA_TYPE_INT,		pAttendanceUserInfo->m_ui8AttendanceDay,		// 2 // DB_DATA_TYPE_INT가 맞는 코드
				DB_DATA_TYPE_INT,		pAttendanceUserInfo->m_ui8AttendanceItem,	// 3 // 
				DB_DATA_TYPE_INT,		pAttendanceUserInfo->m_dt32AttendanceTime,	// 4 //
				DB_DATA_TYPE_TINYINT,	pAttendanceUserInfo->m_ui8ItemNum,
				DB_DATA_TYPE_TINYINT,	i32Type,	// 신규 유저의 경우 유저정보를 Load하지 않아 pAttendanceUserInfo->m_ui8AttendanceType이 세팅 안 됨.
				DB_DATA_TYPE_BIT,		pAttendanceUserInfo->m_bAttendanceTarget,
				DB_DATA_TYPE_END );

			if( FALSE == pADO->ExecuteSP( wstrSQL ) )
			{
				ErrorDBUserSave( i32ThreadIdx, wstrSQL );
				pADO = m_ppSaveDB[ i32ThreadIdx ];
				bRv = FALSE;
			}
		}
	}

	if(SAVE_DATABASE_LOGIN_EVENT & ui32UpdateInfo )
	{
		wchar_t wstrEventItemDate[ SMALLDATE_STRING_SIZE ];
		for( INT32 i = 0 ; i < EVENTITEM_LOGIN_SLOT_COUNT ; i++ )
		{
			if( 0 == pNode->_dt32EventItemDate[i].GetDate() ) continue; 

			pNode->_dt32EventItemDate[i].GetSmallDate( wstrEventItemDate, SMALLDATE_STRING_SIZE );

			S2DB::MakeQueryW( wstrSQL, L"usp_EventLoginState_mer", 
			DB_DATA_TYPE_BIGINT,		pNode->_i64UID,								//	@iUSN_         BIGINT
			DB_DATA_TYPE_INT,			i,											//,	@iEventType_   INT
			DB_DATA_TYPE_SMALLDATETIME,	wstrEventItemDate,							//,	@sdtEventDate_ SMALLDATETIME
			DB_DATA_TYPE_END );
		
			if( FALSE == pADO->ExecuteSP( wstrSQL ) )
			{			
				ErrorDBUserSave(i32ThreadIdx, wstrSQL );
				pADO = m_ppSaveDB[ i32ThreadIdx ];
				bRv = FALSE; 
			}
			else
			{
				if( 0 < pADO->GetRecordCount() )
				{
					if( 0 != pADO->GetINT32Data(0) )
					{
						_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
						bRv = FALSE;
					}
				}
				else
				{
					_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
					bRv = FALSE;
				}
			}
		}
	}

	//레코드 
	if( SAVE_DATABASE_RECORD & ui32UpdateInfo )
	{//length(1197)
		// 유저 전적 정보
		S2DB::MakeQueryW(wstrSQL, L"usp_Record_mer_Assist",
			DB_DATA_TYPE_BIGINT, pNode->_i64UID,								//    @iUSN_          BIGINT
			DB_DATA_TYPE_TINYINT, USER_RECORD_TYPE_SEASON,					//,   @ui8RecordType_ TINYINT
			DB_DATA_TYPE_INT, pNode->_sRecord._smatch,					//,   @i32Match_      INT
			DB_DATA_TYPE_INT, pNode->_sRecord._swin,						//,   @i32Win_        INT
			DB_DATA_TYPE_INT, pNode->_sRecord._slose,						//,   @i32Lose_       INT
			DB_DATA_TYPE_INT, pNode->_sRecord._sdraw,						//,   @i32Draw_       INT
			DB_DATA_TYPE_INT, pNode->_sRecord._skillcount,				//,   @i32Kill_       INT
			DB_DATA_TYPE_INT, pNode->_sRecord._sdeath,					//,   @i32Death_      INT
			DB_DATA_TYPE_INT, pNode->_sRecord._sheadshot,					//,   @i32Headshot_   INT
			DB_DATA_TYPE_INT, pNode->_sRecord._sdis,						//,   @i32Disconnect_ INT
			DB_DATA_TYPE_INT, pNode->_sRecord._sdmatch,					//,   @i32dMatch_     INT
			DB_DATA_TYPE_INT, pNode->_sRecord._sdkillcount,				//,   @i32dKill_      INT
			DB_DATA_TYPE_INT, pNode->_sRecord._sAssistCount,				//,   @i32Assist      INT			
			DB_DATA_TYPE_END);
		if( FALSE == pADO->ExecuteSP( wstrSQL ) )
		{
			ErrorDBUserSave( i32ThreadIdx, wstrSQL );
			pADO = m_ppSaveDB[ i32ThreadIdx ];
			bRv = FALSE; 
		}
		else
		{
			if( 0 < pADO->GetRecordCount() )
			{
				if( 0 != pADO->GetINT16Data(0) )
				{
					_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
					bRv = FALSE;
				}
			}
			else
			{
				_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
				bRv = FALSE;
			}
		}

		// 유저 전적 정보
		S2DB::MakeQueryW(wstrSQL, L"usp_EventDailyRecord_mer_Assist",
			DB_DATA_TYPE_BIGINT, pNode->_i64UID,								//    @iUID_					BIGINT
			DB_DATA_TYPE_INT, pNode->_sDaily._win,						//,   @iWin_					INT
			DB_DATA_TYPE_INT, pNode->_sDaily._lose,						//,   @iLose_					INT
			DB_DATA_TYPE_INT, pNode->_sDaily._draw,						//,   @iDraw_					INT
			DB_DATA_TYPE_INT, pNode->_sDaily._killcount,					//,   @iKillCNT_				INT
			DB_DATA_TYPE_INT, pNode->_sDaily._death,						//,   @iDeathCNT_				INT
			DB_DATA_TYPE_INT, pNode->_sDaily._headshot,					//,   @iHeadShot_				INT
			DB_DATA_TYPE_INT, pNode->_sDaily._exp,						//,   @iExp_					INT
			DB_DATA_TYPE_INT, pNode->_sDaily._point,						//,   @iPoint_				    INT
			DB_DATA_TYPE_INT, pNode->_sDaily._playtime,					//,   @iPlayTime_				INT
			DB_DATA_TYPE_INT, 0,											//,   @iDailyEventDate_		    INT
			DB_DATA_TYPE_INT, pNode->_sDaily._DGPlaytime,					//,	  @iDailyGiftPlayTime		INT
			DB_DATA_TYPE_TINYINT, pNode->_sDaily._ui8GetItem,					//,   @iGetItem_				TINYINT
			DB_DATA_TYPE_INT, pNode->_sDaily._Assist,					//,   @iAssistCNT_				INT
			DB_DATA_TYPE_END);

		if( FALSE == pADO->ExecuteSP( wstrSQL ) )
		{			
			ErrorDBUserSave( i32ThreadIdx, wstrSQL );
			pADO = m_ppSaveDB[ i32ThreadIdx ];
			bRv = FALSE; 
		}
		else
		{
			if( 0 < pADO->GetRecordCount() )
			{
				if( 0 != pADO->GetINT16Data(0) )
				{
					_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
					bRv = FALSE;
				}
			}
			else
			{
				_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
				bRv = FALSE;
			}
		}
	}

	//장착
	if( SAVE_DATABASE_CHARA_COMMON & ui32UpdateInfo ) 
	{
		// Common Save
		ITEM_INFO aCommonEquips[ CHAR_EQUIPMENT_COMMON_COUNT ];
		pNode->m_MultiSlot.GetCommonEquipmentAll( aCommonEquips );
		UINT8 ui8RedTeamSlot = (UINT8)pNode->m_MultiSlot.GetTeamSlot(TEAM_RED);

		UINT8 ui8BlueTeamSlot = (UINT8)pNode->m_MultiSlot.GetTeamSlot(TEAM_BLUE);
		
		S2DB::MakeQueryW( wstrSQL, L"usp_Equipment_mer", 
			DB_DATA_TYPE_BIGINT,		pNode->_i64UID,													//    @iUSN_          BIGINT
			DB_DATA_TYPE_INT,			aCommonEquips[ CHAR_EQUIPMENT_COMMON_DINO ].m_TItemDBIdx,		//,   @iDinoIDX_      BIGINT 
			DB_DATA_TYPE_TINYINT,		ui8RedTeamSlot,			  										//,   @iSlotRedTeam_ TINYINT
			DB_DATA_TYPE_TINYINT,		ui8BlueTeamSlot,												//,   @iSlotBlueTeam_ TINYINT
			DB_DATA_TYPE_END );
		if( FALSE == pADO->ExecuteSP( wstrSQL ) )
		{
			ErrorDBUserSave( i32ThreadIdx, wstrSQL );
			pADO = m_ppSaveDB[ i32ThreadIdx ];
			bRv = FALSE;
		}
		if( 0 < pADO->GetRecordCount() )
		{
			if( 0 != pADO->GetINT16Data(0) )
			{
				_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
				bRv = FALSE;
			}
		}
		else
		{
			_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
			bRv = FALSE;
		}
	}

	if( pNode->m_ui64CharaUpdateInfo > 0 )
	{
		// Char Save
		S2MO_CHAR_BASE_INFO		CharBaseInfo;
		S2MO_CHAR_SKILL_INFO	CharSkillInfo;
		ITEM_INFO				aCharEquipment[ CHAR_EQUIPMENT_COUNT ];

		wchar_t					wstrCreateDate[ SMALLDATE_STRING_SIZE ];

		INT32 i32SaveWaeponCount = 0;		// 저장해야할 WeaponDB Count;
		INT32 i32ActiveWeaponDBIdx = -1;	// 유효한 WeaponDBIdx

		pNode->m_pDesign->GetSaveWeaponDBInfo(&pNode->m_MultiSlot, i32SaveWaeponCount, i32ActiveWeaponDBIdx);

		INT32 i32SavedWaeponCount = 0;		// 저장한 WeaponDB Count;

		for( INT32 i = 0 ; i < S2MO_MULTI_SLOT_COUNT ; i++ )
		{
			UINT64 ui64Flag = (UINT64)0x01 << i ;

			if( 0 == (ui64Flag & pNode->m_ui64CharaUpdateInfo ) )	continue;

			UINT8	ui8Enable = (UINT8)pNode->m_MultiSlot.GetActive(i);

			pNode->m_MultiSlot.GetCharBaseInfo( i, &CharBaseInfo );
  			pNode->m_MultiSlot.GetCharEquipmentAll( i, aCharEquipment );

			//
			INT32 i32WeaponDBIdx = -1;
			if (i32ActiveWeaponDBIdx < 0)
			{
				i32WeaponDBIdx = CharBaseInfo.m_i32WeaponDBIdx;
			}
			else i32WeaponDBIdx = i32ActiveWeaponDBIdx;
			

			// Weapon 
			if( i32SavedWaeponCount < i32SaveWaeponCount)
			{
				S2DB::MakeQueryW( wstrSQL, L"usp_Weapon_mer", 
					DB_DATA_TYPE_INT,			i32WeaponDBIdx,												//,	  @iIDX_				 INT 
					DB_DATA_TYPE_INT,			aCharEquipment[ CHAR_EQUIPMENT_PRIMARY ].m_TItemDBIdx,		//,   @iHeadGear_		     INT 
					DB_DATA_TYPE_INT,			aCharEquipment[ CHAR_EQUIPMENT_SECONDARY ].m_TItemDBIdx,	//,   @iFaceGear_	         INT 
					DB_DATA_TYPE_INT,			aCharEquipment[ CHAR_EQUIPMENT_MELEE ].m_TItemDBIdx,		//,   @iBeret_		         INT 
					DB_DATA_TYPE_INT,			aCharEquipment[ CHAR_EQUIPMENT_THROWING1 ].m_TItemDBIdx,	//,   @iUpper_	             INT 
					DB_DATA_TYPE_INT,			aCharEquipment[ CHAR_EQUIPMENT_THROWING2 ].m_TItemDBIdx,	//,   @iLower_	             INT 
					DB_DATA_TYPE_END );

				if( FALSE == pADO->ExecuteSP( wstrSQL ) )
				{
					ErrorDBUserSave( i32ThreadIdx, wstrSQL );
					pADO = m_ppSaveDB[ i32ThreadIdx ];
					bRv = FALSE;
				}
				else
				{
					if( 0 < pADO->GetRecordCount() )
					{
						INT16 i16Result = pADO->GetINT16Data(0);

						switch( i16Result )
						{
						case 1:	// UPDATE 성공 
							++i32SavedWaeponCount;
							break;
						case 2: // INSERT 성공 
							++i32SavedWaeponCount;
							i32WeaponDBIdx = pADO->GetINT32Data(1);
							break;
						default:
							_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
							bRv = FALSE;
							break;
						}
					}
					else
					{
						_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
						bRv = FALSE;
					}
				}
			}

			// Base, Parts 
			pNode->m_MultiSlot.GetCharSkillInfo( i, &CharSkillInfo );
			

			INT32 i32MainSkill;
			INT32 i32AssistSkill;
			INT32 i32CommonSkill;

			SetMainSkillBit( (MAIN_SKILL_TYPE)CharSkillInfo.m_ui8MainSkill,			CharSkillInfo.m_ui8MainSkillLv,		i32MainSkill );
			SetAssistSkillBit( (ASSIST_SKILL_TYPE)CharSkillInfo.m_ui8AssistSkill,	CharSkillInfo.m_ui8AssistSkillLv,	i32AssistSkill );
			SetCommonSkillBit( (COMMON_SKILL_TYPE)CharSkillInfo.m_ui8CommonSkill,	CharSkillInfo.m_ui8CommonSkillLv,	i32CommonSkill );

			TTCHAR strCharNick[CONVERT_STRING_COUNT];
			S2DB::ConvertString( strCharNick, CONVERT_STRING_COUNT, CharBaseInfo.m_strCharName );
		
			S2DB::ConvertUINT32ToSmallDate( wstrCreateDate, SMALLDATE_STRING_SIZE, CharBaseInfo.m_ui32CreateDate );
			
			S2DB::MakeQueryW( wstrSQL, L"usp_Character_mer", 
				DB_DATA_TYPE_BIGINT,		pNode->_i64UID,												//    @i64UserUID_            BIGINT
				DB_DATA_TYPE_TINYINT,		i,															//,   @ui8CharaSlot_          TINYINT
				DB_DATA_TYPE_INT,			i32WeaponDBIdx,												//,	  @iWeapon_				  INT
				DB_DATA_TYPE_INT,			aCharEquipment[ CHAR_EQUIPMENT_CHARA ].m_TItemDBIdx,		//,	  @@iCharacter_			  INT
				DB_DATA_TYPE_TINYINT,		CharSkillInfo.m_ui8Class,									//,	  @ui8Class_              TINYINT
				DB_DATA_TYPE_TTCHAR,		strCharNick,												//,   @strCNickname_          NVARCHAR(50)
				DB_DATA_TYPE_INT,			aCharEquipment[ CHAR_EQUIPMENT_HEAD ].m_TItemDBIdx,			//,   @iHeadGear_		     INT 
				DB_DATA_TYPE_INT,			aCharEquipment[ CHAR_EQUIPMENT_FACE ].m_TItemDBIdx,			//,   @iFaceGear_	         INT 
				DB_DATA_TYPE_INT,			aCharEquipment[ CHAR_EQUIPMENT_BERET ].m_TItemDBIdx,		//,   @iBeret_		         INT 
				DB_DATA_TYPE_INT,			aCharEquipment[ CHAR_EQUIPMENT_UPPER ].m_TItemDBIdx,		//,   @iUpper_	            INT 
				DB_DATA_TYPE_INT,			aCharEquipment[ CHAR_EQUIPMENT_LOWER ].m_TItemDBIdx,		//,   @iLower_	            INT 
				DB_DATA_TYPE_INT,			aCharEquipment[ CHAR_EQUIPMENT_GLOVE ].m_TItemDBIdx,		//,   @iGlove_	            INT 
				DB_DATA_TYPE_INT,			aCharEquipment[ CHAR_EQUIPMENT_BELT ].m_TItemDBIdx,			//,   @iBelt_	             INT 
				DB_DATA_TYPE_INT,			aCharEquipment[ CHAR_EQUIPMENT_HOLSTER ].m_TItemDBIdx,		//,   @iHolster_			 INT 
				DB_DATA_TYPE_INT,			aCharEquipment[ CHAR_EQUIPMENT_SKIN ].m_TItemDBIdx,			//,   @iSkin_				  INT  
				DB_DATA_TYPE_INT,			CharBaseInfo.m_ui32BattleTime,								//,   @i32BattleTime_         INT 
				DB_DATA_TYPE_INT,			i32MainSkill,												//,   @i32MainSkill_          INT
				DB_DATA_TYPE_INT,			i32AssistSkill,												//,   @i32AssistSkill_        INT
				DB_DATA_TYPE_INT,			i32CommonSkill,												//,   @i32CommonSkill_        INT
				DB_DATA_TYPE_INT,			CharBaseInfo.m_ui32SkillRelTime,							//,   @i32SkillReleaseTime_   INT
				DB_DATA_TYPE_TINYINT,		ui8Enable,													//,   @ui8Enable_             TINYINT
				DB_DATA_TYPE_SMALLDATETIME,	wstrCreateDate,												//,   @sdtCreate_			  SMALLDATETIME
				DB_DATA_TYPE_END );

			if( FALSE == pADO->ExecuteSP( wstrSQL ) )
			{
				ErrorDBUserSave( i32ThreadIdx, wstrSQL );
				pADO = m_ppSaveDB[ i32ThreadIdx ];
				bRv = FALSE;
			}
			else
			{
				if( 0 < pADO->GetRecordCount() )
				{
					if( 0 != pADO->GetINT16Data(0) )
					{
						_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
						bRv = FALSE;
					}
				}
				else 
				{
					_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
					bRv = FALSE;
				}
			}
		}
	}

	// 임무카드 & 호칭
	if(SAVE_DATABASE_QUEST & ui32UpdateInfo) 
	{
		wchar_t	wstrMission0[ SERVER_STRING_COUNT ];
		wchar_t	wstrMission1[ SERVER_STRING_COUNT ];
		wchar_t	wstrMission2[ SERVER_STRING_COUNT ];
		wchar_t	wstrMission3[ SERVER_STRING_COUNT ];
		wchar_t	wstrMissionIng[ SERVER_STRING_COUNT ];

		S2DB::ConvertStringToBinary( wstrMission0,		SERVER_STRING_COUNT, (char*)pNode->_sQuest._pQuestCtxActive[0], (sizeof(UINT8) * MAX_QUEST_PER_CARDSET_NEW) );
		S2DB::ConvertStringToBinary( wstrMission1,		SERVER_STRING_COUNT, (char*)pNode->_sQuest._pQuestCtxActive[1], (sizeof(UINT8) * MAX_QUEST_PER_CARDSET_NEW) );
		S2DB::ConvertStringToBinary( wstrMission2,		SERVER_STRING_COUNT, (char*)pNode->_sQuest._pQuestCtxActive[2], (sizeof(UINT8) * MAX_QUEST_PER_CARDSET_NEW) );
		S2DB::ConvertStringToBinary( wstrMission3,		SERVER_STRING_COUNT, (char*)pNode->_sQuest._pQuestCtxActive[3], (sizeof(UINT8) * MAX_QUEST_PER_CARDSET_NEW) ); 
		S2DB::ConvertStringToBinary( wstrMissionIng,	SERVER_STRING_COUNT, (char*)&pNode->_sQuest._QuestIngInfo,		sizeof(QUESTING_INFO) );
		
		S2DB::MakeQueryW( wstrSQL, L"usp_MissionCard_mer",
			DB_DATA_TYPE_INT,			i32ThreadIdx,
			DB_DATA_TYPE_INT,			pNode->_sQuest._pMiniature,
			DB_DATA_TYPE_INT,			pNode->_sQuest._pInsignia,
			DB_DATA_TYPE_INT,			pNode->_sQuest._pOrder,
			DB_DATA_TYPE_INT,			pNode->_sQuest._pMaster,
			DB_DATA_TYPE_BIGINT,		pNode->_sQuest._ui64GainUserTitle,
			DB_DATA_TYPE_BINARY,		wstrMission0,
			DB_DATA_TYPE_BINARY,		wstrMission1,
			DB_DATA_TYPE_BINARY,		wstrMission2,
			DB_DATA_TYPE_BINARY,		wstrMission3,
			DB_DATA_TYPE_BINARY,		wstrMissionIng,
			DB_DATA_TYPE_INT,			pNode->_sQuest._iQuestEventDate,
			DB_DATA_TYPE_TINYINT,		pNode->_sQuest._EquipUserTitle[0],
			DB_DATA_TYPE_TINYINT,		pNode->_sQuest._EquipUserTitle[1],
			DB_DATA_TYPE_TINYINT,		pNode->_sQuest._EquipUserTitle[2],
			DB_DATA_TYPE_BIGINT,		pNode->_i64UID,
			DB_DATA_TYPE_END);

		if( FALSE == pADO->ExecuteSP( wstrSQL ) )
		{			
			ErrorDBUserSave(i32ThreadIdx, wstrSQL );
			pADO = m_ppSaveDB[ i32ThreadIdx ];
			bRv = FALSE;
		}
		else
		{
			if( 1 == pADO->GetRecordCount() )
			{
				if( 1 != pADO->GetINT32Data(0))
				{
					_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
					bRv = FALSE;
				}
			}
			else
			{
				_DataBaseResultFailLog( i32ThreadIdx, wstrSQL );
				bRv = FALSE;
			}

		}
	}

	if( g_pConfig->IsContentsEnabled( CONTENTS_MEDAL) )
	{
		///////////////////////////////////////////////////////////////////////////////////////
		// 1. Save Medal Basic info
		if( SAVE_DATABASE_MEDAL_BASIC & ui32UpdateInfo ) 
		{
			NOTIFY_MEDAL* pMedal0 = &pNode->m_MedalAllInfoDB.m_aNotifyMedal[0];
			NOTIFY_MEDAL* pMedal1 = &pNode->m_MedalAllInfoDB.m_aNotifyMedal[1];
			NOTIFY_MEDAL* pMedal2 = &pNode->m_MedalAllInfoDB.m_aNotifyMedal[2];
			NOTIFY_MEDAL* pMedal3 = &pNode->m_MedalAllInfoDB.m_aNotifyMedal[3];

			S2DB::MakeQueryW( wstrSQL, L"usp_MedalBasic_mer",
				DB_DATA_TYPE_BIGINT,		pNode->_i64UID,
				DB_DATA_TYPE_TINYINT,		pMedal0->m_ui8MedalType,
				DB_DATA_TYPE_TINYINT,		pMedal1->m_ui8MedalType,
				DB_DATA_TYPE_TINYINT,		pMedal2->m_ui8MedalType,
				DB_DATA_TYPE_TINYINT,		pMedal3->m_ui8MedalType,
				DB_DATA_TYPE_SMALLINT,		pMedal0->m_ui16Idx,
				DB_DATA_TYPE_SMALLINT,		pMedal1->m_ui16Idx,
				DB_DATA_TYPE_SMALLINT,		pMedal2->m_ui16Idx,
				DB_DATA_TYPE_SMALLINT,		pMedal3->m_ui16Idx,
				DB_DATA_TYPE_SMALLINT,		pMedal0->m_ui16Count,
				DB_DATA_TYPE_SMALLINT,		pMedal1->m_ui16Count,
				DB_DATA_TYPE_SMALLINT,		pMedal2->m_ui16Count,
				DB_DATA_TYPE_SMALLINT,		pMedal3->m_ui16Count,
				DB_DATA_TYPE_TINYINT,		0,
				DB_DATA_TYPE_END);
			if( FALSE == pADO->ExecuteSP( wstrSQL ) )
			{						
				ErrorDBUserSave( i32ThreadIdx, wstrSQL );
				pADO = m_ppSaveDB[ i32ThreadIdx ];
				bRv = FALSE;
			}
			else if( pADO->GetRecordCount() > 0 ) 
			{		
				// 결과
				INT16 ui16Rv = pADO->GetINT16Data(0);
				switch( ui16Rv ) 
				{ 
				case -9999:
					//i32Rv = EVENT_ERROR_PBV15_MEDAL_SAVE_SP_FAIL_MEDAL_BASIC_MER_FAIL;
					bRv = FALSE;
					break;
				case -1:
					//i32Rv = EVENT_ERROR_PBV15_MEDAL_SAVE_SP_FAIL_MEDAL_BASIC_NOUSER;
					bRv = FALSE;
					break;
				case 0:
					bRv = TRUE;
					break;
				default:
					//i32Rv = EVENT_ERROR_PBV15_MEDAL_SAVE_SP_FAIL_MEDAL_BASIC_UNRESULT;
					bRv = FALSE;
					break;
				}
			}	
			else 
			{	
				//i32Rv = EVENT_ERROR_PBV15_MEDAL_SAVE_SP_FAIL_MEDAL_BASIC_NORESULT;
				bRv = FALSE;
			}
		}

		///////////////////////////////////////////////////////////////////////////////////////
		// 2. Save Medal   );
		if( SAVE_DATABASE_MEDAL & ui32UpdateInfo ) 
		{
			for( UINT16 i = 0 ; i < pNode->m_MedalAllInfoDB.m_ui16MedalCount; i++ )
			{
				// DB Save 플래그가 꺼져있다면 Pass.
				if( 0 == pNode->m_MedalAllInfoDB.m_aMedal[i].m_ui8IsSave ) continue;

				S2DB::MakeQueryW( wstrSQL, L"usp_MedalNormal_mer",
					DB_DATA_TYPE_BIGINT,		pNode->_i64UID,
					DB_DATA_TYPE_SMALLINT,		pNode->m_MedalAllInfoDB.m_aMedal[i].m_Medal.m_ui16Idx,
					DB_DATA_TYPE_SMALLINT,		pNode->m_MedalAllInfoDB.m_aMedal[i].m_Medal.m_ui16Count,
					DB_DATA_TYPE_SMALLINT,		pNode->m_MedalAllInfoDB.m_aMedal[i].m_Medal.m_ui8GetReward,
					DB_DATA_TYPE_END);

				if( FALSE == pADO->ExecuteSP( wstrSQL ) )
				{						
					ErrorDBUserSave( i32ThreadIdx, wstrSQL );
					pADO = m_ppSaveDB[ i32ThreadIdx ];
					bRv = FALSE;
				}
				else if( pADO->GetRecordCount() > 0 ) 
				{		
					// 결과
					INT16 ui16Rv = pADO->GetINT16Data(0);
					switch( ui16Rv ) 
					{
					case -9999:
						//i32Rv = EVENT_ERROR_PBV15_MEDAL_SAVE_SP_FAIL_MEDAL_MER_FAIL;
						bRv = FALSE;
						break;
					case 0:
						bRv = TRUE;
						break;
					default:
						//i32Rv = EVENT_ERROR_PBV15_MEDAL_SAVE_SP_FAIL_MEDAL_UNRESULT;
						bRv = FALSE;
						break;
					}
				}	
				else 
				{	
					//i32Rv = EVENT_ERROR_PBV15_MEDAL_SAVE_SP_FAIL_MEDAL_NORESULT;
					bRv = FALSE;
				}
			}
		}

		///////////////////////////////////////////////////////////////////////////////////////
		// 3. Save Cur Medal Set
		if( SAVE_DATABASE_CUR_MEDAL_SET & ui32UpdateInfo ) 
		{
			for( UINT16 i = 0 ; i < MAX_CUR_MEDAL_SET_COUNT ; i++ )
			{
				// DB에 저장할 필요 없는 경우.
				CUR_MEDAL_SET* pMedalSet = &pNode->m_MedalAllInfoDB.m_aCurMedalSet[i];

				if( MEDAL_SET_TYPE_NORMAL == pMedalSet->m_ui8Type || INIT_MEDAL_NUMBER == pMedalSet->m_ui16Idx ) continue;

				wchar_t wstrRecvDate[SERVER_STRING_COUNT];
				S2DB::ConvertUINT32ToSmallDate( wstrRecvDate, SERVER_STRING_COUNT, pMedalSet->m_ui32RecvDate );

				S2DB::MakeQueryW( wstrSQL, L"usp_MedalSetCurrent_mer", 
					DB_DATA_TYPE_BIGINT,			pNode->_i64UID,							
					DB_DATA_TYPE_TINYINT,			pMedalSet->m_ui8Type,				
					DB_DATA_TYPE_SMALLDATETIME,		wstrRecvDate,						
					DB_DATA_TYPE_SMALLINT,			pMedalSet->m_ui16Idx,				
					DB_DATA_TYPE_SMALLINT,			pMedalSet->m_ui16Count[0],			
					DB_DATA_TYPE_SMALLINT,			pMedalSet->m_ui16Count[1],			
					DB_DATA_TYPE_SMALLINT,			pMedalSet->m_ui16Count[2],			
					DB_DATA_TYPE_SMALLINT,			pMedalSet->m_ui16Count[3],			
					DB_DATA_TYPE_SMALLINT,			pMedalSet->m_ui16Count[4],			
					DB_DATA_TYPE_SMALLINT,			pMedalSet->m_ui16Count[5],
					DB_DATA_TYPE_TINYINT,			pMedalSet->m_ui8GetReward,
					DB_DATA_TYPE_END );

				if( FALSE == pADO->ExecuteSP( wstrSQL ) )
				{						
					ErrorDBUserSave( i32ThreadIdx, wstrSQL );
					pADO = m_ppSaveDB[ i32ThreadIdx ];
					bRv = FALSE;
				}
				else if( pADO->GetRecordCount() > 0 ) 
				{		
					// 결과
					INT16 ui16Rv = pADO->GetINT16Data(0);
					switch( ui16Rv ) 
					{
					case -9999:
						//i32Rv = EVENT_ERROR_PBV15_MEDAL_SAVE_SP_FAIL_CUR_MEDAL_SET_MER_FAIL;
						bRv = FALSE; 
						break;
					case -1:
						//i32Rv = EVENT_ERROR_PBV15_MEDAL_SAVE_SP_FAIL_CUR_MEDAL_SET_ALCOM;
						bRv = FALSE; 
						break;
					case 0:
						bRv = TRUE;
						break;

					default:
						//i32Rv = EVENT_ERROR_PBV15_MEDAL_SAVE_SP_FAIL_CUR_MEDAL_SET_UNRESULT;
						bRv = FALSE; 
						break;
					}
				}	
				else 
				{	
					//i32Rv = EVENT_ERROR_PBV15_MEDAL_SAVE_SP_FAIL_CUR_MEDAL_SET_NORESULT;
					bRv = FALSE; 
				}
			}
		}
	}

	// Quick Join
	if( SAVE_DATABASE_QUICK_JOIN & ui32UpdateInfo ) 
	{		

		S2DB::MakeQueryW( wstrSQL, L"usp_QuickJoinNew_mer",
			DB_DATA_TYPE_BIGINT,		pIn->pNode->_i64UID,
			DB_DATA_TYPE_INT,			pIn->pNode->m_aQuickJoinInfo[0].m_ui32StageID,
			DB_DATA_TYPE_INT,			pIn->pNode->m_aQuickJoinInfo[1].m_ui32StageID,
			DB_DATA_TYPE_INT,			pIn->pNode->m_aQuickJoinInfo[2].m_ui32StageID,			
			DB_DATA_TYPE_END);

		if( FALSE == pADO->ExecuteSP( wstrSQL ) )
		{			
			ErrorDBUserSave( i32ThreadIdx, wstrSQL );
			pADO = m_ppSaveDB[ i32ThreadIdx ];
			bRv = FALSE;
		}
		else if( pADO->GetRecordCount() > 0 ) 
		{		
			// 결과
			INT16 i16Rv = pADO->GetINT16Data(0);
			switch( i16Rv ) 
			{
			case -9999:		
				bRv = FALSE; 
				break;
			case 0:
				bRv = TRUE;
				break;
			default:
				bRv = FALSE; 
				break;
			}
		}	
	}

	// AAS
	if( SAVE_DATABASE_AAS & ui32UpdateInfo )
	{
		S2DB::MakeQueryW( wstrSQL, L"usp_Basic_upd_AAS",
			DB_DATA_TYPE_BIGINT,		pIn->pNode->_i64UID,
			DB_DATA_TYPE_SMALLINT,		pIn->pNode->m_ui16AASAccMin,
			DB_DATA_TYPE_END);

		if( FALSE == pADO->ExecuteSP( wstrSQL ) )
		{
			ErrorDBUserSave( i32ThreadIdx, wstrSQL );
			pADO = m_ppSaveDB[ i32ThreadIdx ];
			bRv = FALSE;
		}
	}

#ifdef ENABLE_GACHA_SHOP
	if( ui32UpdateInfo & SAVE_DATABASE_GACHA_RECORD ) 
	{
		for( INT32 i = 0; i < pIn->pNode->GetGachaPurchaseRecordCnt(); ++i )
		{
			CPurchaseRecord* pPurchaseRecord = pIn->pNode->m_GachaPurchase.GetPurchaseRecord(i);
			if( !pPurchaseRecord )
				continue;

			S2DB::MakeQueryW( wstrSQL, L"usp_GachaGloryCount_mer",
				DB_DATA_TYPE_BIGINT,		pIn->pNode->_i64UID,
				DB_DATA_TYPE_INT,			pPurchaseRecord->GetItemKey(),
				DB_DATA_TYPE_INT,			pPurchaseRecord->GetPurchaseCount(),
				DB_DATA_TYPE_END);
			if( !pADO->ExecuteSP( wstrSQL ) )
			{
				ErrorDBUserSave( i32ThreadIdx, wstrSQL );
				pADO = m_ppSaveDB[i32ThreadIdx];
				bRv = FALSE;
			}
		}
	}
#endif

	if( ui32UpdateInfo & SAVE_DATABASE_ITEM )
	{
		S2DB::MakeQueryW( wstrSQL, L"usp_AccountItemData_mer",
			DB_DATA_TYPE_BIGINT,			pNode->_i64UID,							
			DB_DATA_TYPE_TINYINT,			pNode->_sInvenData._ui8NickColor,				
			DB_DATA_TYPE_INT,				pNode->_sInvenData._wRank,		
			DB_DATA_TYPE_INT,				pNode->_sInvenData._wAprilFoolRank,		
			DB_DATA_TYPE_TTCHAR,			pNode->_sInvenData._cNick,			
			DB_DATA_TYPE_TINYINT,			pNode->_sInvenData._ui8CrossHairColor,
			DB_DATA_TYPE_TINYINT,			pNode->_sInvenData._ui8ChattingColor,			
			DB_DATA_TYPE_SMALLINT,			pNode->_sInvenData._ui16MuzzleFlashColor,						
			DB_DATA_TYPE_END );
		
		if( FALSE == pADO->ExecuteSP( wstrSQL ) )
		{			
			ErrorDBUserSave( i32ThreadIdx, wstrSQL );
			pADO = m_ppSaveDB[ i32ThreadIdx ];
			bRv = FALSE; 
		}
		else
		{
			if( 0 != pADO->GetINT32Data(0))
			{
				ErrorDBUserSave( i32ThreadIdx, wstrSQL );
				bRv = FALSE;
			}
		}

	}


	return bRv; 
}

INT32 CModuleDBUserSave::PushExitSave( RING_EXIT_SAVE_IN* pExitSave )
{		
	UINT32 i32WriteIdx = (UINT32)( pExitSave->pNode->_i64UID % DATABASE_THREAD_COUNT );
	return m_ppExitSaveIn[ i32WriteIdx ]->Push( pExitSave );
}

INT32 CModuleDBUserSave::GetExitSaveCount(void)
{
	INT32 i32Rv = 0; 
	for(INT32 i = 0; i < DATABASE_THREAD_COUNT; i++)
	{
		i32Rv += m_ppExitSaveOut[i]->GetBufferCount();
	}

	return i32Rv; 
}

RING_EXIT_SAVE_OUT * CModuleDBUserSave::GetExitSave(void)
{
	for(INT32 i = 0; i < DATABASE_THREAD_COUNT; i++)
	{
		m_i32ExitSaveOutIdx++; 
		if(m_i32ExitSaveOutIdx >= DATABASE_THREAD_COUNT)m_i32ExitSaveOutIdx = 0; 

		if(m_ppExitSaveOut[m_i32ExitSaveOutIdx]->GetBufferCount())
		{
			return (RING_EXIT_SAVE_OUT *)m_ppExitSaveOut[m_i32ExitSaveOutIdx]->Pop(); 
		}
	}

	return NULL; 
}

void CModuleDBUserSave::GetExitSaveIdx(void)
{
	m_ppExitSaveOut[ m_i32ExitSaveOutIdx ]->PopIdx(); 
}