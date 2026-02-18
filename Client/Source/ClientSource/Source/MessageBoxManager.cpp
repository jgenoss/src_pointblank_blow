#include "pch.h"
#include "GlobalVariables.h"
#include "MessageBoxManager.h"

#include "UI/UIMessageBox.h"
#include "UI/UIMainFrame.h"
#include "UI/UILogin.h"

#include "i3Base/string/ext/utf16_to_mb.h"


CMsgBoxCallBack::CMsgBoxCallBack( LPCTSTR _File, int _Line, MSG_BOX_CALLBACK CallBack, bool IfOkExit, bool bForcedExit )
: m_File( _File)
, m_Line(_Line)
, m_CallBack(CallBack)
, m_IfOkExit(IfOkExit)
, m_bForcedExit(bForcedExit)
{
}

void CMsgBoxCallBack::OnCallback( void * pThis, INT32 nParam ) const
{
	if( m_IfOkExit && nParam == MBR_OK)
	{
		TCHAR	temp[1024];
		i3::snprintf(temp, 1024, _T("-- Exit From Msg CallBack\n  File:%s\n  Line:%d"), m_File, m_Line);
		I3PRINTLOG(I3LOG_DEFALUT, temp);

		if( m_bForcedExit == true)
			g_pFramework->GotoNextStage("Exit");
		else
			GameEventSender::i()->SetEvent( EVENT_GAME_END );

		if( i3::same_of<UILogin*>(g_pFramework->GetCurrentStage()))
		{
			g_pFramework->GotoNextStage("Exit");
		}
	}

	if ( m_CallBack )
		m_CallBack(pThis, nParam);
}




I3_CLASS_INSTANCE(CMessageBoxManager);

CMessageBoxManager::CMessageBoxManager()
{
	m_pSceneNode = i3Node::new_object();
}

CMessageBoxManager::~CMessageBoxManager()
{
	_Release();
}

UIMessageBox * CMessageBoxManager::OpenMessageBox(const i3::rc_wstring& strMessage, const i3::rc_wstring& strCaption,
												  INT32 flagStyle,
												const CMsgBoxCallBack & fnEventCB,void*pThis,INT32 nFontSize, INT32 nAlignX, INT32 nAlignY)
{
	MsgBoxOpenParam	Param;
	Param.Msg		= strMessage;
	Param.Caption	= strCaption;
	Param.Style		= flagStyle;
	Param.CB		= fnEventCB;
	Param.pThis		= pThis;
	Param.FontSize	= nFontSize;
	Param.AlignX	= nAlignX;
	Param.AlignY	= nAlignY;

	return OpenMessageBox(Param);
}

UIMessageBox * CMessageBoxManager::OpenMessageBox( const MsgBoxOpenParam & Param)
{
	I3ASSERT(m_pSystemMessageRoot);
	I3ASSERT(m_pGameMessageRoot);

	if( m_pOldMessageBox)
	{
		UIMessageBox * pFind = _FindMessageBox( Param.Msg);

		//	동일한 메시지를 무한 반복적으로 호출하는 것을 막기 위해 체크합니다. 이 함수를 계속 호출하는 코드를 수정해야 합니다. komet 
		if( pFind != nullptr)
		{
			I3TRACE( "\"%s\" : \n이전 메시지와 동일한 메시지를 반복적으로 호출하고 있습니다.\n", Param.Msg );
			return nullptr;
		}
	}

	i3GameObjBase* pMessageRoot;
	
	if( (Param.Style & MBF_SYSTEM) == MBF_SYSTEM)
	{
		pMessageRoot = m_pSystemMessageRoot;
	}
	else //if( flagStyle & MBF_GAME == MBF_GAME)	// 시스템 아니면 일단 게임메시지 취급
	{
		pMessageRoot = m_pGameMessageRoot;
		m_nCountGameMessgeTop++;
	}

	UIMessageBox* pMessageBox = UIMessageBox::new_object();
	if( pMessageBox->Create( pMessageRoot, this, Param)) // pMessageRoot의 Child 등록
	{

		if( (Param.Style & MBF_GAME) == MBF_GAME )
		{
			_ArrayGameMessageBox();
		}
		else
		{
			i3::pack::POINT2D ptCenter = g_pFramework->GetCenterPosition(pMessageBox->GetWidth(), pMessageBox->GetHeight());

			pMessageBox->SetPosition(ptCenter.x, ptCenter.y);
		}

		_EnableTop();
	}
	else
	{
		I3_SAFE_RELEASE(pMessageBox);
	}

	m_pOldMessageBox = pMessageBox;
	m_is_open_message_box = true;
	return pMessageBox;
}

