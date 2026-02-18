#include "i3GuiPCH.h"
#include "i3GuiRoot.h"
#include "i3GuiEditBox.h"
#include "i3GuiTextBox.h"

//I3_CLASS_INSTANCE(i3GuiRoot, i3GameNode);
I3_CLASS_INSTANCE(i3GuiRoot);

static i3Viewer*				s_pViewer = nullptr;

static bool						s_ControlOnceUpdate = true;
static bool						s_bChangeResolution = false;

REAL32	i3GuiRoot::m_sfGuiResolutionWidth = 1.f;
REAL32	i3GuiRoot::m_sfGuiResolutionHeight = 1.f;

REAL32	i3GuiRoot::m_sfGuiResolutionWidthInGame = 1.f;
REAL32	i3GuiRoot::m_sfGuiResolutionHeightInGame = 1.f;

i3GuiObjBase* i3GuiRoot::m_spCapturedMouseControl = nullptr;
i3GuiObjBase* i3GuiRoot::m_spFocusControl = nullptr;	

I3GUI_TEXT_USAGE_TYPE	i3GuiRoot::m_sTextUsageType = I3GUI_TEXT_USAGE_TYPE_TEXTNODE;
bool	i3GuiRoot::m_sbDisableInput = false;
i3GuiObjBase* 	i3GuiRoot::m_pOldOnMouse = nullptr;	


UINT32	i3GuiRoot::m_sMouseButtonState = 0;

i3GuiRoot::i3GuiRoot()
{ 	
	SetDoubleClickTime();	// 500ms, default double-click time			
}

i3GuiRoot::~i3GuiRoot()
{	
	I3_SAFE_NODE_RELEASE(m_pNode);
	I3_SAFE_NODE_RELEASE(m_pGuiAttrSet);	

//	s_pViewer = nullptr;			// i3ResourceFile별로 i3GuiRoot는 여러개가 될수 있는데..전역변수클리어가 맞지 않는것 같다.(2012.09.17.수빈)
	m_spFocusControl = nullptr;	
	m_spCapturedMouseControl = nullptr;
	m_sbDisableInput = false;
	m_sTextUsageType = I3GUI_TEXT_USAGE_TYPE_TEXTNODEDX;
	m_sMouseButtonState = 0;
}

bool i3GuiRoot::Create( i3Node *pParentRoot, const char *pFontName, UINT32 CharSet, INT32 nTextMaxCount, INT32 nTextSize)
{
	g_pGuiRoot = this;

	I3_SAFE_RELEASE( m_pNode);
	m_pNode = i3Node::new_object();

	m_FontSetInfo.SetFontName(pFontName);
	m_FontSetInfo.SetCharset(CharSet);
	m_nDefaultTextCount		= nTextMaxCount;
	m_nDefaultTextSize		= nTextSize;

	// Scene Graph가 생성되었는지 검사합니다.
	if( pParentRoot == nullptr )
	{
		I3TRACE0( "ERROR : pParentRoot(Scene Graph) is nullptr.\n" );
		goto ExitLabel;
	}

#if defined( I3_DEBUG)
	m_pNode->SetName("i3GuiRoot");
#endif

	m_pGuiAttrSet = i3AttrSet::new_object();
    
	{
		i3TextureFilterAttr * pAttr = i3TextureFilterAttr::new_object_ref();
		pAttr->SetMagFilter( I3G_TEXTURE_FILTER_LINEAR);
		pAttr->SetMinFilter( I3G_TEXTURE_FILTER_LINEAR);
		m_pGuiAttrSet->AddAttr( pAttr);
	}

	pParentRoot->AddChild( m_pGuiAttrSet);

	// Scene Graph에 연결
	m_pGuiAttrSet->AddChild(m_pNode);

	return true;

ExitLabel:
	return false;
}

