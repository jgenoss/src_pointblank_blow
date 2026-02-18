#if !defined( __UI_TAB_PROFILE_DESIGNATION_H__)
#define __UI_TAB_PROFILE_DESIGNATION_H__

#include "UITabProfileBase_V11.h"

#define MAX_EVENT_DESIGNATION	6	// ÀÌº¥Æ® È£Äª °¹¼ö

class UITabProfileDesignation_V11 : public UITabProfileBase_V11
{
	I3_CLASS_DEFINE( UITabProfileDesignation_V11, UITabProfileBase_V11);
public:
	UITabProfileDesignation_V11();
	virtual ~UITabProfileDesignation_V11();

	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual void	ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData) override;
	virtual bool	OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE ) override;
	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual bool	OnClick( i3UIControl * pControl) override;
	virtual bool	OnEnterMouse( i3UIControl * pControl) override;
	virtual void	OnLoadAllScenes() override;
	virtual void	OnUnloadAllScenes() override;

	INT32			GetCurShowingDesigType() const { return m_nCurShowingDesignationType; }
	INT32			GetSelectedDesignationReleaseSlotIdx() const { return m_SelectedDesignationReleaseSlotIdx; }

private:
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload( i3UIScene * pScene) override;

	void	_Update_All();
	void	_Update_DetailInfoBox();
	void	_Update_GetInfoBox();
	void	_Update_DesignationTree();
	bool 	_CheckDesignationEquip();
	void	__SetCurrentButton(INT32 idx);
	void	__OnDoubleClickedDesignationButton();

public:
	// Lua function call
	void	__UpdateInfoFromLuaGlue();
	void	__MyDesigClickFromLuaGlue(INT32 idx);
	void	__MyDesigDoubleClickFromLuaGlue(INT32 idx);
	void	__MyEquipReleaseFromLuaGlue(INT32 idx);
	// Message Callback 
	void	__OnDesignationChange(void);

private:
	enum DESIGNATION_MODE
	{
		DESIGNATION_MODE_GET = 0,
		DESIGNATION_MODE_CHANGE,
		DESIGNATION_MODE_RELEASE,
		DESIGNATION_MODE_COUNT,
	};

	INT32 				m_SelectedDesignationIdx;
	INT32				m_OldSelectedDesignationIdx;
	INT32 				m_SelectedDesignationChangeSlotIdx;
	INT32 				m_SelectedDesignationReleaseSlotIdx;
	INT32				m_MySlotIdx;
	DESIGNATION_MODE	m_DesignationMode;	
	INT32				m_nCurDesignationSlotCount;
	INT32 				m_nCurShowingDesignationType;
	i3::vector<bool>	m_vecGetDesignation;

	i3UITemplate *	m_pCheckBoxTemplate;
	i3UICheckBox *	m_pCheckBox[ MAX_DEFAULT_USER_TITLE_COUNT];
	i3UIImageBox *	m_pImageBox[ MAX_DEFAULT_USER_TITLE_COUNT];
	i3UIButtonImageSet * m_pButtonImage[ MAX_DEFAULT_USER_TITLE_COUNT];
	i3UIImageBox*	m_dummyButton[MAX_EQUIP_USER_TITLE];
};

#endif
