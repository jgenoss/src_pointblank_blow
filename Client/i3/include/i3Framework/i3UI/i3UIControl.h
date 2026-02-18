// ============================================================================
//
// File: i3UIControlBase.h
//
// Desc: UI Control Base Components
//
// Copyright (c) Zepetto Inc. All rights reserved.
//
// ============================================================================

#ifndef __I3UICONTROL_H__
#define __I3UICONTROL_H__

#include "../i3GameObj.h"
#include "i3UITemplate.h"

class i3UIControl;
class i3UIRenderer;

struct I3UI_OVERLAP_INFO
{
	REAL32 x = 0.0f;
	REAL32 y = 0.0f;

	i3UIControl *	m_pFind = nullptr;
	INT32	m_FindDepth = 0;
};

class i3UIFrameWnd;
class i3UIScene;

struct SPRITE2D
{
	i3UIAnimation2DSprite*	_pSprite = nullptr;
};

// ============================================================================
//
// i3UIControl : UI 요소들의 기본이 되는 클래스
//
// ============================================================================
class I3_EXPORT_FRAMEWORK i3UIControl : public i3GameNode
{
	I3_EXPORT_CLASS_DEFINE(i3UIControl, i3GameNode);

	friend class i3UIListView_Box;
	friend class i3UIListView_Item;
	friend class i3UIListView_Sep;

protected:
	UINT128				m_hashTemplate = { 0, 0 };

	i3UIScene		*	m_pAncestor = nullptr;		//	해당 컨트롤이 포함된 screen root
	i3UIFrameWnd	*	m_pOwner = nullptr;			//	해당 컨트롤의 소유자. 없다면 최상위 윈도우
	i3UITemplate	*	m_pTemplate = nullptr;		//	ui template

	i3EventReceiver	*	m_pReceiver = nullptr;
	i3::rc_string		m_strEventHandler;	//  lua event handler name
	i3::rc_string		m_strScript;		//	script

	UINT64				m_nEvtEnableState = 0;
	UINT64				m_nNotifyEnableState = 0;

	VEC2D				m_TempPos;	// 임시 위치값(Align하기 전 Pos)
	VEC3D				m_Pos;				//	혹시 모를 z값을 위해.3 dimension
	VEC3D				m_AccPos;			//	누적 위치계산값, 부모를 찾아야해서 느림. 따라서. 이동시 계산해둠.
	VEC2D				m_Size;				//	width, height
	COLOR				m_Color = { 255, 255, 255, 255 };

	I3UI_STYLE			m_nControlStyle = I3UI_STYLE_SIZE_RATIO;	// style : align, resizing, handled by owner
	I3UI_STATE			m_nControlState = I3UI_STATE_INVALIDSPRITE | I3UI_STATE_WAITLOAD;	// state : show/hide, activate, deactivate

	INT32				m_nControlID = -1;		// ID for Control Notify
	INT32				m_nShapeCount = 0;

	// UI 각 Control에 사용되는 i3Sprite입니다.
	INT32		*		m_hSprites = nullptr;
	i3::pack::RECT *	m_pRects = nullptr;

	i3Node *			m_pSceneNode = nullptr;
	INT32				m_HoldIdx = 0;

	bool				m_bBlockAutoTrashTex = false;
	bool				m_bIgnoreInput = false;
	bool				m_bCalcDoneAccumPos = false;	// 위치 계산이 완료된 경우 true
	bool				m_bResetSprites = true;		// Sprites가 해제된 경우 true
	bool				m_bScissors;
	i3Rect				m_rtScissors;

	i3UIFrameWnd *		m_pTopFrameWnd = nullptr;

	bool				m_bAnimationState = false;
	SPRITE2D*			m_pSpriteAnimation = nullptr;


protected:
	void			_DestroySprites();
	void			_Rec_FindOnMouse( INT32 depth, I3UI_OVERLAP_INFO * pInfo);
	virtual bool	_ResizeControl(void);	// virtual로 변경했습니다, false 반환 시 처리하지 않습니다 - praptor

	void			_recModifyStyle( i3UIControl * pControl, I3UI_STYLE addStyle, I3UI_STYLE removeStyle);

