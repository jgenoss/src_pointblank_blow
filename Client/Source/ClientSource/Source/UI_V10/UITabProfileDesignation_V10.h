#if !defined( __UI_TAB_PROFILE_DESIGNATION_V10_H__)
#define __UI_TAB_PROFILE_DESIGNATION_V10_H__

#include "UITabProfileBase_V10.h"

class UITabProfileDesignation_V10 : public UITabProfileBase_V10
{
	I3_CLASS_DEFINE( UITabProfileDesignation_V10, UITabProfileBase_V10);
private:
	enum DESIGNATION_MODE
	{
		DESIGNATION_MODE_GET = 0,
		DESIGNATION_MODE_CHANGE,
		DESIGNATION_MODE_RELEASE,
		DESIGNATION_MODE_COUNT,
	};


public:
	UITabProfileDesignation_V10();
	virtual ~UITabProfileDesignation_V10();

	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual void	ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData) override;
	virtual bool	OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE ) override;
	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual bool	OnClick( i3UIControl * pControl) override;
	virtual bool	OnEnterMouse( i3UIControl * pControl) override;
	virtual void	OnLoadAllScenes() override;
	virtual void	OnUnloadAllScenes() override;
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;

private:
	virtual void	_ClearAtUnload( i3UIScene * pScene) override;

private:
	i3UIButtonImageSet * m_pButtonImage[ MAX_DEFAULT_USER_TITLE_COUNT];
	i3UIButtonImageSet * m_pShapeImage[ MAX_DEFAULT_USER_TITLE_COUNT];
	i3UIImageBox	   * m_pGainDesigFlag[MAX_DEFAULT_USER_TITLE_COUNT];
	i3UIScene*			 m_pScene;
	i3UIFrameWnd*		 m_pDesigFmw;

	enum{ MAX_GET_INFO_COUNT  = 4 };
	i3UIStaticText *	 m_pHavedMedal[MAX_GET_INFO_COUNT];
	i3UIStaticText *	 m_pNeedMedal[MAX_GET_INFO_COUNT];

	i3UIStaticText *	 m_pNeedTitle[2];
	i3UIStaticText *	 m_pNeedRank;

	INT32				 m_SelectedDesignationIdx;
	INT32				 m_OldSelectedDesignationIdx;
	INT32				 m_SelectedDesignationReleaseSlotIdx;
	DESIGNATION_MODE	 m_DesignationMode;
	INT32				 m_nCurShowingDesignationType;
	i3::vector<bool>	 m_vecGetDesignation;
	INT32 				m_SelectedDesignationChangeSlotIdx;
	INT32				 m_nCurDesignationSlotCount;

private:
	
	void				_Update_DetailInfoBox();
	void				_Update_GetInfoBox();
	void				_Update_DesignationTree();
	void				__OnDoubleClickedDesignationButton();

	INT32				_GetDesigIdx(i3UIControl* pCtrl);
	bool				_CheckDesignationEquip();
	void				__SetCurrentButton(INT32 idx);

	void				_InitTabInfo();
	void				_ChangeButtonImgState(I3UI_EVT _evt, INT32 idx);
	
public:
	void				_Update_All();
	INT32				GetSelectedDesignationReleaseSlotIdx() const { return m_SelectedDesignationReleaseSlotIdx; }
	INT32				GetCurShowingDesigType() const { return m_nCurShowingDesignationType; }
	void				__UpdateInfoFromLuaGlue();
	void				__OnDesignationChange(INT32 targetSlotIdx);

};

#endif
