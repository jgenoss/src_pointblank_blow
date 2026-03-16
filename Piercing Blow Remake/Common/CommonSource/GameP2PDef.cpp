#include "pch.h"
#include "GameP2PDef.h"

#if defined( I3_DEBUG) && 0
#define	UDPTRACE	I3TRACE
#else
#define UDPTRACE	__noop
#endif

#if defined( CLIENT_SIDE)
#include "WeaponBase.h"
#include "StageBattle/HitPartContext.h"
#include "MyRoomInfoContext.h"
#include "Weapon/WeaponItemID.h"
#include "../ClientSource/Source/StageBattle/NetworkContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"

extern REAL32 g_rOne; 
UINT8 g_uiSeedForOne = 0;

void InitForCheckHacking()
{
VM_START 
#if defined( MEM_ENCRYPTION)
	g_uiSeedForOne = (UINT8) ((i3Math::Rand() % 7) + 1);
	GlobalFunc::EncryptValue( &g_rOne, g_uiSeedForOne );
#endif
VM_END
}



bool CheckHackingInGameP2PDef()
{
VM_START
	REAL32 rTemp = 1.f;
	REAL32 rOne = 0.f;
#if defined( MEM_ENCRYPTION)
	rOne = GlobalFunc::DecryptValue( &g_rOne, g_uiSeedForOne );
#else
	rOne = g_rOne;
#endif	

	if(rOne == rTemp)
	{
		VM_END
		return true;
	}
	else
	{
		VM_END
		return false;
	}

}

#endif // CLIENT_SIDE

UINT16 UDP_UTIL::BuildHead( char * pOutPacket, UINT8 Protocol, UINT8 idx, REAL32 gameTime, UINT8 roundNum, UINT16 size, bool Ping, UINT8 ui8PacketUpdateIdx, UINT8 RespawnIdx, UINT8 TouchDownIdx, UINT8 Extra )
{
	N_GAME_HEADER	Head;

	if( Ping)
		Head._ProtocolID = 0x80 | Protocol;
	else
		Head._ProtocolID = Protocol;

	Head._Index				= idx;
	Head._GameTime			= gameTime;
	Head._RoundNum			= roundNum;
	Head._Size				= size;
	Head._RespawnCount		= RespawnIdx;
	Head._TouchDownCount	= TouchDownIdx;
	Head._ui8Extra= Extra;

#if defined( CLIENT_SIDE)
	Head._ui8UID		= NetworkContext::i()->UID%256;
#endif

#ifdef __USE_PACKET_OPTIMIZE__
	Head.m_ui8PacketUpdateIdx	= ui8PacketUpdateIdx;
#endif

	return Head.CopyToPacket( pOutPacket);
}

UINT16 UDP_UTIL::BuildSubHead( char * pOutPacket, P2P_SUB_HEADTYPE type, UINT16 idx, UINT16 size )
{
	N_GAME_SUBHEAD	SubHead;
	
	SubHead._Index = idx;
	SubHead._Type = (UINT8) type;
	SubHead._Code = size;
	return SubHead.CopyToPacket( pOutPacket);
}

bool UDP_UTIL::IsPackSize( P2P_SUB_HEADTYPE type, UINT16 pos, UINT16 limitSize)
{
	UINT16 check = 0;

	switch( type)
	{
	case P2P_SUB_HEAD_STAGEINFO_CHARA : // flag, life
		check = sizeof( N_GAME_SUBHEAD) + sizeof( N_PCINFO_DEF2) + sizeof( UINT16);
		break;
	case P2P_SUB_HEAD_OBJECT :
		check = sizeof( N_GAME_SUBHEAD) + sizeof(N_OBJINFO_STATE) + sizeof( N_OBJINFO_HP) + sizeof(N_OBJINFO_EVENTSENDER) + sizeof(N_POS)
			+ sizeof(N_ROT) + sizeof(N_OBJINFO_ANIMPATH) + sizeof(N_OBJINFO_ANIMKEY) + sizeof(N_OBJINFO_SHELLCOUNT) + sizeof(N_OBJINFO_RESPAWN) + sizeof(N_OBJINFO_OWNER_INDEX);
		break;
	case P2P_SUB_HEAD_STAGEINFO_MISSION:
		check = sizeof( N_GAME_SUBHEAD) + sizeof( N_POS);
		break;
	}

	if( pos + check >= limitSize)
		return false;

	return true;
}

bool UDP_UTIL::IsCharaPackSize( P2P_INFO_FLAG flag, UINT16 pos, UINT16 limitSize /*= 1024*/)
{
	UINT16 check = 0;

	switch( flag)
	{
	case P2P_INFO_FLAG_DEF:
		check = sizeof( N_GAME_SUBHEAD) + sizeof( N_PCINFO_DEF2);
		break;
	case P2P_INFO_FLAG_CHARASTATE :
		check = sizeof( N_PCINFO_CHARA_STATE);
		break;
	case P2P_INFO_FLAG_ONLOADOBJECT :
		check = sizeof( N_PCINFO_ONLOADOBJECT);
		break;
	case P2P_INFO_FLAG_CHARAPOS :
		check = sizeof( N_PCINFO_POSROTATION);
		break;
	case P2P_INFO_FLAG_ACTIONKEY :
		check = sizeof( N_PCINFO_ACTIONKEY);
		break;
	case P2P_INFO_FLAG_SYNCOBJECTSTATE :
		check = sizeof( N_PCINFO_SYNCOBJECT);
		break;
	case P2P_INFO_FLAG_RADIO:
		check = sizeof( N_PCINFO_RADIOCHAT);
		break;
	case P2P_INFO_FLAG_CS_LIFE :
		check = sizeof( N_PCINFO_LIFE);
		break;
	case P2P_INFO_FLAG_SUICIDEDAMAGE :
		check = sizeof( N_PCINFO_SUICIDEDMG);
		break;
	case P2P_INFO_FLAG_MISSION :
		check = sizeof( N_PCINFO_MISSION);
		break;
	case P2P_INFO_FLAG_DOMI_DEATH:
		check = sizeof( N_PCINFO_DEATH_FOR_DOMI );
		break;
	case P2P_INFO_FLAG_DOMI_INSTALL:
		check = sizeof( N_PCINFO_DOMI_INSTALL );
		break;
	case P2P_INFO_FLAG_DOMI_REPAIR:
		check = sizeof( N_PCINFO_DOMI_REPAIR );
		break;
	case P2P_INFO_FLAG_DOMI_UPGRADE:
		check = sizeof( N_PCINFO_DOMI_UPGRADE );
		break;
	case P2P_INFO_FLAG_DOMI_USE:
		check = sizeof( N_PCINFO_DOMI_USE );
		break;
	case P2P_INFO_FLAG_DOMI_CATCH:
		check = sizeof( N_PCINFO_DOMI_CATCH );
		break;
	case P2P_INFO_FLAG_GETWEAPON :
		check = sizeof( N_PCINFO_GETWEAPON);
		break;
	case P2P_INFO_FLAG_DROPWEAPON :
		check = sizeof( N_PCINFO_DROPWEAPON);
		break;
	case P2P_INFO_FLAG_DIRECTPICKUP:
		check = sizeof(N_PCINFO_DIRECTPICKUP);
		break;
	case P2P_INFO_FLAG_FIRE :
		check = sizeof( N_PCINFO_BULLET);
		break;
	case P2P_INFO_FLAG_HITBYCHARA :
		check = sizeof( N_PCINFO_HIT_DEDICATED2);
		break;
	case P2P_INFO_FLAG_HITBYEXPLOSION :
		check = sizeof( N_PCINFO_HIT_BYEXPLOSION);
		break;
	case P2P_INFO_FLAG_HITBYOBJECT :
		check = sizeof( N_PCINFO_HIT_BYOBJECT);
		break;
	case P2P_INFO_FLAG_CS_DEATH :
		check = sizeof( N_PCINFO_DEATH);
		break;
	case P2P_INFO_FLAG_CS_SUFFERING:
		check = sizeof( N_PCINFO_SUFFER);
		break;
	}

	if( pos + check >= limitSize)
		return false;

	return true;
}

void GAMEINFO_CHARA_HP::CopyTo( GameNetworkInfoBase * pDest)
{
	GAMEINFO_CHARA_HP * pInfo = (GAMEINFO_CHARA_HP*) pDest;
	pInfo->_nCharaHP			= _nCharaHP;
}

UINT16 GAMEINFO_CHARA_HP::Pack( char * pOutPacket, UINT32 ui32BufSize )
{
	N_PCINFO_LIFE * pOut = (N_PCINFO_LIFE*) pOutPacket;

	pOut->iLife			= getHP();

	return sizeof( N_PCINFO_LIFE);
}

UINT16 GAMEINFO_CHARA_HP::Unpack( const char * pInPacket)
{
	N_PCINFO_LIFE * pIn = (N_PCINFO_LIFE*) pInPacket;

	setHP( pIn->iLife);

	return sizeof( N_PCINFO_LIFE);
}

UINT16 GAMEINFO_CHARA_HP::getHP( void) const
{
#if defined( MEM_ENCRYPTION)
	return GlobalFunc::DecryptValue( &_nCharaHP, ENC_SHIFT_CHARA_HP);
#else
	return _nCharaHP;
#endif
}

void GAMEINFO_CHARA_HP::setHP( UINT16 hp)
{
#if defined( MEM_ENCRYPTION)
	GlobalFunc::EncryptValue( &hp, sizeof( hp), ENC_SHIFT_CHARA_HP);
#endif

	_nCharaHP = hp;
}


void GAMEINFO_CHARA_WEAPON::Reset()
{
	_bWeaponExtension	= false;
	_bSwap				= false;
	_ui32WeaponID		= 0;
	_AttackType			= WEAPON::ATTACK_TYPE_L1;
}

void GAMEINFO_CHARA_WEAPON::CopyTo( GameNetworkInfoBase * pDest)
{
	GAMEINFO_CHARA_WEAPON * pInfo = static_cast<GAMEINFO_CHARA_WEAPON*>(pDest);
	pInfo->_bWeaponExtension	= _bWeaponExtension;
	pInfo->_ui32WeaponID		= _ui32WeaponID;
	pInfo->_AttackType			= _AttackType;
	pInfo->_bSwap				= _bSwap;
}

UINT16 GAMEINFO_CHARA_WEAPON::Pack( char * pOutPacket, UINT32 ui32BufSize )
{
	if( sizeof(N_PCINFO_WEAPON) > ui32BufSize )			return 0;

	N_PCINFO_WEAPON * pOut = (N_PCINFO_WEAPON*) pOutPacket;

	I3ASSERT( getWeaponClass() <= 255);
	I3ASSERT( getWeaponNumber() <= 999);

	pOut->Reset();
	pOut->SetWeaponID( _ui32WeaponID);
	pOut->SetIsExtension( _bWeaponExtension);
	pOut->SetAttackType( (UINT8) _AttackType );
	pOut->SetIsSwap( _bSwap );
	
	return sizeof( N_PCINFO_WEAPON);
}

UINT16 GAMEINFO_CHARA_WEAPON::Pack_Param( char * pOutPacket, UINT32 ui32BufSize, WeaponParamClass iClass)
{
	if( sizeof(N_PCINFO_WEAPON_PARAM) > ui32BufSize )	return 0;

	N_PCINFO_WEAPON_PARAM * pOut = (N_PCINFO_WEAPON_PARAM*) pOutPacket;

	pOut->Reset();
	pOut->_tWeapon.SetWeaponID( _ui32WeaponID );

#if defined( CLIENT_SIDE)
	CWeaponInfo * pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( _ui32WeaponID, true);

	// 이 부분이 수정되면 데디도 함께 수정되어야 합니다.
	pOut->iClass  = (UINT8) iClass;
	switch( iClass )
	{
	case WEAPON_PARAM_RECOIL:
		{
			pOut->iCount  = 5;
			pOut->Args[0] = pWeaponInfo->GetRecoilHorzAngle();
			pOut->Args[1] = pWeaponInfo->GetRecoilHorzMax();
			pOut->Args[2] = pWeaponInfo->GetRecoilVertAngle();
			pOut->Args[3] = pWeaponInfo->GetRecoilVertMax();
			pOut->Args[4] = pWeaponInfo->GetDeviation();
		}
		break;
	case WEAPON_PARAM_BULLET_COUNT:
		{
			pOut->iCount  = 1;
			pOut->Args[0] = (REAL32)pWeaponInfo->GetFireBulletCount();
		}
		break;
	case WEAPON_PARAM_DAMAGE:
		{
			pOut->iCount  = 1;
			pOut->Args[0] = (REAL32)pWeaponInfo->GetDamage();
		}
		break;
	default:
		break;
	}
#endif 

	return sizeof( N_PCINFO_WEAPON_PARAM );
}

UINT16 GAMEINFO_CHARA_WEAPON::Unpack( const char * pInPacket)
{
	N_PCINFO_WEAPON * pIn = (N_PCINFO_WEAPON*) pInPacket;

	if( pIn->GetWeaponUsage() >= WEAPON_SLOT_COUNT ) {
		UDPERROR( UDP_ERROR_SYNCWEAPON, pIn->GetWeaponUsage(), "invalid usage type");
	}

	if( pIn->GetWeaponClass() >= WEAPON_CLASS_COUNT) {
		UDPERROR( UDP_ERROR_SYNCWEAPON, pIn->GetWeaponClass(), "invalid class type");
	}

	if( pIn->GetWeaponNumber() > 999) {
		UDPERROR( UDP_ERROR_SYNCWEAPON, pIn->GetWeaponNumber(), "invalid number");
	}
	
	BOOL	nExtension	=  pIn->IsExtension();
	BOOL	bSwap		=  pIn->IsSwap();

	setWeaponExtension( nExtension ? true:false);
	SetAttackType( (WEAPON::ATTACK_TYPE) pIn->GetAttackType());
	setWeaponID( pIn->GetWeaponID());
	setWeaponSwap( bSwap ? true : false);
	
	return sizeof( N_PCINFO_WEAPON);
}

UINT16 GAMEINFO_CHARA_WEAPON::Unpack_Param( const char * pInPacket)
{
	N_PCINFO_WEAPON_PARAM * pIn = (N_PCINFO_WEAPON_PARAM*) pInPacket;
	I3ASSERT( pIn->_tWeapon.GetWeaponClass() != 0 );

	if( pIn->iCount  > MAX_WEAPON_PARAM_ARGS ) {
		UDPERROR( UDP_ERROR_WEAPONPARAM, pIn->iCount, "invalid count");
	}

	if( pIn->iClass >= MAX_WEAPON_PARAM) {
		UDPERROR( UDP_ERROR_WEAPONPARAM, pIn->iClass, "invalid parameter class");
	}
	
	return sizeof( N_PCINFO_WEAPON_PARAM );
}


void GAMEINFO_CHARA_POSROT::Reset()
{
	_fTheta			= 0.f;
	_fRho			= 0.f;
	i3Vector::Set( &_vPos, 0.f, 100.f, 0.f );
	_SectorIdx		= -1;
}

void GAMEINFO_CHARA_POSROT::CopyTo( GameNetworkInfoBase * pDest)
{
	GAMEINFO_CHARA_POSROT * pInfo = (GAMEINFO_CHARA_POSROT*) pDest;
	pInfo->_fTheta				= _fTheta;
	pInfo->_fRho				= _fRho;
	i3Vector::Copy( &pInfo->_vPos, &_vPos );
	pInfo->_SectorIdx			= _SectorIdx;
}

UINT16 GAMEINFO_CHARA_POSROT::Pack( char * pOutPacket, UINT32 ui32BufSize )
{
	N_PCINFO_POSROTATION * pOutInfo = (N_PCINFO_POSROTATION*) pOutPacket;

	UDP_UTIL::ConvertVec3DtoNet3D( &pOutInfo->_Pos, &_vPos );
	UDP_UTIL::ConvertRealToNet( &pOutInfo->iTheta, _fTheta );
	UDP_UTIL::ConvertRealToNet( &pOutInfo->iRho, _fRho );
	pOutInfo->iSectorIdx = _SectorIdx;

	return sizeof( N_PCINFO_POSROTATION);
}

UINT16 GAMEINFO_CHARA_POSROT::Unpack( const char * pInPacket)
{
	N_PCINFO_POSROTATION * pInInfo = (N_PCINFO_POSROTATION*) pInPacket;
	
	UDP_UTIL::ConvertNet3DtoVec3D( &_vPos, &pInInfo->_Pos, __FILE__, __LINE__ );
	UDP_UTIL::ConvertNetToReal( &_fTheta, pInInfo->iTheta );
	UDP_UTIL::ConvertNetToReal( &_fRho, pInInfo->iRho );

	_SectorIdx = pInInfo->iSectorIdx;

	return sizeof( N_PCINFO_POSROTATION);
}

UINT16 GAMEINFO_CHARA_POSROT::Pack_CommandMove( char * pOutPacket)
{
	N_PCINFO_COMMAND_MOVE * pOut = (N_PCINFO_COMMAND_MOVE*) pOutPacket;

	UDP_UTIL::ConvertVec3DtoNet3D( &pOut->vPos, &_vPos );

	return sizeof(N_PCINFO_COMMAND_MOVE);
}

UINT16 GAMEINFO_CHARA_POSROT::Unpack_CommandMove( const char * pInPacket)
{
	N_PCINFO_COMMAND_MOVE * pIn = (N_PCINFO_COMMAND_MOVE*) pInPacket;

	UDP_UTIL::ConvertNet3DtoVec3D( &_vPos, &pIn->vPos, __FILE__, __LINE__ );

	return sizeof(N_PCINFO_COMMAND_MOVE);
}

void GAMEINFO_ACTIONKEY::Reset()
{
	_Count = 0;
	for( INT32 i = 0 ; i < MAX_ACTION_COUNT ; i++ )
	{
		_ActionData[ i ]._nMoveKey				= 0;
		_ActionData[ i ]._nActionKey			= 0;
		_ActionData[ i ]._nUseActionObjectIdx	= 0xFFFF;
	}
}

void GAMEINFO_ACTIONKEY::CopyTo( GameNetworkInfoBase * pDest)
{
	GAMEINFO_ACTIONKEY * pInfo = (GAMEINFO_ACTIONKEY*) pDest;
	pInfo->_Count		= _Count;
	for( INT32 i = 0 ; i < _Count ; i++ )
	{
		pInfo->_ActionData[ i ]._nMoveKey				= _ActionData[ i ]._nMoveKey;
		pInfo->_ActionData[ i ]._nActionKey				= _ActionData[ i ]._nActionKey;
		pInfo->_ActionData[ i ]._nUseActionObjectIdx	= _ActionData[ i ]._nUseActionObjectIdx;
	}
}

UINT16 GAMEINFO_ACTIONKEY::Pack( char * pOutPacket, UINT32 ui32BufSize )
{
	UINT16 ui16Pos = 0;

	*((UINT8*)pOutPacket) = (UINT8)_Count;
	ui16Pos += sizeof(UINT8);
	
	N_PCINFO_ACTIONKEY* pActionKey;
	for( INT32 i = 0 ; i < _Count ; i++ )
	{
		pActionKey = (N_PCINFO_ACTIONKEY*)&pOutPacket[ ui16Pos ];
		ui16Pos += sizeof(N_PCINFO_ACTIONKEY);

		pActionKey->iMoveKey				= _ActionData[i]._nMoveKey;
		pActionKey->iEventActionKey			= _ActionData[i]._nActionKey;
		pActionKey->iUseObjectIdx			= _ActionData[i]._nUseActionObjectIdx;
	}

	return ui16Pos;
}

UINT16 GAMEINFO_ACTIONKEY::Unpack( const char * pInPacket)
{
	UINT16 ui16Pos = 0;

	_Count = (*(UINT8*)pInPacket);
	ui16Pos += sizeof(UINT8);

	N_PCINFO_ACTIONKEY* pActionKey;
	for( INT32 i = 0 ; i < _Count ; i++ )
	{
		pActionKey = (N_PCINFO_ACTIONKEY*)&pInPacket[ ui16Pos ];
		ui16Pos += sizeof(N_PCINFO_ACTIONKEY);

		if( (pActionKey->iUseObjectIdx != 0xFFFF) &&
		(pActionKey->iUseObjectIdx >= OBJ_TOTAL_COUNT) )
		{
			//UDPERROR( UDP_ERROR_ACTIONKEY, pActionKey->iUseObjectIdx, "Invalid Object Index");
		}

		_ActionData[i]._nMoveKey			= pActionKey->iMoveKey;	// 0x0F move key, 0xF0 ActionKey
		_ActionData[i]._nActionKey			= pActionKey->iEventActionKey;
		_ActionData[i]._nUseActionObjectIdx	= pActionKey->iUseObjectIdx;	// use object idx
	}

	return ui16Pos;
}