i3GuiWindow* i3GuiRoot::ADDWindowByGUI(const char * szPath)
{
  	i3ResourceFile File;	
	i3GuiWindow* pWindow = nullptr;

	i3Viewer* pViewer = s_pViewer;
	I3GUI_TEXT_USAGE_TYPE usageType = m_sTextUsageType;

	if( File.Load( szPath) == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_FATAL,  "ADDWindowByGUI() Fail-Load Fail.");
		return nullptr;
	}

	i3GuiRoot	* pRoot	= nullptr;
	pRoot = (i3GuiRoot*)File.FindObjectByMeta(i3GuiRoot::static_meta());	
	I3ASSERT(pRoot != nullptr);
			
	for( INT32 i = 0; i< File.GetObjectCount() ; ++i)
	{
		i3PersistantElement * pObj;
		pObj = File.GetObject( i);

		// GUI중 EditBox Control및 EditBox의 상속을 받는 경우 Text를 생성해야 합니다.
		if( i3::same_of<i3GuiWindow*>(pObj))
		{
			pWindow = (i3GuiWindow*)pObj;
			I3_MUST_ADDREF(pWindow);

			((i3GameNode*)pRoot)->RemoveChild((i3GameNode*)pWindow);
			i3GameNode::AddChild((i3GameNode*)pWindow);

			I3_MUST_RELEASE(pWindow);
							
			_Rec_BuildSceneGraphByWindow( (i3GuiObjBase*)pWindow);
		}

		// GUI중 EditBox Control및 EditBox의 상속을 받는 경우 Text를 생성해야 합니다.
		if( i3::same_of<i3GuiEditBox* >(pObj)
			|| i3::same_of<i3GuiTextBox* >(pObj))
		{
			((i3GuiEditBox*)pObj)->CreateTextEx(&m_FontSetInfo, m_nDefaultTextCount, m_nDefaultTextSize);
		}
	}
	

//	pRoot->Release();		// 이제 이게 여기있으면 안됨..(2012.09.17.수빈)

	s_pViewer = pViewer;
	m_sTextUsageType = usageType;

	return pWindow;
}

i3GuiWindow* i3GuiRoot::ADDControlByGUI(const char * szPath, bool bAddToTail)
{
  	i3ResourceFile File;	
	i3GuiControl* pControl = nullptr;

	i3Viewer* pViewer = s_pViewer;
	I3GUI_TEXT_USAGE_TYPE usageType = m_sTextUsageType;

	if( File.Load( szPath) == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_FATAL,  "ADDControlByGUI() Fail-Load Fail.");
		return nullptr;
	}

	i3GuiRoot	* pRoot	= nullptr;
	pRoot = (i3GuiRoot*)File.FindObjectByMeta(i3GuiRoot::static_meta());	
#ifdef I3_DEBUG
	SetName(szPath);
#endif 
//	pRoot->AddRef();
	I3ASSERT(pRoot != nullptr);

	i3GuiWindow * pWindow = (i3GuiWindow*)pRoot->getFirstChild();
	I3ASSERT(pWindow != nullptr);
			
	for( INT32 i = 0; i< File.GetObjectCount() ; ++i)
	{
		i3PersistantElement * pObj;
		pObj = File.GetObject( i);

		// GUI중 EditBox Control및 EditBox의 상속을 받는 경우 Text를 생성해야 합니다.
		if( i3::kind_of<i3GuiControl*>(pObj))
		{
			pControl = (i3GuiControl*)pObj;

			if( i3::same_of<i3GuiWindow*>(pControl->getParent()))
			{
				I3_MUST_ADDREF(pControl);

				((i3GameNode*)pWindow)->RemoveChild(pControl);

				if(bAddToTail)
				{
					getFirstChild()->AddChild((i3GameNode*)pControl);
				}
				else
				{
					getFirstChild()->AddChildToHead((i3GameNode*)pControl);
				}
				

				I3_MUST_RELEASE(pControl);
								
				_Rec_BuildSceneGraphByControl( (i3GuiControl*)pControl);
			}
		}

		// GUI중 EditBox Control및 EditBox의 상속을 받는 경우 Text를 생성해야 합니다.
		if( i3::same_of<i3GuiEditBox* >(pObj)
			|| i3::same_of<i3GuiTextBox* >(pObj))
		{
			((i3GuiEditBox*)pObj)->CreateTextEx(&m_FontSetInfo, m_nDefaultTextCount, m_nDefaultTextSize);

			((i3GuiEditBox*)pObj)->SetTextSpace(m_nDefaultTextSpaceX, m_nDefaultTextSpaceY);
			((i3GuiEditBox*)pObj)->SetOffsetPos(m_fDefaultTextOffsetX, m_fDefaultTextOffsetY);
		}
	}
	

