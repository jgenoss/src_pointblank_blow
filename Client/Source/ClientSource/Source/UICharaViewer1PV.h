#pragma once

#include "../UI/UIPopupBase.h"
//#include "UIPhaseInventory.h"

class UICharaView;
class UILoadingIcon;
class CGameCharaBase;

class UICharaViewer1PV : public i3GameNode
{
	I3_CLASS_DEFINE( UICharaViewer1PV, i3GameNode);

private:
	REAL32				m_rCharaChangePoseTime;

	REAL32				m_LightTheta;
	REAL32				m_LightRho;
	REAL32				m_LightDist;
	REAL32				m_timeLocal;

	i3UI3DViewBox *		m_p3DView;
	i3Light *			m_pLight;

	CGameCharaBase *	m_pCharacter;
	VIEW_CHARA_INFO		m_viewCharaInfo;
	VEC3D				m_CharacterPosition;
	CGameCamera_FPS*	m_FPSCamera;

protected:

	void			_AddLight( void);
	void			_ReleaseLight( void);


public:
	UICharaViewer1PV();
	virtual ~UICharaViewer1PV();

	virtual void	OnCreate( i3GameNode * pParent);

	void			Create3DViewBox( i3UIControl * pParent);

	void			Delete3DViewBox( void);

	void			CreateViewStage();

	void			ReleaseRoulette( void);


	virtual void	OnUpdate( REAL32 rDeltaSeconds);

	virtual bool	OnKeyInput( i3InputDeviceManager * pInputMng);

public:

	bool			UpdateCharacter( INT32 idx, bool bFirstUpdate, WEAPON_SLOT_TYPE mustUpdateSlot);
	void			SetLoadedChara( INT32 idx, Avatar * pLoadedAvatar);
	static void		sAfterLoadChara( void * pAvatar, INT32 idx = -1, AI_BIND_DATA * pAIBind = NULL);

	void			setTeam( TEAM_TYPE team)		{ m_viewCharaInfo._charaInfo.SetTeam( (team==CHARACTER_TEAM_RED)?TEAM_RED:TEAM_BLUE );  }
	bool			_CheckWeapon( NET_CHARA_INFO * pNetInfo, WEAPON_SLOT_TYPE mustUpdateSlot);

#ifdef IMPROVEMENT_MULTI_SLOT
	bool			_CheckEquip( ITEM_INFO* pParts);
	bool			_RecreateSlotChara( const ITEM_INFO* pParts);
#else
	bool			_CheckEquip( const CHARA_PARTS_INFO * pParts);
	bool			_RecreateSlotChara( const CHARA_PARTS_INFO * pPartsInfo);
#endif
	

public:
	void			InitCamera();
	void			ChangeCamera();


};