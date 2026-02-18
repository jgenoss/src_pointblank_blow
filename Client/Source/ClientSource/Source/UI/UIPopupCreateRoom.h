#if !defined( __UI_POPUP_CREATEROOM_H__)
#define __UI_POPUP_CREATEROOM_H__

/*	

	==========================================================
	방만들기 팝업			-			MADE BY KDY 2016.05.25
	
	* 구 UIPopupCreateRoomEx를 이것으로 업그레이드 합니다.
	* 기존 구조가 단점이 많아 이번 기회에 정리하여 통일합니다.
	==========================================================

	-핵심 구조-

		메인 데이터는 ClientStageInfoContext 에서 관리하는게 기본 골자.

*/

#include "UIDefine.h"
#include "UIMapSlot.h"
#include "UIPushButton.h"
#include "CreateRoomBase.h"

#include "CSI_TypeDef.h"

class UIPopupCreateRoom : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupCreateRoom, UIPopupBase);

	enum CreateRoomBase
	{
		CR_BASE_RULE = 0,
		CR_BASE_STAGE,
		CR_BASE_ROOMINFO,
		CR_BASE_MAIN_CD,
		CR_BASE_SUB_CD,
		CR_BASE_OBSERVER_CD,
		CR_BASE_WEAPON_CD,

		CR_BASE_MAX
	};

public:		//퍼블릭 함수
	
	UIPopupCreateRoom();
	~UIPopupCreateRoom();

public:

	virtual void	OnCreate(i3GameNode * pParent) override;

	/*
	param[in] pArg1 bool*	m_bCreateRoom
	*/
	virtual bool	OnEntranceStart(void * pArg1 = nullptr, void * pArv2 = nullptr) override;
	virtual void	OnEntranceEnd() override;

	virtual bool	OnKeyInput(i3InputDeviceManager * pInputMng) override;

	virtual bool	OnKey_Enter() override;
	virtual bool	OnKey_Escape() override;

	virtual bool	OnPlayClickSound(i3UIControl * pCtrl) override;

protected:

	virtual void	_InitializeAtLoad(i3UIScene * pScene) override;

private:

	void			_InitializeCreateRoomInfo(void);						//로비에서 팝업 온.

private:

	CreateRoom_Base*	m_pCreateRoomBase[CR_BASE_MAX];

	bool				m_bCreateRoom;

public:

	//룰과 스테이지가 변경되면 업데이트 하는 기능들.
	void				UpdateforRule();
	void				UpdateforStage();

	//각 담당 구역 CreateRoom_Base들 불러오기.
	CreateRoom_Base*	GetCreateRoomBase(CreateRoomBase base_enum);

	//UI EVENT 임시 함수.
	void	ClickOk();
	void	ClickCancel();

};

#endif