//	pRoot->Release();		// 이제 이게 여기있으면 안됨..(2012.09.17.수빈)

	s_pViewer = pViewer;
	m_sTextUsageType = usageType;

	return pWindow;
}

// Viewer를 설정합니다.
void i3GuiRoot::SetViewer(i3Viewer* pViewer, bool bNoResize)			
{
	s_pViewer = pViewer;

	s_bChangeResolution = bNoResize;

	m_nWidth = pViewer->GetViewWidth();
	m_nHeight = pViewer->GetViewHeight();

	if( s_bChangeResolution)
	{
		i3GuiRoot::m_sfGuiResolutionWidth = (REAL32)m_nWidth / I3GUI_RESOLUTION_WIDTH;
		i3GuiRoot::m_sfGuiResolutionHeight = (REAL32)m_nHeight / I3GUI_RESOLUTION_HEIGHT;

		i3GuiRoot::m_sfGuiResolutionWidthInGame = (REAL32)m_nWidth / I3GUI_RESOLUTION_WIDTH;
		i3GuiRoot::m_sfGuiResolutionHeightInGame = (REAL32)m_nHeight / I3GUI_RESOLUTION_HEIGHT;
	}
	else
	{
		i3GuiRoot::m_sfGuiResolutionWidth = 1.0f;
		i3GuiRoot::m_sfGuiResolutionHeight = 1.0f;
	}
}

//
void i3GuiRoot::OnUpdate( REAL32 rDeltaSeconds )
{ 
	// OnEvent를 통한 입력처리 후에 이루어져야한다.
	i3GameNode::OnUpdate( rDeltaSeconds );

	if( !s_ControlOnceUpdate)
	{
		s_ControlOnceUpdate = true;

		i3GuiWindow * pNext		= nullptr;
		i3GuiWindow* pWindow = (i3GuiWindow*)getFirstChild();

		while( pWindow != nullptr )
		{
			pNext = (i3GuiWindow*)pWindow->getNext();

			//i3GuiObjBase * pBaseNext = nullptr;	//C4189
			i3GuiObjBase * pBase = (i3GuiObjBase*)pWindow->getFirstChild();

			_Rec_ChangeUpdate( pBase);

			pWindow = pNext;
		}
	}
}

bool i3GuiRoot::OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code)
{
	if( code == I3_EVT_CODE_ACTIVATE)
	{
		if (I3_EVT_INPUT == event)
		{
			ResetControlNotify();	// 이전 프레임에서 처리되지 않은 Notify 모두 제거

			if (i3::kind_of< i3InputDeviceManager*>(pObj))
			{
				// Typecast parameter
				REAL32 rDeltaSeconds;
				i3mem::Copy(&rDeltaSeconds, &param2, sizeof(UINT32));		// Typecast param2
				i3InputDeviceManager* pMgr = (i3InputDeviceManager*)pObj;	// Typecast param1

				// Process GUI input
				OnInput(pMgr, rDeltaSeconds);
			}

			return true;
		}
	}

	return i3GameNode::OnEvent(event, pObj, param2, code);
}

// GuiRoot에 Window를 추가합니다.(List와, SceneObject에 추가합니다)
void i3GuiRoot::AddChild(i3GuiWindow* pWindow)
{	
	// Scene Graph에 연결
	getSceneGraphNode()->AddChild(pWindow->GetNode());

	// Game Graph에 연결
	i3GameNode::AddChild((i3GameNode*)pWindow);
}

void i3GuiRoot::RemoveChild(i3GuiWindow* pWindow)
{
	// Scene Graph에 연결 해제 및 릴리즈
	getSceneGraphNode()->RemoveChild(pWindow->GetNode());

	// Game Graph에 연결 해제 및 릴리즈 
	i3GameNode::RemoveChild((i3GameNode*)pWindow);
}

void i3GuiRoot::RemoveAllChild()
{
	i3GuiWindow * pNext		= nullptr;
	i3GuiWindow * pWindow	= (i3GuiWindow*)getFirstChild();

	while( pWindow != nullptr )
	{
		pNext = (i3GuiWindow*)pWindow->getNext();

		RemoveChild( pWindow );

		pWindow = pNext;
	}
}