UIMessageBox * CMessageBoxManager::_FindMessageBox( const i3::rc_wstring& strMessage)
{
	UINT32 crc = CRC32(0xFFFFFFFF, (UINT8*) strMessage.c_str(), strMessage.size() * sizeof(wchar_t) );

	i3GameNode * pChild = m_pGameMessageRoot->getFirstChild();
	while(pChild)
	{
		if( i3::same_of<UIMessageBox*>(pChild))
		{
			UIMessageBox * pFind = (UIMessageBox*) pChild;

			//	동일한 내용의 메시지 박스가 출력되어 있다.
			if( crc == pFind->GetCheckCRC())
				return pFind;
		}

		pChild = pChild->getNext();
	}

	pChild = m_pSystemMessageRoot->getFirstChild();
	while(pChild)
	{
		if( i3::same_of<UIMessageBox*>(pChild))
		{
			UIMessageBox * pFind = (UIMessageBox*) pChild;

			//	동일한 내용의 메시지 박스가 출력되어 있다.
			if( crc == pFind->GetCheckCRC())
				return pFind;
		}

		pChild = pChild->getNext();
	}

	return nullptr;
}


void CMessageBoxManager::CloseMessageBox( UIMessageBox* pMessageBox)
{
	I3ASSERT(pMessageBox);
	
	if( m_pGameMessageRoot->GetChildIndex(pMessageBox) > -1)
	{
		;
	}
	else
	{
		// 존재하지 않는 메시지박스
		return;
	}

	pMessageBox->CloseDirect();

	if( pMessageBox->getParent() != nullptr)
		pMessageBox->getParent()->RemoveChild( pMessageBox);

	// 다음 messagebox가 있으면 열어준다.
	I3_SAFE_RELEASE(pMessageBox);

	m_pOldMessageBox = nullptr;

	_EnableTop();
}

void CMessageBoxManager::CloseAllMessageBox(void)
{
	I3ASSERT(m_pSystemMessageRoot);
	I3ASSERT(m_pGameMessageRoot);

	// 시스템 메시지박스를 모두 닫는다.
	i3GameNode* systemBox = m_pSystemMessageRoot->getFirstChild();
	
	while(systemBox)
	{
		UIMessageBox* messageBox = (UIMessageBox*)systemBox;
		systemBox = messageBox->getNext();
		messageBox->CloseDirect();
		if( messageBox->getParent() != nullptr)
			messageBox->getParent()->RemoveChild( messageBox);
		I3_SAFE_RELEASE(messageBox);
	}


	// 게임 메시지박스를 모두 닫는다.
	i3GameNode* gameBox = m_pGameMessageRoot->getFirstChild();

	while(gameBox)
	{
		UIMessageBox* messageBox = (UIMessageBox*)gameBox;
		gameBox = messageBox->getNext();
		CloseMessageBox( messageBox);
	}

    // 메시지박스 활성화 정보를 초기화한다.
	m_nCountGameMessgeTop = 0;
	m_pOldMessageBox = nullptr;
}

void CMessageBoxManager::CloseGameMessageBox(void)
{
	I3ASSERT(m_pGameMessageRoot);

	// 게임 메시지박스를 모두 닫는다.
	i3GameNode* gameBox = m_pGameMessageRoot->getFirstChild();

	while(gameBox)
	{
		UIMessageBox* messageBox = (UIMessageBox*)gameBox;
		gameBox = messageBox->getNext();
		//	messageBox->SetEnable( false);
		messageBox->CloseDirect();
	}

	// 메시지박스 활성화 정보를 초기화한다.
	m_nCountGameMessgeTop = 0;
	m_pOldMessageBox = nullptr;

}