	void			_recsetModifyState( i3UIControl * pControl, bool bModify);
	void			_recEnableCtrl( i3UIControl * pControl, bool bEnable);
	void			_recCalcAccumulatedPos( i3UIControl * pParent);
	void			_recCalcScissorsRect(i3UIControl * pControl);
	void			 resetScissorRect(i3UIControl * pControl);

	bool			isReadyToShow(void);

	virtual void	prepareSprites(void);
	virtual void	resetSprites( bool bRelease);
	void			_AdjustOwnSpriteOrder(void);

	virtual void	OnCompletedLoading(void);

	/** \breif Center Align을 사용하는 경우 Center를 기준으로 Position을 계산해준다. */
	void						_RecalcCenterPosToSave( VEC3D * pOut);

	void			_UpdateAlignedPos( void);

	void			_EnableSprite_IfSlave_Recursive(bool bEnable);

	void			DrawShapes( i3UIRenderer * pRenderer);

public:
	i3UIControl(void);
	virtual ~i3UIControl(void);

	bool			isStyle( I3UI_STYLE nStyle)				{	return ((m_nControlStyle & nStyle) != 0) ? true : false;}
	bool			isState( I3UI_STATE nState)	const		{	return ((m_nControlState & nState) != 0) ? true : false;}
	bool			isIgnoreInput(void)
	{
		return isStyle( I3UI_STYLE_IGNOREINPUT) || m_bIgnoreInput;
	}

	void			setInputDisable( bool bDisable)			{ m_bIgnoreInput = bDisable; }

	// style
	I3UI_STYLE		getStyle(void)							{	return m_nControlStyle;}
	void			setStyle( I3UI_STYLE style)				{	m_nControlStyle = style;}
	void			addStyle( I3UI_STYLE add)				{	m_nControlStyle	|= add;}
	void			removeStyle( I3UI_STYLE remove)			{	m_nControlStyle &= ~remove;}

