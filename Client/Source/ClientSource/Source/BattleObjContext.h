#pragma once

#pragma message ("BattleObjContext.h")

#include "ObjectInfoDef.h"
#include "StageObject/ObjectEvent.h"
#include "StageObject/ObjectWeaponBox.h"


#ifdef DOMI_STING_POISON_GRENADE
#include "Character/Fullbody/DomiDinoGrenades.h"
#endif

class BattleObjContext : public i3::shared_ginst<BattleObjContext>
{
public: 
	typedef void	(BattleObjContext::*netFunc)( i3Object * pObj, CGameCharaBase * pChara);
	void			RegisterFunctionWriteObj( UINT32 eventType, netFunc func);
	void			RegisterFunctionReceveObj( UINT32 eventType, netFunc func);
	void			ObjectHitWriteFunction(OBJ_EVENT_TYPE  ObjIdx, i3Object * pObj, CGameCharaBase * pChara = nullptr);
	void			ObjectHitReceveFunction(OBJ_EVENT_TYPE  ObjIdx, i3Object * pObj, CGameCharaBase * pChara = nullptr);
private : 
	i3::unordered_map< UINT32, netFunc >	m_ObjFuncMap_HitWrite;
	i3::unordered_map< UINT32, netFunc >	m_ObjFuncMap_HitReceve;

	void		__Register_Object_Function( void);
	void		__Function_Object_Write_Mission(i3Object * pObj = nullptr, CGameCharaBase * pChara = nullptr);
	void		__Function_Object_Write_Escape(i3Object * pObj = nullptr, CGameCharaBase * pChara = nullptr);
	
	void		__Function_Object_Receve_Mission(i3Object * pObj = nullptr, CGameCharaBase * pChara = nullptr);
	void		__Function_Object_Write_Mission_Trigger(i3Object * pObj = nullptr, CGameCharaBase * pChara = nullptr);

public:
	BattleObjContext();
	~BattleObjContext();

	const GAMEINFO_THROW_WEAPON *	getGameInfo_ThrowWeapon( INT32 userIdx, INT32 idx ) const
	{
		I3_BOUNDCHK( userIdx, SLOT_MAX_COUNT);	I3_BOUNDCHK( idx, MAX_THROWWEAPON_SLOP);
		return &m_ThrowWeaponGameInfo[ userIdx][ idx];
	}

	GAMEINFO_THROW_WEAPON *	setGameInfo_ThrowWeapon( INT32 userIdx, INT32 idx )
	{
		I3_BOUNDCHK( userIdx, SLOT_MAX_COUNT);	I3_BOUNDCHK( idx, MAX_THROWWEAPON_SLOP);
		return &m_ThrowWeaponGameInfo[ userIdx][ idx];
	}

	const GAMEINFO_THROW_WEAPON *	getGameInfo_ThrowWeapon( INT32 idx ) const
	{
		I3_BOUNDCHK( idx, MAX_THROWWEAPON_COUNT);
		return &m_ThrowWeaponGameInfo[ idx / MAX_THROWWEAPON_SLOP][ idx % MAX_THROWWEAPON_SLOP];
	}

	GAMEINFO_THROW_WEAPON *	setGameInfo_ThrowWeapon( INT32 idx )
	{
		I3_BOUNDCHK( idx, MAX_THROWWEAPON_COUNT);
		return &m_ThrowWeaponGameInfo[ idx / MAX_THROWWEAPON_SLOP][ idx % MAX_THROWWEAPON_SLOP];
	}

	void						ResetGameInfo_ThrowWeapon();

	const GAMEINFO_OBJECT *		getGameInfo_Object( INT32 idx ) const	{ I3_BOUNDCHK( idx, OBJ_TOTAL_COUNT);			return &m_ObjectGameInfo[ idx ]; }
	GAMEINFO_OBJECT *			setGameInfo_Object( INT32 idx )			{ I3_BOUNDCHK( idx, OBJ_TOTAL_COUNT);			return &m_ObjectGameInfo[ idx ]; }

