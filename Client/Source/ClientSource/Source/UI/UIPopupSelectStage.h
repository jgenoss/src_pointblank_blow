#if !defined( __UI_POPUP_SELECTSTAGE_H__)
#define __UI_POPUP_SELECTSTAGE_H__

#include "UIPopupSubBase.h"

#define MAPSLOT_COUNT 9
#define MAPSTAGE_HORIZEN_COUNT	3

class UIPopupSettingRoom;
class UISlotStage;

struct MAPINFO
{
	char 		_szName[ MAX_STRING_COUNT];
	INT32		_ImageIdx;
	bool		_bNew;
	STAGE_ID	_StageID;
};

class UIPopupSelectStage : public UIPopupSubBase
{
	I3_CLASS_DEFINE( UIPopupSelectStage);

private:
	i3UIFrameWnd *	m_pImageDummy;

	i3UIScrollBar *	m_pScrollBar;

	//i3List			m_MapList;
	UISlotStage *	m_pSlot[ MAPSLOT_COUNT];

	STAGE_ID		m_StageID;
	bool			m_bScroll;
	INT32			m_StartIdx;
	INT32			m_CurrentSlot;

	STAGE_OPTION_TYPE m_StageOption;

	MAPINFO			m_StageMapInfo[ STAGE_ORD_MAX];
	INT32			m_StageCount;

	i3List			m_SelectRandomMapList;
	bool			m_bRandomMap;
	
protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene);
	virtual void	_ClearAtUnload( i3UIScene * pScene);

	UIPopupSettingRoom *	_GetSettingRoomPopup( void);

	void			_ClearMapInfo( void);

	virtual void	_OnClose( void);

public:

	MAPINFO *		getStageMapInfo( INT32 idx)
	{
		I3_BOUNDCHK( idx, STAGE_ORD_MAX);
		return &m_StageMapInfo[ idx];
	}

	UISlotStage *	GetSlot( INT32 idx)
	{
		I3_BOUNDCHK( idx, MAPSLOT_COUNT);
		return m_pSlot[ idx];
	}

	STAGE_ID		GetStageID(INT32 idx)
	{
		I3_BOUNDCHK( idx, m_StageCount);
		return m_StageMapInfo[ idx]._StageID;
	}

	const bool isRandomMap() { return m_bRandomMap;}

public:
	UIPopupSelectStage();
	virtual ~UIPopupSelectStage();

	virtual void	OnCreate( i3GameNode * pParent);

	virtual bool	OnEntranceStart( void * pArg1 = NULL, void * pArg2 = NULL);

	virtual void	OnEntranceEnd( void);

	virtual bool	OnExitStart( void);

	virtual void	OnExitEnd( void);

	virtual bool	OnCloseButton( void);

	virtual bool	OnKey_Enter( void) {  SelectOK(); return true;    }
	virtual bool	OnKey_Escape() { _OnClose(); return true; }

	virtual void	OnWheel( i3UIControl * pControl, INT32 zDelta);

	void			Scroll();

	void			InitMapList( STAGE_TYPE type, STAGE_OPTION_TYPE option);

	void			UpdateMapList( void);

	INT32			FindSlot( LuaState * L);

	void			SelectSlot( INT32 slot);
	void			SelectOK( void);

	void			ClickRandomMap( void);
	virtual void	SetInputDisable( bool bDisable);
};

#endif