i3GuiWindow*  i3GuiRoot::GetOnMouseWindow(i3InputMouse* pMouse)
{
	i3GuiWindow* pDestWindow = nullptr;

#if defined( I3_WINDOWS)
	i3GuiWindow* pWindow = (i3GuiWindow*)getFirstChild();
	
	while(pWindow) 
	{
		pWindow->setSize( (INT32)(pWindow->getOriginalWidth() * i3GuiRoot::getGuiResolutionWidth()), (INT32)(pWindow->getOriginalHeight() * i3GuiRoot::getGuiResolutionHeight()));

		if( pWindow->IsOnMouse( pMouse->GetX(), pMouse->GetY()))
		{
			if(pWindow->isEnable())
			{
				pDestWindow = pWindow;
				
				if( (pWindow->getState() & I3GUI_WINDOW_STATE_FOCUS) ||
					(pWindow->getState() & I3GUI_WINDOW_STATE_ALLWAYS_FOCUS) )
				{
					return pDestWindow;
				}
			}
		}

		pWindow = (i3GuiWindow*)pWindow->getNext();
	}
#endif
	return pDestWindow;
}

// 선택된 Window를 최상위 Layer로 변경합니다.(또한 Focus도 가집니다)
void i3GuiRoot::SetTopLayer(i3GuiWindow* pWindow)
{
	if(pWindow == nullptr) return;
	
	// 이미 focus된 윈도우면 처리할 필요 없음
	if( m_pOldFocusWindow == pWindow) return;

	// Window의 State를 변경합니다.
	//if(m_pOldFocusWindow && ((m_pOldFocusWindow->getState() & I3GUI_WINDOW_STATE_ALLWAYS_FOCUS) != 1) )
	if( m_pOldFocusWindow ) // 뒷부분 논리값이 항상 true인것 같아 뺍니다 - praptor
	{
		m_pOldFocusWindow->OnChangeFocus();
		m_pOldFocusWindow->removeState(I3GUI_WINDOW_STATE_FOCUS);
	}

	{
		// Graph에 연결 해제
		I3_MUST_ADDREF(pWindow);
		RemoveChild( pWindow );

		if( pWindow->getNext() != nullptr )
		{
			i3GuiWindow * pNext = (i3GuiWindow*)pWindow->getNext();
			I3_MUST_ADDREF(pNext);
			pWindow->SetNext( nullptr );
		}
		// Graph에 다시 연결
		AddChild( pWindow );
		I3_MUST_RELEASE(pWindow);
	}

	m_pOldFocusWindow = pWindow;
}

// Index의 Window의 활성/비활성화를 설정합니다.
void i3GuiRoot::SetEnableWindowByIndex(INT32 nIndex, bool bTrue)
{
	i3GameNode* pChild = GetChildByIndex(nIndex);
	pChild->SetEnable(bTrue);
}

void i3GuiRoot::ClearMouseState(void)
{
	m_mkPress = 0;
	m_mkStroke = 0;
	m_msMouseState = 0;
	m_mkLastStroke = 0;
	
	m_rClickTimeStamp = 0.0f;
	
	for( INT32 i = 0; i < 3; i++)
	{
		m_rRepeatTimeStamp[i] = 0.0f;
	}
}

void i3GuiRoot::SetEnable( bool bFlag)
{
	if( bFlag)
	{
		m_pNode->RemoveFlag( I3_NODEFLAG_DISABLE | I3_NODEFLAG_INVISIBLE);		
	}
	else
	{
		m_pNode->SetFlag( I3_NODEFLAG_DISABLE | I3_NODEFLAG_INVISIBLE);
	}

	i3GameNode::SetEnable( bFlag);
}

