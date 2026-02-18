#include "pch.h"
#include "UILBScrollSlot.h"

#include "UIMainFrame.h"

#include "UISlide.h"
#include "UIBase.h"

//****************************************////****************************************//
//UILBScrollSlotSet
namespace
{
	class UILBScrollSlot_Callback_None : public UILBScrollSlot_Callback
	{
	public:
		virtual INT32	OnScroll( INT32 idx, INT32 old_idx ) { return idx; }				// 로드된 경우, 초기화할 호출 위치..
	} __none;
}

UILBScrollSlotSet::UILBScrollSlotSet() : m_idxStartSlot(0), m_bUseScroll(false), m_SlotSetCallback(&__none)
{
	m_ScrollCallback.SetObj(this);
	m_Scroll.SetCallback( &m_ScrollCallback );
}

UILBScrollSlotSet::~UILBScrollSlotSet()
{

}

void	UILBScrollSlotSet::RegisterSlotScene(const char* szSlotScene, const LuaFuncReg* Lua_Func, const char* szScrollScene)
{
	UILBSlotSet::RegisterSlotScene( szSlotScene, Lua_Func, false );

	if( m_bUseScroll == false )	return;

	m_Scroll.OnCreate(m_parent);
	m_Scroll.RegisterScene(szScrollScene);
}

void	UILBScrollSlotSet::RegisterSlotScene(const char* szSlotScene, const LuaFuncReg* Lua_Func, bool bLoaded, bool bEquip)
{
	UILBSlotSet::RegisterSlotScene( szSlotScene, Lua_Func, bLoaded );
	
	if( m_bUseScroll == false )	return;

	m_Scroll.OnCreate(m_parent);

	// UITFT 4TH : UI팀 요청으로 [Equip] 메뉴에 나타나는 스크롤바는 _black버전으로 출력합니다 gyungju.kim
	if(!bEquip)
		m_Scroll.RegisterScene("Scene/Main/PointBlankRe_Scroll.i3UIs");
	else
		m_Scroll.RegisterScene("Scene/Main/PointBlankRe_Scroll_black.i3UIs");
}

void UILBScrollSlotSet::SetPosition(INT32 start_x, INT32 start_y, UINT32 offset_x /* = 0 */, UINT32 offset_y /* = 0 */)
{
	i3UIScene* scn = m_parent->GetScene(m_offset_scn_idx);
	if( !scn ) return;

	m_offsetPos.x = offset_x;
	m_offsetPos.y = offset_y;
	
	i3UIControl* item_ctrl = static_cast<i3UIControl*>(scn->getFirstChild());
	const VEC2D& item_size = *item_ctrl->getSize();

	REAL32 offsetX = item_size.x + offset_x;
	REAL32 offsetY = item_size.y + offset_y;

	for (INT32 i = 0, count = 0; i < m_numRow ; ++i )
	{
		for (INT32 j = 0; j < m_numCol; ++j, ++count)
		{
			i3UIScene* slot_scn = m_parent->GetScene(m_offset_scn_idx + count);
			if( !slot_scn ) continue;

			item_ctrl = static_cast<i3UIControl*>(slot_scn->getFirstChild());
			item_ctrl->setPos(start_x + offsetX * REAL32(j), start_y + offsetY * REAL32(i));
		}
	}

	UpdatePosition( start_x, start_y, offset_x, offset_y);

	if( m_bUseScroll == false ) return;

	m_Scroll.SetPosition( (UINT32)m_SlotSize.x + 2, start_y);
}

void UILBScrollSlotSet::OnLoadAllScenes( INT32 start_x,  INT32 start_y, UINT32 offset_x,  UINT32 offset_y, UINT32 addsize)
{
	UILBSlotSet::OnLoadAllScenes(start_x, start_y, offset_x,  offset_y);

	UpdatePosition( start_x, start_y, offset_x, offset_y);

	i3UIFrameWnd* pSlideSizeWnd = (i3UIFrameWnd*)m_parent->GetScene(0)->FindChildByName("ScrollSizeWnd");

	UINT32 slide_size  = 9 + addsize;				//기존 하드 코딩..
	UINT32 slide_pos_x = (UINT32)m_SlotSize.x + 2;	//기존 하드 코딩..

	if (pSlideSizeWnd != nullptr)
	{
		slide_size  = (UINT32)pSlideSizeWnd->getWidth();
		slide_pos_x = (UINT32)pSlideSizeWnd->getPos()->x;
	}

	if( m_bUseScroll == true )
	{
		m_Scroll.OnLoadAllScenes();

		m_Scroll.SetPosition( slide_pos_x, start_y);
		m_Scroll.SetSize( slide_size, (UINT32)m_SlotSize.y);
		m_Scroll.SetEnable(false);
	}
}