UINT16 GAMEINFO_ACTIONKEY::Pack(UINT8* pui8Count, char * pOutPacket, UINT32 ui32BufSize)
{
	UINT16 ui16Pos = 0;

	*pui8Count = (UINT8)_Count;

	N_PCINFO_ACTIONKEY* pActionKey;
	for (INT32 i = 0; i < _Count; i++)
	{
		pActionKey = (N_PCINFO_ACTIONKEY*)&pOutPacket[ui16Pos];
		ui16Pos += sizeof(N_PCINFO_ACTIONKEY);

		pActionKey->iMoveKey		= _ActionData[i]._nMoveKey;
		pActionKey->iEventActionKey	= _ActionData[i]._nActionKey;
		pActionKey->iUseObjectIdx	= _ActionData[i]._nUseActionObjectIdx;
	}

	return ui16Pos;
}

BOOL GAMEINFO_ACTIONKEY::Inset( GameNetworkInfoBase* pDest )
{
	GAMEINFO_ACTIONKEY * pInfo = (GAMEINFO_ACTIONKEY*) pDest;

	if( MAX_ACTION_COUNT < _Count + pInfo->_Count )	return FALSE;

	INT32 i32Idx;
	for( INT32 i = 0 ; i < pInfo->_Count ; i++ )
	{
		i32Idx = _Count + i;
		_ActionData[ i32Idx ]._nMoveKey				= pInfo->_ActionData[ i ]._nMoveKey;
		_ActionData[ i32Idx ]._nActionKey			= pInfo->_ActionData[ i ]._nActionKey;
		_ActionData[ i32Idx ]._nUseActionObjectIdx	= pInfo->_ActionData[ i ]._nUseActionObjectIdx;
	}
	_Count		+= pInfo->_Count;

	return TRUE;
}

void GAMEINFO_CHARA_STATE::Reset()
{
	_nJumpState = _Body = _Body2 = _Upper = _Lower = 0;
	_nFireAction = 0;
	_nHelmet = 0;
	_FBState = 0;
	_FBSubState = 0;
	_bWeaponExtension = false;
	_nWeaponSlot = WEAPON_SLOT_PRIMARY;
	_nMultiWeaponSlot = 0;
	_nWeaponFireOrder = WEAPON::PRIMARY_FIRE;
}

void GAMEINFO_CHARA_STATE::CopyTo( GameNetworkInfoBase * pDest)
{
	GAMEINFO_CHARA_STATE * pInfo = (GAMEINFO_CHARA_STATE*) pDest;

	pInfo->_Body = _Body;
	pInfo->_Body2 = _Body2;
	pInfo->_Upper = _Upper;
	pInfo->_Lower = _Lower;
	pInfo->_nJumpState = _nJumpState;
	pInfo->_nFireAction = _nFireAction;
	pInfo->_nHelmet = _nHelmet;
	pInfo->_FBState = _FBState;
	pInfo->_FBSubState = _FBSubState;
	pInfo->_bWeaponExtension = _bWeaponExtension;
	pInfo->_nWeaponSlot = _nWeaponSlot;
	pInfo->_nMultiWeaponSlot = _nMultiWeaponSlot;
	pInfo->_nWeaponFireOrder = _nWeaponFireOrder;
}

UINT16 GAMEINFO_CHARA_STATE::Pack( char * pOutPacket, UINT32 ui32BufSize )
{
	if( sizeof(N_PCINFO_CHARA_STATE) > ui32BufSize )	return 0;

	N_PCINFO_CHARA_STATE * pState = (N_PCINFO_CHARA_STATE*) pOutPacket;

	pState->iState = (UINT32) ( (_bWeaponExtension << 31) | (_nWeaponSlot << 28) | (_Upper << 20) | (_Lower << 16) | (_nFireAction << 15) | (_Body << 14) | (_Body2 << 10) | (_nMultiWeaponSlot << 6) | (_nJumpState << 3 ) | (_nWeaponFireOrder << 2) | (_nHelmet & 0x0003));

	return sizeof( N_PCINFO_CHARA_STATE);
}

UINT16 GAMEINFO_CHARA_STATE::Unpack( const char * pInPacket)
{
	N_PCINFO_CHARA_STATE * pState = (N_PCINFO_CHARA_STATE*) pInPacket;

	UINT8 extension = 0;
	UINT8 fireOrder = 0;
	UINT8 weaponSlot = 0;
	UINT8 multiWeaponSlot = 0;
	UDP_UTIL::ConvertNetActiontoChara( &extension, &weaponSlot, &multiWeaponSlot, &_nFireAction, &_Body, &_Body2, &_Upper, &_Lower, &_nJumpState, &fireOrder, &_nHelmet, pState->iState );
	_bWeaponExtension = extension?true:false;
	_nWeaponFireOrder = (WEAPON::FIRE_ORDER) fireOrder;
	_nWeaponSlot = (WEAPON_SLOT_TYPE) weaponSlot;
	_nMultiWeaponSlot = multiWeaponSlot;
	/// Check
#if defined( CLIENT_SIDE)
	if( _Body >= CHARA_BODYLOWER_MAXCOUNT)
	{
		UDPERROR( UDP_ERROR_ACTIONSTATE, _Body, "Body state invalid");
	}

	if( _Body2 >= CHARA_BODYUPPER_MAXCOUNT)
	{
		UDPERROR( UDP_ERROR_ACTIONSTATE, _Body2, "Body2 state invalid");
	}

	if( _Upper >= CHARA_UPPER_MAXCOUNT)
	{
		UDPERROR( UDP_ERROR_ACTIONSTATE, _Upper, "Upper state invalid");
	}

	if( _Lower >= CHARA_LOWER_MAXCOUNT)
	{
		UDPERROR( UDP_ERROR_ACTIONSTATE, _Lower, "Lower state invalid");
	}

	if( _nWeaponFireOrder >= WEAPON::FIRE_ORDER_COUNT)
	{
		UDPERROR( UDP_ERROR_ACTIONSTATE, _nWeaponFireOrder, "FireOrder state invalid");
	}

	if( _nHelmet >= NET_HELMET_STATE_COUNT)
	{
		UDPERROR( UDP_ERROR_ACTIONSTATE, _nHelmet, "Helmet state invalid");
	}
#endif

	return sizeof( N_PCINFO_CHARA_STATE);
}

UINT16 GAMEINFO_CHARA_STATE::Pack_FullBody( char * pOutPacket)
{
	N_PCINFO_AI_STATE * pOut = (N_PCINFO_AI_STATE*) pOutPacket;

	pOut->iState = _FBState;
	pOut->iSubState = _FBSubState;

	return sizeof(N_PCINFO_AI_STATE);
}

UINT16 GAMEINFO_CHARA_STATE::Unpack_FullBody( const char * pInPacket)
{
	N_PCINFO_AI_STATE * pIn = (N_PCINFO_AI_STATE*) pInPacket;

	_FBState = pIn->iState;
	_FBSubState = pIn->iSubState;

	return sizeof(N_PCINFO_AI_STATE);
}


void GAMEINFO_TAKING_OBJECT::Reset()
{
	_bOnLoadObject = false;
	_iOnLoadObjectIndex = 0;
}

void GAMEINFO_TAKING_OBJECT::CopyTo( GameNetworkInfoBase * pDest)
{
	GAMEINFO_TAKING_OBJECT * pInfo = (GAMEINFO_TAKING_OBJECT*) pDest;
	pInfo->_bOnLoadObject		= _bOnLoadObject;
	pInfo->_iOnLoadObjectIndex	= _iOnLoadObjectIndex;
}

UINT16 GAMEINFO_TAKING_OBJECT::Pack( char * pOutPacket, UINT32 ui32BufSize )
{
	N_PCINFO_ONLOADOBJECT * pOut = (N_PCINFO_ONLOADOBJECT*) pOutPacket;

	pOut->OnLoadObjectFlag	= 0;

	if( _bOnLoadObject )	pOut->OnLoadObjectFlag |= 0x01;
	else					pOut->OnLoadObjectFlag &= 0xFE;

	UINT16 ui16Idx = MIN( 0x1FF, _iOnLoadObjectIndex );
	pOut->OnLoadObjectFlag |= (ui16Idx << 4);

	return sizeof(N_PCINFO_ONLOADOBJECT);
}

UINT16 GAMEINFO_TAKING_OBJECT::Unpack( const char * pInPacket)
{
	N_PCINFO_ONLOADOBJECT * pInfo = (N_PCINFO_ONLOADOBJECT*) pInPacket;

	_bOnLoadObject			= ((pInfo->OnLoadObjectFlag & 0x01) == 0x01);
	_iOnLoadObjectIndex		= ((pInfo->OnLoadObjectFlag >> 4) & 0x1FF);

	return sizeof( N_PCINFO_ONLOADOBJECT);
}

void GAMEINFO_SYNC_OBJECT::Reset()
{
	_nSyncObjectState = 0;
	_iSyncObjectIdx = 0;
}

void GAMEINFO_SYNC_OBJECT::CopyTo( GameNetworkInfoBase * pDest)
{
	GAMEINFO_SYNC_OBJECT * pInfo = (GAMEINFO_SYNC_OBJECT*) pDest;
	pInfo->_nSyncObjectState = _nSyncObjectState;
	pInfo->_iSyncObjectIdx = _iSyncObjectIdx;
}

UINT16 GAMEINFO_SYNC_OBJECT::Pack( char * pOutPacket, UINT32 ui32BufSize )
{
	N_PCINFO_SYNCOBJECT * pOut = (N_PCINFO_SYNCOBJECT*) pOutPacket;
	pOut->Reset();
	pOut->SetSyncState( _nSyncObjectState);
	pOut->SetObjectIndex( _iSyncObjectIdx);
	return sizeof(N_PCINFO_SYNCOBJECT);
}

UINT16 GAMEINFO_SYNC_OBJECT::Unpack( const char * pInPacket)
{
	N_PCINFO_SYNCOBJECT * pInfo = (N_PCINFO_SYNCOBJECT*) pInPacket;

	if( pInfo->GetObjectIndex() != 0x1FF && pInfo->GetObjectIndex() >= OBJ_TOTAL_COUNT )
	{
		UDPERROR( UDP_ERROR_SYNCOBJ, pInfo->GetObjectIndex(), "Object Index is invalid");
	}

	_nSyncObjectState	= pInfo->GetSyncState();
	_iSyncObjectIdx		= pInfo->GetObjectIndex();
	return sizeof(N_PCINFO_SYNCOBJECT);
}

void GAMEINFO_RADIO_CHAT::Reset()
{
	_nRadio = 0;
	_nSectorIdx = 0;
}

void GAMEINFO_RADIO_CHAT::CopyTo( GameNetworkInfoBase * pDest)
{
	GAMEINFO_RADIO_CHAT * pInfo = (GAMEINFO_RADIO_CHAT*) pDest;
	pInfo->_nRadio = _nRadio;
	pInfo->_nSectorIdx = _nSectorIdx;
}

UINT16 GAMEINFO_RADIO_CHAT::Pack( char * pOutPacket, UINT32 ui32BufSize )
{
	N_PCINFO_RADIOCHAT * pOut = (N_PCINFO_RADIOCHAT*) pOutPacket;

	pOut->iRadio	= _nRadio;
	pOut->iSectorIdx= _nSectorIdx;

	return sizeof( N_PCINFO_RADIOCHAT);
}

UINT16 GAMEINFO_RADIO_CHAT::Unpack( const char * pInPacket)
{
	N_PCINFO_RADIOCHAT * pInfo = (N_PCINFO_RADIOCHAT*) pInPacket;

	I3ASSERT( pInfo->iRadio < (RADIOCHAT_TYPE_COUNT * 10));

	_nRadio				= pInfo->iRadio;
	_nSectorIdx			= pInfo->iSectorIdx;

	return sizeof( N_PCINFO_RADIOCHAT);
}

void GAMEINFO_HIT::Reset()
{
	_FireHitCount	= 0;

	VEC3D _ZeroVec;
	i3Vector::Zero(&_ZeroVec);

	for( INT32 i = 0; i < MAX_FIREHIT_COUNT; i++ )
	{
		_tHitData[i]._Type			= (GAMEINFO_HIT_TYPE)0;
		_tHitData[i]._FireHitWho	= 0;
		_tHitData[i]._FireHitPart	= 0;
		_tHitData[i]._FireHitDamage	= 0;
		_tHitData[i]._CritalFlag	= 0;
		_tHitData[i]._HelmetFlag	= 0;

		i3Vector::Zero( &_tHitData[i]._HitDirection);
		_tHitData[i]._DeathType		= CHARA_DEATH_UN;
		_tHitData[i]._ByObjectState	= 0xFF;
		_tHitData[i]._ByObjectIndex	= 0xFFFF;
		setFirePos(i,&_ZeroVec);
		setHitPos(i,&_ZeroVec);

		_tHitData[i].HitbyObjectExplosion =0;
		_tHitData[i].m_ui8AttackType = 0;

		_tHitData[i]._WeaponInfo.Reset();
	}
}

void GAMEINFO_HIT::CopyTo( GameNetworkInfoBase * pDest)
{
	GAMEINFO_HIT * pInfo = (GAMEINFO_HIT*) pDest;

	pInfo->_FireHitCount	= _FireHitCount;
	for( INT32 i = 0; i < _FireHitCount; i++ )
	{
		pInfo->_tHitData[i]._Type			= _tHitData[i]._Type;
		pInfo->_tHitData[i]._FireHitWho		= _tHitData[i]._FireHitWho;
		pInfo->_tHitData[i]._FireHitPart	= _tHitData[i]._FireHitPart;
		pInfo->_tHitData[i]._FireHitDamage	= _tHitData[i]._FireHitDamage;
		pInfo->_tHitData[i]._CritalFlag		= _tHitData[i]._CritalFlag;
		pInfo->_tHitData[i]._HelmetFlag		= _tHitData[i]._HelmetFlag;

		i3Vector::Copy( &pInfo->_tHitData[i]._HitDirection, &_tHitData[i]._HitDirection);
		pInfo->_tHitData[i]._DeathType		= _tHitData[i]._DeathType;
		pInfo->_tHitData[i]._ByObjectState	= _tHitData[i]._ByObjectState;
		pInfo->_tHitData[i]._ByObjectIndex	= _tHitData[i]._ByObjectIndex;

#ifdef CLIENT_SIDE
		VEC3D vFirePos = getFirePos(i);
		VEC3D vHitPos = getHitPos(i);
		pInfo->setFirePos(i,&vFirePos);
		pInfo->setHitPos(i,&vHitPos);
#else
		pInfo->setFirePos(i,getFirePos(i));
		pInfo->setHitPos(i,getHitPos(i));
#endif
		
		pInfo->_tHitData[i].HitbyObjectExplosion = _tHitData[i].HitbyObjectExplosion;
		pInfo->_tHitData[i].m_ui8AttackType	= _tHitData[i].m_ui8AttackType;

		_tHitData[i]._WeaponInfo.CopyTo( &pInfo->_tHitData[i]._WeaponInfo);
	}
}

UINT16 GAMEINFO_HIT::Pack( char * pOutPacket, UINT32 ui32BufSize )
{
	UINT16 pos = 0;

	if( _FireHitCount > MAX_FIREHIT_COUNT )
	{
		I3PRINTLOG(I3LOG_WARN,  "invalid hit count P2P_INFO_FLAG_HIT - swoongoo" );
		_FireHitCount = MAX_FIREHIT_COUNT;
	}

	if( sizeof(UINT8) > ui32BufSize )	return 0;

	*((UINT8*)pOutPacket) = (UINT8) _FireHitCount; 
	pos += sizeof( UINT8 );

	for( INT32 i = 0; i < _FireHitCount; i++)
	{
		pos += Pack_SingleHit( &pOutPacket[pos], ui32BufSize-pos, i);
	}

	return pos;
}

UINT16 GAMEINFO_HIT::Pack_SingleHit( char * pOutPacket, UINT32 ui32BufSize, INT32 idx )
{
	UINT16 pos = 0;

	N_PCINFO_HIT * pOut = (N_PCINFO_HIT*) &pOutPacket[pos];
	pos += sizeof( N_PCINFO_HIT);

	pOut->Reset();
	pOut->SetHitType( (UINT8) getHitType( idx));
	pOut->SetIndex( getHitWho( idx));
	pOut->SetHitPart( getHitPart( idx));
	pOut->SetDamage( getHitDamage( idx));
	pOut->SetHelmet( getHelmetFlag( idx));

	getWeaponInfo(idx)->Pack( (char*) &pOut->_WeaponInfo, ui32BufSize );

	UDP_UTIL::ConvertVec3DtoNet3D( &pOut->_Direction, getHitDirection( idx) );
	pOut->SetDeathType( (UINT8) getDeathType( idx));

	pOut->SetByObjectState( getByObjectState( idx));
	pOut->SetByObjectIndex( getByObjectIndex( idx));

	return pos;
}

UINT16 GAMEINFO_HIT::Unpack( const char * pInPacket)
{
	VEC3D vDir;

	UINT16 pos = 0;
	UINT8 nHitCount = *((UINT8*) pInPacket);
	pos += sizeof( UINT8);

	if( nHitCount > MAX_FIREHIT_COUNT)
	{
		UDPERROR( UDP_ERROR_HIT, nHitCount, "Hit count invalid");
	}

	for( INT32 i = 0; i < nHitCount; i++)
	{
		N_PCINFO_HIT * pHitPacket = (N_PCINFO_HIT *) &pInPacket[ pos];
		pos += sizeof( N_PCINFO_HIT);

		// 맞은 사람의 데이터입니다. (nIndex == 맞은 사람 )
		if( pHitPacket->GetHitType() == GHT_CHARA)
		{	
			if( pHitPacket->GetIndex() >= SLOT_MAX_COUNT )
			{	// 오브젝트나 아이템 히트를 받았다?
				// 이것은 Dedicated Server에서 보낼 필요가 없는 패킷임.
				UDPERROR( UDP_ERROR_HIT, pHitPacket->GetIndex(), "HitWho is invalid");
			}

			UINT8 HitPart = pHitPacket->GetHitPart();
			if( HitPart == CHARA_HIT_UNKNOWN || HitPart >= CHARA_HIT_MAXCOUNT )
			{
				UDPERROR( UDP_ERROR_HIT, HitPart, "Part is invalid");
			}
		}

		if( pHitPacket->GetHitType() == GHT_AI)
		{
			if( pHitPacket->GetIndex() >= MAX_COUNT_DOMI_DINO)
			{
				UDPERROR( UDP_ERROR_HIT, pHitPacket->GetIndex(), "HitWho(AI) is invalid");
			}
		}

		setHitType( i, (GAMEINFO_HIT_TYPE) pHitPacket->GetHitType() );
		setHitWho( i, pHitPacket->GetIndex());
		setHitPart( i, pHitPacket->GetHitPart());
		setHitDamage( i, pHitPacket->GetDamage());
		
		// DedicatedMode packet
		setHelmetFlag( i, pHitPacket->GetHelmet());

		if( getWeaponInfo(i)->Unpack( (const char*) &pHitPacket->_WeaponInfo ) & UDP_ERROR)
		{
			return UDP_ERROR_HIT;
		}

		UDP_UTIL::ConvertNet3DtoVec3D( &vDir, &pHitPacket->_Direction, __FILE__, __LINE__ );
		setHitDirection( i, &vDir);
		setDeathType( i, (CHARA_DEATH_TYPE) pHitPacket->GetDeathType());

		setByObjectState( i, pHitPacket->GetByObjectState());
		setByObjectIndex( i, pHitPacket->GetByObjectIndex());
	}

	setHitCount( nHitCount);

	return pos;
}

