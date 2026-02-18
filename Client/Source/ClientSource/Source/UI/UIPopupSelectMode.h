#if !defined( __UI_POPUP_SELECTMODE_H__)
#define __UI_POPUP_SELECTMODE_H__

#include "UIPopupSubBase.h"
#include "UIDefine.h"

#define MAX_UI_RESPAWNMODE_BTN_COUNT	12
#define MAX_UI_MISSIONMODE_BIN_COUNT	9
#define MAX_UI_DINOMODE_BIN_COUNT		3

#define MAX_UI_SPECIAL_COUNT 6

enum USM_TYPE
{
	USM_TYPE_NORMAL = 0,
	USM_TYPE_MISSION,
	USM_TYPE_DINO,
	USM_TYPE_SPECIAL,

	USM_TYPE_MAX
};

class UIPopupSelectMode : public UIPopupSubBase
{
	I3_CLASS_DEFINE( UIPopupSelectMode);

	friend class UIPopupCreateRoom;

private:
	i3List			m_ModeList;
	i3List			m_NormalList;
	i3List			m_MissionList;
	i3List			m_DinoList;

	i3List			m_SpecialList;

	INT32			m_StartSpecialIndex;
	INT32			m_CurrentSpecialIndex;

	i3UIScrollBar *	m_pScrollBar;

private:
	void			__AddMode( USM_TYPE type, STAGE_TYPE stageType,
		STAGE_OPTION_TYPE option = STAGE_OPTION_NONE, STAGE_SLOT_MODE slot = STAGE_SLOT_MODE_NORMAL);
	void			__ReleaseModeList( void);

protected:
	void			_SetModeInfo( void);
	void			_SetInitUI(void);

	virtual void	_InitializeAtLoad( i3UIScene * pScene);

	virtual void	_ClearAtUnload( i3UIScene * pScene);

	virtual void	_OnClose( void);

	void			_FindPossibleMode( void);

	void			_UpdateSpecialMode( void);

	void			_EnableScrollBar( bool bEnable);

public:
	UIPopupSelectMode();
	virtual ~UIPopupSelectMode();

	virtual void	OnCreate( i3GameNode * pParent);

	virtual bool	OnEntranceStart( void * pArg1 = NULL, void * pArg2 = NULL);
	virtual void	OnEntranceEnd( void);

	virtual bool	OnCloseButton( void);
	virtual bool	OnKey_Escape() { _OnClose(); return true; }
	virtual bool	OnKey_Enter() { _OnClose(); return true; }

	virtual void	OnWheel( i3UIControl * pControl, INT32 zDelta);

	/** \brief mode¿« selection */
	void			SelectMode( USM_TYPE type, INT32 slot);

	void			Scroll( void);
};

#endif
