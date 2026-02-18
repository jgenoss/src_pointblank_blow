#include "i3FrameworkPCH.h"
#include "i3UIManager.h"
#include "../i3Framework.h"

bool i3UIManager::DispatchLButtonDown( UINT32 nFlag, POINT point)
{
	if( m_bDisableInput)	return true;

	// 해당 Point에 위치한 Window를 검색한다.
	I3UI_PICK_INFO info;

	ConvertMouseCoord( &point);

	info.init( (REAL32) point.x, (REAL32) point.y);

	i3UIFrameWnd * pWnd = HitTest( &info);

	//if( info.m_pPickedCtrl != nullptr)
		//I3TRACE("i3UIManager::DispatchLButtonDown    %s\n", info.m_pPickedCtrl->GetMeta()->GetClassNameA());

	if( m_bExclusiveActive)
	{
		// Modal 상태에서 ModalWindow 외의 다른 window가 선택되었다면 무시.
		if( pWnd != GetFocusFrameWnd()) {
			// 다른 Window가 선택되었으면 CANCEL주고 EndModal 했었는데 Notify로 바꿈 (일단 RESERVED10 이벤트)
			GetFocusFrameWnd()->Notify( I3UI_EVT_RESERVED10, 0, 0);
			return false;
		}
	}

	// 해당 Window를 Activate 시킨다.
	if( pWnd != GetFocusFrameWnd())
	{
		SetActiveFrameWnd( pWnd, false, false);		// nullptr일 수 있다.
	}

	// 실제 Input Dispatch는 하지 않는다.
	// 그것은 Framework가 실질적인 Dispatch 처리를 하기 때문이다.

	// Prev
	if( i3::same_of<i3UIEditBox*>( info.m_pPickedCtrl))
	{
		//i3UIControl * pPrevFocus = (i3UIControl*)m_pFramework->getFocus();

		if( pWnd != nullptr)
			pWnd->SetFocusCtrl( nullptr);

		m_pFramework->SetFocus(info.m_pPickedCtrl);

		// 이전 focus와 pick이 같으면, filtering되어서 onsetfocus가 안불려 지는데다가, 다시 찍을때 ime의 wnd가 killfocus된다.
//		if( pPrevFocus != nullptr && pPrevFocus == info.m_pPickedCtrl)
//		{
//			info.m_pPickedCtrl->OnSetFocus();
//		}
	}
	else
	{
		if( pWnd != nullptr)
		{
			pWnd->SetPreFocusCtrl( info.m_pPickedCtrl);
			m_pFramework->SetFocus( pWnd);
			pWnd->SetFocusCtrl( info.m_pPickedCtrl);
		}
		else
		{
			 m_pFramework->SetFocus( info.m_pPickedCtrl);
		}
	}

	return true;
}

bool i3UIManager::DispatchLButtonUp( UINT32 nFlag, POINT point)
{
	if( m_bDisableInput)	return true;

	ConvertMouseCoord( &point);

	// 해당 Point에 위치한 Window를 검색한다.
	I3UI_PICK_INFO info;

	info.init( (REAL32) point.x, (REAL32) point.y);

	i3UIFrameWnd * pWnd = HitTest( &info);

	//if( info.m_pPickedCtrl != nullptr)
		//I3TRACE("i3UIManager::DispatchLButtonUp    %s\n", info.m_pPickedCtrl->GetMeta()->GetClassNameA());

	if( m_bExclusiveActive)
	{
		// Modal 상태에서 ModalWindow 외의 다른 window가 선택되었다면 무시.
		if( pWnd != GetFocusFrameWnd())
			return false;
	}

	return true;
}

