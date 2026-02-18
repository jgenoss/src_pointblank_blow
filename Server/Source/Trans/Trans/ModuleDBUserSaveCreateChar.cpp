#include "pch.h"
#include "ModuleDBUserSave.h"

BOOL CModuleDBUserSave::_WorkingCreateChar( INT32 i32ThreadIdx )
{
	if( 0 == m_ppCreateCharIn[ i32ThreadIdx ]->GetBufferCount() )	return FALSE;

	RING_CREATE_CHAR* pRingIn	= (RING_CREATE_CHAR *)m_ppCreateCharIn[ i32ThreadIdx ]->Pop();
	RING_CREATE_CHAR* pRingOut	= (RING_CREATE_CHAR *)m_ppCreateCharOut[ i32ThreadIdx ]->PushPointer();
	if( NULL == pRingOut )
	{
		g_pLog->WriteLog( L"Create Char Out Buffer Full" );
		return FALSE;
	}
	else if( NULL == pRingIn )
	{
		g_pLog->WriteLog( L"Create Char In Buffer Not Find" );
		return FALSE;
	}

	pRingOut->m_TUID = pRingIn->m_TUID;
	pRingOut->m_TGoodsID = pRingIn->m_TGoodsID;

	if( FALSE == _SaveCreateChar( i32ThreadIdx, pRingIn, pRingOut ) )
	{
		g_pLog->WriteLog( L"[Error]Create Char Not Save" );
	}

	m_ppCreateCharIn[ i32ThreadIdx ]->PopIdx();
	m_ppCreateCharOut[ i32ThreadIdx ]->PushPointerIdx();

	return TRUE;
}

