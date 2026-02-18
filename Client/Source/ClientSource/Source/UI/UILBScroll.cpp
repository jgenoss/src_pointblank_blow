#include "pch.h"
#include "UILBScroll.h"

#include "UIBase.h"
#include "UIMainFrame.h"

/////////////////////////////////////////////////////////

namespace
{
	class UILBScroll_Callback_None : public UILBScroll_Callback
	{
	public:
		virtual void	OnScroll( INT32 idx, INT32 old_idx ) {}				// 로드된 경우, 초기화할 호출 위치..
	} __none;

	bool enable_step_hide = false;
	bool act_step_hide = false;
	REAL32 curr_time = 0.f, start_time = 0.f;
}


UILBScroll::UILBScroll() : m_parent(nullptr), m_callback(&__none), 
		m_scn_idx(0), m_pFrameWnd(0), m_pControl(0),
		m_curr_scroll_idx(0), m_old_scroll_idx(0), 
		m_once_disable_callback(0), is_hide_action(false)
{
}

UILBScroll::~UILBScroll()
{
}

void	UILBScroll::OnCreate(UIBase* parent)
{
	m_parent = parent;
}

namespace
{
	i3UIScrollBar* _findScrollBar(i3UIFrameWnd* pWnd)
	{
		i3::vector<i3UIControl*> List;
		pWnd->GetChildControls(List);

		const size_t size = List.size();
		for (size_t i = 0; i<size; ++i)
		{
			i3UIControl* pTemp = List[i];

			if( i3::kind_of<i3UIScrollBar*>(pTemp))
				return (i3UIScrollBar*)pTemp;
		}

		return 0;
	}
}

void	UILBScroll::RegisterScene( const char* szSlotScene )		// 씬이름과 루아 함수 등록..
{
	m_scn_idx = m_parent->GetSceneCount();

	m_parent->AddScene(szSlotScene);
}

void UILBScroll::OnLoadAllScenes()
{
	m_pFrameWnd = static_cast<i3UIFrameWnd*>(m_parent->GetScene(m_scn_idx)->getFirstChild());
	m_pControl = _findScrollBar(m_pFrameWnd);
	m_old_scroll_idx = m_pControl->getScrollPos();
}

void	UILBScroll::OnUnloadAllScenes()
{
	i3UIScene* scn = m_parent->GetScene(m_scn_idx);
	if (scn && scn->isEnable() == true)
		scn->EnableScene(false);
}


i3UIScene* UILBScroll::GetScene() const
{
	return m_parent->GetScene(m_scn_idx);
}

void UILBScroll::Hide()
{
	SetEnable(false); 
	is_hide_action = true; 
}

void	UILBScroll::SetEnable(bool bEnable)
{
	is_hide_action = false;
	m_pControl->getParent()->SetEnable(bEnable);
}

bool UILBScroll::IsEnable() const
{
	if( is_hide_action == true ) return true;

	return (m_pControl->getParent()->isEnable() == true) ? true : false;
}

void UILBScroll::SetPosition(UINT32 x, UINT32 y)
{
	m_pFrameWnd->setPos((REAL32)x, (REAL32)y);
	m_pControl->setPos(0.f, 0.f);
}

void UILBScroll::SetSize(UINT32 w,UINT32 h)
{
	m_pFrameWnd->setSize((REAL32)w, (REAL32)h);
	m_pControl->setSize((REAL32)w, (REAL32)h);
}

void	UILBScroll::SetScrollPos(UINT32 idx)
{
	m_pControl->setScrollPos(idx);
}

void	UILBScroll::SetRange(UINT32 max, UINT32 min)
{
	m_pControl->setScrollRange(min, max);
}

void	UILBScroll::SetPage(UINT32 page)
{
	m_pControl->setPageSize(page);
}

void	UILBScroll::SetTick(UINT32 tick)
{
	m_pControl->setTickSize(tick);
}

void UILBScroll::SetFocus()
{
	i3UI::SetFocusControl(m_pControl);
}

void UILBScroll::KillFocus()
{
	g_pFramework->SetFocus( nullptr );
}

bool UILBScroll::IsFocused() const
{
	return ( g_pFramework->getFocus() == m_pControl );
}

void	UILBScroll::_SetScrollPos(UINT32 idx)
{
	m_once_disable_callback = true;
	m_curr_scroll_idx = idx;
	m_pControl->setScrollPos(idx);
}

/*virtual*/ bool UILBScroll::OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code /*= I3_EVT_CODE_ACTIVATE*/)
{
	if( code == I3_EVT_CODE_ACTIVATE)
	{
		if( event == I3_EVT_UI_NOTIFY)
		{
			I3UI_CONTROL_NOTIFY * pNotify = (I3UI_CONTROL_NOTIFY*) param2;
			i3UIControl * pCtrl = pNotify->m_pCtrl;
			if( pCtrl == nullptr )	return false;

			switch( pNotify->m_nEvent )
			{
				case I3UI_EVT_CLICKED:
				case I3UI_EVT_R_CLICKED:
					if (pCtrl != m_pControl)
					{
						if (i3::same_of<i3UIEditBox*>(pCtrl) ||			// 상품검색 editBox
							i3::same_of<i3UIButtonComposed3*>(pCtrl))	// 콤보박스 버튼
						{
							return false;
						}
						else
						{
							SetFocus();
							return true;
						}
					}						
					break;

				case I3UI_EVT_SCROLL:
					if( pCtrl == m_pControl )
					{
						if( m_once_disable_callback )		{	m_once_disable_callback = false;	return true;	}

						m_curr_scroll_idx = m_pControl->getScrollPos();
						m_callback->OnScroll( m_curr_scroll_idx, m_old_scroll_idx );
						m_old_scroll_idx = m_curr_scroll_idx;
						return true;
					}
					break;
				case I3UI_EVT_MOUSEWHEEL:
					if( pCtrl != m_pControl )
					{
						if( m_once_disable_callback )		{	m_once_disable_callback = false;	return true;	}

						INT32 curr_pos = m_pControl->getScrollPos();
						m_callback->OnScroll( curr_pos, m_old_scroll_idx );
						m_old_scroll_idx = curr_pos;
						return true;
					}
					break;
			}
		}
	}
	return false;
}

bool UILBScroll::HitTest(INT32 x, INT32 y) const
{
	if(m_pControl == 0)
		return false;

	if(m_pControl->isEnable() == false)
		return false;

	VEC3D* ctrlPos = m_pControl->getAccumulatedPos();
	VEC2D* ctrlSize = m_pControl->getSize();

	REAL32 right	= ctrlPos->x + ctrlSize->x;
	REAL32 bottom = ctrlPos->y + ctrlSize->y;

	if( REAL32(x) >=  ctrlPos->x && REAL32(x) <= right )
	{
		if( REAL32(y) >= ctrlPos->y && REAL32(y) <= bottom )
		{
			return true;
		}
	}

	return false;
}

void UILBScroll::SetAlpha(REAL32 factor) //0.f ~ 1.f 값
{
	if( IsEnable() == false ) return;

	m_pControl->SetAlpha(factor);
}