void i3GuiRoot::OnInput(i3InputDeviceManager* pMgr,REAL32 rDeltaSeconds)
{
#if defined( I3_WINDOWS)
	// 마우스 입력 계산
	m_sMouseButtonState = OnMouseInput(pMgr, rDeltaSeconds);

	// Gui 입력이 비활성화 상태라면, 처리하지 않는다.
	if (m_sbDisableInput)
	{
		return;
	}
	
	// 마우스가 올라가 있는 GuiWindow가 있을 경우에만 처리한다
	i3InputMouse* pMouse = pMgr->GetMouse();
	i3GuiWindow* pWindow = GetOnMouseWindow(pMouse);

	if(pWindow)
	{
		// Focus이거나 항상 입력을 받는 control이면 UserInput
		if( pWindow->getState() & I3GUI_WINDOW_STATE_FOCUS || 
			pWindow->getState() & I3GUI_WINDOW_STATE_ALLWAYS_FOCUS )
		{
			// 버튼이 눌렸으면 setfocus
			if( m_sMouseButtonState & I3GUI_MOUSE_STATE_MASK_PRESS)
			{	
				SetTopLayer(pWindow);
				pWindow->OnPushControl();
			}

			
			pWindow->UserInput( pMouse->GetX(), pMouse->GetY(), m_sMouseButtonState);
		}
		else if( m_sMouseButtonState & I3GUI_MOUSE_STATE_MASK_PRESS)
		{
			// 일반 control이면 버튼이 눌렸을때 setfocus
			// 이경우엔 OnPushControl이 없어도 되는가? - praptor
			SetTopLayer(pWindow);
			pWindow->setState(I3GUI_WINDOW_STATE_FOCUS);
		}

		// 이전 Mouse-Over 와 다른 GuiControl이 Mouse Over되었다면...
		i3GuiObjBase * pNewOnMouse = pWindow->GetOnMouse( pMouse->GetX(), pMouse->GetY());

		if( pNewOnMouse != m_pOldOnMouse)
		{
			if( m_pOldOnMouse != nullptr)
				m_pOldOnMouse->OnGuiEvent( i3GUI::EVT_LEAVEMOUSE, (UINT32) pMouse, 0);
	
			if( pNewOnMouse != nullptr)
				pNewOnMouse->OnGuiEvent( i3GUI::EVT_ENTERMOUSE, (UINT32) pMouse, 0);

			m_pOldOnMouse = pNewOnMouse;
		}
	}
#endif
}

void i3GuiRoot::SET_MOUSE_STATE(UINT32 idx, UINT32 key, UINT32 down, UINT32 up, UINT32 dclick, UINT32 repeat, UINT32 mkOldPress)
{
	if( m_mkPress & key)
	{
		if( m_mkStroke & key)
		{
			if( m_rClickTimeStamp > 0.0f && m_mkLastStroke == key)
			{
				m_msMouseState |= dclick;
				m_mkLastStroke = 0;
			}
			else
			{
				m_msMouseState |= down;
				m_mkLastStroke = key;
				m_rClickTimeStamp = m_rDoubleClickTime;
			}
		}
		if( m_rRepeatTimeStamp[idx] == 0.0f)
		{
			m_msMouseState |= repeat;
			if( m_mkStroke & key)
			{
				m_rRepeatTimeStamp[idx] = 0.5f;
			}
			else
			{
				m_rRepeatTimeStamp[idx] = 0.066f;
			}
		}
	}
	else
	{
		if( mkOldPress & key) m_msMouseState |= up;
		m_rRepeatTimeStamp[idx] = 0.0f;
	}
}

