#include "pch.h"
#include "UILBSlot.h"

#include "UIPhaseShop.h"
#include "UIMainFrame.h"

#include "ItemImageMgr.h"

/////////////////////////////////////////////////////////

namespace
{
	class UILBSlot_Callback_None : public UILBSlot_Callback
	{
	public:
		virtual void	OnLoadSlot( INT32 slot_idx, i3UIScene* slot_scn ) {}				// 로드된 경우, 초기화할 호출 위치..
		virtual void	OnUnloadSlot( INT32 slot_idx, i3UIScene* slot_scn) {}				// 언로드된 경우 정리할 호출 위치..
		virtual void	OnEnabledSlot( INT32 slot_idx, i3UIScene* slot_scn) {}			// enable호출된 경우 업데이트할 정보를 채울 함수..
		virtual void	OnDisabledSlot( INT32 slot_idx, i3UIScene* slot_scn) {}
		virtual void	OnEnterSlot( INT32 slot_idx, i3UIScene* slot_scn) {} 
		virtual void	OnLeaveSlot( INT32 slot_idx, i3UIScene* slot_scn) {}
		virtual bool	EnableMouseOver() { return true; }
	} g_callback_none;
}


UILBSlotSet::UILBSlotSet() : m_parent(nullptr), m_callback(&g_callback_none), m_offset_scn_idx(0), m_numRow(0), m_numCol(0), m_numSlot(0), m_numActiveSlot(0),
m_idx_slot_mouseover(-1), m_isEnableMouseOver(true), m_isEnableMouseOverFromCB(true), m_skip_slot(true)
{
	m_prevMousePos.x = m_prevMousePos.y = LONG_MAX; 
	m_offsetPos.x = 0;
	m_offsetPos.y = 0;
}

UILBSlotSet::~UILBSlotSet()
{
	
}

void	UILBSlotSet::OnCreate(UIBase* parent, INT32 row, INT32 col)
{
	m_parent = parent;	m_numRow = row;	m_numCol = col;	m_numSlot = row * col;
}

void	UILBSlotSet::RegisterSlotScene( const char* szSlotScene, const LuaFuncReg* Lua_Func, bool bLoaded )		// 씬이름과 루아 함수 등록..
{
	m_offset_scn_idx = m_parent->GetSceneCount();
	for (INT32 i = 0 ; i < m_numSlot ; ++i )
	{
		m_parent->AddScene(szSlotScene, Lua_Func, bLoaded, !bLoaded);					// 슬롯을 한꺼번에 로딩할때 현재 문제점은 일단 씬리소스파일과 루아파일이 중복로딩되는것 아닌가 하는 점..
	}
}

void	UILBSlotSet::SetVisible(bool bEnable)
{
	for (INT32 i = 0 ; i < m_numSlot ; ++i)
	{
		i3UIScene* scn = m_parent->GetScene(m_offset_scn_idx + i);
		i3UIFrameWnd* pWnd = static_cast<i3UIFrameWnd*>( scn->getFirstChild() );
	//	pWnd->SetVisible(bEnable);
		pWnd->EnableCtrl(bEnable, true);
	}
}

void UILBSlotSet::OnLoadAllScenes( INT32 start_x,  INT32 start_y, UINT32 offset_x,  UINT32 offset_y)
{
	m_skip_slot = false;

	m_offsetPos.x = offset_x;
	m_offsetPos.y = offset_y;

	UpdatePosition( start_x, start_y, offset_x, offset_y);
	
	for (INT32 i = 0 ; i < m_numSlot ; ++i)
	{
		i3UIScene* scn = m_parent->GetScene(m_offset_scn_idx + i);
		if (scn->isEnable() == false)
			scn->EnableScene(true);
	}

	for (INT32 i = 0 ; i < m_numSlot ; ++i )
	{
		m_callback->OnLoadSlot(i, m_parent->GetScene(m_offset_scn_idx + i) );
	}
}