BOOL CModuleDBUserSave::_SaveCreateChar( INT32 i32Threadidx, RING_CREATE_CHAR* pIn, RING_CREATE_CHAR* pOut )
{
	wchar_t	wstrSQL[ SERVER_STRING_COUNT ];
	CADODatabase * pADO = m_ppSaveDB[ i32Threadidx ];

	// Char 추가
	UINT32	ui32CreateDate		= i3ThreadTimer::LocalTime().GetDateTimeYYMMDDHHMI();
	UINT32	ui32SkillRelTime	= 0;
	
	switch( pIn->m_eType )
	{
	case CHARA_RING_TYPE_USE :
		{ 
			S2DB::MakeQueryW( wstrSQL, L"usp_Character_upd_PaySuccess", 
				DB_DATA_TYPE_BIGINT,		pIn->m_TUID,										//    @i64UserUID_        BIGINT
				DB_DATA_TYPE_TINYINT,		pIn->m_ui8CharSlot,									//,   @ui8CharaSlot_      TINYINT
				DB_DATA_TYPE_TINYINT,		pIn->m_ui8SlotState,								//,   @iEnable_				TINYINT
				DB_DATA_TYPE_END );

			if( FALSE == pADO->ExecuteSP( wstrSQL ) )
			{
				ErrorDBUserSave( i32Threadidx, wstrSQL );
				pOut->m_i32Result	= EVENT_ERROR_DATABASE_EXECUTE_SP;
				return FALSE;
			}
			// 실패.
			if( 1 != pADO->GetRecordCount() )
			{
				ErrorDBUserSave( i32Threadidx, wstrSQL );
				pOut->m_i32Result	= EVENT_ERROR_DATABASE_SP_RESULT;
				return FALSE;
			}

			INT32 i32Result = (INT32)pADO->GetINT16Data(0);
			if( 0 != i32Result )
			{
				pOut->m_i32Result	= EVENT_ERROR_DATABASE_SP_RESULT;
				return FALSE;
			}

			pOut->m_i32Result						= EVENT_ERROR_SUCCESS;
			pOut->m_eType							= pIn->m_eType;
			pOut->m_ui8CharSlot						= pIn->m_ui8CharSlot;	 
			pOut->m_ui8SlotState					= pIn->m_ui8SlotState;	 
			return TRUE;
		}
		break;
	case CHARA_RING_TYPE_DELETE :
		{
			wchar_t wstrDate[ SMALLDATE_STRING_SIZE ];
			i3ThreadTimer::LocalTime().GetSmallDate( wstrDate, SMALLDATE_STRING_SIZE );

			S2DB::MakeQueryW( wstrSQL, L"usp_Character_upd_PayFail", 
				DB_DATA_TYPE_BIGINT,		pIn->m_TUID,										//    @i64UserUID_        BIGINT
				DB_DATA_TYPE_TINYINT,		pIn->m_ui8CharSlot,									//,   @ui8CharaSlot_      TINYINT
				DB_DATA_TYPE_SMALLDATETIME,	wstrDate,											//,   @sdtDelete_         SMALLDATETIME
				DB_DATA_TYPE_END );

			if( FALSE == pADO->ExecuteSP( wstrSQL ) )
			{
				ErrorDBUserSave( i32Threadidx, wstrSQL );
				pOut->m_i32Result	= EVENT_ERROR_DATABASE_EXECUTE_SP;
				return FALSE;
			}
			// 실패.
			if( 1 != pADO->GetRecordCount() )
			{
				ErrorDBUserSave( i32Threadidx, wstrSQL );
				pOut->m_i32Result	= EVENT_ERROR_DATABASE_SP_RESULT;
				return FALSE;
			}

			INT32 i32Result = (INT32)pADO->GetINT16Data(0);
			if( 0 != i32Result )
			{
				pOut->m_i32Result	= EVENT_ERROR_DATABASE_SP_RESULT;
				return FALSE;
			}

			// 삭제 완료.
			pOut->m_i32Result						= EVENT_ERROR_SUCCESS;
			pOut->m_eType							= pIn->m_eType;
			pOut->m_ui8CharSlot						= pIn->m_ui8CharSlot;	 
			pOut->m_Inven._TItemWareDBIdx			= pIn->m_Inven._TItemWareDBIdx;
			return TRUE;
		}
		break;
	case CHARA_RING_TYPE_CREATE:	
		if (pIn->m_Inven._TItemWareDBIdx & DEFAULT_ITEM_DBIDX_MIN)
		{	// 기본 캐릭터는 영구로 인벤에 넣습니다.
			//pIn->m_Inven._ui32ItemArg = 1;
			//pIn->m_Inven._ui8ItemType = ITEM_ATTR_TYPE_BASIC;

			S2DB::MakeQueryW(wstrSQL, L"usp_Weapon_mer",
				DB_DATA_TYPE_INT, pIn->m_CharBaseInfo.m_i32WeaponDBIdx,	//,	  @iIDX_				 INT 
				DB_DATA_TYPE_INT, 0,	//,   @iHeadGear_		     INT 
				DB_DATA_TYPE_INT, 0,	//,   @iFaceGear_	         INT 
				DB_DATA_TYPE_INT, 0,	//,   @iBeret_		         INT 
				DB_DATA_TYPE_INT, 0,	//,   @iUpper_	             INT 
				DB_DATA_TYPE_INT, 0,	//,   @iLower_	             INT 
				DB_DATA_TYPE_END);

			if (FALSE == pADO->ExecuteSP(wstrSQL))
			{
				ErrorDBUserSave(i32Threadidx, wstrSQL);
				pOut->m_i32Result = EVENT_ERROR_DATABASE_EXECUTE_SP;
				return FALSE;
			}
			 
			if (pADO->GetRecordCount() < 1 )
			{
				ErrorDBUserSave(i32Threadidx, wstrSQL);
				pOut->m_i32Result = EVENT_ERROR_DATABASE_SP_RESULT;
				return FALSE;
			}

			INT16 i16Result = pADO->GetINT16Data(0);

			switch (i16Result)
			{
			case 2: // INSERT 성공
				pOut->m_CharBaseInfo.m_i32WeaponDBIdx = pADO->GetINT32Data(1);
				break;
			default:
				break;
			}
		}
		//	break;	// 없는게 맞음
	case CHARA_RING_TYPE_BUY_POINT:
	case CHARA_RING_TYPE_BUY_CASH:
		{
			wchar_t wstrDate[ SMALLDATE_STRING_SIZE ];
			i3ThreadTimer::LocalTime().GetSmallDate( wstrDate, SMALLDATE_STRING_SIZE );

			S2DB::MakeQueryW( wstrSQL, L"usp_Character_mer_Buy", 
				DB_DATA_TYPE_BIGINT,		pIn->m_TUID,							//    @i64UserUID_        BIGINT
				DB_DATA_TYPE_TINYINT,		pIn->m_ui8CharSlot,						//,   @ui8CharaSlot_      TINYINT
				DB_DATA_TYPE_INT,			(INT32)pIn->m_Inven._TItemWareDBIdx,	//,   @i32CharaDBIDX_     INT 
				DB_DATA_TYPE_TTCHAR,		pIn->m_CharBaseInfo.m_strCharName,		//,   @strCNickname_      NVARCHAR(50)
				DB_DATA_TYPE_TINYINT,		pIn->m_ui8SlotState,					//,	  @ui8CharaState_	  TINYINT
				DB_DATA_TYPE_SMALLDATETIME,	wstrDate,								//,   @sdtCreate_         SMALLDATETIME 
				DB_DATA_TYPE_END );	
		}
		break;
	}

	if( FALSE == pADO->ExecuteSP( wstrSQL ) )
	{
		ErrorDBUserSave( i32Threadidx, wstrSQL );
		pOut->m_i32Result	= EVENT_ERROR_DATABASE_EXECUTE_SP;
		return FALSE;
	}

	if( 1 != pADO->GetRecordCount() )
	{
		ErrorDBUserSave( i32Threadidx, wstrSQL );
		pOut->m_i32Result	= EVENT_ERROR_DATABASE_SP_RESULT;
		return FALSE;
	}


	INT32 i32Result = (INT32)pADO->GetINT16Data(0);
	if( 0 != i32Result )
	{
		pOut->m_i32Result	= EVENT_ERROR_DATABASE_SP_RESULT;
		return FALSE;
	}

	if( CHARA_RING_TYPE_CREATE == pIn->m_eType )
	{
		ui32SkillRelTime	= pADO->GetINT32Data(1);
	}

	// 생성 완료.
	pOut->m_i32Result						= EVENT_ERROR_SUCCESS;
	pOut->m_eType							= pIn->m_eType;
	pOut->m_bAddCharaSlot					= pIn->m_bAddCharaSlot;
	pOut->m_ui8SlotState					= pIn->m_ui8SlotState;
	pOut->m_ui8CharSlot						= pIn->m_ui8CharSlot;	
	pOut->m_ui8Class						= pIn->m_ui8Class;
	pOut->m_TGoodsID						= pIn->m_TGoodsID; 
	pOut->m_CharBaseInfo.m_ui32CreateDate	= ui32CreateDate;
	pOut->m_CharBaseInfo.m_ui32SkillRelTime	= ui32SkillRelTime;
	i3mem::Copy( &pOut->m_Inven, &pIn->m_Inven, sizeof( INVEN_BUFFER_DB ) );
	i3String::Copy( pOut->m_CharBaseInfo.m_strCharName, pIn->m_CharBaseInfo.m_strCharName, NET_NICK_NAME_SIZE );

	return TRUE;
}