	void			setEvtEnable( I3UI_EVT evt, bool bEnable)
	{
		if( bEnable)
			m_nEvtEnableState |= (1i64 << evt);
		else
			m_nEvtEnableState &= ~(1i64 << evt);
	}
	bool			isEvtEnable( I3UI_EVT evt)
	{
		return ((m_nEvtEnableState >> evt) & 0x01)?true:false;
	}
	void			setNotifyEnable( I3UI_EVT evt, bool bEnable)
	{
		if( bEnable)
			m_nNotifyEnableState |= (1i64 << evt);
		else
			m_nNotifyEnableState &= ~(1i64 << evt);
	}
	bool			isNotifyEnable(I3UI_EVT evt)
	{
		return ((m_nNotifyEnableState >> evt) & 0x01)?true:false;
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Control State를 설정합니다.
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	I3UI_STATE		getState(void)							{	return m_nControlState;}
	void			setState( I3UI_STATE State)				{	m_nControlState = State;}
	void			addState( I3UI_STATE add)				{	m_nControlState	|= add;}
	void			removeState( I3UI_STATE remove)			{	m_nControlState &= ~remove;}

	bool			isSelected(void) const					{ return isState( I3UI_STATE_SELECTED); }
	bool			isVisible(void)							{ return (isState( I3UI_STATE_HIDED) == false) && (isStyle(I3UI_STYLE_HIDE) == false); }
	bool			isFocused(void)							{ return isState( I3UI_STATE_FOCUSED); }
	bool			isActivated(void)						{ return isState( I3UI_STATE_DEACTIVATED) == false;}

	bool			isPushFlag(void)						{ return isState( I3UI_STATE_PUSHED);		}
	void			SetPushFlag(bool bDisable)
	{
		if( bDisable)	addState( I3UI_STATE_PUSHED);
		else			removeState( I3UI_STATE_PUSHED);
	}

	void			SetHoldImage(bool bHold, INT32 idx = 0)
	{
		if( bHold)	addState( I3UI_STATE_HOLD_IMAGE);
		else		removeState( I3UI_STATE_HOLD_IMAGE);

		m_HoldIdx = idx;
	}

	bool			isHoldImage()							{ return isState( I3UI_STATE_HOLD_IMAGE);	}
	INT32			GetHoldImage()							{ return m_HoldIdx;		}

	virtual bool	isFrameWnd(void)						{ return false;}

	void			setTemplateHashcode(UINT128 hash)		{ m_hashTemplate = hash;}
	UINT128			getTemplateHashCode(void)				{ return m_hashTemplate;}
	// modified
	void			setModifyState( bool bModified = true)
	{
		if( bModified)	addState( I3UI_STATE_NEED_UPDATE);
		else			removeState( I3UI_STATE_NEED_UPDATE);
	}
	bool			getModifyState(void)					{ return isState( I3UI_STATE_NEED_UPDATE);}

	//
	void			setAncestor( i3UIScene * pScene);
	i3UIScene	 *	getAncestor(void)					{ return m_pAncestor; }
	void			setOwner( i3UIFrameWnd * pWnd)	{ m_pOwner = pWnd; }
	i3UIFrameWnd *	getOwner(void)					{ return m_pOwner; }

	void			setEventReceiver( i3EventReceiver * pRecv)	{	I3_REF_CHANGE( m_pReceiver, pRecv);}
	i3EventReceiver*getEventReceiver(void)						{	return m_pReceiver;}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void			AddControl( i3UIControl * pCtrl, bool bAddGg = true);
	void			RemoveControl( i3UIControl * pCtrl);
	void			RemoveAllControl(void);
	i3UIControl *	FindCtrlByName( const char * pszName, bool bRecursive = false);
	bool			IsChildControl( i3UIControl * pCtrl);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 위치를 설정한다.
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void			setPos( REAL32 x, REAL32 y, REAL32 z = 0.0f);
	void			setPos( VEC3D *pvPosition)							{ setPos( pvPosition->x, pvPosition->y, pvPosition->z); }
	void			setDirectPos( REAL32 x, REAL32 y, REAL32 z = 0.0f)	{ m_bCalcDoneAccumPos = false; setPos( x, y, z); }
	void			setDirectPos( VEC3D *pvPosition)					{ m_bCalcDoneAccumPos = false; setPos( pvPosition); }

	virtual void	SetPosNoNotify( REAL32 x, REAL32 y, REAL32 z = 0.f);
	virtual void	SetPosNoNotify( VEC3D *pvPosition)					{ SetPosNoNotify( pvPosition->x, pvPosition->y, pvPosition->z); }

	VEC3D		*	getPos(void)										{	return &m_Pos;}

	void			setTempPos( REAL32 x, REAL32 y)						{	i3Vector::Set( &m_TempPos, x, y); }
	VEC2D		*	getTempPos( void)									{ return &m_TempPos; }

	VEC3D		*	getAccumulatedPos(void)								{	return &m_AccPos;}


	void			CalcAccumulatedPos(void);
	void			CalcAlignedPos( const i3::pack::RECT * pRect, VEC3D * pResult);

	//해당 컨트롤에 중점을 알려준다.(부모에 대한 상대 위치 값)
	virtual void				getCenPos(VEC3D *pVec3) const;
	//해당 컨트롤에 중점을 알려준다.(최상위 부모에 대한 상대 위치 값)
	virtual void				getAccumulatedCenPos(VEC3D *pVec3);

	//left,top,right,bottom은 모두 절대 좌표이다.
	//right와 bottom는 left와 top 에서 시작한 거리를 말하는게 아니다.
	virtual void getRect(i3::pack::RECT *pRect) const;
	virtual void getAccumulatedRect(i3::pack::RECT *pRect);

	//left, top, right, bottom 모두 절대 좌표이다.
	//left, top, width, height 라고 생각하면 안된다.
	virtual void setRect(i3::pack::RECT *pRect);

	//주어진 pCenPos를 중심으로 위치한다.
	virtual void moveCenter(VEC3D *pAccumulatedCenPos);
	//주어진 pCenPos를 중심으로 위치한다.
	virtual void moveCenter(i3UIControl *pi3UIControl);

	// Hit Test
	virtual bool	isOnMouse( I3UI_OVERLAP_INFO * pInfo);
	bool			HitTest( I3UI_PICK_INFO * pInfo, bool bRecursive = true);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 크기를 설정한다.
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void			setSize( REAL32 cx, REAL32 cy);
	void			setSize( VEC2D * pVec)					{ setSize( pVec->x, pVec->y); }

	//OnSize를 호출하지 않는 setSize. 
	//: OnSize는 Editor에서 size를 직접 조절하는 경우에만 호출되어야 한다. 
	virtual void	SetSizeNoNotify( REAL32 cx, REAL32 cy);
	void			SetSizeNoNotify( VEC2D * pVec)			{ SetSizeNoNotify( pVec->x, pVec->y); }

	REAL32			getWidth()								{ return m_Size.x;}
	REAL32			getHeight()								{ return m_Size.y;}
	VEC2D		*	getSize(void)							{ return &m_Size;}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 색상 설정
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void			setColor( COLOR* pColor);
	COLOR*			getColor( void)							{ return &m_Color; }

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual void	EnableCtrl( bool bTrue, bool bRecursive = false);

	INT32			getID(void)											{ return m_nControlID; }
	void			setID(INT32 nControlID)								{ m_nControlID = nControlID; }

	// bNotifySelf 추가 ;jisoo
	void			Notify( I3UI_EVT nEvent, INT32 nPriParam, INT32 nSecParam, bool bNotifySelf = false);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	i3UITemplate *	getTemplate(void)						{	return m_pTemplate;}
	virtual void	SetTemplate( i3UITemplate* pShape);
	virtual void	SetTemplateByName( const char* pszTemplateName);
	virtual bool	CreateBaseSceneObjects();
	virtual void	OnUpdate( REAL32 rDeltaSeconds ) override;
	virtual void	UpdateRenderObjects();

	virtual bool	OnLostDevice( bool bLostDevice) override;
	virtual bool	OnRevive( i3RenderContext * pCtx) override;

	virtual void	OnRecreate( void) override;

	// 정렬 설정
	virtual void	SetAlign( UINT32 AlignX, UINT32 AlignY );

	//shape
	bool			isAllocedSprite( void)
	{
		return (bool)(m_hSprites != nullptr) && !m_bResetSprites;
	}

			INT32	getSpriteHandle( INT32 idx)
			{
				I3_BOUNDCHK(idx, m_nShapeCount);
				I3ASSERT( m_hSprites != nullptr);
				return m_hSprites[idx];
			}
			void	setSpriteHandle(INT32 idx, INT32 nHandle)	{	I3_BOUNDCHK(idx, m_nShapeCount); m_hSprites[idx] = nHandle;}
			INT32	getEndSpriteHandle(void);

			INT32	getShapeCount( void)	const				{	return m_nShapeCount;	}
	virtual void	HideShapes( bool bHide);	//recursively
			void	setAllShapeVisible( bool bVisible);
			void	setShapeVisible( INT32 index, bool bVisible);

			bool	isValidShape( INT32 index);
	i3UIShape *		getShape( INT32 idx);


	virtual INT32	OnCommand(LuaState * pState);
	void			SetEventHandler( const char * pszEventHandler);
	const char *	getEventHandler(void) const	{	return m_strEventHandler.c_str();}

	void			setScript( const char * pszScript);
	const char *	getScript(void) const	{	return m_strScript.c_str();}

	i3Node*				GetNode(void)					{ return m_pSceneNode; }
	void				SetNode( i3Node * pNode);

	void			setTopFrameWnd( i3UIFrameWnd * pFrm)	{ m_pTopFrameWnd = pFrm; }
	i3UIFrameWnd*	getTopFrameWnd( void)					{ return m_pTopFrameWnd; }

	///
protected:
	virtual void	_ForceVisible( bool bFlag) override;

	friend	i3UIScene;

	virtual void	_UpdateShapeState(void) {}	// 현재 state에 따른 visible state를 update한다.

	virtual bool	_PrepareTexForSprite(void);
	virtual bool	_PrepareShow(void);

public:
	void			AdjustTemplate( void);

	//
	// AddControl 등과 같이 Z-Order가 변경되는 경우, Sprite List들을 재배치 하기 위한 함수.
	// Parent에 의해 호출되며, Recursive하게 처리되어야 한다.
	virtual INT32	AdjustSpriteOrder( INT32 hSpriteEnd = -1, bool bRec = true);
	

	/** \brief Visible을 enable시 Child에 대한 설정을 합니다.
		\desc I3UI_STYLE_HIDE를 삭제합니다. 해당 Flag가 있으면 Rendering을 하지 않음.*/
	virtual void	OnVisibleScene(void);

	/** \brief Visible을 disable시 Child에 대한 설정을 합니다.
		\desc I3UI_STYLE_HIDE를 추가합니다. 해당 Flag가 있으면 Rendering을 하지 않음.
			Sprite를 Disable 시킵니다. */
	virtual void	OnInvisibleScene(void);

	void			ShowTop(void);

	//////
	void			ParentNotify( I3UI_EVT event, INT32 param1, INT32 param2);

	i3UIFrameWnd *	FindOwner(void);

	virtual void	OnCreate(void);
	virtual void	OnBindTemplate(void);
	virtual void	OnChangedTemplate(void);					// Template가 수정되었을 때 호출됩니다.
	virtual bool	OnActivate( bool bActive);
	virtual void	OnShow( bool bShow);
	virtual void	OnEnabled( bool bEnable);
	virtual void	OnEnterMouse( REAL32 x, REAL32 y, UINT32 keys);
	virtual void	OnLeaveMouse(void);
	virtual void	OnClicked( REAL32 x, REAL32 y);
	virtual void	OnDblClicked(void);
	virtual void	OnRClicked(void);
	virtual void	OnRDblClicked(void);
	virtual void	OnNotify( i3UIControl * pCtrl, I3UI_EVT event, INT32 param1, INT32 param2) {}
	virtual bool	OnSetFocus(void);
	
	/** \brief Kill Focus
		\param[in] pByCtrl this Control을 KillFocus하는 주최 Control */
	virtual bool	OnKillFocus( i3UIControl * pByCtrl = nullptr);
	virtual bool	OnDragStart( INT32 x, INT32 y);
	virtual bool	OnDragging( INT32 x, INT32 y);
	virtual bool	OnDragEnd( INT32 x, INT32 y);
	virtual void	OnMove( REAL32 x, REAL32 y);
	virtual void	OnSize( REAL32 cx, REAL32 cy);

	//UIEditor의 편집모드에서 해당 컨트롤을 클릭했을 때 호출
	virtual i3UIControl*	OnSelect( void);

	virtual void	OnMapCoord( POINT * pPoint);
	virtual void	OnDraw( i3UIRenderer * pRenderer);
	virtual void	GripDraw( HDC hdc);

	virtual void	OnLButtonDown( UINT32 nFlag, POINT point) override;
	virtual void	OnLButtonUp( UINT32 nFlag, POINT point) override;
	virtual void	OnLButtonDblClk( UINT32 nFlag, POINT point) override;

	virtual void	OnMouseMove( UINT32 nFlag, POINT point) override;

	virtual void	OnRButtonDown( UINT32 nFlag, POINT point) override;
	virtual void	OnRButtonUp( UINT32 nFlag, POINT point) override;
	virtual void	OnRButtonDblClk( UINT32 nFlag, POINT point);

	virtual void	OnMButtonDown( UINT32 nFlag, POINT point) override;
	virtual void	OnMButtonUp( UINT32 nFlag, POINT point) override;
	virtual void	OnMButtonDblClk( UINT32 nFlag, POINT point);

	virtual void	OnMouseWheel( UINT32 nFlag, short zDelta, POINT point) override;

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method ) override;
	virtual void	OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual void	OnInitAfterLoad() override;