UINT16 GAMEINFO_HIT::Pack_Dedicated( char * pOutPacket, UINT32 ui32BufSize, UINT8 count)
{
	UINT16 pos = 0;

	if( sizeof(UINT8)  > ui32BufSize )	return 0;

	*((UINT8*)pOutPacket) = count;
	pos += sizeof( UINT8);

	for( INT32 i = 0; i < getHitCount(); i++)
	{
		if( getByType( i) != GHBT_CHARA)
			continue;

		if( (sizeof(N_PCINFO_HIT_DEDICATED2) + pos) > ui32BufSize )	return 0;

		N_PCINFO_HIT_DEDICATED2 * pOut = (N_PCINFO_HIT_DEDICATED2*) &pOutPacket[pos];
		pos += sizeof( N_PCINFO_HIT_DEDICATED2);

		pOut->Reset();

		pOut->_tDefault.Set( (UINT8) getHitType(i), getHitWho(i), getHitPart(i), getHelmetFlag(i), getHitDamage(i), getCriticalFlag(i), getHitbyObjectExplosion(i));

		// Weapon
		UDPTRACE("GAMEINFO_HIT::Pack_Dedicated() - WeaponID = %d\n", getWeaponID(i));
		getWeaponInfo(i)->Pack( (char*)&pOut->_tWeapon, ui32BufSize );

#ifdef CLIENT_SIDE
		// hitted position
		VEC3D tHitPos = getHitPos( i);
		pOut->_HitPos.x = getX( &tHitPos );
		pOut->_HitPos.y = getY( &tHitPos );
		pOut->_HitPos.z = getZ( &tHitPos );

		VEC3D tFirePos = getFirePos( i);
		pOut->_stFirePos.x = getX( &tFirePos );
		pOut->_stFirePos.y = getY( &tFirePos );
		pOut->_stFirePos.z = getZ( &tFirePos );
#else
		// hitted position
		VEC3D* ptHitPos = getHitPos( i);
		pOut->_HitPos.x = getX( ptHitPos );
		pOut->_HitPos.y = getY( ptHitPos );
		pOut->_HitPos.z = getZ( ptHitPos );
		
		VEC3D* ptFirePos = getFirePos( i);
		pOut->_stFirePos.x = getX( ptFirePos );
		pOut->_stFirePos.y = getY( ptFirePos );
		pOut->_stFirePos.z = getZ( ptFirePos );
#endif
	}

	return pos;
}

UINT16 GAMEINFO_HIT::Unpack_Dedicated( const char * pInPacket, INT32 & preIdx)
{
	UINT8 i;
	VEC3D vPos;

	UINT16 pos = 0;
	UINT8 nHitCount = *((UINT8*)pInPacket);
	pos += sizeof( UINT8);

	if( preIdx + nHitCount > MAX_FIREHIT_COUNT)
	{
		UDPERROR( UDP_ERROR_HIT, preIdx + nHitCount, "Hit count invalid");
	}

	for( i = 0; i < nHitCount; i++)
	{
		INT32 idx = preIdx + i;

		N_PCINFO_HIT_DEDICATED2 * pHitPacket = (N_PCINFO_HIT_DEDICATED2 *) &pInPacket[ pos];
		pos += sizeof( N_PCINFO_HIT_DEDICATED2);

		I3ASSERT( pHitPacket->_tWeapon.GetWeaponClass() != 0 );
		
		// 맞은 사람의 데이터입니다. (nIndex == 맞은 사람 )
		if( pHitPacket->_tDefault.GetHitType() == GHT_CHARA)
		{	
			if( pHitPacket->_tDefault.GetIndex() >= SLOT_MAX_COUNT )
			{	// 오브젝트나 아이템 히트를 받았다?
				// 이것은 Dedicated Server에서 보낼 필요가 없는 패킷임.
				UDPERROR( UDP_ERROR_HIT, pHitPacket->_tDefault.GetIndex(), "HitWho is invalid");
			}
			
#if defined( CLIENT_SIDE)
			UINT8 HitPart = static_cast<UINT8>( HitPartContext::i()->DecryptHitPart( pHitPacket->_tDefault.GetHitPart()));
#else
			UINT8 HitPart = pHitPacket->_tDefault.GetHitPart();
#endif
			if( HitPart == CHARA_HIT_UNKNOWN || HitPart >= CHARA_HIT_MAXCOUNT )
			{
				UDPERROR( UDP_ERROR_HIT, HitPart, "Part is invalid");
			}
		}

		if( pHitPacket->_tDefault.GetHitType() == GHT_AI)
		{
			if( pHitPacket->_tDefault.GetIndex() >= MAX_COUNT_DOMI_DINO)
			{
				UDPERROR( UDP_ERROR_HIT, pHitPacket->_tDefault.GetIndex(), "HitWho(AI) is invalid");
			}
		}

		setHitType( idx, (GAMEINFO_HIT_TYPE) pHitPacket->_tDefault.GetHitType() );
		setHitWho( idx, pHitPacket->_tDefault.GetIndex());
		setHitPart( idx, pHitPacket->_tDefault.GetHitPart());
		setHelmetFlag( idx, pHitPacket->_tDefault.GetHelmet());
		setHitDamage( idx, pHitPacket->_tDefault.GetDamage());
		setCriticalFlag( idx, pHitPacket->_tDefault.GetCritical());

		if( getWeaponInfo(idx)->Unpack( (const char*) &pHitPacket->_tWeapon ) & UDP_ERROR)
		{
			return UDP_ERROR_HIT;
		}

		i3Vector::Set( &vPos, pHitPacket->_stFirePos.x, pHitPacket->_stFirePos.y, pHitPacket->_stFirePos.z);
		setFirePos( idx, &vPos);

		i3Vector::Set( &vPos, pHitPacket->_HitPos.x, pHitPacket->_HitPos.y, pHitPacket->_HitPos.z);
		setHitPos( idx, &vPos);
		setByType( idx, GHBT_CHARA);
	}

	preIdx += nHitCount;
	
	setHitCount( preIdx);

	return pos;
}

UINT16 GAMEINFO_HIT::Pack_Dedicated_Explosion( char * pOutPacket, UINT32 ui32BufSize, UINT8 count, UINT16 netSlotIdx)
{
	UINT16 pos = 0;

	if( sizeof(UINT8) > ui32BufSize )	return 0;

	*((UINT8*)pOutPacket) = count;
	pos += sizeof(UINT8);

	for( INT32 i = 0; i < getHitCount(); i++)
	{
		if( getByType( i) != GHBT_EXPLOSION)
			continue;

		if( (sizeof(N_PCINFO_HIT_BYEXPLOSION) + pos) > ui32BufSize )	return 0;

		N_PCINFO_HIT_BYEXPLOSION * pOut = (N_PCINFO_HIT_BYEXPLOSION*) &pOutPacket[pos];
		pos += sizeof( N_PCINFO_HIT_BYEXPLOSION);
		
		pOut->Reset();

		pOut->_tDefault.Set( (UINT8) getHitType(i), getHitWho(i), getHitPart(i), getHelmetFlag(i), getHitDamage(i), 0, getHitbyObjectExplosion(i));

		//Weapon
		UDPTRACE("UDP_UTIL::BuildCharaInfo_Hit_ByExplosion() - WeaponID = %d\n", getWeaponID(i));
		getWeaponInfo(i)->Pack( (char*) &pOut->_stWeapon, ui32BufSize );

		pOut->_i8AddInfo = (UINT8) getDeathType(i);
		pOut->_i16NetSlotIdx = netSlotIdx;

#ifdef CLIENT_SIDE
		VEC3D tFirePos = getFirePos( i);
		VEC3D tHitPos = getHitPos( i);

		UDP_UTIL::ConvertVec3DtoNet3D( &pOut->_stExplosionPos, &tFirePos );
		UDP_UTIL::ConvertVec3DtoNet3D( &pOut->_stHitPos, &tHitPos );
#else
		VEC3D* ptFirePos = getFirePos( i);
		VEC3D* ptHitPos = getHitPos( i);

		UDP_UTIL::ConvertVec3DtoNet3D( &pOut->_stExplosionPos, ptFirePos );
		UDP_UTIL::ConvertVec3DtoNet3D( &pOut->_stHitPos, ptHitPos );
#endif

		UDP_UTIL::ConvertVec3DtoNet3D( &pOut->_Direction, getHitDirection(i) );
	}

	return pos;
}

UINT16 GAMEINFO_HIT::Unpack_Dedicated_Explosion( const char * pInPacket, INT32 & preIdx)
{
	VEC3D vDir;

	UINT16 pos = 0;
	UINT8 nHitCount = *((UINT8*)pInPacket);
	pos += sizeof( UINT8);

	if( preIdx + nHitCount > MAX_FIREHIT_COUNT)
	{
		UDPERROR( UDP_ERROR_HIT, preIdx + nHitCount, "Hit count invalid");
	}

	for( UINT8 i = 0; i < nHitCount; i++)
	{
		INT32 idx = preIdx + i;

		N_PCINFO_HIT_BYEXPLOSION * pHitPacket = (N_PCINFO_HIT_BYEXPLOSION *) &pInPacket[ pos];
		pos += sizeof( N_PCINFO_HIT_BYEXPLOSION);

		I3ASSERT( pHitPacket->_stWeapon.GetWeaponClass() != 0 );
		
		// 맞은 사람의 데이터입니다. (nIndex == 맞은 사람 )
		if( pHitPacket->_tDefault.GetHitType() == GHT_CHARA)
		{	
			if( pHitPacket->_tDefault.GetIndex() >= SLOT_MAX_COUNT )
			{	// 오브젝트나 아이템 히트를 받았다?
				// 이것은 Dedicated Server에서 보낼 필요가 없는 패킷임.
				UDPERROR( UDP_ERROR_HIT, pHitPacket->_tDefault.GetIndex(), "HitWho is invalid");
			}

			UINT8 HitPart = pHitPacket->_tDefault.GetHitPart();
			if( HitPart == CHARA_HIT_UNKNOWN || HitPart >= CHARA_HIT_MAXCOUNT )
			{
				UDPERROR( UDP_ERROR_HIT, HitPart, "Part is invalid");
			}
		}

		if( pHitPacket->_tDefault.GetHitType() == GHT_AI)
		{
			if( pHitPacket->_tDefault.GetIndex() >= MAX_COUNT_DOMI_DINO)
			{
				UDPERROR( UDP_ERROR_HIT, pHitPacket->_tDefault.GetIndex(), "HitWho(AI) is invalid");
			}
		}

		setHitType( idx, (GAMEINFO_HIT_TYPE) pHitPacket->_tDefault.GetHitType() );
		setHitWho( idx, pHitPacket->_tDefault.GetIndex());
		setHitPart( idx, pHitPacket->_tDefault.GetHitPart());
		setHelmetFlag( idx, pHitPacket->_tDefault.GetHelmet());
		setHitDamage( idx, pHitPacket->_tDefault.GetDamage());
		getWeaponInfo(idx)->Unpack( (const char*)&pHitPacket->_stWeapon );

		UDP_UTIL::ConvertNet3DtoVec3D( &vDir, &pHitPacket->_Direction, __FILE__, __LINE__ );
		setHitDirection( idx, &vDir);
		setDeathType( idx, static_cast<CHARA_DEATH_TYPE>(pHitPacket->GetDeathType()));
		setByType( idx, GHBT_EXPLOSION );
	}
	
	preIdx += nHitCount;

	setHitCount( preIdx);

	return pos;
}

UINT16 GAMEINFO_HIT::Pack_Dedicated_Object( char * pOutPacket, UINT32 ui32BufSize, UINT8 count)
{
	UINT16 pos = 0;

	if( (sizeof(UINT8)) > ui32BufSize )								return 0;

	*((UINT8*)pOutPacket) = count;
	pos			+= sizeof( UINT8);

	for( INT32 i = 0; i < getHitCount(); i++)
	{
		if( getByType( i) != GHBT_OBJECT)
			continue;

		if( (sizeof(N_PCINFO_HIT_BYOBJECT) + pos) > ui32BufSize )	return 0;

		N_PCINFO_HIT_BYOBJECT * pOut = (N_PCINFO_HIT_BYOBJECT*) &pOutPacket[pos];
		pos += sizeof( N_PCINFO_HIT_BYOBJECT);

		pOut->Reset();

		pOut->_tDefault.Set( (UINT8) getHitType(i), getHitWho(i), getHitPart(i), 0, getHitDamage(i), 0,0);
		pOut->SetByObjectIndex( getByObjectIndex(i) );
		pOut->SetByObjectState( getByObjectState(i) );
		pOut->SetDeathType( (UINT8) getDeathType(i) );

		// Dedicated Mode Packet
		UDP_UTIL::ConvertVec3DtoNet3D( &pOut->_Direction, getHitDirection(i) );
	}

	return pos;
}

UINT16 GAMEINFO_HIT::Unpack_Dedicated_Object( const char * pInPacket, INT32 & preIdx)
{
	VEC3D vDir;

	UINT16 pos = 0;
	UINT8 nHitCount = *((UINT8*) pInPacket);
	pos += sizeof( UINT8);

	if( preIdx + nHitCount > MAX_FIREHIT_COUNT)
	{
		UDPERROR( UDP_ERROR_HIT, preIdx + nHitCount, "Hit count invalid");
	}

	for( UINT8 i = 0; i < nHitCount; i++)
	{
		INT32 idx = preIdx + i;

		N_PCINFO_HIT_BYOBJECT * pHitPacket = (N_PCINFO_HIT_BYOBJECT*) &pInPacket[ pos ];
		pos += sizeof( N_PCINFO_HIT_BYOBJECT);

		// 맞은 사람의 데이터입니다. (nIndex == 맞은 사람 )
		if( pHitPacket->_tDefault.GetHitType() == GHT_CHARA)
		{	
			if( pHitPacket->_tDefault.GetIndex() >= SLOT_MAX_COUNT )
			{	// 오브젝트나 아이템 히트를 받았다?
				// 이것은 Dedicated Server에서 보낼 필요가 없는 패킷임.
				UDPERROR( UDP_ERROR_HIT, pHitPacket->_tDefault.GetIndex(), "HitWho is invalid");
			}

			UINT8 HitPart = pHitPacket->_tDefault.GetHitPart();
			if( HitPart == CHARA_HIT_UNKNOWN || HitPart >= CHARA_HIT_MAXCOUNT )
			{
				UDPERROR( UDP_ERROR_HIT, HitPart, "Part is invalid");
			}
		}

		if( pHitPacket->_tDefault.GetHitType() == GHT_AI)
		{
			if( pHitPacket->_tDefault.GetIndex() >= MAX_COUNT_DOMI_DINO)
			{
				UDPERROR( UDP_ERROR_HIT, pHitPacket->_tDefault.GetIndex(), "HitWho(AI) is invalid");
			}
		}

		setHitType( idx, (GAMEINFO_HIT_TYPE) pHitPacket->_tDefault.GetHitType() );
		setHitWho( idx, pHitPacket->_tDefault.GetIndex());
		setHitPart( idx, pHitPacket->_tDefault.GetHitPart());
		setHelmetFlag( idx, pHitPacket->_tDefault.GetHelmet());
		setHitDamage( idx, pHitPacket->_tDefault.GetDamage());
		
		UDP_UTIL::ConvertNet3DtoVec3D( &vDir, &pHitPacket->_Direction , __FILE__, __LINE__);
		setHitDirection( idx, &vDir);
		setDeathType( idx, static_cast<CHARA_DEATH_TYPE>(pHitPacket->GetDeathType()));
		setByType( idx, GHBT_OBJECT );
	}
	
	preIdx += nHitCount;
	setHitCount( preIdx);

	return pos;
}

void GAMEINFO_FIRE::Reset()
{
	_nId			= MAX_THROWWEAPON_COUNT;
	_nRand			= 0;
	i3Vector::Zero( &_vStartPos );
	i3Vector::Zero( &_vTargetPos );
	_nNetSlotIdx	= 0;
	_tHitData.Reset();

#if defined( CHECK_LOADEDBULLET)
	_LoadedBullet	= 0;
	_LoadedDualBullet = 0;
#endif

	_TargetX		= 0.0f;
	_TargetZ		= 0.0f;
	_WeaponInfo.Reset();
}

void GAMEINFO_FIRE::CopyTo( GameNetworkInfoBase * pDest)
{
	GAMEINFO_FIRE * pInfo = (GAMEINFO_FIRE*) pDest;

	CopyToFire( pInfo);
	_tHitData.CopyTo( &pInfo->_tHitData);
	_WeaponInfo.CopyTo( &pInfo->_WeaponInfo);

	pInfo->_TargetX = _TargetX;
	pInfo->_TargetZ = _TargetZ;
}

UINT16 GAMEINFO_FIRE::Pack( char * pOutPacket, UINT32 ui32BufSize )
{
	N_PCINFO_BULLET * pOut = (N_PCINFO_BULLET*) pOutPacket;

	pOut->Id				= _nId;
	pOut->Rand				= (UINT16) _nRand;
	UDP_UTIL::ConvertVec3DtoNet3D( &pOut->_TargetPos, &_vTargetPos );

	UINT16 ui16Size = _WeaponInfo.Pack( (char*) &pOut->_WeaponInfo, ui32BufSize );
	if( 0 == ui16Size )		return 0;

#if defined( CHECK_LOADEDBULLET)
	pOut->_LoadedBullet		= _LoadedBullet;
	pOut->_LoadedDualBullet	= _LoadedDualBullet;
#endif

	return sizeof(N_PCINFO_BULLET);
}

UINT16 GAMEINFO_FIRE::Unpack( const char * pInPacket)
{
	N_PCINFO_BULLET * pIn = (N_PCINFO_BULLET*) pInPacket;

	_nRand		= pIn->Rand;

	UDP_UTIL::ConvertNet3DtoVec3D( &_vTargetPos, &pIn->_TargetPos, __FILE__, __LINE__ );

	_nNetSlotIdx	= pIn->Id;
	_nId			= pIn->Id;

#if defined( CHECK_LOADEDBULLET)
	_LoadedBullet	= pIn->_LoadedBullet;
	_LoadedDualBullet = pIn->_LoadedDualBullet;
#endif
	
	// Fire 패킷에 들어있는 무기 정보를 세팅합니다. - 서동권
	_WeaponInfo.Unpack( (const char*) &pIn->_WeaponInfo);

	return sizeof( N_PCINFO_BULLET);
}

UINT16 GAMEINFO_FIRE::Pack_TargetPos( char * pOutPacket)
{
	N_PCINFO_TARGETPOS * pOut = (N_PCINFO_TARGETPOS*) pOutPacket;

	pOut->r16x = _TargetX;
	pOut->r16z = _TargetZ;

	return sizeof( N_PCINFO_TARGETPOS);
}

UINT16 GAMEINFO_FIRE::Unpack_TargetPos( const char * pInPacket)
{
	N_PCINFO_TARGETPOS* pIn = (N_PCINFO_TARGETPOS*) pInPacket;

	_TargetX = (REAL32) pIn->r16x;
	_TargetZ = (REAL32) pIn->r16z;

	return sizeof(N_PCINFO_TARGETPOS);
}

UINT16 GAMEINFO_FIRE::Pack_Dino( char * pOutPacket)
{
	N_PCINFO_DINO_BULLET* pOut = (N_PCINFO_DINO_BULLET*) pOutPacket;

	UDP_UTIL::ConvertVec3DtoNet3D( &pOut->_TargetPos, &_vTargetPos);

	return sizeof(N_PCINFO_DINO_BULLET);
}

UINT16 GAMEINFO_FIRE::Unpack_Dino( const char * pInPacket)
{
	N_PCINFO_DINO_BULLET* pIn = (N_PCINFO_DINO_BULLET*) pInPacket;

	UDP_UTIL::ConvertNet3DtoVec3D( &_vTargetPos, &pIn->_TargetPos, __FILE__, __LINE__ );

	return sizeof(N_PCINFO_DINO_BULLET);
}

void GAMEINFO_SUFFER::Reset()
{
	_nShooter	= 0;
	_nShooterType = 0;
	_nHitpart	= 0;
	_nDeathType	= 0;
	i3Vector::Zero( &_vHitDir );
	_bDeath		= FALSE;
	_nNeedDamageMotion = 0;
	_WeaponItemID = 0;
}