INT32 CModuleDBUserSave::PushCreateChar( RING_CREATE_CHAR* pCreateChar )
{		
	UINT32 ui32WriteIdx = (UINT32)( pCreateChar->m_TUID % DATABASE_THREAD_COUNT );
	if( ui32WriteIdx >= DATABASE_THREAD_COUNT ) return  RING_BUFFER_ERROR_BUFFER_FULLIDX;

	return m_ppCreateCharIn[ ui32WriteIdx ]->Push( pCreateChar );
}

INT32 CModuleDBUserSave::GetCreateCharCount(void)
{
	INT32 i32Rv = 0; 
	for(INT32 i = 0; i < DATABASE_THREAD_COUNT; i++)
	{
		i32Rv += m_ppCreateCharOut[i]->GetBufferCount(); 
	}

	return i32Rv; 
}

RING_CREATE_CHAR * CModuleDBUserSave::GetCreateChar(void)
{
	for(INT32 i = 0; i < DATABASE_THREAD_COUNT; i++)
	{
		m_i32CreateCharOutIdx++; 
		if( m_i32CreateCharOutIdx >= DATABASE_THREAD_COUNT ) m_i32CreateCharOutIdx = 0; 

		if( m_ppCreateCharOut[ m_i32CreateCharOutIdx ]->GetBufferCount())
		{
			return ( RING_CREATE_CHAR* )m_ppCreateCharOut[ m_i32CreateCharOutIdx ]->Pop(); 
		}
	}

	return NULL; 
}

void CModuleDBUserSave::GetCreateCharIdx(void)
{
	m_ppCreateCharOut[ m_i32CreateCharOutIdx ]->PopIdx(); 
}