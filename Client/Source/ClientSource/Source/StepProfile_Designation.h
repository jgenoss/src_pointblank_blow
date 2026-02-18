#if !defined( __STEP_PROFILE_DESIGNATION_H__)
#define __STEP_PROFILE_DESIGNATION_H__

#include "StepProfileDef.h"

#define MAX_EVENT_DESIGNATION	6	// ÀÌº¥Æ® È£Äª °¹¼ö

struct __tagBadgeNeed
{
	i3GuiStatic* 		_pBadgeNeedImage;
	i3GuiEditBox* 		_pBadgeNeedText;
	i3GuiEditBox* 		_pBadgeAppendixCount;
	i3GuiEditBox* 		_pBadgeNeedCount;
};

class CGuiNotifyReceiver;

class CStepProfile_Designation : public i3ElementBase
{
	I3_CLASS_DEFINE( CStepProfile_Designation);
private :
	enum DESIGNATION_MODE
	{
		DESIGNATION_MODE_GET = 0,
		DESIGNATION_MODE_CHANGE,
		DESIGNATION_MODE_RELEASE,

		DESIGNATION_MODE_COUNT,
	};

protected:
	CGuiNotifyReceiver *		m_pNotifyBase;

	i3GuiStatic*		m_pDesignationSect;

	i3GuiEditBox*		m_pDesignationChangeText;
	
	i3GuiButton*		m_pDesignationGetButton;
	i3GuiButton*		m_pDesignationChangeButton;
	i3GuiEditBox* 		m_pDesignationNameText;
	i3GuiEditBox* 		m_pDesignationInfoText;
	i3GuiEditBox* 		m_pDesignationPlusEffectText;
	i3GuiEditBox* 		m_pDesignationMinusEffectText;
	i3GuiEditBox* 		m_pDesignationAddEffectText;

	__tagBadgeNeed		m_tBadgeNeed[ BADGE_TYPE_COUNT];

	i3GuiEditBox* 		m_pDesignationNeedText;
	i3GuiEditBox* 		m_pDesignationNeedText2;
	i3GuiEditBox* 		m_pDesignationNeedRankText;
	i3GuiEditBox* 		m_pDesignationNeedChangeExplain;


	i3GuiEditBox* 		m_pDesignationTreeCategory;
	i3GuiStatic*  		m_pDesignationTreeEventCategory_BG;
	i3GuiEditBox* 		m_pDesignationTreeEventCategory;
	i3GuiEditBox* 		m_pDesignationDeatailCategory;
	i3GuiEditBox* 		m_pDesignationGetInfoCategory;
	i3GuiEditBox* 		m_pDesignationPlusEffectKey;
	i3GuiEditBox* 		m_pDesignationMinusEffectKey;
	i3GuiEditBox* 		m_pDesignationAddEffectKey;
	i3GuiEditBox* 		m_pDesignationNeedDesignationKey;
	i3GuiEditBox* 		m_pDesignationNeedRankKey;

	i3GuiButton* 		m_pDesignationButton[MAX_DEFAULT_USER_TITLE_COUNT];
	i3GuiStatic* 		m_pDesignationImage[MAX_DEFAULT_USER_TITLE_COUNT];	
	i3GuiStatic* 		m_pDesignationSelectedImage;	
	i3GuiButton* 		m_pDesignationEventButton[MAX_EVENT_DESIGNATION];
	i3GuiStatic* 		m_pDesignationEventImage[MAX_EVENT_DESIGNATION];

	i3GuiStatic*		m_pDesignationChangeBG;

	i3GuiStatic*		m_pDesignationChange_Slot1;
	i3GuiStatic*		m_pDesignationChange_Slot2;
	i3GuiStatic*		m_pDesignationChange_Slot3;

	i3GuiStatic*		m_pDesignationChange_Slot_BG[MAX_EQUIP_USER_TITLE];
	i3GuiStatic*		m_pDesignationChange_Slot_Image[MAX_EQUIP_USER_TITLE];
	i3GuiStatic*		m_pDesignationChange_Slot_Image_Lock[MAX_EQUIP_USER_TITLE];
	i3GuiEditBox*		m_pDesignationChange_Slot_Text[MAX_EQUIP_USER_TITLE];

	INT32 				m_SelectedDesignationIdx;
	INT32 				m_SelectedDesignationChangeSlotIdx;
	INT32 				m_SelectedDesignationReleaseSlotIdx;
	DESIGNATION_MODE	m_DesignationMode;	
	BOOL				m_bGetDesignation[ MAX_DEFAULT_USER_TITLE_COUNT];
	INT32				m_nCurDesignationSlotCount;

	DESIGNATION_TYPE	m_nCurShowingDesignation;

	bool				m_bIsMyInfoView;


public:
	INT32	getSelectedDesignationIdx( void)			{ return m_SelectedDesignationIdx; }

	void	setMyInfoView( bool bEnable)				{ m_bIsMyInfoView = bEnable; }

public:
	CStepProfile_Designation();
	virtual ~CStepProfile_Designation();

	BOOL	LinkControl( CGuiNotifyReceiver * pBase, i3BinList * plist, i3GameNode * pControl);
	void	Init( void);
	void	Update( REAL32 tm);
	void	GuiNotify( I3GUI_CONTROL_NOTIFY* pNotify);

	BOOL	isEnable( void)				{ return m_pDesignationSect->isEnable(); }
	void	SetEnable( BOOL bEnable);

	void	Reset( void);

	static void CbDesignationGet(void* pThis, INT32 nParam);
	static void CbDesignationRelese(void* pThis, INT32 nParam);

	void	OnGameEvent( INT32 event, INT32 arg);


private:

	void	Event_ChangeUserInfo( void);

	void	__UpdateDetailInfoBox( void);
	void	__UpdateGetInfoBox_GainedBadge( void);
	void	__UpdateGetInfoBox( void);
	void	__UpdateChangePopup( REAL32 rDeltaSeconds);
	void	__UpdateDesignationTree( void);

	void	__OnMouseDesignationButton( INT32 iIdx);
	void	__OnClickedDesignationButton( INT32 iIdx);
	void	__OnDoubleClickedDesignationButton( INT32 iIdx);

	void	__OnSelectDesignationChangeSlot( INT32 iSlot);

	void	__OnDesignationGet(void);
	void	__OnDesignationChange( void);
	void	__OnDesignationRelease(void);
	void	__CheckUserTitlesBeret(INT32 SlotIndex, UINT8 DesignationID);

	void	__OnGameEvent_DesingationGet( INT32 result);
	
	// È£Äª È¹µæ º¸»ó
	BOOL	__FindWeaponByDesignationEffect( DESIGNATION_SET * pSet, INT32 idx, WEAPON_CLASS_TYPE &classType, INT32 &weaponNum);
	BOOL	__GetPrizeWeaponInfoText(char * pText, const size_t sizeTextMax, WEAPON_CLASS_TYPE classType, INT32 weaponNum);
	void	__OpenGainWindowForDesignationWhenGetUserTitle(INT32 nGetGainUserTitleIdx);

	void 	__CheckDesignationSlotGet(void);

	void 	RefreshGainedTitle( const CTitleInfo &titleInfo );

	BOOL 	__CheckDesignationEquip(void);
	BOOL 	__CheckDesignationSameType(void);

	INT32	__FindSameDesignationIndexByType( DESIGNATION_TYPE iTitleType);

	// update getinfo box
	void	__UpdateRequirementBadge( INT32 nType);
	void	__UpdateRequirementDesignation( INT32 nType);
};

#endif