void GAMEINFO_SUFFER::CopyTo( GameNetworkInfoBase * pDest )
{
	GAMEINFO_SUFFER * pInfo = (GAMEINFO_SUFFER*) pDest;

	pInfo->_nShooter	= _nShooter;
	pInfo->_nHitpart	= _nHitpart;
	pInfo->_nDeathType	= _nDeathType;
	i3Vector::Copy( &pInfo->_vHitDir, &_vHitDir );
	pInfo->_bDeath		= _bDeath;
	pInfo->_nNeedDamageMotion		= _nNeedDamageMotion;
	pInfo->_WeaponItemID = _WeaponItemID;
}

UINT16 GAMEINFO_SUFFER::Pack_Death( char * pOutPacket)
{
	N_PCINFO_DEATH * pOut = (N_PCINFO_DEATH*) pOutPacket;

	I3_BOUNDCHK( _nShooter, SLOT_MAX_COUNT);
	I3_BOUNDCHK( _nShooter, 16);
	I3_BOUNDCHK( _nHitpart, CHARA_HIT_MAXCOUNT);

	if (_nDeathType >= CHARA_DEATH_MAXCOUNT)
	{
		I3ASSERT(_nDeathType < CHARA_DEATH_MAXCOUNT);
		return UINT16(-1);
	}

#if CHARA_DEATH_MAXCOUNT != 16
	if (_nDeathType >= 16)
	{
		I3ASSERT(_nDeathType < 16);
		return UINT16(-1);
	}
#endif	
	
	pOut->DeathInfo		= _nDeathType;
	pOut->DeathInfo		|= (_nShooter << 4);
	pOut->HitPart		= _nHitpart;

	UDP_UTIL::ConvertVec3DtoNet3D( &pOut->_HitDir, &_vHitDir );

	return sizeof( N_PCINFO_DEATH);
}

UINT16 GAMEINFO_SUFFER::Unpack_Death( const char * pInPacket)
{
	N_PCINFO_DEATH * pIn = (N_PCINFO_DEATH*) pInPacket;

	UINT8	nDeathType	= (pIn->DeathInfo & 0x0F);

	if( nDeathType >= CHARA_DEATH_MAXCOUNT )
	{
		UDPERROR( UDP_ERROR_DEATH, nDeathType, "Death Type invalid");
	}
	if( pIn->HitPart >= CHARA_HIT_MAXCOUNT )
	{
		UDPERROR( UDP_ERROR_DEATH, pIn->HitPart, "hit part invalid");
	}

	_nDeathType		= nDeathType;
	_nShooter		= (pIn->DeathInfo >> 4) & 0x0F;
	_nHitpart		= pIn->HitPart;

	UDP_UTIL::ConvertNet3DtoVec3D( &_vHitDir, &pIn->_HitDir, __FILE__, __LINE__ );
	
	return sizeof( N_PCINFO_DEATH);
}

UINT16 GAMEINFO_SUFFER::Pack_Suffer( char * pOutPacket)
{
	N_PCINFO_SUFFER * pOut = (N_PCINFO_SUFFER*) pOutPacket;

	pOut->Reset();
	pOut->SetAttackerIndex( _nShooter);
	pOut->SetDeathType( _nDeathType);
	pOut->SetHitPart( _nHitpart);
	pOut->SetNeedDamageMotion( _nNeedDamageMotion);

	return sizeof( N_PCINFO_SUFFER);
}

UINT16 GAMEINFO_SUFFER::Unpack_Suffer( const char * pInPacket)
{
	N_PCINFO_SUFFER * pIn = (N_PCINFO_SUFFER*) pInPacket;

	UINT8 shooter = pIn->GetAttackerIndex();
	if( shooter >= SLOT_MAX_COUNT )
	{
		UDPERROR( UDP_ERROR_SUFFER, shooter, "invalid shooter index");
	}

	UINT8 deathType = pIn->GetDeathType();
	if( deathType >= CHARA_DEATH_MAXCOUNT)
	{
		UDPERROR( UDP_ERROR_SUFFER, deathType, "invalid death type");
	}

	UINT8 HitPart = pIn->GetHitPart();
	if( HitPart >= CHARA_HIT_MAXCOUNT )		// HitPart == CHARA_HIT_UNKNOWN ||
	{
		UDPERROR( UDP_ERROR_SUFFER, HitPart, "invalid hit part");
	}
	UINT8 NeedDamageMotion = pIn->GetNeedDamageMotion();
	if( (NeedDamageMotion == 0 || NeedDamageMotion == 1) == false )
	{
		UDPERROR( UDP_ERROR_SUFFER, NeedDamageMotion, "invalid NeedDamageMotion");
	}

	_nDeathType  = deathType;
	_nShooter	= shooter;
	_nHitpart	= HitPart;
	_nNeedDamageMotion	= NeedDamageMotion;

	return sizeof( N_PCINFO_SUFFER);
}

UINT16 GAMEINFO_SUFFER::Unpack_Death_ForDomination( const char * pInPacket)
{
	N_PCINFO_DEATH_FOR_DOMI * pIn = (N_PCINFO_DEATH_FOR_DOMI*) pInPacket;

	_nDeathType	= pIn->m_ui8DeathType;
	_nHitpart	= pIn->m_ui8HitPart;
	_nShooterType = pIn->m_i8ShooterType; // To. 동욱님: 이건 알아서 쓰세요.
	_nShooter	= pIn->m_i8ShooterIdx;

	UDP_UTIL::ConvertNet3DtoVec3D( &_vHitDir, &pIn->m_HitDir, __FILE__, __LINE__ );

	return sizeof( N_PCINFO_DEATH_FOR_DOMI);
}

void GAMEINFO_SUICIDEDAMAGE::Reset()
{
	_Count = 0;

	_tagSUICIDEDAMAGE* pDamage;
	for( INT32 i = 0; i < MAX_SUICIDE_COUNT; i++)
	{
		pDamage = &_tSuicideDamage[ i ];

		pDamage->_DeathType		= CHARA_DEATH_UN;
		pDamage->_HitPart		= CHARA_HIT_UNKNOWN;
		pDamage->_nDamage		= 0;
		i3Vector::Zero( &pDamage->_vHitDir );
		pDamage->_nObjectIdx	= 0xFFFF;
		pDamage->_WeaponInfo.Reset();
		pDamage->_KillerIdx		= 0;
	}
}

void GAMEINFO_SUICIDEDAMAGE::CopyTo( GameNetworkInfoBase * pDest)
{
	GAMEINFO_SUICIDEDAMAGE * pInfo = (GAMEINFO_SUICIDEDAMAGE*) pDest;
	I3ASSERT( _Count <= MAX_SUICIDE_COUNT);

	pInfo->_Count		= _Count;
	
	_tagSUICIDEDAMAGE* pSrcDamage, *pDesDamage;
	for( INT32 i = 0; i < _Count; i++)
	{
		pSrcDamage = &_tSuicideDamage[ i ];
		pDesDamage = &pInfo->_tSuicideDamage[ i ];

		pDesDamage->_DeathType		= pSrcDamage->_DeathType;
		pDesDamage->_HitPart		= pSrcDamage->_HitPart;
		pDesDamage->_nDamage		= pSrcDamage->_nDamage;
		i3Vector::Copy( &pDesDamage->_vHitDir, &pSrcDamage->_vHitDir);
		pDesDamage->_nObjectIdx		= pSrcDamage->_nObjectIdx;
		pSrcDamage->_WeaponInfo.CopyTo( &pDesDamage->_WeaponInfo);
		pDesDamage->_KillerIdx		= pSrcDamage->_KillerIdx;
	}
}

UINT16 GAMEINFO_SUICIDEDAMAGE::Pack( char * pOutPacket, UINT32 ui32BufSize )
{
	UINT16 ui16WriteSize = 0;
	UINT16 ui16Size;

	if( sizeof(UINT8) > ui32BufSize )	return 0;

	*((UINT8*)pOutPacket) = (UINT8) _Count;
	ui16WriteSize += sizeof(UINT8);
	
	I3ASSERT( _Count <= MAX_SUICIDE_COUNT);

	for( INT32 i = 0; i < _Count; i++)
	{
		ui16Size = Pack_Single( &pOutPacket[ ui16WriteSize ], ui32BufSize-ui16WriteSize, i );
		if( 0 == ui16Size )	return 0;
		ui16WriteSize += ui16Size;
	}

	return ui16WriteSize;
}

UINT16 GAMEINFO_SUICIDEDAMAGE::Pack_Single( char * pOutPacket, UINT32 ui32BufSize, INT32 idx )
{
	if( sizeof(N_PCINFO_SUICIDEDMG) > ui32BufSize )	return 0;

	N_PCINFO_SUICIDEDMG* pOut = (N_PCINFO_SUICIDEDMG*)pOutPacket;

	_tagSUICIDEDAMAGE* pDamage = &_tSuicideDamage[ idx ];

	pOut->Data = 0;
	pOut->_WeaponInfo.Reset();
	pOut->_HitDir.Reset();

	//I3ASSERT( pDamage->_DeathType < 16 );
	pOut->Data |= pDamage->_DeathType;

	//I3ASSERT( pDamage->_HitPart < 64 );
	pOut->Data |= (pDamage->_HitPart << 4);

	//I3ASSERT( pDamage->_nDamage < 4096 );
	pOut->Data |= (pDamage->_nDamage << 20);

	UDP_UTIL::ConvertVec3DtoNet3D( &pOut->_HitDir, &pDamage->_vHitDir );

	if( pDamage->_nObjectIdx != 0xFFFF)
	{
		pOut->_WeaponInfo.SetAttackType( 1 );
		//I3ASSERT( pDamage->_nObjectIdx < 512 );
		pOut->Data |= (pDamage->_nObjectIdx << 11);
	}
	else
	{
		pOut->_WeaponInfo.SetAttackType( 0 );
		//I3ASSERT( pDamage->_KillerIdx < 512 );
		pOut->Data |= (pDamage->_KillerIdx << 11);
	}

	pDamage->_WeaponInfo.Pack( (char*) &pOut->_WeaponInfo, ui32BufSize );

	return sizeof( N_PCINFO_SUICIDEDMG);
}

UINT16 GAMEINFO_SUICIDEDAMAGE::Unpack( const char * pInPacket)
{
	UINT16 ui16WriteSize = 0;
	UINT16 ui16Size;

	_Count = *((UINT8*) pInPacket);
	ui16WriteSize += sizeof(UINT8);

	//I3ASSERT( _Count <= MAX_SUICIDE_COUNT );

	for( UINT8 i = 0; i < _Count; i++)
	{
		ui16Size = UnPack_Single( &pInPacket[ ui16WriteSize ], i );
		if( 0 == ui16Size )	return 0;
		ui16WriteSize += ui16Size;
	}

	return ui16WriteSize;
}

UINT16 GAMEINFO_SUICIDEDAMAGE::UnPack_Single( const char * pInPacket, INT32 idx )
{
	_tagSUICIDEDAMAGE* pDamage = &_tSuicideDamage[ idx ];

	N_PCINFO_SUICIDEDMG * pIn = (N_PCINFO_SUICIDEDMG*)pInPacket;

	pDamage->_DeathType		= (CHARA_DEATH_TYPE)(pIn->Data & 0x0F);
	pDamage->_HitPart		= (CHARA_HIT_PART)((pIn->Data >> 4) & 0x3F);
	pDamage->_nDamage		= (UINT16)((pIn->Data >> 20) & 0xFFF);;
	UDP_UTIL::ConvertNet3DtoVec3D( &pDamage->_vHitDir, &pIn->_HitDir, __FILE__, __LINE__ );
	
	UINT16 ui16AttackerIdx = ((pIn->Data >> 11) & 0x1FF);

	if( ((pIn->Data >> 10) & 0x01) == 0 )
	{
		pDamage->_KillerIdx		= ui16AttackerIdx;
		pDamage->_nObjectIdx	= 0xFFFF;
	}
	else
	{
		pDamage->_KillerIdx		= 0;
		pDamage->_nObjectIdx	= ui16AttackerIdx;
	}
		
	pDamage->_WeaponInfo.setWeaponID( pIn->_WeaponInfo.GetWeaponID() );

	I3ASSERT( pDamage->_WeaponInfo.getWeaponClass() != 0 );
		
	if( pDamage->_DeathType >= CHARA_DEATH_MAXCOUNT)
	{
		UDPERROR( UDP_ERROR_SUICIDE, pDamage->_DeathType, "invalid death type");
	}

	if( pDamage->_HitPart >= CHARA_HIT_MAXCOUNT)
	{
		UDPERROR( UDP_ERROR_SUICIDE, pDamage->_HitPart, "invalid hit part");
	}

	if( pDamage->_WeaponInfo.getWeaponClass() >= WEAPON_CLASS_COUNT)
	{
		UDPERROR( UDP_ERROR_SUICIDE, pDamage->_WeaponInfo.getWeaponClass(), "invalid Weapon class");
	}

	if( pDamage->_WeaponInfo.getWeaponUsage() >= WEAPON_SLOT_COUNT)
	{
		UDPERROR( UDP_ERROR_SUICIDE, pDamage->_WeaponInfo.getWeaponUsage(), "invalid Weapon Usage");
	}

	return sizeof(N_PCINFO_SUICIDEDMG);
}

void GAMEINFO_MISSION::Reset()
{
	_eState			= NET_MISSION_NONE;
	_eArea			= BOMB_AREA_A;
	_r32StartTime	= 0.f;

	_ItemID			= 0;
	_ObjType		= 0;
	_ObjIdx			= 0;
}

void GAMEINFO_MISSION::CopyTo( GameNetworkInfoBase * pDest )
{
	GAMEINFO_MISSION * pInfo = (GAMEINFO_MISSION*) pDest;

	pInfo->_eState			= _eState;
	pInfo->_eArea			= _eArea;
	pInfo->_r32StartTime	= GetStartTime();

	pInfo->_ItemID			= _ItemID;
	pInfo->_ObjType			= _ObjType;
	pInfo->_ObjIdx			= _ObjIdx;
}

UINT16 GAMEINFO_MISSION::Pack( char * pOutPacket, UINT32 ui32BufSize )
{
	N_PCINFO_MISSION * pOut = (N_PCINFO_MISSION*) pOutPacket;

	I3_BOUNDCHK( _eState, NET_MISSION_STATE_MAXCOUNT );
	I3_BOUNDCHK( _eArea, MAX_BOMB_AREA_COUNT );

	pOut->State				= static_cast<UINT8>(_eState | (_eArea << 4));
	pOut->r32StartTime		= GetStartTime();

	return sizeof( N_PCINFO_MISSION);
}

UINT16 GAMEINFO_MISSION::Unpack( const char * pInPacket)
{
	N_PCINFO_MISSION * pIn = (N_PCINFO_MISSION*) pInPacket;

	_eState			= (NET_MISSION_STATE)(pIn->State & 0x0F);
	_eArea			= (BOMB_AREA) ((pIn->State >> 4) & 0x0F);
	_r32StartTime	= pIn->r32StartTime;

	if( _eState >= NET_MISSION_STATE_MAXCOUNT )
	{
		UDPERROR( UDP_ERROR_MISSION, _eState, "invalid State");
	}
	if( _eArea > MAX_BOMB_AREA_COUNT)
	{
		UDPERROR( UDP_ERROR_MISSION, _eArea, "invalid Area");
	}

	return sizeof( N_PCINFO_MISSION);
}

void GAMEINFO_BULLET::Reset()
{
	i3mem::FillZero( m_ui8LoadBullet,		sizeof(m_ui8LoadBullet) );
	i3mem::FillZero( m_ui16StandbyBullet,	sizeof(m_ui16StandbyBullet) );
}

void GAMEINFO_BULLET::CopyTo( GAMEINFO_BULLET* pDest )
{
	i3mem::Copy( pDest->m_ui8LoadBullet,		m_ui8LoadBullet,		sizeof(m_ui8LoadBullet) );
	i3mem::Copy( pDest->m_ui16StandbyBullet,	m_ui16StandbyBullet,	sizeof(m_ui16StandbyBullet) );
}

void GAMEINFO_BULLET::SetBullet( UINT8 (*ppui8LoadBullet)[WEAPON::HAND_TYPE_MAX], UINT16 (*ppui16StandbyBullet)[WEAPON::HAND_TYPE_MAX] )
{
	i3mem::Copy( m_ui8LoadBullet,		ppui8LoadBullet,		sizeof(m_ui8LoadBullet) );
	i3mem::Copy( m_ui16StandbyBullet,	ppui16StandbyBullet,	sizeof(m_ui16StandbyBullet) );
}

void GAMEINFO_BULLET::GetBullet( UINT8 (*ppui8LoadBullet)[WEAPON::HAND_TYPE_MAX], UINT16 (*ppui16StandbyBullet)[WEAPON::HAND_TYPE_MAX] )
{
	i3mem::Copy( ppui8LoadBullet,		m_ui8LoadBullet,		sizeof(m_ui8LoadBullet) );
	i3mem::Copy( ppui16StandbyBullet,	m_ui16StandbyBullet,	sizeof(m_ui16StandbyBullet) );
}

void GAMEINFO_DROPWEAPON::Reset()
{
	_ui8Success			= 0;
	_ui8DropType		= 0;
	_nIdx				= 0;
	m_ui8MultiWeaponIdx	= 0;
	_ui32WeaponID		= 0;
	m_Bullet.Reset();
}

void GAMEINFO_DROPWEAPON::CopyTo( GameNetworkInfoBase * pDest)
{
	GAMEINFO_DROPWEAPON * pInfo = (GAMEINFO_DROPWEAPON*) pDest;

	pInfo->_ui8Success			= _ui8Success;
	pInfo->_ui8DropType			= _ui8DropType;
	pInfo->_nIdx				= _nIdx;
	pInfo->m_ui8MultiWeaponIdx	= m_ui8MultiWeaponIdx;
	pInfo->_ui32WeaponID		= _ui32WeaponID;
	m_Bullet.CopyTo( &pInfo->m_Bullet );
}

UINT16 GAMEINFO_DROPWEAPON::Pack( char * pOutPacket, UINT32 ui32BufSize )
{
	I3_BOUNDCHK( (INT8)_nIdx, MAX_DROPEDWEAPON_COUNT );
		
	N_PCINFO_DROPWEAPON * pOut = (N_PCINFO_DROPWEAPON*) pOutPacket;
	
	pOut->State					= (_ui8Success << 7);					// 1bit success flag
	pOut->State					|= ((_ui8DropType & 0x01) << 6);		// 1bit death flag
	//pOut->State					|= ((getWeaponUsage() & 0x01) << 5);	// 1bit slot
	pOut->State					|= (_nIdx & 0x1F);						// 5bit idx
	pOut->m_ui32WeaponID		= _ui32WeaponID;
	pOut->m_ui8MultiWeaponIdx	= m_ui8MultiWeaponIdx;
	m_Bullet.GetBullet( pOut->m_ui8LoadBullet, pOut->m_ui16StandbyBullet );

	//pOut->m_ui8LoadedBullet		= _nLoadedBullet;
	//pOut->m_ui8DualLoadedBullet	= _nDualLoadedBullet;
	//pOut->m_ui16MaxBullet		= _nMaxBullet;

	return sizeof( N_PCINFO_DROPWEAPON);
}

UINT16 GAMEINFO_DROPWEAPON::Unpack( const char * pInPacket)
{
	N_PCINFO_DROPWEAPON * pInfo = (N_PCINFO_DROPWEAPON*) pInPacket;

	I3ASSERT( pInfo->m_ui32WeaponID != 0 );
	
	UINT8 ui8State = pInfo->State;

	_ui8Success			= (ui8State >> 7) & 0x01;// 1bit
	_ui8DropType		= (ui8State >> 6) & 0x01;// 1bit
	_nIdx				= (ui8State & 0x1F);	 // 5bit
	_ui32WeaponID		= pInfo->m_ui32WeaponID;
	m_ui8MultiWeaponIdx = pInfo->m_ui8MultiWeaponIdx;
	m_Bullet.SetBullet( pInfo->m_ui8LoadBullet, pInfo->m_ui16StandbyBullet );

	//_nLoadedBullet		= pInfo->m_ui8LoadedBullet;
	//_nDualLoadedBullet	= pInfo->m_ui8DualLoadedBullet;
	//_nMaxBullet			= pInfo->m_ui16MaxBullet;

	return sizeof( N_PCINFO_DROPWEAPON);
}

