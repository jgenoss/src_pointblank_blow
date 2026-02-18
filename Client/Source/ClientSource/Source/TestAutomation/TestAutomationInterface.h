#pragma once

#if defined( TEST_AUTOMATION)

#include "CaseRunSDK.h"
#include "Capture.h"

#include "UI/UIShopFunction.h"
#include "UI/UIItemInfoUtil.h"
#include "GameMissionManager.h"

using namespace CaseRun;

class TestAutomationInterface : public CaseRun::Interface
{
protected:
	i3TextNodeDX2 *		m_pTextTitle;
	i3TextNodeDX2 *		m_pTextStatus;
	i3TextNodeDX2 *		m_pTextMsg;

	float				m_tmTextDisplay;
	wstring				m_txtStatus;

	CaseRunCapture *	m_pCapture;

	i3Sprite2D *		m_pInd;
	i3TextNodeDX2 *		m_pInd_Text;
	COLOR				m_colIndicator;

	bool				m_MouseBtnPressed[3];
	POINT				m_MousePos;
	bool				m_bValidMousePos;

protected:
	void	CalcLayout(void);

	i3UIControl *	FindCtrl( const wchar_t * pszCtrlPath);

	bool			ToggleCapture(void);

	vector<wstring *> *	GetStageList(STAGE_MODE stageType, int count);
	vector<wstring *> *	EnumStageValues(const wchar_t * pszValue, int count);

	vector<wstring *> *	GetGoodsList( item_def::Category cate, int count, WEAPON_CLASS_TYPE weaponType = WEAPON_CLASS_UNKNOWN);
	vector<wstring *> *	EnumGoodsValues(const wchar_t * pszValue, int count);
	vector<wstring *> *	EnumInvenValues(const wchar_t * pszValue, int count);

	i3UIControl *		PopupPickedCtrls( std::vector<i3UIControl *> & list);
	void				PickCtrl(void);

protected:
	void				CreateIndicator(void);
	void				ShowIndicator( const RECT & rect, const wchar_t * pszMsg);
	void				UpdateIndicator( float alpha);
	void				HideIndicator();

	void				ShowText(float duration);
	void				HideText(void);
protected:
	// Custom
	TESTRESULT			MoveToLobbyScreen(void);
	TESTRESULT			IsAtLobbyScreen(void);
	TESTRESULT			IsAtLoginScreen(void);
	TESTRESULT			IsAtServerSelectScreen(void);
	TESTRESULT			IsAtCharaCreateScreen(void);
	TESTRESULT			WaitPlayerRespawn(void);
	TESTRESULT			IsAtGiftScreen(void);
	TESTRESULT			IsAtShopScreen(void);
	TESTRESULT			IsAtWaitRoomScreen(void);
	TESTRESULT			IsAtClanLobbyScreen(void);
	TESTRESULT			IsInBattle(void);
	TESTRESULT			IsBombInstalled(void);
	TESTRESULT			IsBombUninstalled(void);
	TESTRESULT			IsBombExploded(void);
	TESTRESULT			CloseAllPopup(void);
	TESTRESULT			BuyGoods(const wchar_t * goods_id);

	VEC3D				GetBombInstallPos(BOMB_AREA area);


	GAME_MISSION_STATE	getMissionState(void);
	bool				FindGoodsByName(const wchar_t * pszName, const SHOP_GOODS ** ppGoods, const SHOP_GOODS_PACK ** ppPack);

protected:
	CGameCharaBase *	getPlayer(void);

	void				UpdateStatus(void);

	i3UIControl *		FindClickable(i3UIControl * pCtrl);
	void				_GetCtrlLocation(i3UIControl * pCtrl, RECT * pRect);

public:
	TestAutomationInterface(void);
	~TestAutomationInterface(void);

	bool	Create( CGameFramework * pFramework);

	bool	OnSize(void);

	virtual wstring		GetAppName(void);
	virtual HWND		GetHWND(void);
	virtual bool		IsReady(void);
	virtual void		UpdateText( const wchar_t * pszTitle, const wchar_t * pszMsg);

	virtual bool		GetCtrlRect( const wchar_t * pszCtrlPath, RECT * pRect);
	virtual bool		FindCtrlText_GetRect(const wchar_t * pszCtrlPath, const wchar_t * pszTxt, bool must_be_same, RECT * pRect);
	virtual bool		IsCtrlReady( const wchar_t * pszCtrlPath);
	virtual bool		GetCtrlText( const wchar_t * pszCtrlPath, std::wstring & str);

	virtual void		OnShowIndicator( const RECT & rect, const wchar_t * pszMsg);
	virtual void		OnIndicating( float alpha);
	
	virtual void		OnTest( NodeBase * pNode);

	virtual void		OnTick( float tm);
	virtual TESTRESULT	OnExit(void);
	virtual void		OnKeyPress(float tmLocal, const wchar_t * pszKeyName, bool bPress);
	virtual void		OnMouseAction(float tmLocal, MOUSE_ACTION action, POINT pt, BUTTON btn);

	virtual TESTRESULT	OnUISetText(float tmLocal, const wchar_t * pszCtrlPath, const wchar_t * pszText);
	virtual TESTRESULT	OnUISelectItem( float tmLocal, const wchar_t * pszCtrlPath, const wchar_t * pszText, bool bExact, bool dblClick);
	virtual TESTRESULT	OnUIScanText( float tmLocal, const wchar_t * pszCtrlPath, const wchar_t * pszText, bool bExact, bool bSubCtrl);
	virtual TESTRESULT	OnUICheckState( float tmLocal, const wchar_t * pszCtrlPath, CTRL_STATE state);
	virtual TESTRESULT	OnUICheckTopWindow( float tmLocal, const wchar_t * pszCtrlPath);

	virtual TESTRESULT	OnMoveTo( float tmLocal, LOCTYPE type, wstring & value);
	virtual TESTRESULT	OnLookAt( float tmLocal, DIRTYPE type, wstring & value);

	virtual TESTRESULT	OnCustom( float tmLocal, const wchar_t * pszFunction, const wchar_t * param1, const wchar_t * param2);

	virtual vector<std::wstring *> * EnumVariableValue( const wchar_t * pszVar, const wchar_t * pValue, int count);
};

#endif