bool i3UIManager::DispatchLButtonDblClk( UINT32 nFlag, POINT point)
{
	if( m_bDisableInput)	return true;

	ConvertMouseCoord( &point);

	// 해당 Point에 위치한 Window를 검색한다.
	I3UI_PICK_INFO info;

	info.init( (REAL32) point.x, (REAL32) point.y);

	i3UIFrameWnd * pWnd = HitTest( &info);

	//if( m_bExclusiveActive)
	//{
	//	// Modal 상태에서 ModalWindow 외의 다른 window가 선택되었다면 무시.
	//	if( pWnd != GetFocusFrameWnd())
	//		return false;
	//}

	//if( info.m_pPickedCtrl != nullptr)
		//I3TRACE("i3UIManager::DispatchLButtonDblClk    %s\n", info.m_pPickedCtrl->GetMeta()->GetClassNameA());

	if( m_bExclusiveActive)
	{
		// Modal 상태에서 ModalWindow 외의 다른 window가 선택되었다면 무시.
		if( pWnd != GetFocusFrameWnd()) {
			// 다른 Window가 선택되었으면 CANCEL주고 EndModal 했었는데 Notify로 바꿈 (일단 RESERVED10 이벤트)
			GetFocusFrameWnd()->Notify( I3UI_EVT_RESERVED10, 0, 0);
			return false;
		}
	}

	// 해당 Window를 Activate 시킨다.
	if( pWnd != GetFocusFrameWnd())
	{
		SetActiveFrameWnd( pWnd, false, false);		// nullptr일 수 있다.
	}

	// 실제 Input Dispatch는 하지 않는다.
	// 그것은 Framework가 실질적인 Dispatch 처리를 하기 때문이다.

	// Prev
	if(  i3::same_of<i3UIEditBox*>(info.m_pPickedCtrl))
	{
		i3UIControl * pPrevFocus = (i3UIControl*)m_pFramework->getFocus();

		m_pFramework->SetFocus(info.m_pPickedCtrl);

		// 이전 focus와 pick이 같으면, filtering되어서 onsetfocus가 안불려 지는데다가, 다시 찍을때 ime의 wnd가 killfocus된다.
		if( pPrevFocus != nullptr && pPrevFocus == info.m_pPickedCtrl)
		{
			info.m_pPickedCtrl->OnSetFocus();
		}
	}
	else
	{
		if( pWnd != nullptr)
		{
			pWnd->SetPreFocusCtrl( info.m_pPickedCtrl);
			m_pFramework->SetFocus( pWnd);

			pWnd->SetFocusCtrl( info.m_pPickedCtrl);
		}
		else
		{
			 m_pFramework->SetFocus( info.m_pPickedCtrl);
		}
	}

	return true;
}

bool i3UIManager::DispatchMButtonDown( UINT32 nFlag, POINT point)
{
	if( m_bDisableInput)	return true;

	ConvertMouseCoord( &point);

	// 해당 Point에 위치한 Window를 검색한다.
	I3UI_PICK_INFO info;

	info.init( (REAL32) point.x, (REAL32) point.y);

	i3UIFrameWnd * pWnd = HitTest( &info);

	if( m_bExclusiveActive)
	{
		// Modal 상태에서 ModalWindow 외의 다른 window가 선택되었다면 무시.
		if( pWnd != GetFocusFrameWnd())
			return false;
	}

	return true;
}

bool i3UIManager::DispatchMButtonUp( UINT32 nFlag, POINT point)
{
	if( m_bDisableInput)	return true;

	ConvertMouseCoord( &point);

	// 해당 Point에 위치한 Window를 검색한다.
	I3UI_PICK_INFO info;

	info.init( (REAL32) point.x, (REAL32) point.y);

	i3UIFrameWnd * pWnd = HitTest( &info);

	if( m_bExclusiveActive)
	{
		// Modal 상태에서 ModalWindow 외의 다른 window가 선택되었다면 무시.
		if( pWnd != GetFocusFrameWnd())
			return false;
	}

	return true;
}