void GAMEINFO_DIRECTPICKUP::Reset()
{
	m_DropWeapon.Reset();

	_ui32WeaponID = 0;
}

void GAMEINFO_DIRECTPICKUP::CopyTo(GameNetworkInfoBase * pDest)
{
	GAMEINFO_DIRECTPICKUP * pInfo = (GAMEINFO_DIRECTPICKUP*)pDest;

	pInfo->_ui32WeaponID = _ui32WeaponID;

	m_DropWeapon.CopyTo( &pInfo->m_DropWeapon);
}

UINT16 GAMEINFO_DIRECTPICKUP::Pack(char * pOutPacket, UINT32 ui32BufSize)
{
	N_PCINFO_DIRECTPICKUP * pOut = (N_PCINFO_DIRECTPICKUP*)pOutPacket;
	
	I3_BOUNDCHK((INT8)m_DropWeapon.getIndex(), MAX_DROPEDWEAPON_COUNT);

	pOut->_DropWeapon.State				= (m_DropWeapon.getSuccess() << 7);					// 1bit success flag
	pOut->_DropWeapon.State				|= ((m_DropWeapon.getDropType() & 0x01) << 6);		// 1bit death flag
	//pOut->State						|= ((getWeaponUsage() & 0x01) << 5);	// 1bit slot
	pOut->_DropWeapon.State				|= (m_DropWeapon.getIndex() & 0x1F);						// 5bit idx
	pOut->_DropWeapon.m_ui32WeaponID	= m_DropWeapon.getWeaponID();
	pOut->_DropWeapon.m_ui8MultiWeaponIdx = m_DropWeapon.GetMultiWeaponSlot();

	m_DropWeapon.GetBullet()->GetBullet(pOut->_DropWeapon.m_ui8LoadBullet, pOut->_DropWeapon.m_ui16StandbyBullet);
	
	pOut->m_ui32WeaponID = _ui32WeaponID;

	return sizeof(N_PCINFO_DIRECTPICKUP);
}

UINT16 GAMEINFO_DIRECTPICKUP::Unpack(const char * pInPacket)
{
	N_PCINFO_DIRECTPICKUP * pInfo = (N_PCINFO_DIRECTPICKUP*)pInPacket;

	I3ASSERT(pInfo->m_ui32WeaponID != 0);

	UINT8 ui8State = pInfo->_DropWeapon.State;

	m_DropWeapon.setSuccess((ui8State >> 7) & 0x01);	// 1bit
	m_DropWeapon.setDropType((ui8State >> 6) & 0x01);	// 1bit
	m_DropWeapon.setIndex((ui8State & 0x1F));			// 5bit
	m_DropWeapon.setWeaponID(pInfo->_DropWeapon.m_ui32WeaponID);
	m_DropWeapon.SetMultiWeaponSlot(pInfo->_DropWeapon.m_ui8MultiWeaponIdx);

	m_DropWeapon.GetBullet()->SetBullet(pInfo->_DropWeapon.m_ui8LoadBullet, pInfo->_DropWeapon.m_ui16StandbyBullet);

	_ui32WeaponID = pInfo->m_ui32WeaponID;

	return sizeof(N_PCINFO_DIRECTPICKUP);
}

void GAMEINFO_TOUCHDOWN::Reset()
{
	_ui16TouchDownIdx		= 0;
}

void GAMEINFO_TOUCHDOWN::CopyTo( GameNetworkInfoBase * pDest)
{
	GAMEINFO_TOUCHDOWN * pInfo = (GAMEINFO_TOUCHDOWN*)pDest;

	pInfo->_ui16TouchDownIdx	= _ui16TouchDownIdx;
}

UINT16 GAMEINFO_TOUCHDOWN::Pack( char * pOutPacket, UINT32 ui32BufSize )
{
	UINT16* pOut = (UINT16*)pOutPacket;
	
	*pOut = _ui16TouchDownIdx;

	return sizeof(UINT16);
}

UINT16 GAMEINFO_TOUCHDOWN::Unpack( const char * pInPacket)
{
	UINT16* pInfo = (UINT16*) pInPacket;

	_ui16TouchDownIdx	= *pInfo;

	return sizeof(UINT16);
}

void GAMEINFO_DOMI_DINO_GRENADE::Reset()
{
	m_type			= INVALID_IDX;
	m_calculatorIdx	= INVALID_IDX;
	m_slot			= INVALID_IDX;
	i3Vector::Zero(&m_targetPos);
}

void GAMEINFO_DOMI_DINO_GRENADE::CopyTo( GameNetworkInfoBase * pDest )
{
	GAMEINFO_DOMI_DINO_GRENADE * pInfo = (GAMEINFO_DOMI_DINO_GRENADE*) pDest;
	pInfo->m_type = m_type;
	pInfo->m_calculatorIdx = m_calculatorIdx;
	pInfo->m_slot = m_slot;
	i3Vector::Copy( &pInfo->m_targetPos, &m_targetPos);
}

UINT16 GAMEINFO_DOMI_DINO_GRENADE::Pack( char * pOutPacket, UINT32 ui32BufSize )
{
	N_PCINFO_DINO_GRENADE* pOut = (N_PCINFO_DINO_GRENADE*) pOutPacket;

	pOut->m_i8Idx = m_type;
	pOut->m_i8Operation = m_calculatorIdx;
	UDP_UTIL::ConvertVec3DtoNet3D( &pOut->m_TargetPos, &m_targetPos);

	return sizeof(N_PCINFO_DINO_GRENADE);
}

UINT16 GAMEINFO_DOMI_DINO_GRENADE::Unpack( const char * pInPacket)
{
	N_PCINFO_DINO_GRENADE* pInfo = (N_PCINFO_DINO_GRENADE*) pInPacket;

	m_type = pInfo->m_i8Idx;
	m_calculatorIdx = pInfo->m_i8Operation;
	UDP_UTIL::ConvertNet3DtoVec3D( &m_targetPos, &pInfo->m_TargetPos, __FILE__, __LINE__ );

	return sizeof(N_PCINFO_DINO_GRENADE);
}


void GAMEINFO_THROW_WEAPON_NETDATA::Reset( void)
{
	VEC3D vTemp;
	
	SetState(0);
	SetBoundType(0);
#if defined( CLIENT_SIDE)
	SetPos(vTemp);
	SetDir(vTemp);
	SetAttachPos(vTemp);
#else
	SetPos(&vTemp);
	SetDir(&vTemp);
	SetAttachPos(&vTemp);
#endif

	_FlyCount = 0;
	_nRemainingTime = 0;
	i3Vector::Zero(&_vPrevPos);
	_WeaponInfo.Reset();
}

void GAMEINFO_THROW_WEAPON_NETDATA::CopyTo( GameNetworkInfoBase * pDest)
{
	GAMEINFO_THROW_WEAPON_NETDATA * pInfo = (GAMEINFO_THROW_WEAPON_NETDATA*) pDest;
	pInfo->SetState(GetState());
	pInfo->SetBoundType(GetBoundType());
	pInfo->SetPos(GetPos());
	pInfo->SetDir(GetDir());
	pInfo->SetAttachPos(GetAttachPos());

	pInfo->_nRemainingTime = _nRemainingTime;
	pInfo->_FlyCount = _FlyCount;

	_WeaponInfo.CopyTo( &pInfo->_WeaponInfo);
}

UINT16 GAMEINFO_THROW_WEAPON_NETDATA::Pack( char * pOutPacket, UINT32 ui32BufSize )
{
	if( sizeof(N_WEAPONINFO_THROW) > ui32BufSize )	return 0;

	N_WEAPONINFO_THROW * pOut = (N_WEAPONINFO_THROW*) pOutPacket;

	pOut->Reset();

	_WeaponInfo.Pack( (char*) &pOut->_tWeapon, ui32BufSize );

	pOut->State				= GetState();
	pOut->BoundType			= GetBoundType();

#if defined( CLIENT_SIDE)
	VEC3D vPos = GetPos();
	VEC3D vDir = GetDir();
	VEC3D vAttachedObjPos = GetAttachPos();

	UDP_UTIL::ConvertVec3DtoNet3D( &pOut->_Pos, &vPos );
	UDP_UTIL::ConvertVec3DtoNet3D( &pOut->_Dir, &vDir );
	UDP_UTIL::ConvertVec3DtoNet3D(&pOut->_AttachedObjectPos, &vAttachedObjPos);
#else
	VEC3D* vPos = GetPos();
	VEC3D* vDir = GetDir();
	VEC3D* vAttachedObjPos = GetAttachPos();

	UDP_UTIL::ConvertVec3DtoNet3D( &pOut->_Pos, vPos );
	UDP_UTIL::ConvertVec3DtoNet3D( &pOut->_Dir, vDir );
	UDP_UTIL::ConvertVec3DtoNet3D(&pOut->_AttachedObjectPos, vAttachedObjPos);
#endif

	pOut->_FlyCount			= _FlyCount;
	pOut->_RemainingTime	= _nRemainingTime;
	UDPTRACE("pOut->_FlyCount:%d, pos:%f:%f:%f\n", pOut->_FlyCount, pOut->_Pos.x, pOut->_Pos.y, pOut->_Pos.z );

	return sizeof( N_WEAPONINFO_THROW);
}

UINT16 GAMEINFO_THROW_WEAPON_NETDATA::Unpack( const char * pInPacket)
{
	N_WEAPONINFO_THROW * pInfo	= (N_WEAPONINFO_THROW *) pInPacket;

#if defined( CLIENT_SIDE)
	if( pInfo->BoundType >= I3_TERRAIN_TYPE_COUNT)
	{
		UDPERROR( UDP_ERROR_GRENADE, pInfo->BoundType, "Bound type invalid");
	}
#endif

	SetState(pInfo->State);
	SetBoundType(pInfo->BoundType);
	_FlyCount = pInfo->_FlyCount;
	_nRemainingTime = pInfo->_RemainingTime;
	UDPTRACE("받은 시간 %d\n", pInfo->_RemainingTime);

	VEC3D vPos;
	VEC3D vDir;
	VEC3D vAttachObjPos;
	UDP_UTIL::ConvertNet3DtoVec3D( &vPos, &pInfo->_Pos, __FILE__, __LINE__ );
	UDP_UTIL::ConvertNet3DtoVec3D( &vDir, &pInfo->_Dir, __FILE__, __LINE__ );
	UDP_UTIL::ConvertNet3DtoVec3D(&vAttachObjPos, &pInfo->_AttachedObjectPos, __FILE__, __LINE__);
#if defined( CLIENT_SIDE)
	SetPos(vPos);
	SetDir(vDir);
	SetAttachPos(vAttachObjPos);
#else
	SetPos(&vPos);
	SetDir(&vDir);
	SetAttachPos(&vAttachObjPos);
#endif

	_WeaponInfo.Unpack( (const char*) &pInfo->_tWeapon);
	
	return sizeof( N_WEAPONINFO_THROW );
}

void GAMEINFO_OBJECT_NETDATA::Reset()
{
	_nState			= 0;
	_nHP			= 0;
	_fKey			= 0.f;
	_nPathKey		= 0;
	i3mem::FillZero( &_vPos, sizeof(N_POS) );
	i3Quat::Identity( &_qRot);
	_nShellCount	= 0;
	_EnterCharaIdx	= 0xFF;
	_EventSender	= 0xFF;
	_ObjType = 0;
}

void GAMEINFO_OBJECT_NETDATA::CopyTo( GameNetworkInfoBase * pDest)
{
	GAMEINFO_OBJECT_NETDATA * pInfo = (GAMEINFO_OBJECT_NETDATA*) pDest;

	pInfo->_ObjType |= _ObjType;

	if( _ObjType & UDP_OBJECT_TYPE_STATE)
	{
		pInfo->_nState = _nState;
	}

	if( _ObjType & UDP_OBJECT_TYPE_HP )
	{
		pInfo->_nHP = _nHP;
	}

	if( _ObjType & UDP_OBJECT_TYPE_SENDER)
	{
		pInfo->_EnterCharaIdx = _EnterCharaIdx;
	}

	if( _ObjType  & UDP_OBJECT_TYPE_OWN)
	{
		pInfo->_EnterCharaIdx = _EnterCharaIdx;
	}

	if( _ObjType & UDP_OBJECT_TYPE_POSITION)
	{
		i3mem::Copy( &pInfo->_vPos, &_vPos, sizeof(N_POS));
	}

	if( _ObjType & UDP_OBJECT_TYPE_ROTATION)
	{
		i3Quat::Copy( &pInfo->_qRot, &_qRot);
	}

	if( _ObjType & UDP_OBJECT_TYPE_ANIMPATH)
	{
		pInfo->_nPathKey = _nPathKey;
	}

	if( _ObjType & UDP_OBJECT_TYPE_ANIMKEY)
	{
		pInfo->_fKey = _fKey;
	}

	if( _ObjType & UDP_OBJECT_TYPE_SHELLCOUNT)
	{
		pInfo->_nShellCount = _nShellCount;
		//pInfo->_nMaxShellCount = _nMaxShellCount;
	}

	if( _ObjType & UDP_OBJECT_TYPE_RESPAWN)
	{
		pInfo->_fRespawnTime = _fRespawnTime;
	}	
}

UINT16 GAMEINFO_OBJECT_NETDATA::Pack( char * pOutPacket, UINT32 ui32BufSize )
{
	return 0;
}

UINT32 GAMEINFO_OBJECT_NETDATA::PackPacket( char * pOutPacket, UINT32 ui32BufSize )
{
	UINT32 ui32WriteSize = 0;

	if( sizeof(T_OBJ) > ui32BufSize )											return 0;

	T_OBJ * pObjType = (T_OBJ*) &pOutPacket[ ui32WriteSize ];
	*pObjType = _ObjType;
	ui32WriteSize += sizeof(T_OBJ);

	if( _ObjType & UDP_OBJECT_TYPE_STATE) 
	{
		if( ui32BufSize < ui32WriteSize + sizeof(N_OBJINFO_STATE) )				return 0;

		N_OBJINFO_STATE * pOut = (N_OBJINFO_STATE*) &pOutPacket[ui32WriteSize];
		pOut->_nState = _nState;
		ui32WriteSize += sizeof( N_OBJINFO_STATE);
	}

	if( _ObjType & UDP_OBJECT_TYPE_HP )
	{
		if( ui32BufSize < ui32WriteSize + sizeof(N_OBJINFO_HP) )				return 0;

		N_OBJINFO_HP * pOut = (N_OBJINFO_HP*) &pOutPacket[ui32WriteSize];
		pOut->Set( _nHP);
		ui32WriteSize += sizeof( N_OBJINFO_HP);
	}

	if( _ObjType & UDP_OBJECT_TYPE_SENDER)
	{
		if( ui32BufSize < ui32WriteSize + sizeof(N_OBJINFO_EVENTSENDER) )		return 0;

		N_OBJINFO_EVENTSENDER * pOut = (N_OBJINFO_EVENTSENDER*) &pOutPacket[ui32WriteSize];
		I3ASSERT( (0xFF == _EventSender) || (_EventSender < SLOT_MAX_COUNT ) );
		pOut->Set( _EventSender);
		ui32WriteSize += sizeof( N_OBJINFO_EVENTSENDER);
	}

	if( _ObjType  & UDP_OBJECT_TYPE_OWN)
	{
		if( ui32BufSize < ui32WriteSize + sizeof(N_OBJINFO_OWNER_INDEX) )		return 0;

		N_OBJINFO_OWNER_INDEX * pOut = (N_OBJINFO_OWNER_INDEX*) &pOutPacket[ui32WriteSize];
		pOut->_nIndex = _EnterCharaIdx;
		ui32WriteSize += sizeof( N_OBJINFO_OWNER_INDEX);
	}

	if( _ObjType & UDP_OBJECT_TYPE_POSITION)
	{
		if( ui32BufSize < ui32WriteSize + sizeof(N_POS) )						return 0;

		N_POS * pOut = (N_POS*) &pOutPacket[ui32WriteSize];
		memcpy( pOut, &_vPos, sizeof(N_POS) );
		ui32WriteSize += sizeof( N_POS);
	}

	if( _ObjType & UDP_OBJECT_TYPE_ROTATION)
	{
		if( ui32BufSize < ui32WriteSize + sizeof(N_ROT) )						return 0;

		N_ROT * pOut = (N_ROT*) &pOutPacket[ui32WriteSize];
		UDP_UTIL::ConvertQuattoNetRot( pOut, &_qRot, NET_ROTATE_SCALE );
		ui32WriteSize += sizeof( N_ROT);
	}

	if( _ObjType & UDP_OBJECT_TYPE_ANIMPATH)
	{
		if( ui32BufSize < ui32WriteSize + sizeof(N_OBJINFO_ANIMPATH) )			return 0;

		N_OBJINFO_ANIMPATH * pOut = (N_OBJINFO_ANIMPATH*) &pOutPacket[ui32WriteSize];
		pOut->_nPath			= _nPathKey;
		ui32WriteSize += sizeof( N_OBJINFO_ANIMPATH);
	}

	if( _ObjType & UDP_OBJECT_TYPE_ANIMKEY)
	{
		if( ui32BufSize < ui32WriteSize + sizeof(N_OBJINFO_ANIMKEY) )			return 0;

		N_OBJINFO_ANIMKEY * pOut = (N_OBJINFO_ANIMKEY*) &pOutPacket[ui32WriteSize];
		pOut->_KeyFrame			= _fKey;
		ui32WriteSize += sizeof( N_OBJINFO_ANIMKEY);
	}

	if( _ObjType & UDP_OBJECT_TYPE_SHELLCOUNT)
	{
		if( ui32BufSize < ui32WriteSize + sizeof(N_OBJINFO_SHELLCOUNT) )		return 0;

		N_OBJINFO_SHELLCOUNT * pOut = (N_OBJINFO_SHELLCOUNT*) &pOutPacket[ui32WriteSize];
		I3_BOUNDCHK( _nShellCount, 256);
		pOut->_nShellCount = (UINT8)_nShellCount;
		ui32WriteSize += sizeof( N_OBJINFO_SHELLCOUNT);
	}

	if( _ObjType & UDP_OBJECT_TYPE_RESPAWN)
	{
		if( ui32BufSize < ui32WriteSize + sizeof(N_OBJINFO_RESPAWN) )			return 0;

		N_OBJINFO_RESPAWN * pOut = (N_OBJINFO_RESPAWN*) &pOutPacket[ui32WriteSize];
		pOut->_fRespawnTime = _fRespawnTime;
		ui32WriteSize += sizeof( N_OBJINFO_RESPAWN);
	}

	return ui32WriteSize;
}