bool CMessageBoxManager::Create(i3Node* pParentNode,bool bEnable)
{
	if( !i3GameObjBase::Create(pParentNode,bEnable))
	{
		return false;
	}
	
	// 일반 메시지 박스 기반 준비
	{
		m_pGameMessageRoot = i3GameObjBase::new_object();

		i3Node* pNode = i3Node::new_object_ref();
		m_pGameMessageRoot->SetNode( pNode);
		if( !m_pGameMessageRoot->Create(GetNode()))
		{
			_Release();
			return false;
		}

		I3_GAMENODE_ADDCHILD( this, m_pGameMessageRoot);
	}

	// 시스템 경고 메시지 박스 기반 준비
	{
		m_pSystemMessageRoot = i3GameObjBase::new_object();
		i3Node* pNode = i3Node::new_object_ref();
		m_pSystemMessageRoot->SetNode(pNode);
		if( !m_pSystemMessageRoot->Create(GetNode()))
		{
			_Release();
			return false;
		}

		I3_GAMENODE_ADDCHILD( this, m_pSystemMessageRoot);
	}

	return true;
}

bool CMessageBoxManager::OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code)
{
	if( code == I3_EVT_CODE_ACTIVATE)
	{
		if (I3_EVT_INPUT == event)
		{
			REAL32 rDeltaSeconds;
			i3mem::Copy(&rDeltaSeconds, &param2, sizeof(UINT32));
			i3InputDeviceManager* pMgr = (i3InputDeviceManager*)pObj;

			const INT32 sys_count = m_pSystemMessageRoot->GetChildCount();

			if ( sys_count > 0)
			{
				UIMessageBox* pos_last = (UIMessageBox*)m_pSystemMessageRoot->GetChildByIndex( sys_count - 1);
					
				if (pos_last->IsClosing() == false)
				{
					return pos_last->OnEvent(event, pObj, param2, code);
				}
			}
		
			const INT32 game_count = m_pGameMessageRoot->GetChildCount();
			if (game_count > 0)
			{
				UIMessageBox* pos_last = (UIMessageBox*)m_pGameMessageRoot->GetChildByIndex( game_count - 1);

				if (pos_last->IsClosing() == false)
				{
					return pos_last->OnEvent(event, pObj, param2, code);
				}
			}

			return _ProcessInput(pMgr);		
		}
	}

	return false;
}

void CMessageBoxManager::OnResize(void)
{

}

void CMessageBoxManager::OnUpdate(REAL32 rDeltaSeconds)
{
	i3GameObjBase::OnUpdate(rDeltaSeconds);

	// 시스템 메시지 최상위 처리
	if ( m_bPhaseTransition )		// 페이즈 전환 중이면 설정된 AllHide를 유지하도록 함.
	{
		UIMainFrame* main_frame = g_pFramework->GetUIMainframe();
		
		if (main_frame)
		{
			if ( main_frame->IsPhaseAndSubPhaseSlideDone() == false )
			{
				return;
			}
		}

		m_bPhaseTransition = false;
	}
	
	
	UIMessageBox* pos = (UIMessageBox*)m_pSystemMessageRoot->getFirstChild();

	if( pos)
	{
		while( pos->getNext())
			pos = (UIMessageBox*)pos->getNext();
	
		if( pos->IsIdentified())
		{
			pos->getParent()->RemoveChild( pos);
			I3_SAFE_RELEASE( pos);
			m_pOldMessageBox = nullptr;

			_EnableTop();
		}
		else if (pos->IsHided() )
		{
			_EnableTop();
		}

		return;
	}

	// Process GameMessage
	pos = (UIMessageBox*)m_pGameMessageRoot->getFirstChild();

	if( pos)
	{
		while( pos->getNext())
			pos = (UIMessageBox*)pos->getNext();

		if(pos->IsIdentified())
		{
			m_nCountGameMessgeTop--;

			pos->getParent()->RemoveChild( pos);
			I3_SAFE_RELEASE(pos);

			m_pOldMessageBox = nullptr;

			_EnableTop();
		}
		else if (pos->IsHided() )
		{
			_EnableTop();
		}
	}
}

void CMessageBoxManager::_RemoveAll(void)
{
	I3ASSERT(m_pGameMessageRoot);
	I3ASSERT(m_pSystemMessageRoot);
    
	m_pSystemMessageRoot->RemoveAllChild();
	m_pGameMessageRoot->RemoveAllChild();
}

void CMessageBoxManager::_Release(void)
{
	CloseAllMessageBox();

	if(m_pGameMessageRoot)
	{
		m_pGameMessageRoot->RemoveAllChild();
		I3_SAFE_RELEASE(m_pGameMessageRoot);
	}

	if(m_pSystemMessageRoot)
	{
		m_pSystemMessageRoot->RemoveAllChild();
		I3_SAFE_RELEASE(m_pSystemMessageRoot);
	}
}