void	UILBScrollSlotSet::OnLoadAllScenes(i3UIControl* attach_pos, UINT32 offset_x,  UINT32 offset_y)
{
	// 월드포지션을 얻기 위해 씀..
	attach_pos->CalcAccumulatedPos();

	UINT32 start_x = (UINT32)attach_pos->getAccumulatedPos()->x;
	UINT32 start_y = (UINT32)attach_pos->getAccumulatedPos()->y;

	UILBScrollSlotSet::OnLoadAllScenes( start_x, start_y, offset_x, offset_y );
}

void	UILBScrollSlotSet::OnUnloadAllScenes()
{
	UILBSlotSet::OnUnloadAllScenes();

	ResetScrollBar();

	if( m_bUseScroll == true )
	{
		m_Scroll.OnUnloadAllScenes();
	}
}

void UILBScrollSlotSet::UpdatePosition( INT32 start_x,  INT32 start_y, UINT32 offset_x,  UINT32 offset_y)
{
	//Slot 전체 크기를 구한다.
	const VEC2D& size = UILBSlotSet::GetSlotSize();

	m_SlotSize.x = start_x + (size.x * m_numCol) + (offset_x * (m_numCol - 1) );
	m_SlotSize.y = (size.y * m_numRow) + (offset_y * (m_numRow-1) );
}

void UILBScrollSlotSet::ResetScrollBar()
{
	//Scroll을 사용 안 하면 리턴한다.
	if( m_bUseScroll == false )	return;

	m_RefListSize = 0;
	m_idxStartSlot = 0;
	m_idxPosSlot = 0;
}

void UILBScrollSlotSet::Adjust(INT32 list_size)
{
	//Scroll을 사용 안 하면 리턴한다.
	if( m_bUseScroll == false )	return;

	//현재와 같은 내용일 경우
	if( m_RefListSize == list_size )
	{
		if( m_numSlot < list_size )
		{
			m_Scroll.SetEnable(true);
		}
		else
		{
			m_Scroll.SetEnable(false);
			m_Scroll.SetScrollPos(0);
		}
		return;
	}

	//현재와 다른 경우엔 초기화 루틴을 갖는다.
	m_RefListSize = list_size;
	m_idxStartSlot = 0;
	m_idxPosSlot = 0;

	if( m_numSlot < list_size )
	{
		INT32 numGoods = list_size;
		INT32 max = (numGoods + (m_numCol - 1)) / m_numCol;
		max = (max > m_numRow ) ? max - (m_numRow-1) : 1;

		m_Scroll.SetRange(max);
		m_Scroll.SetPage(1);
		m_Scroll.SetTick(1);
		m_Scroll.SetEnable(true);
	}
	else
	{
		m_Scroll.SetEnable(false);
	}

	m_Scroll.SetScrollPos(0);
}

void UILBScrollSlotSet::UpdateList(INT32 list_size, bool auto_focus_scroll)
{
	if( m_bUseScroll == true )
		Adjust(list_size);

	UILBSlotSet::UpdateList( list_size, m_idxStartSlot);

	if( auto_focus_scroll )
		SetScrollFocus();
}

void UILBScrollSlotSet::AddSlide(UISlide* pSlide, INT32 type)
{
	if( m_bUseScroll == false )	return;

	i3UIScene* scn = m_Scroll.GetScene();
	pSlide->AddFrameWnds(scn, (UISlide::slider_type)type);
}


void UILBScrollSlotSet::_notifyScroll( INT32 idx, INT32 old_idx )
{
	if( m_Scroll.IsEnable() == false )	return;

	INT32 new_idx = m_SlotSetCallback->OnScroll(idx, old_idx);
	if( new_idx == old_idx )
		m_Scroll._SetScrollPos(new_idx);

	m_idxPosSlot = new_idx;
	m_idxStartSlot = m_idxPosSlot * m_numCol;

	UILBSlotSet::UpdateList( m_RefListSize, m_idxStartSlot);

	//마우스 커서가 slot이 scroll 될 때 움직이지 않았다면 enter slot 이 변경 안되기 때문에 임의로 변경해야 된다.
	POINT pt = i3UI::getManager()->getCurrentMousePosition();
	if( pt.x == m_prevMousePos.x && pt.y == m_prevMousePos.y )
	{
		if( m_idx_slot_mouseover != -1 )
		{
			m_callback->OnEnterSlot(m_idx_slot_mouseover, UILBSlotSet::GetSlotScene(m_idx_slot_mouseover) );
		}
	}
}

bool UILBScrollSlotSet::OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code)
{
	if( m_bUseScroll == false ) return false;
	if( m_skip_slot ) return false;

	if( m_Scroll.OnEvent(event, pObj, param2, code) )
		return true;

	return false;	
}

bool UILBScrollSlotSet::HitTest(INT32 x, INT32 y) const
{
	const VEC3D& ctrlPos = UILBSlotSet::GetSlotPos(0);

	REAL32 right	= ctrlPos.x + m_SlotSize.x;
	REAL32 bottom = ctrlPos.y + m_SlotSize.y;

	if( REAL32(x) >=  ctrlPos.x && REAL32(x) <= right )
	{
		if( REAL32(y) >= ctrlPos.y && REAL32(y) <= bottom )
		{
			return true;
		}
	}
	return false;
}