UINT16 GAMEINFO_OBJECT_NETDATA::Unpack( const char * pInPacket)
{
	UINT16	pos = 0;
	
	_ObjType = *((T_OBJ*) pInPacket);
	pos += sizeof(T_OBJ);

	if( UDP_OBJECT_TYPE_STATE & _ObjType )
	{
		N_OBJINFO_STATE * pIn = (N_OBJINFO_STATE*) &pInPacket[pos];
		_nState = pIn->_nState;
		pos += sizeof(N_OBJINFO_STATE);
	}

	if( UDP_OBJECT_TYPE_HP & _ObjType )
	{
		N_OBJINFO_HP * pIn	= (N_OBJINFO_HP *) &pInPacket[pos];
		_nHP			= pIn->_nHP;
		pos += sizeof( N_OBJINFO_HP );
	}

	if( UDP_OBJECT_TYPE_SENDER & _ObjType )
	{
		N_OBJINFO_EVENTSENDER * pIn = (N_OBJINFO_EVENTSENDER*) &pInPacket[pos];
		_EventSender	= pIn->_nEventSender;
		pos += sizeof( N_OBJINFO_EVENTSENDER);
	}

	if( UDP_OBJECT_TYPE_OWN & _ObjType )
	{
		N_OBJINFO_OWNER_INDEX * pIn = (N_OBJINFO_OWNER_INDEX*) &pInPacket[pos];
		_EnterCharaIdx = pIn->_nIndex;
		pos += sizeof( N_OBJINFO_OWNER_INDEX);
	}

	if( UDP_OBJECT_TYPE_POSITION & _ObjType )
	{
		N_POS * pIn = (N_POS *) &pInPacket[pos];
		i3mem::Copy( &_vPos, pIn, sizeof(N_POS) );
		pos += sizeof( N_POS );
	}

	if( UDP_OBJECT_TYPE_ROTATION & _ObjType)
	{
		N_ROT * pIn = (N_ROT *) &pInPacket[pos];
		UDP_UTIL::ConvertNetRottoQuat( &_qRot, pIn, NET_ROTATE_INVSCALE );
		pos += sizeof( N_ROT );
	}

	if( UDP_OBJECT_TYPE_ANIMPATH & _ObjType )
	{
		N_OBJINFO_ANIMPATH * pIn = (N_OBJINFO_ANIMPATH *) &pInPacket[pos];
		_nPathKey	= pIn->_nPath;
		pos += sizeof( N_OBJINFO_ANIMPATH );
	}

	if( UDP_OBJECT_TYPE_ANIMKEY & _ObjType )
	{
		N_OBJINFO_ANIMKEY * pIn = (N_OBJINFO_ANIMKEY*) &pInPacket[pos];
		_fKey = pIn->_KeyFrame;
		pos += sizeof( N_OBJINFO_ANIMKEY);
	}

	if( UDP_OBJECT_TYPE_SHELLCOUNT & _ObjType )
	{
		N_OBJINFO_SHELLCOUNT * pIn = (N_OBJINFO_SHELLCOUNT*) &pInPacket[pos];
		_nShellCount = pIn->_nShellCount;
		//_nMaxShellCount = pIn->_nMaxShellCount;
		pos += sizeof( N_OBJINFO_SHELLCOUNT);
	}

	if( UDP_OBJECT_TYPE_RESPAWN & _ObjType )
	{
		N_OBJINFO_RESPAWN * pIn = (N_OBJINFO_RESPAWN*) &pInPacket[pos];
		_fRespawnTime = pIn->_fRespawnTime;
		pos += sizeof(N_OBJINFO_RESPAWN);
	}

	return pos;
}

void GAMEINFO_DROPEDWEAPON_NETDATA::Reset()
{
	_ui8IsDrop = 0;
	_ui8State	= 0;
	i3Vector::Zero( &_vPos);
	i3Quat::Identity( &_qRot);
}

void GAMEINFO_DROPEDWEAPON_NETDATA::CopyTo( GameNetworkInfoBase * pDest)
{
	GAMEINFO_DROPEDWEAPON_NETDATA * pInfo = (GAMEINFO_DROPEDWEAPON_NETDATA*) pDest;
	pInfo->_ui8IsDrop = _ui8IsDrop;
	pInfo->_ui8State = _ui8State;
	i3Vector::Copy( &pInfo->_vPos, &_vPos);
	i3Quat::Copy( &pInfo->_qRot, &_qRot);
}

UINT16 GAMEINFO_DROPEDWEAPON_NETDATA::Pack( char * pOutPacket, UINT32 ui32BufSize )
{
	N_DROPEDWEAPONINFO * pOut = (N_DROPEDWEAPONINFO*) pOutPacket;

	pOut->State				= _ui8IsDrop << 4 | (_ui8State & 0x0F);// _nState;
	UDP_UTIL::ConvertVec3DtoNet3D( &pOut->_Pos, &_vPos );
	UDP_UTIL::ConvertQuattoNetRot( &pOut->_Rot, &_qRot, NET_ROTATE_SCALE );

	return sizeof( N_DROPEDWEAPONINFO );
}

UINT16 GAMEINFO_DROPEDWEAPON_NETDATA::Unpack( const char * pInPacket)
{
	N_DROPEDWEAPONINFO * pIn = (N_DROPEDWEAPONINFO*) pInPacket;

	_ui8IsDrop				= (pIn->State >> 4 ) & 0x0F;
	_ui8State				= pIn->State & 0x0F;

	UDP_UTIL::ConvertNet3DtoVec3D( &_vPos, &pIn->_Pos, __FILE__, __LINE__ );
	UDP_UTIL::ConvertNetRottoQuat( &_qRot, &pIn->_Rot, NET_ROTATE_INVSCALE );

	return sizeof( N_DROPEDWEAPONINFO );
}

/*****************************************/
//GAMEINFO_THROW_WEAPON
void GAMEINFO_THROW_WEAPON::Reset()
{
#if defined( CLIENT_SIDE)
// 	if( _pWeaponBase != nullptr)
// 	{
// 		I3ASSERT( _pWeaponBase->getNetworkIdx() != -1);
// 	}
	_pWeaponBase	= nullptr;
#endif

	_bRecv			= false;
	_bSend			= false;
	_fSendTime		= 0.f;

	_fBroadcastCheckSleepTime = 0.f;
	_fBroadcastCheckSyncTime = 0.f;

	_tNetData.Reset();
	_tAssistData.Reset();
}

void GAMEINFO_THROW_WEAPON::CopyTo( GameNetworkInfoBase * pDest)
{
	GAMEINFO_THROW_WEAPON * pInfo = (GAMEINFO_THROW_WEAPON*) pDest;

#if defined( CLIENT_SIDE)
	pInfo->_pWeaponBase = _pWeaponBase;
#endif // CLIENT_SIDE

	pInfo->_bRecv = _bRecv;

	_tNetData.CopyTo( &pInfo->_tNetData);
	_tAssistData.CopyTo( &pInfo->_tAssistData);

	pInfo->_bSend = _bSend;					// Send할지 체크합니다 ( 1이면 샌드 )
	pInfo->_fSendTime = _fSendTime;				// sync 타임 계산용

	// 이 시간이 되면 보내도록 세팅합니다.(던진 수류탄이 바인드될때 세팅됨)
	pInfo->_fBroadcastCheckSleepTime = _fBroadcastCheckSleepTime;	// Sleep 된 수류탄의 싱크 타임
	pInfo->_fBroadcastCheckSyncTime = _fBroadcastCheckSyncTime;	// 날라가는 수류탄의 싱크 타임
}


#if defined( CLIENT_SIDE)
void GAMEINFO_THROW_WEAPON::Bind( WeaponBase * pWeapon, REAL32 sleepTime, REAL32 syncTime, VEC3D * pInitPos)
{
	_pWeaponBase	= pWeapon;

	T_ItemID weaponID = _pWeaponBase->getWeaponInfo()->GetItemID();
	/*T_ItemID originalID = _pWeaponBase->getWeaponInfo()->GetOriginalWeaponItemID();
	if( originalID > 0)
		weaponID = originalID;*/

	_tNetData.setWeaponID( weaponID);

	_bRecv			= false;
	_bSend			= false;
	_fSendTime		= 0.f;

	_fBroadcastCheckSleepTime			= sleepTime;
	_fBroadcastCheckSyncTime			= syncTime;
	_tAssistData._fInterpolateTime		= 0.f;
#if defined( CLIENT_SIDE)
	_tNetData.SetPos(*pInitPos);
#else
	_tNetData.SetPos( pInitPos);
#endif
	i3Vector::Copy( &_tAssistData._vPostPos, pInitPos );

	_tNetData.setFlyCount( 0);
	_tNetData.setRemainingTime( 0);
}
#endif

#if defined( CLIENT_SIDE)
void GAMEINFO_DROPED_WEAPON::Bind( WeaponBase * pWeapon, CGameCharaBase * pChara, REAL32 sendTime)
{
	_bRecv					= false;
	_fDropedWeaponLifeTime	= 0.f;
	_tNetData.Reset();
	_tAssistData.Reset();
	_bSend					= false;

	_pWeaponRef = pWeapon;
	setState( NET_WEAPONDROPED_FLY);
	setDroped( false);
	_fSendTime	= sendTime;

	MATRIX mat;
	// Receive Assist Info를 설정한다.
	i3Vector::Copy( &_tAssistData._vPostPos, pWeapon->GetPos() );
	_tNetData.setPosition( pWeapon->GetPos() );
	i3Vector::Scale( &_tAssistData._vDir, i3Matrix::GetAt( pChara->GetMatrix()), 0.1f );

	// 최초 회전 방향을 설정한다.
	i3Matrix::Copy( &mat, pChara->GetMatrix());
	i3Matrix::SetPos( &mat, 0.f, 0.f, 0.f);
	i3Matrix::PreRotateX( &mat, I3_PI2);
	i3Matrix::PreRotateY( &mat, I3_PI);
	i3Matrix::PreRotateZ( &mat, I3_PI2);
	i3Matrix::GetRotateQuat( &_tAssistData._qPostRot, &mat );
	_tNetData.setRotation( &_tAssistData._qPostRot );
}
#endif

void GAMEINFO_NPC::Reset()
{
	_nState			= 0;
	_nHP			= 0;
	i3Vector::Zero( &_vTargetPos);
	_nHitUserPart	= 0;
	_bBroadcast		= false;
}

UINT16 GAMEINFO_NPC::Pack( char * pOutPacket, UINT32 ui32BufSize )
{
	if( _bBroadcast == false)
		return 0;

	N_NPCINFO_DEF * pOut = (N_NPCINFO_DEF*) pOutPacket;

	pOut->State	= _nState;
	pOut->HP		= _nHP;

	if( _nHitUserPart == 0)
	{
		UDP_UTIL::ConvertVec3DtoNet3D( &pOut->_Target, &_vTargetPos );
	}
	else
	{
		pOut->_Target.x = (INT16) _nHitUserPart;
	}

	_bBroadcast = false;

	return sizeof( N_NPCINFO_DEF);
}

UINT16 GAMEINFO_NPC::Unpack( const char * pInPacket)
{
	N_NPCINFO_DEF * pIn = (N_NPCINFO_DEF *) pInPacket;

	_nState		= pIn->State;
	_nHP		= pIn->HP;

	if( (pIn->State & 0x0F) == NPC_AI_STATE_HITARROW)
	{
		_nHitUserPart = (INT32) pIn->_Target.x;
	}
	else
	{
		UDP_UTIL::ConvertNet3DtoVec3D( &_vTargetPos, &pIn->_Target, __FILE__, __LINE__ );
	}

	return sizeof( N_NPCINFO_DEF );
}

void GAMEINFO_MISSION_INTRUDER::Reset()
{
	m_MissionMode		= 0;
	m_MissionInstallWho	= -1;
	i3Vector::Zero( &m_MissionInstalledPos);
}

void GAMEINFO_MISSION_INTRUDER::CopyTo( GameNetworkInfoBase * pDest)
{
}

UINT16 GAMEINFO_MISSION_INTRUDER::Pack( char * pOutPacket, UINT32 ui32BufSize )
{
	return UDP_UTIL::ConvertVec3DtoNet3D( (N_POS*) pOutPacket, &m_MissionInstalledPos );
}

UINT16 GAMEINFO_MISSION_INTRUDER::Unpack( const char * pInPacket)
{
	m_MissionMode = 1;
	
	N_POS * pPos = (N_POS*) pInPacket;
	UDP_UTIL::ConvertNet3DtoVec3D( &m_MissionInstalledPos, pPos, __FILE__, __LINE__ );
	
	return sizeof(N_POS);
}


void GAMEINFO_CHARA_NETDATA::Reset()
{
	_iInfoFlag			= 0;
	_tHP.Reset();
	_tCharaState.Reset();
	_tActionKey.Reset();
	_tTakingObject.Reset();
	_tSyncObject.Reset();
	_tRadioChat.Reset();

	_tWeaponData.Reset();
	_tPosRotData.Reset();
#if defined( CLIENT_SIDE)
	if( MyRoomInfoContext::i() != nullptr &&
		!MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE) )
#endif
	{
		//공룡 탈출 모드일 때는 특별히 해당 데이터를 리셋하지 않습니다.
		//수류탄 등의 에러가 있기 때문인데... 별로 좋지 않은 방법이라고 생각합니다.
		//후에 구조를 변경해야될 것 같습니다. 지금은 v3가 급하니.. ㅠ.ㅠ
		_tFireData.Reset();
	}
	_tSufferData.Reset();
	_tSuicideDamageData.Reset();
	_tMissionData.Reset();
	_tDropWeaponData.Reset();
	_tGetWeaponData.Reset();
	_tDirectPickUp.Reset();
	_ui16TouchDownIdx	= 0;
	_tDomiDinoGrenadeData.Reset();
	_tDomiObjectData.Reset();
}

void GAMEINFO_CHARA_NETDATA::CopyTo( GameNetworkInfoBase * pDest)
{
	GAMEINFO_CHARA_NETDATA * pInfo = (GAMEINFO_CHARA_NETDATA*) pDest;
	pInfo->_iInfoFlag	= _iInfoFlag;
	_tHP.CopyTo( &pInfo->_tHP);
	_tCharaState.CopyTo( &pInfo->_tCharaState);
	_tActionKey.CopyTo( &pInfo->_tActionKey);
	_tTakingObject.CopyTo( &pInfo->_tTakingObject);
	_tSyncObject.CopyTo( &pInfo->_tSyncObject);
	_tRadioChat.CopyTo( &pInfo->_tRadioChat);

	_tWeaponData.CopyTo( &pInfo->_tWeaponData);
	_tPosRotData.CopyTo( &pInfo->_tPosRotData);
	_tFireData.CopyTo( &pInfo->_tFireData);
	_tSufferData.CopyTo( &pInfo->_tSufferData);
	_tSuicideDamageData.CopyTo( &pInfo->_tSuicideDamageData);
	_tMissionData.CopyTo( &pInfo->_tMissionData);
	_tDropWeaponData.CopyTo( &pInfo->_tDropWeaponData);
	_tGetWeaponData.CopyTo( &pInfo->_tGetWeaponData);
	_tDirectPickUp.CopyTo(&pInfo->_tDirectPickUp);
	pInfo->_ui16TouchDownIdx = _ui16TouchDownIdx;
	_tDomiDinoGrenadeData.CopyTo( &pInfo->_tDomiDinoGrenadeData);
	_tDomiObjectData.CopyTo( &pInfo->_tDomiObjectData);
}

UINT16 GAMEINFO_CHARA_NETDATA::Pack( char * pOutPacket, UINT32 ui32BufSize )
{
	UINT32 ui32WriteSize	= 0;
	UINT32 ui32Size;

	// 예외 처리해야 되는 패킷
	// 특수한 패킷은 동시에 못 보낼 수 있다.
	if( _iInfoFlag & P2P_INFO_FLAG_DROPWEAPON )
	{	// P2P_INFO_FLAG_DROPWEAPON 과 P2P_INFO_FLAG_FIRE 는 함께 보내면 절대 안됩니다.
		_iInfoFlag &= ~P2P_INFO_FLAG_FIRE;
	}
	
	{	// KEYWORD : Dedicated
		if( sizeof(N_PCINFO_DEF2) > ui32BufSize )	return static_cast<UINT16>(ui32WriteSize);

		N_PCINFO_DEF2	Packet;
		Packet.iInfoFlag = _iInfoFlag;
		ui32Size = Packet.CopyToPacket( &pOutPacket[ ui32WriteSize ] );
		ui32WriteSize	+= ui32Size;
		ui32BufSize		-= ui32Size;
	}

	if( _iInfoFlag & P2P_INFO_FLAG_CHARASTATE)
	{
		ui32Size = _tCharaState.Pack( &pOutPacket[ ui32WriteSize ], ui32BufSize );
		if( 0 == ui32Size )							return static_cast<UINT16>(ui32WriteSize);
		ui32WriteSize	+= ui32Size;
		ui32BufSize		-= ui32Size;
	}

	if( _iInfoFlag & P2P_INFO_FLAG_ONLOADOBJECT )
	{
		ui32Size = _tTakingObject.Pack( &pOutPacket[ ui32WriteSize ], ui32BufSize );
		if( 0 == ui32Size )							return static_cast<UINT16>(ui32WriteSize);
		ui32WriteSize	+= ui32Size;
		ui32BufSize		-= ui32Size;
	}

	if( _iInfoFlag & P2P_INFO_FLAG_CHARAPOS )
	{
		ui32Size = _tPosRotData.Pack( &pOutPacket[ ui32WriteSize ], ui32BufSize );
		if( 0 == ui32Size )							return static_cast<UINT16>(ui32WriteSize);
		ui32WriteSize	+= ui32Size;
		ui32BufSize		-= ui32Size;
	}

	if( _iInfoFlag & P2P_INFO_FLAG_ACTIONKEY )
	{
		ui32Size = _tActionKey.Pack( &pOutPacket[ ui32WriteSize ], ui32BufSize );
		if( 0 == ui32Size )							return static_cast<UINT16>(ui32WriteSize);
		ui32WriteSize	+= ui32Size;
		ui32BufSize		-= ui32Size;
	}

	if( _iInfoFlag & P2P_INFO_FLAG_RADIO )
	{
		ui32Size = _tRadioChat.Pack( &pOutPacket[ ui32WriteSize ], ui32BufSize );
		if( 0 == ui32Size )							return static_cast<UINT16>(ui32WriteSize);
		ui32WriteSize	+= ui32Size;
		ui32BufSize		-= ui32Size;
	}

	if( _iInfoFlag & P2P_INFO_FLAG_SYNCWEAPON )
	{
		ui32Size = _tWeaponData.Pack( &pOutPacket[ ui32WriteSize ], ui32BufSize );
		if( 0 == ui32Size )							return static_cast<UINT16>(ui32WriteSize);
		ui32WriteSize	+= ui32Size;
		ui32BufSize		-= ui32Size;
	}

	if( _iInfoFlag & P2P_INFO_FLAG_WEAPONPARAM )
	{
		ui32Size = _tWeaponData.Pack_Param( &pOutPacket[ ui32WriteSize ], ui32BufSize, WEAPON_PARAM_RECOIL);
		if( 0 == ui32Size )							return static_cast<UINT16>(ui32WriteSize);
		ui32WriteSize	+= ui32Size;
		ui32BufSize		-= ui32Size;
	}

	if( _iInfoFlag & P2P_INFO_FLAG_CS_LIFE)
	{
		ui32Size = _tHP.Pack( &pOutPacket[ ui32WriteSize ], ui32BufSize );
		if( 0 == ui32Size )							return static_cast<UINT16>(ui32WriteSize);
		ui32WriteSize	+= ui32Size;
		ui32BufSize		-= ui32Size;
	}

	// Decent
	if( _iInfoFlag & P2P_INFO_FLAG_SUICIDEDAMAGE )
	{
		ui32Size = _tSuicideDamageData.Pack( &pOutPacket[ ui32WriteSize ], ui32BufSize );
		if( 0 == ui32Size )							return static_cast<UINT16>(ui32WriteSize);
		ui32WriteSize	+= ui32Size;
		ui32BufSize		-= ui32Size;
	}

	// 미션 정보가 있으면 미션 정보만 보낸다. 
	if( _iInfoFlag & P2P_INFO_FLAG_MISSION )
	{
		ui32Size = _tMissionData.Pack( &pOutPacket[ ui32WriteSize ], ui32BufSize );
		if( 0 == ui32Size )							return static_cast<UINT16>(ui32WriteSize);
		ui32WriteSize	+= ui32Size;
		ui32BufSize		-= ui32Size;
	}

	//_tDomiObjectData._iInfoFlag = _iInfoFlag;
	//ui32Size = _tDomiObjectData.Pack( &pOutPacket[ ui32WriteSize ], ui32BufSize );
	//if( 0 == ui32Size )								return 0;
	//ui32WriteSize	+= ui32Size;
	//ui32BufSize		-= ui32Size;

	//Drop
	if( _iInfoFlag & P2P_INFO_FLAG_DROPWEAPON )
	{
		ui32Size = _tDropWeaponData.Pack( &pOutPacket[ ui32WriteSize ], ui32BufSize );
		if( 0 == ui32Size )								return static_cast<UINT16>(ui32WriteSize);
		ui32WriteSize	+= ui32Size;
		ui32BufSize		-= ui32Size;
	}

	//Get Weapon
	if( _iInfoFlag & P2P_INFO_FLAG_GETWEAPON )
	{
		ui32Size = _tGetWeaponData.Pack( &pOutPacket[ ui32WriteSize ], ui32BufSize );
		if( 0 == ui32Size )								return static_cast<UINT16>(ui32WriteSize);
		ui32WriteSize	+= ui32Size;
		ui32BufSize		-= ui32Size;
	}

	if (_iInfoFlag & P2P_INFO_FLAG_DIRECTPICKUP)
	{
		ui32Size = _tDirectPickUp.Pack(&pOutPacket[ui32WriteSize], ui32BufSize);
		if (0 == ui32Size)								return static_cast<UINT16>(ui32WriteSize);
		ui32WriteSize += ui32Size;
		ui32BufSize -= ui32Size;
	}

	// Fire
	if( _iInfoFlag & P2P_INFO_FLAG_FIRE )
	{
		GAMEINFO_FIRE FirePacket;
		_tFireData.CopyTo( &FirePacket );

#if defined( CLIENT_SIDE)
		if( isNetThrowWeapon( &_tWeaponData) )//수류탄
			FirePacket.setID( _tFireData.getNetSlotIndex() );
#endif
		ui32Size = FirePacket.Pack( &pOutPacket[ ui32WriteSize ], ui32BufSize );
		if( 0 == ui32Size )								return static_cast<UINT16>(ui32WriteSize);
		ui32WriteSize	+= ui32Size;
		ui32BufSize		-= ui32Size;
	}

	// Hit
	if( _iInfoFlag & P2P_INFO_FLAG_HIT )
	{
		ui32Size = _tFireData._tHitData.Pack( &pOutPacket[ ui32WriteSize ], ui32BufSize );
		if( 0 == ui32Size )								return static_cast<UINT16>(ui32WriteSize);
		ui32WriteSize	+= ui32Size;
		ui32BufSize		-= ui32Size;
	}

	if( _iInfoFlag & ( P2P_INFO_FLAG_HITBYCHARA | P2P_INFO_FLAG_HITBYEXPLOSION | P2P_INFO_FLAG_HITBYOBJECT) )
	{
		UINT8 nHitCount = (UINT8) _tFireData._tHitData.getHitCount();

		if( nHitCount > MAX_FIREHIT_COUNT )
		{
			I3PRINTLOG(I3LOG_WARN,  "invalid hit count P2P_INFO_FLAG_HIT_DEDI - swoongoo" );
			nHitCount = MAX_FIREHIT_COUNT;
		}

		if( nHitCount > 0)
		{
			UINT8 nCharaCount = 0;
			UINT8 nExplosionCount = 0;
			UINT8 nObjectCount = 0;

			for( UINT8 i = 0; i < nHitCount; i++)
			{
				switch( _tFireData._tHitData.getByType( i)) 
				{
				case GHBT_CHARA :		nCharaCount++;		break;
				case GHBT_EXPLOSION :	nExplosionCount++;	break;
				case GHBT_OBJECT :		nObjectCount++;		break;
				}
			}

			//Set Count 
			if( nCharaCount > 0 && (_iInfoFlag & P2P_INFO_FLAG_HITBYCHARA) )
			{
				ui32Size = _tFireData._tHitData.Pack_Dedicated( &pOutPacket[ ui32WriteSize ], ui32BufSize, nCharaCount);
				if( 0 == ui32Size )								return static_cast<UINT16>(ui32WriteSize);
				ui32WriteSize	+= ui32Size;
				ui32BufSize		-= ui32Size;
			}

			if( nExplosionCount > 0 && (_iInfoFlag & P2P_INFO_FLAG_HITBYEXPLOSION))
			{
				ui32Size = _tFireData._tHitData.Pack_Dedicated_Explosion( &pOutPacket[ ui32WriteSize ], ui32BufSize, nExplosionCount, _tFireData.getNetSlotIndex());
				if( 0 == ui32Size )								return static_cast<UINT16>(ui32WriteSize);
				ui32WriteSize	+= ui32Size;
				ui32BufSize		-= ui32Size;
			}

			if( nObjectCount > 0 && (_iInfoFlag & P2P_INFO_FLAG_HITBYOBJECT ))
			{
				ui32Size = _tFireData._tHitData.Pack_Dedicated_Object( &pOutPacket[ ui32WriteSize ], ui32BufSize, nObjectCount);
				if( 0 == ui32Size )								return static_cast<UINT16>(ui32WriteSize);
				ui32WriteSize	+= ui32Size;
				ui32BufSize		-= ui32Size;
			}

			UDPTRACE( "Send P2P_INFO_FLAG_HIT_DEDI %d\n", nHitCount);
		}
	}

	if( _iInfoFlag & P2P_INFO_FLAG_TOUCHDOWN )
	{
		if( sizeof(UINT16) > ui32BufSize )						return static_cast<UINT16>(ui32WriteSize);

		*((UINT16*)&pOutPacket[ ui32WriteSize ]) = _ui16TouchDownIdx;

		ui32WriteSize	+= sizeof(UINT16);
		ui32BufSize		-= sizeof(UINT16);
	}

	return static_cast<UINT16>(ui32WriteSize);
}