void CMessageBoxManager::_EnableTop(void)
{
	bool bEnabledTop = false;

	// _DisableAll() 상태라 가정한다
	UIMessageBox* pos = (UIMessageBox*)m_pSystemMessageRoot->getFirstChild();
	if(pos)
	{
		while(pos->getNext())
		{
			pos->Hide();

			pos = (UIMessageBox*)pos->getNext();
		}
		
		if (pos->IsClosing() == false)
		{
			pos->OnEntranceStart();
			bEnabledTop = true;
		}
	}

	if( bEnabledTop == false)
	{
		pos = (UIMessageBox*)m_pGameMessageRoot->getFirstChild();

		if(pos)
		{
			while(pos->getNext())
			{
				pos->Hide();
				pos = (UIMessageBox*)pos->getNext();
			}
			
			if (pos->IsClosing() == false)
			{
				pos->OnEntranceStart();
				bEnabledTop = true;
			}
		}
	}

	if( bEnabledTop)
	{
		if (g_pGUIRoot)
		{
			i3GuiObjBase * pControl = i3GuiRoot::getFocusControl();

			//	kill focus
			if(pControl)
			{
				pControl->OnKillFocus();

				i3GuiRoot::setFocusControl(nullptr);	
			}

			i3GuiRoot::setGuiDisableInput(true);
		}

		if( i3UI::getManager())
		{
			//i3UI::setDisableUIInput( true);
		}

		g_pFramework->SetGameCursor(true);
	}
	else
	{
		if(g_pGUIRoot)
		{
			i3GuiRoot::setGuiDisableInput(false);

			// ! praptor - 닉네임 입력시 IME버그를 우회하기 위한 코드입니다.
			// IME는 GUI가 비활성화되어도 입력을 받고 있습니다.
			/*i3GuiObjBase* pObj = i3GuiRoot::getFocusControl();

			if (i3::same_of<i3GuiEditBox*>(pObj))
			{
				((i3GuiEditBox*)pObj)->InputDone();
			}*/
		}

		if( i3UI::getManager())
		{
			i3UI::setDisableUIInput( false);
		}

		g_pFramework->SetGameCursor(false);
	}
}

void CMessageBoxManager::PreventMessageBoxWhenPhaseChange()
{
	
	// 화면에 등장한 메세지 박스를 모두 Hide처리한다..
	// 여기서는 선두에 등장한 가장 마지막 메세지 박스만 막는다..

	bool bEnabledTop = false;

	UIMessageBox* pos = (UIMessageBox*)m_pSystemMessageRoot->getFirstChild();

	if(pos)
	{
		while(pos->getNext())
		{
			pos->Hide( true);
			pos = (UIMessageBox*)pos->getNext();
		}
		
		pos->Hide( true);
		bEnabledTop = true;
	}

	if (bEnabledTop == false)
	{
		pos = (UIMessageBox*)m_pGameMessageRoot->getFirstChild();

		if(pos)
		{
			while(pos->getNext())
			{
				pos->Hide( true);
				pos = (UIMessageBox*)pos->getNext();
			}

			pos->Hide( true);
		}
	}

	m_bPhaseTransition = true;			// 이건 업데이트에서 조건에 의해 풀어버린다..
}



bool CMessageBoxManager::_ProcessInput(i3InputDeviceManager* pMrg)
{
	// 마우스
	i3InputMouse* pMouse = pMrg->GetMouse();

	m_fPosX = pMouse->GetX();
	m_fPosY = pMouse->GetY();

	UINT32 mkOldPress = m_mkPress;
	m_mkPress = MBIS_NULL;
	if( I3I_MOUSE_LBUTTON & pMouse->GetButtonState())
	{
		m_mkPress |= MBIS_LBUTTON_PRESS;
	}

	m_mkStroke = ~mkOldPress & m_mkPress;
	m_mbInputState = m_mkPress;

	if( m_mkPress & I3I_MOUSE_LBUTTON)
	{
		if( m_mkStroke & I3I_MOUSE_LBUTTON)
		{
			m_mbInputState |= MBIS_LBUTTON_DOWN;
		}
	}
	else
	{
		if( mkOldPress & I3I_MOUSE_LBUTTON)
		{
			m_mbInputState |= MBIS_LBUTTON_UP;
		}
	}

	// 키보드
	i3InputKeyboard* pKeyboard = pMrg->GetKeyboard();

	if (pKeyboard->GetStrokeState(I3I_KEY_ENTER))
	{
		m_mbInputState |= MBIS_ENTER_DOWN;
		return true;
	}

	if (pKeyboard->GetStrokeState(I3I_KEY_ESC))
	{
		m_mbInputState |= MBIS_ESC_DOWN;
		return true;
	}

	return false;
}