bool i3UIManager::DispatchRButtonDown( UINT32 nFlag, POINT point)
{
	if( m_bDisableInput)	return true;

	ConvertMouseCoord( &point);

	// 해당 Point에 위치한 Window를 검색한다.
	I3UI_PICK_INFO info;

	info.init( (REAL32) point.x, (REAL32) point.y);

	i3UIFrameWnd * pWnd = HitTest( &info);

	//if( info.m_pPickedCtrl != nullptr)
		//I3TRACE("i3UIManager::DispatchRButtonDown    %s\n", info.m_pPickedCtrl->GetMeta()->GetClassNameA());

	if( m_bExclusiveActive)
	{
		// Modal 상태에서 ModalWindow 외의 다른 window가 선택되었다면 무시.
		if( pWnd != GetFocusFrameWnd()) {
			// 다른 Window가 선택되었으면 CANCEL주고 EndModal 했었는데 Notify로 바꿈 (일단 RESERVED10 이벤트)
			GetFocusFrameWnd()->Notify( I3UI_EVT_RESERVED10, 0, 0);
			return false;
		}
	}

	// 해당 Window를 Activate 시킨다.
	if( pWnd != GetFocusFrameWnd())
	{
		SetActiveFrameWnd( pWnd, false, false);		// nullptr일 수 있다.
	}

	// 실제 Input Dispatch는 하지 않는다.
	// 그것은 Framework가 실질적인 Dispatch 처리를 하기 때문이다.

	// Prev
	if(  i3::same_of<i3UIEditBox*>(info.m_pPickedCtrl))
	{
		i3UIControl * pPrevFocus = (i3UIControl*)m_pFramework->getFocus();

		m_pFramework->SetFocus(info.m_pPickedCtrl);

		// 이전 focus와 pick이 같으면, filtering되어서 onsetfocus가 안불려 지는데다가, 다시 찍을때 ime의 wnd가 killfocus된다.
		if( pPrevFocus != nullptr && pPrevFocus == info.m_pPickedCtrl)
		{
			info.m_pPickedCtrl->OnSetFocus();
		}
	}
	else
	{
		if( pWnd != nullptr)
		{
			m_pFramework->SetFocus( pWnd);

			pWnd->SetFocusCtrl( info.m_pPickedCtrl);
		}
		else
		{
			 m_pFramework->SetFocus( info.m_pPickedCtrl);
		}
	}

	return true;
}

bool i3UIManager::DispatchRButtonUp( UINT32 nFlag, POINT point)
{
	if( m_bDisableInput)	return true;

	ConvertMouseCoord( &point);

	// 해당 Point에 위치한 Window를 검색한다.
	I3UI_PICK_INFO info;

	info.init( (REAL32) point.x, (REAL32) point.y);

	i3UIFrameWnd * pWnd = HitTest( &info);

	if( m_bExclusiveActive)
	{
		// Modal 상태에서 ModalWindow 외의 다른 window가 선택되었다면 무시.
		if( pWnd != GetFocusFrameWnd())
			return false;
	}

	return true;
}

void i3UIManager::resetOnMouseTarget(void)
{
	m_pFramework->setOnMouseTarget( nullptr);

	if( m_pLastOnMouseTarget != nullptr)
	{
		m_pLastOnMouseTarget->OnLeaveMouse();
		m_pLastOnMouseTarget = nullptr;
	}

	m_pCurActiveFrameWnd = nullptr;
}

bool i3UIManager::DispatchMouseMove( UINT32 nFlag, POINT point)
{
	ConvertMouseCoord( &point);

	m_nCurrentMousePos = point;

	// 해당 Point에 위치한 Window를 검색한다.
	I3UI_PICK_INFO info;

	info.init( (REAL32) point.x, (REAL32) point.y);

	i3UIFrameWnd * pWnd = HitTest( &info);

//	if( (m_pLastOnMouseTarget != nullptr) &&
//		(m_pLastOnMouseTarget != info.m_pPickedCtrl))
	{
//		I3TRACE( "OnLeaveMouse..%s\n", m_pLastOnMouseTarget->GetName());
///		m_pLastOnMouseTarget->OnLeaveMouse();
	}

	if( pWnd == nullptr)
	{
		resetOnMouseTarget();
		return true;
	}

	if( m_bExclusiveActive)
	{
		if( pWnd != m_pCurActiveFrameWnd)
		{
			// Modal 상태에서 다른 window의 Control에 Mouse Cursor가 갔다면...
			// 무시해 주어야 한다.
			resetOnMouseTarget();
			return false;
		}	
	}

	if( m_bDisableInput)	
	{
		resetOnMouseTarget();
		return false;
	}

	if( info.m_pPickedCtrl != nullptr)
	{
		if( m_pLastOnMouseTarget != info.m_pPickedCtrl)
		{
			if( m_pLastOnMouseTarget != nullptr ) m_pLastOnMouseTarget->OnLeaveMouse();

			//I3TRACE( "OnEnterMouse..%s\n", info.m_pPickedCtrl->GetName());
			info.m_pPickedCtrl->OnEnterMouse( (REAL32) point.x, (REAL32) point.y, nFlag);
			// Editbox 등의 Control로 MouseMove 이벤트가 가질 않아서 추가 ;jisoo
			// MouseMove 시 자꾸 focus가 바뀌어서 일단 다시 주석처리
			//pWnd->SetFocusCtrl( info.m_pPickedCtrl);
		}
	}

	m_pLastOnMouseTarget = info.m_pPickedCtrl;
	//m_pFramework->setOnMouseTarget( info.m_pPickedCtrl);

	m_pFramework->setOnMouseTarget( pWnd);

	return true;
}