	virtual void	SaveToXML( i3XMLFile* pFile, i3XMLElement* pXML)	{}
	virtual void	LoadFromXML( i3XMLFile* pFile, i3XMLElement* pXML)	{}

	// 캡션
	void			GetChildControls( i3::vector<i3UIControl*>& List, bool bRecursive = true);

	virtual void	Init( void)		{};

	//
	static void		RegisterLuaFunction(LuaState * pState);

	void			OnSpriteAnimation(REAL32 rDeltaSeconds);

protected:
	i3::vector<i3::rc_string>	m_TemplateList;
	INT32			m_nTemplateIdx = -1;
	void		_UpdateTemplateListAndIndex( void);

public:
	bool			m_bUseCustomTexCoord = false;
	virtual void			CalcCustomTexCoord( i3Rect* pIntersect, i3Rect* pRect);
	bool			calcTexCoordAndRange( i3UIShape * pShape, VEC4D * pCoord, VEC4D * pRange);

//================================
//ToolTip
private:
	//static member
	//이 선언은 어디까지나 이 멤버가 i3UIControl의 멤버라는 것을 알릴 뿐이지 메모리를 할당하지는 않는다. 
	static REAL32	s_fTtTimer;
	static REAL32	s_fTtDelayTime;	
	static POINT	s_CursorPos;

protected:
	bool			m_bHasTtTimer = false;
	WCHAR16*		m_pTtMsg = nullptr;