REAL32 CMessageBoxManager::GetInputPositionX(void) const
{
	return m_fPosX;
}

REAL32 CMessageBoxManager::GetInputPositionY(void) const
{
	return m_fPosY;
}

INT32 CMessageBoxManager::GetInputState(void) const
{
	return m_mbInputState;
}

bool CMessageBoxManager::IsEnabled(void) const
{
	if( m_pGameMessageRoot && m_pGameMessageRoot->getFirstChild() )
	{
		return true;
	}

	if( m_pSystemMessageRoot && m_pSystemMessageRoot->getFirstChild() )
	{
		return true;
	}

	return false;
}

void CMessageBoxManager::RefreshTop(void) const
{
	bool bEnabledTop = false;

	// _DisableAll() 상태라 가정한다
	UIMessageBox* pos = (UIMessageBox*)m_pSystemMessageRoot->getFirstChild();

	if( pos == nullptr) 
		pos = 	(UIMessageBox*)m_pGameMessageRoot->getFirstChild();

	if(pos)
	{
		pos->Hide();
		while(pos->getNext())
		{
			pos->Hide();

			pos = (UIMessageBox*)pos->getNext();
		}

		if (pos->IsClosing() == false)
		{
			pos->OnEntranceStart();
			bEnabledTop = true;
		}
	}
}

void CMessageBoxManager::ChangeButton( UIMessageBox * pMessageBox, const char * pUser, const char * pOK, const char * pCancel)
{
	I3ASSERT(pMessageBox);
	
	pMessageBox->ChangeButton(pUser, pOK, pCancel);
}


void CMessageBoxManager::_ArrayGameMessageBox(void)
{
	UIMessageBox* pMsgBox= (UIMessageBox*)m_pGameMessageRoot->getFirstChild();

	i3::pack::POINT2D ptCenter = g_pFramework->GetCenterPosition(pMsgBox->GetWidth(), pMsgBox->GetHeight());
	
	INT32 nOffsetX = 0;
	INT32 nOffsetY = 0;

	while( pMsgBox)
	{
		pMsgBox->SetPosition(ptCenter.x + nOffsetX, ptCenter.y + nOffsetY);

		nOffsetX += (INT32)(MB_STACK_OFFSET_X * m_fScreenRate);
		nOffsetY += (INT32)(MB_STACK_OFFSET_Y * m_fScreenRate);

		pMsgBox = (UIMessageBox*)pMsgBox->getNext();
	}
}

bool CMessageBoxManager::IsTopMsgBox(UIMessageBox* msg)
{
	if(msg == 0) return false;
	return (GetTopMsgBox() == msg);
}

UIMessageBox * CMessageBoxManager::GetTopMsgBox( void)
{
	I3ASSERT( m_pSystemMessageRoot != nullptr);
	UIMessageBox* pMsgBox = (UIMessageBox*)m_pSystemMessageRoot->getFirstChild();

	if( pMsgBox)
		return pMsgBox;

	I3ASSERT( m_pGameMessageRoot != nullptr);
	return (UIMessageBox*) m_pGameMessageRoot->getFirstChild();
}

UIMessageBox *	CMessageBoxManager::FindMsgBox( LuaState * L)
{
	I3ASSERT( m_pSystemMessageRoot != nullptr);
	UIMessageBox* pMsgBox = (UIMessageBox*)m_pSystemMessageRoot->getFirstChild();

	while( pMsgBox)
	{
		if( pMsgBox->IsLuaContext( L))
			return pMsgBox;

		pMsgBox = (UIMessageBox*) pMsgBox->getNext();
	}

	pMsgBox = (UIMessageBox*) m_pGameMessageRoot->getFirstChild();
	while( pMsgBox)
	{
		if( pMsgBox->IsLuaContext( L))
			return pMsgBox;

		pMsgBox = (UIMessageBox*) pMsgBox->getNext();
	}

	return nullptr;
}