UINT32 i3GuiRoot::OnMouseInput(i3InputDeviceManager* pMgr,REAL32 rDeltaSeconds)
{
#if defined( I3_WINDOWS)
	i3InputMouse* pMouse = pMgr->GetMouse();

	UINT32 mkOldPress = m_mkPress;			// Old key backup
	m_mkPress = pMouse->GetButtonState();	// Set current press
	m_mkStroke = ~mkOldPress & m_mkPress;	// Check stroke	
	m_msMouseState = m_mkPress;				// new init. remain press flag

	m_rClickTimeStamp -= rDeltaSeconds;
	if( m_rClickTimeStamp < 0.0f)
	{
		m_rClickTimeStamp = 0.0f;
	}

	for( INT32 i = 0; i < 3; i++)
	{
		m_rRepeatTimeStamp[i] -= rDeltaSeconds;
		if( m_rRepeatTimeStamp[i] < 0.0f)
		{
			m_rRepeatTimeStamp[i] = 0.0f;
		}
	}

	SET_MOUSE_STATE( 0, I3I_MOUSE_LBUTTON, I3GUI_MOUSE_STATE_LBUTTON_DOWN, I3GUI_MOUSE_STATE_LBUTTON_UP, I3GUI_MOUSE_STATE_LBUTTON_DCLICK, I3GUI_MOUSE_STATE_LBUTTON_REPEAT, mkOldPress);
	SET_MOUSE_STATE( 1, I3I_MOUSE_RBUTTON, I3GUI_MOUSE_STATE_RBUTTON_DOWN, I3GUI_MOUSE_STATE_RBUTTON_UP, I3GUI_MOUSE_STATE_RBUTTON_DCLICK, I3GUI_MOUSE_STATE_RBUTTON_REPEAT, mkOldPress);
	SET_MOUSE_STATE( 2, I3I_MOUSE_MBUTTON, I3GUI_MOUSE_STATE_MBUTTON_DOWN, I3GUI_MOUSE_STATE_MBUTTON_UP, I3GUI_MOUSE_STATE_MBUTTON_DCLICK, I3GUI_MOUSE_STATE_MBUTTON_REPEAT, mkOldPress);
	

	// 마우스와 동시에 눌려지는 ctrl, alt, shift 키에 대한 입력확인
	i3InputKeyboard* pKey = pMgr->GetKeyboard();

	if( pKey == nullptr)
	{
		return m_msMouseState;
	}

	// 왼쪽, 오른쪽 구별없이 동일하게 처리
	if( pKey->GetPressState( I3I_KEY_LCTRL)) m_msMouseState |= I3GUI_MOUSE_STATE_CTRL;
	if( pKey->GetPressState( I3I_KEY_RCTRL)) m_msMouseState |= I3GUI_MOUSE_STATE_CTRL;
	if( pKey->GetPressState( I3I_KEY_LALT)) m_msMouseState |= I3GUI_MOUSE_STATE_ALT;
	if( pKey->GetPressState( I3I_KEY_RALT)) m_msMouseState |= I3GUI_MOUSE_STATE_ALT;
	if( pKey->GetPressState( I3I_KEY_LSHIFT)) m_msMouseState |= I3GUI_MOUSE_STATE_SHIFT;
	if( pKey->GetPressState( I3I_KEY_RSHIFT)) m_msMouseState |= I3GUI_MOUSE_STATE_SHIFT;
#endif
	return m_msMouseState;
}

void i3GuiRoot::SetDoubleClickTime(REAL32 rSeconds)
{
	m_rDoubleClickTime = rSeconds;
}

i3EventReceiver * i3GuiRoot::_Find_EventReceiver( i3GuiObjBase * pLeaf)
{
	while( pLeaf != nullptr)
	{
		if( pLeaf == (void *) this)
			break;

		if( pLeaf->getEventReceiver() != nullptr)
			return pLeaf->getEventReceiver();

		pLeaf = (i3GuiObjBase *) pLeaf->getParent();
	}

	return nullptr;
}

void i3GuiRoot::PushControlNotify(INT32 nID,INT32 nEvent,INT32 nPriParam,INT32 nSecParam, void * pPointer)
{
	I3GUI_CONTROL_NOTIFY* pNotify;
	
	pNotify = m_NotifyQueue + m_idxLast;
	
	pNotify->m_nID = nID;
	pNotify->m_nEvent = nEvent;
	pNotify->m_nPriParam = nPriParam;
	pNotify->m_nSecParam = nSecParam;
	pNotify->m_pCtrl = pPointer;

	if( isManualNotifyDispatch())
	{
		// 수동으로 Control Notify를 Dispatch하겠다...
		if( m_idxLast + 1 < I3GUI_MAX_NOTIFY )
		{
			m_idxLast++;
		}
		else
		{
			m_idxLast = 0;
		}

		if( m_bFulled )
		{
			if( m_idxStart + 1 < I3GUI_MAX_NOTIFY )
			{
				m_idxStart++;
			}
			else
			{
				m_idxStart = 0;
			}

			I3PRINTF("NotifyQueue Overwrited. I3GUI_MAX_NOTIFY = %d\n", I3GUI_MAX_NOTIFY);
		}
		else
		{
			if( m_idxLast == m_idxStart )
			{
				m_bFulled = true;
				
				I3PRINTF("NotifyQueue Fulled. I3GUI_MAX_NOTIFY = %d\n", I3GUI_MAX_NOTIFY);
			}
		}
	}
	else
	{
		// 자동으로 Dispatch
		i3EventReceiver * pReceiver = _Find_EventReceiver( (i3GuiObjBase *) pPointer);

		if( pReceiver != nullptr)
		{
			pReceiver->OnEvent( I3_EVT_GUI_NOTIFY, this, (UINT32) pNotify);
		}
	}
}