void UILBSlotSet::UpdatePosition( INT32 start_x,  INT32 start_y, UINT32 offset_x,  UINT32 offset_y)
{
	I3ASSERT( m_parent != nullptr);

	i3UIControl* item_ctrl = static_cast<i3UIControl*>(m_parent->GetScene(m_offset_scn_idx)->getFirstChild());
	const VEC2D& item_size = *item_ctrl->getSize();

	REAL32 offsetX = item_size.x + offset_x;
	REAL32 offsetY = item_size.y + offset_y;

	for (INT32 i = 0, count = 0; i < m_numRow ; ++i )
	{
		for (INT32 j = 0; j < m_numCol; ++j, ++count)
		{
			item_ctrl = static_cast<i3UIControl*>(m_parent->GetScene(m_offset_scn_idx + count)->getFirstChild());
			item_ctrl->setPos(start_x + offsetX * REAL32(j), start_y + offsetY * REAL32(i));
		}
	}
}

void	UILBSlotSet::OnLoadAllScenes(i3UIControl* attach_pos, UINT32 offset_x,  UINT32 offset_y)
{
	// 월드포지션을 얻기 위해 씀..
	attach_pos->CalcAccumulatedPos();

	UINT32 start_x = (UINT32)attach_pos->getAccumulatedPos()->x;
	UINT32 start_y = (UINT32)attach_pos->getAccumulatedPos()->y;

	OnLoadAllScenes( start_x, start_y, offset_x, offset_y );
}

void	UILBSlotSet::OnUnloadAllScenes()
{
	// 이 안에서는 지울게 없지만 다른데서 소거처리 가능하게 콜백..

	if (m_idx_slot_mouseover != -1)			// 소거되기 전에 마우스 오버가 남아있다면 leave신호를 주도록 한다..(실제로 슬라이드 아웃 상태이므로 반영되기는 어렵다..)
	{
		INT32 prev_slot_mouseover = m_idx_slot_mouseover;
		m_idx_slot_mouseover = -1;
		m_numActiveSlot = 0;

		m_callback->OnLeaveSlot(prev_slot_mouseover, UILBSlotSet::GetSlotScene(prev_slot_mouseover) );
	}

	for (INT32 i = 0 ; i < m_numSlot ; ++i )
	{
		m_callback->OnUnloadSlot(i, m_parent->GetScene(m_offset_scn_idx + i) );
	}

	for (INT32 i = 0 ; i < m_numSlot ; ++i)
	{
		i3UIScene* scn = m_parent->GetScene(m_offset_scn_idx + i);
		if(scn && scn->isEnable() == true)
			scn->EnableScene(false);
	}
}

void	UILBSlotSet::OnUpdate()
{
	if ( m_isEnableMouseOver == false) 	return;
	if( m_skip_slot )	return;
	
	bool res = m_callback->EnableMouseOver();
	if (res != m_isEnableMouseOverFromCB)
	{
		m_isEnableMouseOverFromCB = res;
		UILBSlotSet::_ProcessEnableMouseOver(res);
	}
	
	if (res == false)
		return;

	POINT pt = i3UI::getManager()->getCurrentMousePosition();
	
	if (pt.x != m_prevMousePos.x || pt.y != m_prevMousePos.y)		// OnMouseMove에 해당하는 마우스 이동 조건..
	{
		m_prevMousePos.x = pt.x;	m_prevMousePos.y = pt.y;

		INT32 prev_slot_mouseover = m_idx_slot_mouseover;
		
		UILBSlotSet::HitTestSet(pt.x, pt.y, m_idx_slot_mouseover);
		
		if ( m_idx_slot_mouseover != prev_slot_mouseover )
		{
			if ( prev_slot_mouseover != -1 )	// leave처리..
			{
				m_callback->OnLeaveSlot(prev_slot_mouseover, 
					UILBSlotSet::GetSlotScene(prev_slot_mouseover) );
			}
			
			if ( m_idx_slot_mouseover != -1 )	// enter처리..
			{
				m_callback->OnEnterSlot(m_idx_slot_mouseover, 
					UILBSlotSet::GetSlotScene(m_idx_slot_mouseover) );
			}
		}
	}
}

void	UILBSlotSet::SetEnableMouseOver(bool bEnable)
{
	if (m_isEnableMouseOver == bEnable ) return;

	m_isEnableMouseOver = bEnable;

	UILBSlotSet::_ProcessEnableMouseOver(bEnable);
}