void	 UILBScrollSlotSet::SetEnable(bool bEnable)
{
	for (INT32 i = 0 ; i < m_numSlot ; ++i)
	{
		UILBSlotSet::SetEnable(i, bEnable);
	}

	if(bEnable == false)
		ResetScrollBar();

	if( m_bUseScroll == true )
		m_Scroll.SetEnable(bEnable);
}

void UILBScrollSlotSet::SetScrollFocus()
{
	if( m_bUseScroll == true && m_Scroll.IsEnable() == true )
		m_Scroll.SetFocus();
}

void UILBScrollSlotSet::MoveBackScene()
{
	UILBSlotSet::MoveBackScene();

	if( m_bUseScroll == true )
		m_Scroll.GetScene()->Attach(false);
}

void UILBScrollSlotSet::ReverseScene()
{
	UILBSlotSet::ReverseScene();
}

VEC2D UILBScrollSlotSet::GetLeftTop() const
{
	VEC2D ret;
	
	const VEC3D& ctrlPos = UILBSlotSet::GetSlotPos(0);
	ret.x = ctrlPos.x;
	ret.y = ctrlPos.y;

	return ret;
}
VEC2D UILBScrollSlotSet::GetRightBottom() const
{
	VEC2D ret;

	const VEC3D& ctrlPos = UILBSlotSet::GetSlotPos(0);
	ret.x = m_SlotSize.x;
	ret.y = m_SlotSize.y + ctrlPos.y;

	return ret;
}

namespace UI
{
	class tmHover_Callback_None : public tmHover_Callback
	{
	public:
		virtual void	OnEnter( UINT32 idx, i3UIControl* pWnd ) {}
		virtual void	OnLeave( UINT32 idx, i3UIControl* pWnd ) {}
	} __none;

	tmHover::tmHover() : m_callback(&__none), m_over_idx(UINT_MAX)
	{
		m_prevMousePos.x = m_prevMousePos.y = LONG_MAX; 
	}
	tmHover::~tmHover()
	{
		clear();
	}

	void tmHover::add(i3UIScene* s, const char* name)
	{
		i3UIControl* p = (i3UIControl*)s->FindChildByName(name);
		if( !p )
		{
			I3PRINTLOG(I3LOG_FATAL, "[tmHover] %s 가 없네.. i3Uie 파일 확인", name);
		}
		mControlList.push_back( p );
	}

	void tmHover::add(i3UIControl* pWnd)
	{
		mControlList.push_back( pWnd );
	}

	void tmHover::clear()
	{
		mControlList.clear();
		m_over_idx = UINT_MAX;
	}

	size_t tmHover::get_stayed_hover_idx()
	{
		POINT pt = i3UI::getManager()->getCurrentMousePosition();

		const size_t sizeControl = mControlList.size();
		for(size_t i=0; i< sizeControl; ++i)
		{
			if( mControlList[i]->isEnable() == false )
				continue;

			const VEC3D& pos = *mControlList[i]->getAccumulatedPos();			// 0번슬롯 위치..
			const VEC2D& size = *mControlList[i]->getSize();

			REAL32 right	= pos.x + size.x;
			REAL32 bottom = pos.y + size.y;

			if( REAL32(pt.x) >=  pos.x && REAL32(pt.x) <= right )
			{
				if( REAL32(pt.y) >= pos.y && REAL32(pt.y) <= bottom )
				{
					return i;
				}
			}
		}

		return UINT_MAX;
	}

	void tmHover::OnUpdate(REAL32 rDeltaTime)
	{
		POINT pt = i3UI::getManager()->getCurrentMousePosition();

		if (pt.x == m_prevMousePos.x && pt.y == m_prevMousePos.y)
			return;

		m_prevMousePos.x = pt.x;	m_prevMousePos.y = pt.y;

		size_t cur_inx = UINT_MAX;

		const size_t sizeControl = mControlList.size();
		for(size_t i=0; i< sizeControl; ++i)
		{
			if( mControlList[i]->isEnable() == false )
				continue;

			const VEC3D& pos = *mControlList[i]->getAccumulatedPos();			// 0번슬롯 위치..
			const VEC2D& size = *mControlList[i]->getSize();

			REAL32 right	= pos.x + size.x;
			REAL32 bottom = pos.y + size.y;

			if( REAL32(pt.x) >=  pos.x && REAL32(pt.x) <= right )
			{
				if( REAL32(pt.y) >= pos.y && REAL32(pt.y) <= bottom )
				{
					cur_inx = i;
				}
			}
		}

		if ( cur_inx != m_over_idx )
		{
			if ( m_over_idx != UINT_MAX )
			{
				m_callback->OnLeave(m_over_idx, mControlList[m_over_idx]);
				m_over_idx = UINT_MAX;
			}
			if ( cur_inx != UINT_MAX )
			{
				m_callback->OnEnter(cur_inx, mControlList[cur_inx]);
				m_over_idx = cur_inx;
			}
		}
	}
}