UINT16 GAMEINFO_CHARA_NETDATA::Unpack( const char * pInPacket)
{
	UINT16 Rc;
	UINT16 pos = 0;

	// 패킷 로딩 순서 중요합니다.

	// Def 정보
	{
		N_PCINFO_DEF2 * pIn = (N_PCINFO_DEF2 *) &pInPacket[pos];
		pos += sizeof( N_PCINFO_DEF2);
		_iInfoFlag = pIn->iInfoFlag;
	}

	if( _iInfoFlag & P2P_INFO_FLAG_CHARASTATE)
	{
		Rc = _tCharaState.Unpack( &pInPacket[ pos]);
		UDPCHECK( Rc);
		pos += Rc;
	}

	if( _iInfoFlag & P2P_INFO_FLAG_ONLOADOBJECT)
	{
		Rc = _tTakingObject.Unpack( &pInPacket[ pos]);
		UDPCHECK( Rc);
		pos += Rc;
	}

	if( _iInfoFlag & P2P_INFO_FLAG_CHARAPOS)
	{
		Rc = _tPosRotData.Unpack( &pInPacket[ pos]);
		UDPCHECK( Rc);
		pos += Rc;
	}

	if( _iInfoFlag & P2P_INFO_FLAG_ACTIONKEY )
	{
		Rc = _tActionKey.Unpack( &pInPacket[ pos]);
		UDPCHECK( Rc);
		pos += Rc;
	}

	if( _iInfoFlag & P2P_INFO_FLAG_SYNCOBJECTSTATE)
	{
		Rc = _tSyncObject.Unpack( &pInPacket[ pos]);
		UDPCHECK( Rc);
		pos += Rc;
	}

	if( _iInfoFlag & P2P_INFO_FLAG_RADIO)
	{
		Rc = _tRadioChat.Unpack( &pInPacket[ pos]);
		UDPCHECK( Rc);
		pos += Rc;
	}

	if( _iInfoFlag & P2P_INFO_FLAG_SYNCWEAPON )
	{
		Rc = _tWeaponData.Unpack( &pInPacket[ pos]);
		UDPCHECK( Rc);
		pos += Rc;
	}

	if( _iInfoFlag & P2P_INFO_FLAG_WEAPONPARAM )
	{
		Rc = _tWeaponData.Unpack_Param( &pInPacket[ pos]);
		UDPCHECK( Rc);
		pos += Rc;
	}

	if( _iInfoFlag & P2P_INFO_FLAG_CS_LIFE)
	{
		Rc = _tHP.Unpack( &pInPacket[ pos]);
		UDPCHECK( Rc);
		pos += Rc;
	}
	 
	if( _iInfoFlag & P2P_INFO_FLAG_SUICIDEDAMAGE)
	{
		Rc = _tSuicideDamageData.Unpack( &pInPacket[ pos]);
		UDPCHECK( Rc);
		pos += Rc;
	}

	if( _iInfoFlag & P2P_INFO_FLAG_MISSION)
	{
		Rc = _tMissionData.Unpack( &pInPacket[ pos]);
		UDPCHECK( Rc);
		pos += Rc;
	}

	if( _iInfoFlag & P2P_INFO_FLAG_DOMI_DEATH)
	{
		Rc = _tSufferData.Unpack_Death_ForDomination( &pInPacket[ pos]);
		UDPCHECK( Rc);
		pos += Rc;
	}

	if( _iInfoFlag & P2P_INFO_FLAG_DROPWEAPON)
	{
		Rc = _tDropWeaponData.Unpack( &pInPacket[ pos]);
		UDPCHECK( Rc);
		pos += Rc;
	}

	if( _iInfoFlag & P2P_INFO_FLAG_MOVE_POS)
	{
		Rc = _tPosRotData.Unpack_CommandMove( &pInPacket[ pos]);
		UDPCHECK( Rc);
		pos += Rc;
	}

	if( _iInfoFlag & P2P_INFO_FLAG_GETWEAPON)
	{
		Rc = _tGetWeaponData.Unpack( &pInPacket[ pos]);
		UDPCHECK( Rc);
		pos += Rc;
	}

	if (_iInfoFlag & P2P_INFO_FLAG_DIRECTPICKUP)
	{
		Rc = _tDirectPickUp.Unpack(&pInPacket[pos]);
		UDPCHECK(Rc);
		pos += Rc;
	}

	if( _iInfoFlag & P2P_INFO_FLAG_FIRE)
	{
		Rc = _tFireData.Unpack( &pInPacket[ pos]);
		UDPCHECK( Rc);
		pos += Rc;
	}

	if( _iInfoFlag & P2P_INFO_FLAG_HIT)
	{
		Rc = _tFireData._tHitData.Unpack( &pInPacket[ pos]);
		UDPCHECK( Rc);
		pos += Rc;
	}

	INT32 preIdx = 0;

	// for dedicated server
	if( _iInfoFlag & P2P_INFO_FLAG_HITBYCHARA)
	{
		Rc = _tFireData._tHitData.Unpack_Dedicated( &pInPacket[ pos], preIdx);
		UDPCHECK( Rc);
		pos += Rc;
	}

	// for dedicated server
	if( _iInfoFlag & P2P_INFO_FLAG_HITBYEXPLOSION)
	{
		Rc = _tFireData._tHitData.Unpack_Dedicated_Explosion( &pInPacket[ pos], preIdx);
		UDPCHECK( Rc);
		pos += Rc;
	}

	// for dedicated server
	if( _iInfoFlag & P2P_INFO_FLAG_HITBYOBJECT)
	{
		Rc = _tFireData._tHitData.Unpack_Dedicated_Object( &pInPacket[ pos], preIdx);
		UDPCHECK( Rc);
		pos += Rc;
	}

	// for only client
	if( _iInfoFlag & P2P_INFO_FLAG_CS_DEATH)
	{
		Rc = _tSufferData.Unpack_Death( &pInPacket[ pos]);
		UDPCHECK( Rc);
		pos += Rc;
	}

	// for only client
	if( _iInfoFlag & P2P_INFO_FLAG_CS_SUFFERING)
	{
		Rc = _tSufferData.Unpack_Suffer( &pInPacket[ pos]);
		UDPCHECK( Rc);
		pos += Rc;
	}

	return pos;
}

UINT16 GAMEINFO_CHARA_NETDATA::Pack_StageInfo( char * pOutPacket)
{
	I3ASSERT_0;
	return 0;
}

UINT16 GAMEINFO_CHARA_NETDATA::Unpack_StageInfo( const char * pInPacket)
{
	UINT16 Rc;
	UINT16 pos = 0;

	// Def 정보
	{
		N_PCINFO_DEF2 * pIn = (N_PCINFO_DEF2 *) pInPacket;
		_iInfoFlag = pIn->iInfoFlag;
		pos += sizeof( N_PCINFO_DEF2);
	}

	if( _iInfoFlag & P2P_INFO_FLAG_CS_LIFE)
	{
		Rc = _tHP.Unpack( &pInPacket[ pos]);
		UDPCHECK( Rc);
		pos += Rc;
	}

	return pos;
}


bool GAMEINFO_CHARA_NETDATA::SetInfo_ActionState( UINT8 body, UINT8 body2, UINT8 upper, UINT8 lower, UINT8 fireAction, UINT8 jumpstate, WEAPON::FIRE_ORDER fireOrder)
{
	bool bSend = false;

	if( _tCharaState.getBody() != body)
	{
		_tCharaState.setBody( body);
		bSend = true;
	}

	if( _tCharaState.getBody2() != body2)
	{
		_tCharaState.setBody2( body2);
		bSend = true;
	}

	if( _tCharaState.getUpper() != upper)
	{
		if( (upper != CHARA_UPPER_FIRE) &&				// 공격
			(upper != CHARA_UPPER_SECONDARY_FIRE) &&	// 보조 공격
			(upper != CHARA_UPPER_DAMAGE) &&			// 피격
			(upper != CHARA_UPPER_MOVE) )				// 이동
			//(upper != CHARA_UPPER_SWAPWEAPON) )			// 무기 교체 동작은 이벤트가 발생해도 보내지 않습니다.
		{
			bSend = true;
		}

		_tCharaState.setUpper( upper);
	}

	if( _tCharaState.getLower() != lower)
	{
		_tCharaState.setLower( lower);
		bSend = true;
	}

	if (_tCharaState.getJumpState() != jumpstate)
	{
		_tCharaState.setJumpState(jumpstate);
		bSend = true;
	}

	if( _tCharaState.getFireAction() != fireAction)
	{
		_tCharaState.setFireAction( fireAction);
		_tCharaState.setWeaponFireOrder( fireOrder);
		bSend = true;
	}

	if( bSend)
		_iInfoFlag |= P2P_INFO_FLAG_CHARASTATE;

	return bSend;
}

bool GAMEINFO_CHARA_NETDATA::SetInfo_Movement( INT16 sectorIdx, VEC3D * pPos, REAL32 theta, REAL32 rho)
{
	bool bChanged = false;

	I3ASSERT( sectorIdx < 0xEFFF);
	_tPosRotData.setSectorIndex( sectorIdx);

	if( !i3Vector::IsSame( const_cast<VEC3D*>(_tPosRotData.getPos()), pPos ) )
	{
		I3ASSERT( getX( pPos) > -30000.f);
		I3ASSERT( getX( pPos) < 30000.f);
		I3ASSERT( getY( pPos) > -30000.f);
		I3ASSERT( getY( pPos) < 30000.f);
		I3ASSERT( getZ( pPos) > -30000.f);
		I3ASSERT( getZ( pPos) < 30000.f);

		_tPosRotData.setPos( pPos);
		bChanged = true;
	}

	if( _tPosRotData.getTheta() != theta )
	{
		_tPosRotData.setTheta( theta);
		bChanged = true;
	}

	if( _tPosRotData.getRho() != rho )
	{
		_tPosRotData.setRho( rho);
		bChanged = true;
	}

	if( bChanged)
		_iInfoFlag	|= P2P_INFO_FLAG_CHARAPOS;

	return bChanged;
}

bool GAMEINFO_CHARA_NETDATA::SetInfo_TakingObject( bool bOnLoad, UINT16 objIndex)
{
	if( (_tTakingObject.getOnLoadObject() != bOnLoad ) ||
		(_tTakingObject.getOnLoadObjectIndex() != objIndex) )
	{
		// 일단 애니메이션 오브젝트만 체크합니다.
		_tTakingObject.setOnLoadObject( bOnLoad);
		_tTakingObject.setOnLoadObjectIndex( objIndex);
		_iInfoFlag |= P2P_INFO_FLAG_CHARAPOS | P2P_INFO_FLAG_ONLOADOBJECT;
		return true;
	}

	return false;
}

bool GAMEINFO_CHARA_NETDATA::SetInfo_Weapon( UINT32 weaponID, bool bExtension, WEAPON::ATTACK_TYPE type )
{
	bool bSend = false;

	if( _tWeaponData._ui32WeaponID != weaponID)
	{
		_tWeaponData._ui32WeaponID = weaponID;
		bSend = true;
	}

	_tCharaState.setWeaponSlot( Weapon::GetUsageFromItemID(weaponID) );

	if( _tFireData.getWeaponID() != weaponID)
	{
		_tFireData.setWeaponID( weaponID);
		bSend = true;
	}

	if( _tWeaponData._bWeaponExtension != bExtension)
	{
		_tWeaponData._bWeaponExtension = bExtension;
		bSend = true;
	}
	
	_tCharaState.setWeaponExtension( bExtension);
	
	if( _tFireData.getWeaponExtension() != bExtension)
	{
		_tFireData.setWeaponExtension( bExtension);
		bSend = true;
	}
	if( _tWeaponData.GetAttackType() != type)
	{
		_tWeaponData.SetAttackType( type);
		bSend = true;
	}

	if( bSend)
		_iInfoFlag |= P2P_INFO_FLAG_CHARASTATE;

	return bSend;
}

bool GAMEINFO_CHARA_NETDATA::SetInfo_Fire( UINT32 weaponID, bool bExtension, WEAPON::ATTACK_TYPE type )
{
	bool bSend = SetInfo_Weapon( weaponID, bExtension, type );

	_tCharaState.setWeaponFireOrder( _tWeaponData.getFireOrder());

	if( _tFireData.getWeaponAttackType() != type)
	{
		_tFireData.setWeaponAttackType( type);
		bSend = true;
	}

	return bSend;
}

void GAMEINFO_DOMIOBJECT_REQUEST::Reset()
{
	_iInfoFlag = 0;
	install.Reset();
	repair.Reset();
	upgrade.Reset();
	supplyUse.Reset();
	dropItem.Reset();
}

void GAMEINFO_DOMIOBJECT_REQUEST::CopyTo( GameNetworkInfoBase * dest)
{
	GAMEINFO_DOMIOBJECT_REQUEST * pInfo = (GAMEINFO_DOMIOBJECT_REQUEST*) dest;
	pInfo->_iInfoFlag = _iInfoFlag;
	pInfo->install = install;
	pInfo->repair = repair;
	pInfo->upgrade = upgrade;
	pInfo->supplyUse = supplyUse;
	pInfo->dropItem = dropItem;
}

UINT16 GAMEINFO_DOMIOBJECT_REQUEST::Pack( char * pOutPacket, UINT32 ui32BufSize )
{
	UINT16 pos = 0;

	if( _iInfoFlag & P2P_INFO_FLAG_DOMI_INSTALL )
	{
		install.CopyTo( (N_PCINFO_DOMI_INSTALL*) &pOutPacket[pos]);
		pos += sizeof( N_PCINFO_DOMI_INSTALL);
	}

	if( _iInfoFlag & P2P_INFO_FLAG_DOMI_REPAIR )
	{
		repair.CopyTo( (N_PCINFO_DOMI_REPAIR*) &pOutPacket[pos]);
		pos += sizeof( N_PCINFO_DOMI_REPAIR);
	}

	if( _iInfoFlag & P2P_INFO_FLAG_DOMI_UPGRADE )
	{
		upgrade.CopyTo( (N_PCINFO_DOMI_UPGRADE*) &pOutPacket[pos]);
		pos += sizeof( N_PCINFO_DOMI_UPGRADE);
	}

	if( _iInfoFlag & P2P_INFO_FLAG_DOMI_USE )
	{
		supplyUse.CopyTo( (N_PCINFO_DOMI_USE*) &pOutPacket[pos]);
		pos += sizeof( N_PCINFO_DOMI_USE);
	}

	if( _iInfoFlag & P2P_INFO_FLAG_DOMI_CATCH )
	{
		dropItem.CopyTo( (N_PCINFO_DOMI_CATCH*) &pOutPacket[pos]);
		pos += sizeof( N_PCINFO_DOMI_CATCH);
	}

	return pos;
}

UINT16 GAMEINFO_DOMIOBJECT_REQUEST::Unpack( const char * pInPacket)
{
	UINT16 pos = 0;

	if( _iInfoFlag & P2P_INFO_FLAG_DOMI_INSTALL )
	{
		N_PCINFO_DOMI_INSTALL * pIn = (N_PCINFO_DOMI_INSTALL*) &pInPacket[pos];
		pIn->CopyTo( &install);
		pos += sizeof( N_PCINFO_DOMI_INSTALL);
	}

	if( _iInfoFlag & P2P_INFO_FLAG_DOMI_REPAIR )
	{
		N_PCINFO_DOMI_REPAIR * pIn = (N_PCINFO_DOMI_REPAIR*) &pInPacket[pos];
		pIn->CopyTo( &repair);
		pos += sizeof( N_PCINFO_DOMI_REPAIR);
	}

	if( _iInfoFlag & P2P_INFO_FLAG_DOMI_UPGRADE )
	{
		N_PCINFO_DOMI_UPGRADE * pIn = (N_PCINFO_DOMI_UPGRADE*) &pInPacket[pos];
		pIn->CopyTo( &upgrade);
		pos += sizeof( N_PCINFO_DOMI_UPGRADE);
	}

	if( _iInfoFlag & P2P_INFO_FLAG_DOMI_USE )
	{
		N_PCINFO_DOMI_USE * pIn = (N_PCINFO_DOMI_USE*) &pInPacket[pos];
		pIn->CopyTo( &supplyUse);
		pos += sizeof( N_PCINFO_DOMI_USE);
	}

	if( _iInfoFlag & P2P_INFO_FLAG_DOMI_CATCH )
	{
		N_PCINFO_DOMI_CATCH * pIn = (N_PCINFO_DOMI_CATCH*) &pInPacket[pos];
		pIn->CopyTo( &dropItem);
		pos += sizeof( N_PCINFO_DOMI_CATCH);
	}

	return pos;
}