	const GAMEINFO_DROPED_WEAPON *	getGameInfo_DropedWeapon( INT32 idx ) const 	{ I3_BOUNDCHK( idx, MAX_DROPEDWEAPON_COUNT);	return &m_DropedWeaponGameInfo[ idx ]; }
	GAMEINFO_DROPED_WEAPON *		setGameInfo_DropedWeapon( INT32 idx )			{ I3_BOUNDCHK( idx, MAX_DROPEDWEAPON_COUNT);	return &m_DropedWeaponGameInfo[ idx ]; }

	UINT16		getDropedWeaponCount( void) const					{ return m_nDropedWeaponCount; }
	void		SetDropedWeaponCount(UINT16 DropedWeaponCount)		{ m_nDropedWeaponCount = DropedWeaponCount; }

	const OBJECT_COUNT_INFO *	getObjectCountInfo(void) const	{ return &m_ObjectCountInfo;	}
	OBJECT_COUNT_INFO *			setObjectCountInfo(void)		{ return &m_ObjectCountInfo;	}

	// 오브젝트 Type에 따라 List를 따로 관리합니다.
	// UDP에서 종류별로 Sync를 맞춥니다.
	void					AddObject( i3Object * pObj );


#ifdef DOMI_STING_POISON_GRENADE
	const DomiDinoGrenades* getDomiDinoGrenades() const { return &m_domiDinoGrenades; }
	DomiDinoGrenades*		setDomiDinoGrenades()		{ return &m_domiDinoGrenades; }

	GAMEINFO_THROW_WEAPON_DOMI_DINO *	FindDomiDinoGrenade( INT32 idx)	{ return m_domiDinoGrenades.Find( idx); }
#endif

	//난입 스모크 수류탄 체크		// 플래그 설정 자체는 스모크 수류탄과 관계가 없고, 플래그 검사시에만 약간의 관련이 있다...
	bool	IsInterEnterSmokeSend() const		{ return m_bInterEnterSmokeSend; }
	void	SetInterEnterSmokeSend(bool val)	{ m_bInterEnterSmokeSend = val; }

#if !defined( I3_NO_PROFILE )
	void CrashAllStageObjects( void);
#endif

	void					CreateBattle( void);		// 배틀 진입시 호출
	void					InitRoundData( void);	// 라운드 시작시 호출
	void					getObjectMission( i3::vector<i3GameObj *>& pObject);
	void					OnObjectMission( bool bEnable);
	
	/** \brief EventSender(i3GameObj, Chara, Weapon)가 해제된 경우 Obj 처리
		\note EventSender로 등록되어 있으면 해제한다. */
	void				ResetEventHandler( i3GameObj * pObj);

private:
	// 수류탄 오브젝트 ReceiveBuffer
	GAMEINFO_THROW_WEAPON	m_ThrowWeaponGameInfo[ SLOT_MAX_COUNT][ MAX_THROWWEAPON_SLOP];	// 외부에서 이것에  직접 접근한다면 이런 구현 좋지 않다..(서버쪽 패킷이 이 구조라고 할지라도)
	// 모든 오브젝트 데이터
	GAMEINFO_OBJECT			m_ObjectGameInfo		[ OBJ_TOTAL_COUNT ];

	// 버려진 무기 정보
	GAMEINFO_DROPED_WEAPON	m_DropedWeaponGameInfo	[ MAX_DROPEDWEAPON_COUNT ];
	UINT16					m_nDropedWeaponCount;

	// Object Netindex를 UDP용 Index로 search하기 위한 용도로 List를 관리합니다.
	OBJECT_COUNT_INFO		m_ObjectCountInfo;

#ifdef DOMI_STING_POISON_GRENADE
	DomiDinoGrenades m_domiDinoGrenades;
#endif	

	bool		m_bInterEnterSmokeSend;					// 위치가 마땅치 않아 이쪽으로 이동...상당이 좋지 않은 플래그이다..관련된 모든 소스가
};