void	UILBSlotSet::_ProcessEnableMouseOver(bool bEnable)
{
	if (!bEnable)	// false  끄는 경우, 마우스 온상태라면 Leave처리를 해줌..
	{
		if (m_idx_slot_mouseover != -1)
		{
			m_callback->OnLeaveSlot(m_idx_slot_mouseover, 
				UILBSlotSet::GetSlotScene(m_idx_slot_mouseover) );

			m_idx_slot_mouseover = -1;
		}
	}
	else	// 끈걸 키는 경우, 테스트하여 교차의 경우 Enter처리를 해줌.
	{
		INT32 prev_slot_mouseover = m_idx_slot_mouseover;
		POINT pt = i3UI::getManager()->getCurrentMousePosition();
		UILBSlotSet::HitTestSet(pt.x, pt.y, m_idx_slot_mouseover);
		m_prevMousePos.x = pt.x;	m_prevMousePos.y = pt.y;

		if ( m_idx_slot_mouseover != prev_slot_mouseover )				
		{
			if ( prev_slot_mouseover != -1 )	// leave처리..
			{
				m_callback->OnLeaveSlot(prev_slot_mouseover, 
					UILBSlotSet::GetSlotScene(prev_slot_mouseover) );
			}

			if ( m_idx_slot_mouseover != -1 )	// enter처리..
			{
				m_callback->OnEnterSlot(m_idx_slot_mouseover, 
					UILBSlotSet::GetSlotScene(m_idx_slot_mouseover) );
			}
		}
	}
}

void	UILBSlotSet::UpdateList(const INT32 size_list, const INT32 idx_scroll_start)
{
// 외부 리스트와 실제 슬롯 간 관계를 잘 판별해서 enable, disable로 어느 정도 구분해서 정리..	
	const INT32 numSlot = m_numSlot;
	const bool  Is_Any_Empty_Slot =  (size_list - idx_scroll_start < numSlot);
	const INT32 Num_Visible = (Is_Any_Empty_Slot) ? size_list - idx_scroll_start : numSlot;
	
	m_numActiveSlot = Num_Visible;

	INT32 i = 0;		

	for ( i = 0; i < Num_Visible ; ++i)
	{
		i3UIScene* scn = m_parent->GetScene(m_offset_scn_idx + i);
		m_callback->OnEnabledSlot(i, scn);
	}
	for ( ; i < numSlot; ++i)
	{
		i3UIScene* scn = m_parent->GetScene(m_offset_scn_idx + i);
		m_callback->OnDisabledSlot(i, scn);
	}
}

void	UILBSlotSet::SetEnable(INT32 slot, bool bEnable)
{
	i3UIScene* scn = m_parent->GetScene(m_offset_scn_idx + slot);

	if ( bEnable )
		m_callback->OnEnabledSlot(slot, scn);		//enable된 경우에만 셋팅은 콜백에서..셀렉션도 결국 이 안에서 결정해서 처리할 것...
	else
		m_callback->OnDisabledSlot(slot, scn);
}

i3UIScene*	UILBSlotSet::GetSlotScene(INT32 slot) const
{ 
	return m_parent->GetScene(m_offset_scn_idx + slot); 
}