bool i3GuiRoot::PopControlNotify(I3GUI_CONTROL_NOTIFY* pNotify)
{
	I3ASSERT(pNotify);

	// IsEmpty
	if( m_idxStart == m_idxLast && !m_bFulled)
	{
		return false;
	}

	pNotify->m_nID = m_NotifyQueue[m_idxStart].m_nID;
	pNotify->m_nEvent = m_NotifyQueue[m_idxStart].m_nEvent;
	pNotify->m_nPriParam = m_NotifyQueue[m_idxStart].m_nPriParam;
	pNotify->m_nSecParam = m_NotifyQueue[m_idxStart].m_nSecParam;
	pNotify->m_pCtrl = m_NotifyQueue[m_idxStart].m_pCtrl;

	if( m_idxStart + 1 < I3GUI_MAX_NOTIFY)
	{
		m_idxStart++;
	}
	else
	{
		m_idxStart = 0;
	}

	if( m_bFulled )
	{
		m_bFulled = false;
	}

	return true;
}

void i3GuiRoot::ResetControlNotify(void)
{
	m_idxStart = m_idxLast;
	m_bFulled = false;
}

void i3GuiRoot::_Rec_BuildSceneGraphByControl( i3GuiControl * pObj)
{
	if( pObj == nullptr)	return;

	i3GameNode	* pParent		= pObj->getParent();
	i3Node		* pParentNode	= nullptr;

	if( i3::same_of<i3GuiWindow* >(pParent))
	{
		pParentNode = ((i3GuiWindow*)pParent)->GetNode();
	}
	else if( i3::kind_of< i3GuiControl* >(pParent))
	{
		pParentNode = ((i3GuiControl *)pParent)->GetNode();
	}

	if( (pObj->GetNode() != nullptr) && (pParentNode != nullptr))
	{
		pParentNode->AddChild( pObj->GetNode());		
	}

	//Child
	i3GuiControl * pChild = ( i3GuiControl *)pObj->getFirstChild();
	if( pChild != nullptr)
	{
		_Rec_BuildSceneGraphByControl( pChild);
	}

	//Next
	_Rec_BuildSceneGraphByControl( (i3GuiControl *)pObj->getNext());
}

void i3GuiRoot::_Rec_BuildSceneGraphByWindow( i3GuiObjBase * pObj)
{
	if( pObj == nullptr)	return;

	i3GameNode	* pParent		= pObj->getParent();
	i3Node		* pParentNode	= nullptr;

	if( i3::kind_of< i3GuiRoot*>(pParent))
	{
		pParentNode = ((i3GuiRoot*)pParent)->getSceneGraphNode();
	}
	else if( i3::kind_of<i3GuiObjBase* >(pParent))
	{
		pParentNode = ((i3GuiObjBase *)pParent)->GetNode();
	}

	if( (pObj->GetNode() != nullptr) && (pParentNode != nullptr))
	{
		pParentNode->AddChild( pObj->GetNode());
	}

	//Child
	i3GuiObjBase * pChild = ( i3GuiObjBase *)pObj->getFirstChild();
	if( pChild != nullptr)
	{
		_Rec_BuildSceneGraphByWindow( pChild);
	}

	//Next
	_Rec_BuildSceneGraphByWindow( (i3GuiObjBase *)pObj->getNext());
}

void i3GuiRoot::BuildSceneGraph(void)
{
	_Rec_BuildSceneGraphByWindow( (i3GuiObjBase*) getFirstChild());
}