	void			_LoseTimer( void);
public:
	void			SetToolTipMsg( const WCHAR16* pszMsg);
	const WCHAR16*	GetToolTipMsg( void)				{	return m_pTtMsg;			}
	
	void			SetToolTipDelay( REAL32 fTime)		{	s_fTtDelayTime = fTime;		}
	REAL32			GetToolTipDelayTime( void)			{	return s_fTtDelayTime;		}
	POINT			GetCursorPos(void)					{	return s_CursorPos;			}

	bool			IsSpriteAnimation()					{ return m_bAnimationState; }
	void			SetSpriteAnimation(bool bState)		{ m_bAnimationState = bState; }
//ToolTip
//================================

	//--------------------------------------------------------------------------------//
	//
#if defined( I3_DEBUG)
private:
	bool			m_bLock = false;		/// Lock유무, Lock이 되어 있으면 EditView에서 선택이 안됨
	bool			m_bSpreaded = false;

	bool			m_bFixedSize = false;	// 고정 size 유무. I3UI_STYLE_SIZE_RATIO 설정 여부
	bool			m_bAutoAlign = false;
	bool			m_bTopDraw = false;
	i3::vector<i3::rc_string >	m_HorizontalAlignList;
	i3::vector<i3::rc_string >	m_VerticalAlignList;
	INT32			m_HorizontalIndex = 0;
	INT32			m_VerticalIndex = 0;

public:
	virtual void	setSpreadShape( bool bSpread);