bool	UILBSlotSet::HitTestSet(INT32 x, INT32 y, INT32& outSlotIdx)
{
	// 슬롯 영역 안에서 로컬로 잡는게 편할듯... (현재 상위 프레임과 연결관계를 끊었기 때문에... 0번 슬롯 포지션을 얻어와야한다..)
	outSlotIdx = -1;

	//드물게 m_skip_slot값이 제대로 지정이 안되어 OnUpdate()에서 여기로 넘어오는 경우가 있는 것 같습니다
	if (m_parent == nullptr)
	{
		I3PRINTLOG(I3LOG_VIEWER, "UILBSlotSet Parent is null");
		return false;
	}

	i3UIScene* scn = m_parent->GetScene(m_offset_scn_idx);
	if (scn == nullptr) 
		return false;
	
	i3UIControl* slot_frame = static_cast<i3UIControl*>(scn->getFirstChild());
	if ( slot_frame->isIgnoreInput() )			// 첫슬롯 인풋 불활성화에서 막음..(나머지슬롯도 불활성화된것으로 간주)
		return false;

	const VEC3D& offsetPos = *slot_frame->getAccumulatedPos();			// 0번슬롯 위치..
	const VEC2D& slot_size = *slot_frame->getSize();
	REAL32 size_x = slot_size.x + m_offsetPos.x;	//offst 크기를 추가한다.
	REAL32 size_y = slot_size.y + m_offsetPos.y;

	REAL32 tmp = 0.0f;


	INT32 calc_col_idx = INT32(floorf( (REAL32(x) - offsetPos.x) / size_x ));		// f -> i 비교적 느린 연산..
	if ( calc_col_idx < 0 || calc_col_idx >= m_numCol ) 
		return false;

	tmp = offsetPos.x + (size_x*calc_col_idx);
	if( REAL32(x) - (tmp+slot_size.x) >= 0.0f)
		return false;

	INT32 calc_row_idx = INT32(floorf((REAL32(y) - offsetPos.y) / size_y));		// 
	if ( calc_row_idx < 0 || calc_row_idx >= m_numRow )
		return false;

	tmp = offsetPos.y + (size_y*calc_row_idx);
	if( REAL32(y) - (tmp+slot_size.y) >= 0.0f)
		return false;


	INT32 result_idx = calc_row_idx * m_numCol + calc_col_idx ;
	if (result_idx >= m_numActiveSlot) return false;

	if ( m_parent->GetScene(m_offset_scn_idx + result_idx)->isEnable() == false)
		return false;
	
	outSlotIdx = result_idx;

	return true;
}

bool	UILBSlotSet::HitTestSlot(INT32 slot, INT32 x, INT32 y)
{
	if (slot >= m_numActiveSlot)
		return false;

	i3UIScene* scn = GetSlotScene(m_offset_scn_idx + slot);

	if ( scn == nullptr) return false;

	if ( scn->isEnable() == false)
		return false;
	
	i3UIControl* pCtrl = static_cast<i3UIControl*>(m_parent->GetScene(m_offset_scn_idx + slot)->getFirstChild());
	
	if (pCtrl->isIgnoreInput())
		return false;

	VEC3D* ctrlPos = pCtrl->getAccumulatedPos();
	VEC2D* ctrlSize = pCtrl->getSize();

	REAL32 right	= ctrlPos->x + ctrlSize->x;
	REAL32 bottom	= ctrlPos->y + ctrlSize->y;

	if( REAL32(x) >=  ctrlPos->x && REAL32(x) <= right )
	{
		if( REAL32(y) >= ctrlPos->y && REAL32(y) <= bottom )
		{
			return true;
		}
	}
	return false;
}


const VEC2D&	UILBSlotSet::GetSlotSize() const
{
	i3UIScene* scn = m_parent->GetScene(m_offset_scn_idx);
	if( scn != nullptr)
		return	*static_cast<i3UIControl*>(scn->getFirstChild())->getSize();

	static VEC2D v;
	
	return v;
}

const VEC3D&	UILBSlotSet::GetSlotPos(INT32 slot) const
{
	i3UIScene* scn = m_parent->GetScene(m_offset_scn_idx + slot);	
	return *static_cast<i3UIControl*>(scn->getFirstChild())->getAccumulatedPos();
}

void		UILBSlotSet::SetCallback(UILBSlot_Callback* callback) 
{ 
	m_callback = callback; 
	bool res = callback->EnableMouseOver();
	if ( res != m_isEnableMouseOverFromCB )
	{
		m_isEnableMouseOverFromCB = res;
		UILBSlotSet::_ProcessEnableMouseOver(res);
	}
}

void UILBSlotSet::MoveBackScene()
{
	for (INT32 i = 0 ; i < m_numSlot ; ++i)
	{
		i3UIScene* scn = m_parent->GetScene(m_offset_scn_idx + i);
		scn->Attach(false);
	}
}

void UILBSlotSet::ReverseScene()
{
	for (INT32 i = m_numSlot-1 ; i >= 0; --i)
	{
		i3UIScene* scn = m_parent->GetScene(m_offset_scn_idx + i);
		scn->Attach(false);
	}
}