i3GuiControl*	i3GuiRoot::GetControlByName(const char* pszName, i3EventReceiver * pReceiver)
{
	i3GuiWindow* pWindow = ((i3GuiWindow*)(getFirstChild()));	

	while(pWindow)
	{
		i3GuiControl * pControl = (i3GuiControl*)pWindow->getFirstChild();
		m_ControlBackUpList.clear();
		
		while(pControl)
		{
			//============================================
			// 이곳에 Control을 링크합니다.
			if(pControl->hasName())
			{
				// static
				if( i3::generic_is_iequal( pControl->GetNameString(), pszName) )
				{
					pControl->setEventReceiver( pReceiver);
					return pControl;
					break;
				}
			}
			//============================================

			if( pControl->getFirstChild() != nullptr)
			{
				if((i3GuiControl*)pControl->getNext() != nullptr)
				{
					m_ControlBackUpList.push_back( (i3GuiControl*)pControl->getNext());					
				}
				pControl = (i3GuiControl*)pControl->getFirstChild();				
			}
			else
			{				
				pControl = (i3GuiControl*)pControl->getNext();
				if(pControl == nullptr)
				{
					if( m_ControlBackUpList.size() != 0)
					{
						pControl =	m_ControlBackUpList.back();		// (i3GuiControl*)m_ControlBackUpList.GetLast();
//						m_ControlBackUpList.erase(m_ControlBackUpList.begin() + m_ControlBackUpList.size() - 1);			// -1이 없었는데 버그로 보이며, 원칙상 반드시 필요하다..(2012.05.15.수빈)
						m_ControlBackUpList.pop_back();			// 이걸로 그냥 간단이 교체했다..(2012.05.15.수빈)
					}
				}	
			}
		}

		pWindow = (i3GuiWindow*)(pWindow->getNext());
	}

	// 
	I3ASSERT( "Control didn't exist");
	return nullptr;
}

i3GuiControl *	i3GuiRoot::_Rec_FindChildControlByName( i3GuiControl * pCtrl, const char * pszName)
{
	if( pCtrl->hasName())
	{
		if( i3::generic_is_iequal( pCtrl->GetNameString(), pszName) )
			return pCtrl;
	}

	i3GuiControl * pTemp = (i3GuiControl *) pCtrl->getFirstChild();

	if( pTemp != nullptr)
	{
		pTemp = _Rec_FindChildControlByName( pTemp, pszName);
		if( pTemp != nullptr)
			return pTemp;
	}

	pTemp = (i3GuiControl *) pCtrl->getNext();
	if( pTemp != nullptr)
	{
		pTemp = _Rec_FindChildControlByName( pTemp, pszName);
	}

	return pTemp;
}

i3GuiControl *	 i3GuiRoot::FindChildControlByName( const char * pszName)
{
	i3GuiWindow* pWindow = ((i3GuiWindow *)getFirstChild());
	i3GuiControl * pRoot = (i3GuiControl*) pWindow->getFirstChild();

	return _Rec_FindChildControlByName( pRoot, pszName);
}

bool i3GuiRoot::isControlOnceUpdate(void)
{
	return s_ControlOnceUpdate;
}

i3Viewer* i3GuiRoot::getViewer(void)					
{ 
	return s_pViewer;	
}

void i3GuiRoot::ChangeResolution(i3Viewer* pViewer)
{
	s_pViewer = pViewer;

	s_ControlOnceUpdate = false;

	if( s_bChangeResolution)
	{
		i3GuiRoot::m_sfGuiResolutionWidth = (REAL32)pViewer->GetViewWidth() / I3GUI_RESOLUTION_WIDTH;
		i3GuiRoot::m_sfGuiResolutionHeight = (REAL32)pViewer->GetViewHeight() / I3GUI_RESOLUTION_HEIGHT;

		i3GuiRoot::m_sfGuiResolutionWidthInGame = (REAL32)pViewer->GetViewWidth() / I3GUI_RESOLUTION_WIDTH;
		i3GuiRoot::m_sfGuiResolutionHeightInGame = (REAL32)pViewer->GetViewHeight() / I3GUI_RESOLUTION_HEIGHT;
	}
	else
	{
		i3GuiRoot::m_sfGuiResolutionWidth = 1.0f;
		i3GuiRoot::m_sfGuiResolutionHeight = 1.0f;
	}
}

bool i3GuiRoot::isChangeResolution(void)			
{ 
	return s_bChangeResolution;		
}



void i3GuiRoot::_Rec_ChangeUpdate( i3GuiObjBase * pBase)
{
	if( pBase == nullptr)
		return;

	pBase->m_bOnceUpdate = false;

	i3GuiObjBase * pBaseChild = ( i3GuiObjBase * )pBase->getFirstChild();
	if( pBaseChild) _Rec_ChangeUpdate( pBaseChild);

	i3GuiObjBase * pBaseNext= ( i3GuiObjBase * )pBase->getNext();
	if( pBaseNext) _Rec_ChangeUpdate( pBaseNext);
}

void i3GuiRoot::NoticeGuiObjBaseDestroy(i3GuiObjBase * pObj)
{
	if( m_pOldOnMouse == pObj) m_pOldOnMouse = nullptr;
}
