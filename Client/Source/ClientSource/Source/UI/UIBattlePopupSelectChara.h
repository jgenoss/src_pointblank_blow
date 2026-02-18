#pragma once
#include "UIBattlePopupBase.h"

#include "UIItemInfoUtil.h"

//#if 0
namespace InGame_Util
{
	class CCharacterSlot
	{	
		enum { NONE, EMPTY, EXIST };

		INT8 m_iState;
		INT32 m_nItemId;
		const CCharaInfo* m_pInfo;
	public:
		CCharacterSlot() : m_pInfo(nullptr), m_nItemId(-1), m_iState(0) {}
		~CCharacterSlot() {}

		void SetInfo( const CCharaInfo* pInfo, INT32 nItemId = -1) { if( pInfo == nullptr) { m_iState = EMPTY; } else { m_iState = EXIST; m_pInfo = pInfo; m_nItemId = nItemId; } }
		const CCharaInfo* GetInfo() const { return m_pInfo; }
		INT32 GetItemId() const { return m_nItemId; }
		bool IsEmpty() { return (m_iState == EMPTY || m_iState == NONE); }
	};
};

using namespace InGame_Util;

class UIBattlePopupSelectChara : public UIBattlePopupBase
{
	I3_CLASS_DEFINE( UIBattlePopupSelectChara, UIBattlePopupBase);

public:
	enum SLOT_IDX
	{
		PREV,
		CURRENT,
		NEXT,
		MAX
	};

	enum SKILL_IDX
	{
		PRIMARY,
		SECONDARY,

		SKILL_MAX
	};	

private:	
	ItemInfoList m_List;

	//
	INT32			m_SelectedIdx;	//실제 선택한(OK 눌렀을때) idx
	INT32			m_nCurrentIdx;	//보여지는 idx
	CCharacterSlot* m_pSlot[MAX];

	//
	bool			_IsAvailble( INT32 nItemId);

	//
	void			_Refresh();

	void			_RefreshSlot( INT8 iSlotIdx, CCharacterSlot* pSlot);
	void			_RefreshDesc( CCharacterSlot* pSlot);
	void			_RefreshSkill( CCharacterSlot* pSlot);

	bool			_IsEscapeModeDino( CHARA_RES_ID eID){
		return (eID == CHARA_RES_ID_DINO_RAPTOR) || (eID == CHARA_RES_ID_DINO_STING) || (eID == CHARA_RES_ID_DINO_TANK) || (eID == CHARA_RES_ID_DINO_ACID)
			|| (eID == CHARA_RES_ID_DINO_ELITE);
	}
public:
	UIBattlePopupSelectChara(/*CBattleGui* pGui*/);
	virtual ~UIBattlePopupSelectChara(void);

	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual	void	OnLoadAllScenes() override;

	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual	bool	OnExitStart() override;

	virtual bool	OnKey_Enter() override;
	virtual bool	OnKey_Escape() override;
	virtual bool	OnKeyInput( i3InputDeviceManager * pInputMng) override;

	virtual void	ProcessGameEvent( GAME_EVENT Event, INT32 Arg, const i3::user_data& UserData) override;
	virtual	void	OnUpdate( REAL32 rDeltaSeconds) override;

	void			ResetFocus();

	void			MoveSlot( INT8 nType);
	bool			SelectOk();
	bool			SelectCancel();
};
//#endif