bool i3UIManager::DispatchMouseWheel( UINT nFlag, short zDelta, POINT point)
{
	if( m_bDisableInput)	return true;

	ConvertMouseCoord( &point);

	return true;
}

 static bool CheckConsoleShowKey(UINT32 nKey, bool testKeyOnly=false)
 {
	 if(!i3UIManager::IsUsingConsole(true))
		 return false;

	//콘솔여는키 (Ctrl+'`') 확인
	if((GetKeyState(VK_LCONTROL) & 0x80) && nKey == 0xc0)
	{
		if(!testKeyOnly)
			i3UIManager::ShowConsole(!i3UIManager::IsShowConsole());

		return true;
	}

	return false;
 }

bool i3UIManager::DispatchKeyDown( UINT32 nKey)
{
	if(!IsUsingConsole() && m_bDisableInput)	
		return true;

	if(CheckConsoleShowKey(nKey))
		return false; // msg skip, 콘솔오픈 키 처리가 되었다면 리턴
	
	i3UIScene* pScene = (i3UIScene*)getFirstChild();

	if(IsShowConsole())
		pScene = FindConsoleScene(pScene); // 콘솔이 보이고있다면 콘솔UI에대해서만 처리

	while( pScene != nullptr)
	{
		if( pScene->isEnable())
		{
			pScene->OnEvent( I3UI_EVT_KEYDOWN, nKey);	//nKey <= ASCII (ex: 65 for 'A' pressed)
		}

		pScene = (i3UIScene*)pScene->getNext();

		if(IsShowConsole())
			pScene = FindConsoleScene(pScene);
	}

	return true;
}

bool i3UIManager::DispatchKeyUp( UINT32 nKey)
{
	if(!IsShowConsole() && m_bDisableInput)	
		return true;

	return true;
}

void i3UIManager::OnChangeFocus( i3Framework * pFramework, i3EventReceiver * pFrom, i3EventReceiver * pTo)
{
	if( pFrom != nullptr)
	{
		if( i3::kind_of<i3UIControl* >(pFrom))
		{
			i3UIControl * pWnd = (i3UIControl *) pFrom;

			pWnd->OnKillFocus( (i3UIControl*) pTo);
		}
	}

	if( pTo != nullptr)
	{
		if( i3::kind_of<i3UIControl* >(pTo))
		{
			i3UIControl * pWnd = (i3UIControl *) pTo;
			pWnd->OnSetFocus();
		}
	}
}

bool i3UIManager::DispatchActivate( bool bActivate)
{
	if( !bActivate)
	{
//		if( m_pFramework->getFocus() != nullptr)
//		{
//			m_pFramework->SetFocus(nullptr);
//		}
	}
	return true;
}

bool i3UIManager::DispatchSetFocus(void)
{
	i3EventReceiver * pFocus = m_pFramework->getFocus();

	if( !i3::kind_of<i3UIControl*>(pFocus) )
		return true;

	i3UIControl * pCtrl = (i3UIControl *) pFocus;

	pCtrl->OnSetFocus();

	return true;
}

bool i3UIManager::DispatchKillFocus(void)
{
	i3EventReceiver * pFocus = m_pFramework->getFocus();

	if(	!i3::kind_of<i3UIControl*>(pFocus))
		return true;

	i3UIControl * pCtrl = (i3UIControl *) pFocus;

	pCtrl->OnKillFocus();

	return true;
}

bool i3UIManager::DispatchInput(void)
{
	if( m_bDisableInput)	return true;

	return true;
}

