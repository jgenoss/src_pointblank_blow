#include "pch.h"

#include "iSlot.h"

#include "ItemImageMgr.h"

using namespace UI;

//------------------------------------------------------------------------------//
iSlot::iSlot() : m_old_slot_idx(UI::NON_SLOT_INDEX), m_now_slot_idx(UI::NON_SLOT_INDEX)
{
	m_ppImgSet = 0;

	mSlotCb.SetObj(this);
	mSlot.SetCallback(&mSlotCb);

	m_ScrollCallback.SetObj(this);
	mSlot.SetSlotSetScrollCallback( &m_ScrollCallback );
}

iSlot::~iSlot()
{
	if(m_ppImgSet)
	{
		const INT32 num =	mSlot.GetSlotCount();
		for (INT32 i = 0 ; i < num ; ++i)
		{
			I3_SAFE_RELEASE(m_ppImgSet[i]);
		}
		I3_SAFE_DELETE_ARRAY( m_ppImgSet);
	}
}

//------------------------------------------------------------------------------//
void iSlot::set_select(INT32 idx)
{
	if( mSelectedList.size() > (size_t)idx )
	{
		if( m_old_slot_idx != UI::NON_SLOT_INDEX )
		{
			if( mSelectedList.size() > (size_t)m_old_slot_idx )
				mSelectedList[ m_old_slot_idx ] = false;
		}
		mSelectedList[ idx ] = true;
		m_old_slot_idx = idx;
		m_now_slot_idx = idx;
	}
}

void iSlot::reset_select(size_t size)
{
	m_old_slot_idx = m_now_slot_idx = UI::NON_SLOT_INDEX;

	mSelectedList.clear();
	mSelectedList.resize(size, false);
}

INT32 iSlot::apply_select()
{
	//reset_select 호출후에 마우스 커서 안 움직이고 click 할 경우에 대한 처리를 주로해야 될 듯
	if(m_old_slot_idx != UI::NON_SLOT_INDEX)
	{
		if( mSelectedList.size() > (size_t)m_old_slot_idx )
			mSelectedList[ m_old_slot_idx ] = false;
	}

	if(m_now_slot_idx != UI::NON_SLOT_INDEX)
	{
		if( mSelectedList.size() > (size_t)m_now_slot_idx )
			mSelectedList[ m_now_slot_idx ] = true;
	}
	
	//reset_select 호출후에 마우스 커서 안 움직이고 click 할 경우
	if( m_now_slot_idx == UI::NON_SLOT_INDEX )
	{
		if( mSelectedList.size() > (size_t)m_old_slot_idx )
		{
			m_now_slot_idx = m_old_slot_idx;
		}
		else
		{
			//이 경우는 안 발생 할 것 같다. (클릭 할 수 있다는건 현재 위치에 slot이 있어야 하기 때문에
			//현재 위치 slot 보다 작은 slot 크기일 경우엔 마우스가 움직이고 m_now_slot_idx 이 값은 iSlot::NON_SLOT_INDEX 안 될것이다.

			//2014. 09. 16 박기성
			//너무 이 경우에 당당하게 들어와서 놀랬다
			//탭 슬롯을 바꾼 후, 슬롯 부분을 지나쳐서 캐쉬 충전 버튼을 누르면 들어옴.
			//일단은 첫 번째 슬롯을 선택한 걸로 강제 고정시킨다.
			m_now_slot_idx = 0;	
		}
	}
	
	m_old_slot_idx = m_now_slot_idx;

	return m_now_slot_idx;
}

//------------------------------------------------------------------------------//
INT32 iSlot::_notifyScroll( INT32 idx, INT32 old_idx )
{
	//여기서 Slot 방식 변경하면 된다. //삭제 가능한 코드이다.

	//if( idx == old_idx )
	//{
	//	I3PRINTLOG(I3LOG_FATAL, "일단 여기 오면 안 됌!");
	//}

	//bool is_down_wheel = (idx > old_idx);

	//INT32 x = m_now_slot_idx % mSlot.m_numCol;
	//INT32 y = m_now_slot_idx / mSlot.m_numCol;

	//INT32 top = mSlot.m_idxPosSlot;
	//INT32 bottom = mSlot.m_idxPosSlot + mSlot.m_numRow;

	//if( is_down_wheel )
	//{
	//	INT32 next_idx = m_now_slot_idx + mSlot.m_numCol;
	//	set_select( next_idx );
	//	if( (next_idx / mSlot.m_numCol) >= bottom )
	//		return idx;
	//}
	//else
	//{
	//	INT32 next_idx = m_now_slot_idx - mSlot.m_numCol;
	//	set_select( next_idx );
	//	if( (next_idx / mSlot.m_numCol)  < top )
	//		return idx;
	//}

	return idx;
}

//------------------------------------------------------------------------------//
void	iSlot::cb_Load( INT32 slot_idx, i3UIScene* slot_scn)
{
	cb_Load_impl(slot_idx, slot_scn);
}

void	iSlot::cb_Unload( INT32 slot_idx, i3UIScene* slot_scn)
{
	cb_Unload_impl(slot_idx, slot_scn);
}

void	iSlot::cb_Enabled( INT32 slot_idx, i3UIScene* slot_scn)
{
	cb_Enabled_impl(slot_idx, slot_scn);
}

void	iSlot::cb_Disabled( INT32 slot_idx, i3UIScene* slot_scn)
{
	cb_Disabled_impl(slot_idx, slot_scn);
}

void	iSlot::cb_Enter( INT32 slot_idx, i3UIScene* slot_scn)
{
	m_now_slot_idx = mSlot.GetStartInx() + slot_idx;
	
	cb_Enter_impl(slot_idx, slot_scn);
}

void iSlot::cb_Leave( INT32 slot_idx, i3UIScene* slot_scn)
{
	cb_Leave_impl(slot_idx, slot_scn);
}

bool iSlot::cb_Istooltip()
{
	if ( i3UI::getDisableUIInput() == true )
		return false;

	return true;
}