	// PropertyGrid 관련..
	virtual void		GetTDKGridProperties( i3::vector<PROPERTYINFO*>& List) override;
	virtual void		OnTDKPropertyChanged( const char* szname) override;

	virtual void	GetMessagesForPropertyGrid( i3::vector<PROPERTYMSG*>& List)	{}

#endif

public:
	// 툴 관련// 모드 스위칭
	virtual void			OnEditMode(void) {}
	virtual bool			OnPreviewMode(void) { return true; }

private:

	void					SetEnable() { I3ASSERT_0; }		// 아예 SetEnable 함수 못쓰게 막는다...

	//////////////////////////////////////////////////////////
	// Test Automation을 위해 추가된 기능.
	//
	// 사용자에 의해 Click되거나, 입력되는 UI Control들을 구분하기 위해
	// Ascendant Control들을 모두 포함하는 Path를 사용한다.
	// 이 함수는 해당 Control의 Path를 반환해 주는 함수.
	//
	// ex.) "MainFrm/Dlg_Shop/Btn_Buy"
public:
	virtual void	TA_GetPathID(char * pszFullPath, INT32 limit);
	virtual void	TA_GetCtrlPath( char * pszFullPath, INT32 limit);
	virtual i3UIControl *	TA_FindCtrlByPath( const char * pszPath);
	virtual i3UIControl *	TA_ScanText(const wchar_t * pszText, bool bMustBeSame, bool bSubControl);
	virtual bool	TA_Select(const wchar_t * pszText, bool bMustBeSame);
	virtual void			ResetAnimation();
	
	void		SetScissors(bool bState) { m_bScissors = bState; }
	bool		IsScissors() { return m_bScissors; }

	void		SetScissorsRect(const i3Rect& rt) { m_rtScissors = rt; }
	const i3Rect* GetScissorsRect() const { return &m_rtScissors; }
	void		InitScissorsRect() { return m_rtScissors.set(0,0,0,0); }
};

#endif // __I3UICONTROL_H__