#ifdef CLIENT_SIDE
void GAMEINFO_AI_DINOs_NETDATA::Reset()
{
	i32Mask = 0;
	list.clear();
}

UINT16 GAMEINFO_AI_DINOs_NETDATA::Pack( char * pOutPacket, UINT32 ui32BufSize )
{
	I3ASSERT_0;
	return 0;
}

UINT16 GAMEINFO_AI_DINOs_NETDATA::Unpack( const char * pInPacket)
{
	UINT16 pos = 0;

	//Mask
	{
		i32Mask = *(INT32*) pInPacket;
		pos += sizeof(INT32);
	}
	
	//AI
	list.reserve( MAX_COUNT_DOMI_DINO);

	INT32 idx = 0;

	for( INT32 i = 0; i < MAX_COUNT_DOMI_DINO; ++i)
	{
		GAMEINFO_AI_DINO * buf = &m_AI_Dino_Infos[ idx];

		if( (i32Mask & (0x01 << i)) == 0)
			continue;

		ST_DINO_UDP * data = (ST_DINO_UDP*) &pInPacket[pos];
		pos += sizeof(ST_DINO_UDP);	//10byte
		
		{
			buf->info._iInfoFlag = (P2P_INFO_FLAG_CHARAPOS | P2P_INFO_FLAG_CHARASTATE);

			//Pos
			N_POS _Pos;
			_Pos.x = data->_r16x; _Pos.y = data->_r16y; _Pos.z = data->_r16z;
			VEC3D vPos;
			UDP_UTIL::ConvertNet3DtoVec3D( &vPos, &_Pos, __FILE__, __LINE__);
			buf->info._tPosRotData.setPos( &vPos);


			//DinoType
			buf->i8type = data->_i8DinoKind;


			//DinoState
			buf->info._tCharaState.setFBState( data->_i8State);
			buf->info._tCharaState.setFBSubState( data->_i8SubState);

			//HP
			buf->info._tHP.setHP( data->_i16HP );
		}

		//sub mask
		{
			buf->i8SubMask = data->_i8AddInfoMask;

			if( buf->i8SubMask & (0x01))		//N_PCINFO_TARGETPOS
			{
				N_PCINFO_TARGETPOS* pInfo = (N_PCINFO_TARGETPOS*) &pInPacket[pos];
				pos += sizeof(N_PCINFO_TARGETPOS);

				buf->info._tFireData.setTargetX( (REAL32)pInfo->r16x);
				buf->info._tFireData.setTargetZ( (REAL32)pInfo->r16z);
			}

			if( buf->i8SubMask & (0x01 << 1))	//N_PCINFO_DINO_BULLET
			{
				N_PCINFO_DINO_BULLET* pInfo = (N_PCINFO_DINO_BULLET*) &pInPacket[pos];
				pos += sizeof(N_PCINFO_DINO_BULLET);

				VEC3D vPos;
				UDP_UTIL::ConvertNet3DtoVec3D( &vPos, &pInfo->_TargetPos, __FILE__, __LINE__);
				buf->info._tFireData.setTargetPosition( &vPos);
			}

			if( buf->i8SubMask & (0x01 << 2))	//N_PCINFO_DINO_GRENADE
			{
				pos += buf->info._tDomiDinoGrenadeData.Unpack( &pInPacket[pos]);
			}
		}

		buf->i32Slot = i;
		list.push_back( buf);

		idx++;
		if( idx >= MAX_COUNT_DOMI_DINO)
			idx = 0;
	}

	return pos;
}

void GAMEINFO_DOMIOBJECT_CONT::Reset()
{
	i3mem::FillZero( &skillObj, sizeof( skillObj));
	i3mem::FillZero( &dropItem, sizeof( dropItem));
}

UINT16 GAMEINFO_DOMIOBJECT_CONT::Unpack( const char * pInPacket)
{
	UINT16 pos = 0;

	// 제압 오브젝트(센트리건, 더미) 파싱
	INT8 installMask[20] = {0,};
	for (INT32 i=0; i<10; i++)
	{
		INT8 val = *(INT8*)(pInPacket+pos);
		pos += sizeof(INT8);
		installMask[i*2] = val & 0x0f;
		installMask[(i*2)+1] = (val & 0xf0) >> 4;
	}

	INT8 installerMask[40] = {0,};
	for (INT32 i=0; i<5; i++)
	{
		INT8 val = *(INT8*)(pInPacket+pos);
		pos += sizeof(INT8);
		for (INT32 b=0; b<8; b++)
		{
			installerMask[(i*8)+b] = val & (1 << (b));
		}
	}

	skillObj.idx = *(INT8*)(pInPacket+pos);
	pos += sizeof(INT8);
	skillObj.lv = installMask[skillObj.idx];

	bool comp1 = installerMask[skillObj.idx*2] ? true : false;
	bool comp2 = installerMask[(skillObj.idx*2)+1] ? true : false;
	skillObj.installerIdx = (comp2*4) + (comp1*2);

	skillObj.hp = *(INT16*)(pInPacket+pos);
	pos += sizeof(INT16);

	// 드랍아이템 파싱
	INT32 dropitemMask = *(INT32*)(pInPacket+pos);
	pos += sizeof(INT32);

	if (dropitemMask)
	{
		INT8 idx = *(INT8*)(pInPacket+pos);
		pos += sizeof(INT8);

		dropItem.dropped = ((1 << idx) & dropitemMask) ? true : false;
		dropItem.idx = idx;

		ST_DROPITEM_UDP* data = (ST_DROPITEM_UDP*)(pInPacket+pos);
		pos += sizeof(ST_DROPITEM_UDP);

		N_POS pos2; pos2.x = data->_r16x; pos2.y = data->_r16y; pos2.z = data->_r16z;
		UDP_UTIL::ConvertNet3DtoVec3D( &dropItem.pos, &pos2, __FILE__, __LINE__);
		dropItem.type = data->_i8Kind;
	}

	return pos;
}

void GAMEINFO_DOMIEVENT_CONT::Reset()
{
	Clear();
}

//#ifdef DOMI_UDP_OBJECT
namespace
{
	//----------------------------------------------
	// 제압 이벤트 파서
	//----------------------------------------------
	enum DOMIOBJECT_EVENT_TYPE 
	{
		DO_EVT_NONE,
		DO_EVT_SENTRYDUMMY_REPAIR,
		DO_EVT_SUPPLY_USE,
		DO_EVT_SENTRY_FIRE,
		DO_EVT_DROPITEM_DROPPED,
		DO_EVT_DROPITEM_CATCHED,
		DO_EVT_SCORE,
		DO_EVT_DINO_DEATH,
		DO_EVT_MUTANTTREX_DASH,
		DO_EVT_MUTANTTREX_HOWL,
		DO_EVT_SPECIAL_EFFECT,
		DO_EVT_MAX
	};

	DOMIOBJECT_EVENT_TYPE GetDomiEventType(INT8 type)
	{
		switch (type)
		{
		case UDP_EVENT_REPAIR:		return DO_EVT_SENTRYDUMMY_REPAIR;
		case UDP_EVENT_USE:			return DO_EVT_SUPPLY_USE;
		case UDP_EVENT_FIRE:		return DO_EVT_SENTRY_FIRE;
		case UDP_EVENT_DROP:		return DO_EVT_DROPITEM_DROPPED;
		case UDP_EVENT_CATCH:		return DO_EVT_DROPITEM_CATCHED;
		case UDP_EVENT_SCORE:		return DO_EVT_SCORE;
		case UDP_EVENT_DEATH_DINO:	return DO_EVT_DINO_DEATH;
		case UDP_EVENT_MUTANTTREX_DASH:	return DO_EVT_MUTANTTREX_DASH;
		case UDP_EVENT_MUTANTTREX_HOWL: return DO_EVT_MUTANTTREX_HOWL;
		case UDP_EVENT_SPECIAL_EFFECT:	return DO_EVT_SPECIAL_EFFECT;
		}
		return DO_EVT_NONE;
	}

	struct DomiEventParser
	{
		virtual void operator()(GAMEINFO_DOMIEVENT_CONT* out, const char* packet, UINT16& readPos) = 0;
	};

	struct DomiEventParserNone : DomiEventParser
	{
		virtual void operator()(GAMEINFO_DOMIEVENT_CONT* out, const char* packet, UINT16& readPos) {}
	}g_event_parser_none;

	struct SentryDummyRepairEventParser : DomiEventParser
	{
		virtual void operator()(GAMEINFO_DOMIEVENT_CONT* out, const char* packet, UINT16& readPos)
		{
			GAMEINFO_SENTRY_DUMMY_REPAIR* evt = new GAMEINFO_SENTRY_DUMMY_REPAIR;
			MEMDUMP_NEW( evt, sizeof(GAMEINFO_SENTRY_DUMMY_REPAIR));

			evt->evtType = *(INT8*)(packet+readPos);
			readPos += sizeof(INT8);

			evt->idx = *(INT8*)(packet+readPos);
			readPos += sizeof(INT8);

			out->events.push_back(evt);
		}
	}g_sentry_dummy_repair_event_parser;

	struct SentryGunFireEventParser : DomiEventParser
	{
		virtual void operator()(GAMEINFO_DOMIEVENT_CONT* out, const char* packet, UINT16& readPos)
		{
			GAMEINFO_SENTRYGUN_FIRE* evt = new GAMEINFO_SENTRYGUN_FIRE;
			MEMDUMP_NEW( evt, sizeof(GAMEINFO_SENTRYGUN_FIRE));

			evt->evtType = *(INT8*)(packet+readPos);
			readPos += sizeof(INT8);

			evt->idx = *(INT8*)(packet+readPos);
			readPos += sizeof(INT8);

			evt->target.x = static_cast<REAL32>(*(REAL16*)(packet+readPos)); readPos += sizeof(REAL16);
			evt->target.y = static_cast<REAL32>(*(REAL16*)(packet+readPos)); readPos += sizeof(REAL16);
			evt->target.z = static_cast<REAL32>(*(REAL16*)(packet+readPos)); readPos += sizeof(REAL16);

			out->events.push_back(evt);
		}
	}g_sentrygun_fire_event_parser;

	struct SupplyUseEventParser : DomiEventParser
	{
		virtual void operator()(GAMEINFO_DOMIEVENT_CONT* out, const char* packet, UINT16& readPos)
		{
			GAMEINFO_SUPPLY_USE* evt = new GAMEINFO_SUPPLY_USE;
			MEMDUMP_NEW( evt, sizeof(GAMEINFO_SUPPLY_USE));

			evt->evtType = *(INT8*)(packet+readPos);
			readPos += sizeof(INT8);

			evt->idx = *(INT8*)(packet+readPos);
			readPos += sizeof(INT8);

			evt->customerIdx = *(INT8*)(packet+readPos);
			readPos += sizeof(INT8);

			out->events.push_back(evt);
		}
	}g_supply_use_event_parser;

	struct DropItemDroppedEventParser : DomiEventParser
	{
		virtual void operator()(GAMEINFO_DOMIEVENT_CONT* out, const char* packet, UINT16& readPos)
		{
			GAMEINFO_DROPITEM_DROPPED* evt = new GAMEINFO_DROPITEM_DROPPED;
			MEMDUMP_NEW( evt, sizeof(GAMEINFO_DROPITEM_DROPPED));

			evt->evtType = *(INT8*)(packet+readPos); readPos += sizeof(INT8);
			evt->idx = *(INT8*)(packet+readPos); readPos += sizeof(INT8);
			evt->type = *(INT8*)(packet+readPos); readPos += sizeof(INT8);
			evt->pos.x = static_cast<REAL32>(*(REAL16*)(packet+readPos)); readPos += sizeof(REAL16);
			evt->pos.y = static_cast<REAL32>(*(REAL16*)(packet+readPos)); readPos += sizeof(REAL16);
			evt->pos.z = static_cast<REAL32>(*(REAL16*)(packet+readPos)); readPos += sizeof(REAL16);

			out->events.push_back(evt);
		}
	}g_dropitem_dropped_event_parser;

	struct DropItemCatchedEventParser : DomiEventParser
	{
		virtual void operator()(GAMEINFO_DOMIEVENT_CONT* out, const char* packet, UINT16& readPos)
		{
			GAMEINFO_DROPITEM_CATCHED* evt = new GAMEINFO_DROPITEM_CATCHED;
			MEMDUMP_NEW( evt, sizeof(GAMEINFO_DROPITEM_CATCHED));

			evt->evtType = *(INT8*)(packet+readPos); readPos += sizeof(INT8);
			evt->idx     = *(INT8*)(packet+readPos); readPos += sizeof(INT8);
			evt->userIdx = *(INT8*)(packet+readPos); readPos += sizeof(INT8);

			out->events.push_back(evt);
		}
	}g_dropitem_catched_event_parser;

	struct ScoreEventParser : DomiEventParser	
	{
		virtual void operator()(GAMEINFO_DOMIEVENT_CONT* out, const char* packet, UINT16& readPos)
		{
			GAMEINFO_SP_SCORE* evt = new GAMEINFO_SP_SCORE;
			MEMDUMP_NEW( evt, sizeof(GAMEINFO_SP_SCORE));

			evt->evtType = *(INT8*)(packet+readPos); readPos += sizeof(INT8);

			ST_EVENT_SCORE* data = (ST_EVENT_SCORE*)(packet+readPos);

			evt->totalKill = data->_AccKillDino;
			evt->totalScore = data->_AccScore;
			evt->totalSP = data->_AccSp;
			evt->userIdx = data->_UserIdx;

			out->events.push_back(evt);
			readPos += sizeof(ST_EVENT_SCORE);
		}
	}g_score_event_parser;

	struct DinoDeathEventParser : DomiEventParser	
	{
		virtual void operator()(GAMEINFO_DOMIEVENT_CONT* out, const char* packet, UINT16& readPos)
		{
			GAMEINFO_DINO_DEATH* evt = new GAMEINFO_DINO_DEATH;
			MEMDUMP_NEW( evt, sizeof(GAMEINFO_DINO_DEATH));

			evt->evtType = *(INT8*)(packet+readPos); readPos += sizeof(INT8);

			ST_EVENT_DEATH_DINO* data = (ST_EVENT_DEATH_DINO*)(packet+readPos);
			
			evt->combo = data->_Combo;
			evt->bHeadShot = data->_bHeadShot;

			INT32 i32Add = 3;
			i3mem::FillZero( evt->sp, sizeof( evt->sp ) );
			i3mem::FillZero( evt->score, sizeof( evt->score ) );

			evt->killerIdx = INVALID_IDX;
			
			for( INT32 i = 0; i < MAX_COUNT_DOMI_USER; i += DOMI_USER_INCREASE )
			{
				UINT32 Maska = (data->_ui8UserMask & (0x03 << i)) >> i;
				switch( Maska )
				{
				case DEATHDINO_KILL:
					evt->killerIdx = static_cast<INT8>(i);
				case DEATHDINO_ASSIST:
					evt->sp[i/2] = *((INT16*)((char*)data + i32Add));
					evt->score[i/2] = *((INT16*)((char*)data + i32Add + 2));
					i32Add += 4;
					break;
				}
			}

#ifdef I3_DEBUG
			FILE* fp = fopen( "Score_Record.txt", "at" );
			if (fp)
			{
				fprintf(fp, "KillerIdx:%d, Combo:%d, sp0:%d, score0:%d, sp1:%d, score1:%d, sp2:%d, score2:%d, sp3:%d, score3:%d\n", evt->killerIdx, evt->combo, evt->sp[0], evt->score[0], evt->sp[1], evt->score[1], evt->sp[2], evt->score[2], evt->sp[3], evt->score[3]);
				fclose(fp);
			}
#endif

			out->events.push_back(evt);
			readPos += static_cast<UINT16>(i32Add);
		}
	}g_dino_death_event_parser;

	struct MutantTrexDashEventParser : DomiEventParser	
	{
		virtual void operator()(GAMEINFO_DOMIEVENT_CONT* out, const char* packet, UINT16& readPos)
		{
			GAMEINFO_MUTANTTREX_DASH* evt = new GAMEINFO_MUTANTTREX_DASH;
			MEMDUMP_NEW( evt, sizeof(GAMEINFO_MUTANTTREX_DASH));

			evt->evtType = *(INT8*)(packet+readPos); readPos += sizeof(INT8);
			evt->dash_info = *(INT8*)(packet+readPos); readPos += sizeof(INT8);

			out->events.push_back(evt);
		}
	}g_mutanttrex_dash_event_parser;

	struct MutantTrexHowlEventParser : DomiEventParser
	{
		virtual void operator()(GAMEINFO_DOMIEVENT_CONT* out, const char* packet, UINT16& readPos)
		{
			GAMEINFO_MUTANTTREX_HOWL* evt = new GAMEINFO_MUTANTTREX_HOWL;
			MEMDUMP_NEW( evt, sizeof(GAMEINFO_MUTANTTREX_HOWL));

			evt->evtType = *(INT8*)(packet+readPos);   readPos += sizeof(INT8);
			evt->mutantIdx = *(INT8*)(packet+readPos); readPos += sizeof(INT8);
			evt->threshold = *(INT16*)(packet+readPos); readPos += sizeof(INT16);

			out->events.push_back(evt);
		}
	}g_mutanttrex_howl_event_parser;

	struct SpecialEffectEventParser : DomiEventParser
	{
		virtual void operator()(GAMEINFO_DOMIEVENT_CONT* out, const char* packet, UINT16& readPos)
		{
			GAMEINFO_SPECIAL_EFFECT* evt = new GAMEINFO_SPECIAL_EFFECT;
			MEMDUMP_NEW( evt, sizeof(GAMEINFO_SPECIAL_EFFECT));

			evt->evtType = *(INT8*)(packet+readPos);   readPos += sizeof(INT8);
			i3mem::Copy(&evt->data, packet+readPos, sizeof(PACKET_DOMINATION_DIVULGE_SPECIAL_EFFECT_ACK)); readPos += sizeof(PACKET_DOMINATION_DIVULGE_SPECIAL_EFFECT_ACK);

			out->events.push_back(evt);
		}
	}g_special_effect_event_parser;

	DomiEventParser* g_domi_event_parser[DO_EVT_MAX] = 
	{
		&g_event_parser_none,
		&g_sentry_dummy_repair_event_parser, 
		&g_supply_use_event_parser, 
		&g_sentrygun_fire_event_parser, 
		&g_dropitem_dropped_event_parser, 
		&g_dropitem_catched_event_parser,
		&g_score_event_parser,
		&g_dino_death_event_parser,
		&g_mutanttrex_dash_event_parser,
		&g_mutanttrex_howl_event_parser,
		&g_special_effect_event_parser,
	};
}

UINT16 GAMEINFO_DOMIEVENT_CONT::Unpack( const char * pInPacket)
{
	UINT16 pos = 0;

#if defined( CLIENT_SIDE)
	INT8 evtCnt = *(INT8*)(pInPacket+pos);
	pos += sizeof(INT8);

	events.reserve(evtCnt);

	for (INT32 i=0; i<evtCnt; i++)
	{
		INT8 type = *(INT8*)(pInPacket+pos);
		(*g_domi_event_parser[GetDomiEventType(type)])(this, pInPacket, pos);
	}
#endif

	return pos;
}


UINT16 GAMEINFO_SENTRYGUN_DEGREE::Unpack( const char * pInPacket)
{
	UINT16 pos = 0;
	
	idx = *(INT8*)(pInPacket+pos);
	pos += sizeof(INT8);

	theta = *(REAL32*)(pInPacket+pos);
	pos += sizeof(REAL32);

	return pos;
}

#endif
