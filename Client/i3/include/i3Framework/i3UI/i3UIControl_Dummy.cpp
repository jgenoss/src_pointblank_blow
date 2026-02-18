#include "i3FrameworkPCH.h"
#include "i3UIControl_Dummy.h"
#include "i3UIStaticText.h"
#include "i3UIListView_Cell.h"

I3_CLASS_INSTANCE( i3UIControl_Dummy);

i3UIControl_Dummy::~i3UIControl_Dummy(void)
{
	for( size_t i = 0; i < m_ControlList.size(); ++i)
	{
		i3UIControl* pContorl = m_ControlList[i];
		RemoveControl( pContorl);
	}
	
	m_ControlList.clear();
	m_ChildRectList.clear();
	m_AlignStyleList.clear();
}

void i3UIControl_Dummy::AddChildControl( i3UITemplate* pTemplate)
{
	i3ClassMeta* pMeta = pTemplate->GetInstanceMeta();
	I3ASSERT( pMeta != nullptr && pMeta->kind_of( i3UIControl::static_meta()));

	i3UIControl * pControl = (i3UIControl*)pMeta->create_instance(); //	CREATEINSTANCE( pMeta);

	pControl->SetTemplate( pTemplate);
	pControl->setPos(0,0,0);

	pControl->Init();
	pControl->EnableCtrl( true);


	AddControl( pControl);
	
	//vector에서 관리 하도록 따로 설정 해야됨.
	m_ControlList.push_back( pControl);
	
	//child의 rect 리스트
	i3::pack::RECT ChildRect;
	m_ChildRectList.push_back( ChildRect);

	//child의 style 리스트
	ud::ALING_STYLE nStyle = ud::ALIGN_CENTER;
	m_AlignStyleList.push_back( nStyle);


	// Layer를 Manual로 저절하는 것은 SpriteOrder를 자동으로 하지 않도록 한다.
	if( pControl->isStyle( I3UI_STYLE_NO_VT))
		addStyle( I3UI_STYLE_MANUAL_ORDER);

	// 로딩이 다 되었다고 간주해버린다.
	if( isState( I3UI_STATE_WAITLOAD) == false)
	{
		OnCompletedLoading();
	}

	// set ancestor
	pControl->setAncestor( getAncestor() );
}

void i3UIControl_Dummy::SetChildRect( INT32 nChildIdx, REAL32 x, REAL32 y, REAL32 w, REAL32 h)
{
	if( nChildIdx >= 0 && nChildIdx < static_cast<INT32>(m_ChildRectList.size()))
	{
		i3::pack::RECT rc = m_ChildRectList[nChildIdx];

		rc.left		= x;
		rc.top		= y;
		rc.right	= x + w;
		rc.bottom	= y + h;

		m_ChildRectList[nChildIdx] = rc;
	}
}

void i3UIControl_Dummy::GetChildRect( INT32 nChildIdx, i3::pack::RECT* p)
{
	if( nChildIdx >= 0 && nChildIdx < static_cast<INT32>(m_ChildRectList.size()))
	{
		i3::pack::RECT rc = m_ChildRectList[nChildIdx];

		p->left		= rc.left;
		p->top		= rc.top;
		p->right	= rc.right;
		p->bottom	= rc.bottom;
	}
	else
	{
		p->left		= 0.0f;
		p->top		= 0.0f;
		p->right	= 0.0f;
		p->bottom	= 0.0f;
	}

}

void i3UIControl_Dummy::SetChildPos( INT32 nChildIdx, REAL32 x, REAL32 y, REAL32 z /*= 0.f*/)
{
	if( nChildIdx >= 0 && nChildIdx < static_cast<INT32>(m_ControlList.size()))
	{
		m_ControlList[nChildIdx]->setPos( x, y, z);
	}
}

void i3UIControl_Dummy::SetChildSize( INT32 nChildIdx, REAL32 cx, REAL32 cy)
{
	if( nChildIdx >= 0 && nChildIdx < static_cast<INT32>(m_ControlList.size()))
	{
		m_ControlList[nChildIdx]->setSize( cx, cy);
	}
}

void i3UIControl_Dummy::SetChildAlignStyle( INT32 nChildIdx, INT32 nStyle)
{
	if( nChildIdx >= 0 && nChildIdx < static_cast<INT32>(m_AlignStyleList.size()))
	{
		if( nStyle >= ud::ALIGN_NONE && nStyle < ud::ALIGN_MAX)
		{
			ud::ALING_STYLE eStyle = ud::ALING_STYLE(nStyle);
			m_AlignStyleList[nChildIdx] = eStyle;
		}
	}
}

ud::ALING_STYLE i3UIControl_Dummy::GetChildAlignStyle( INT32 nChildIdx) const
{
	if( nChildIdx >= 0 && nChildIdx < static_cast<INT32>(m_AlignStyleList.size()))
	{
		return m_AlignStyleList[nChildIdx];
	}
	return ud::ALIGN_NONE;
}

void i3UIControl_Dummy::UpdatePosAndSize()
{
	i3UIListView_Cell* pCell = (i3UIListView_Cell*)getParent();

	setPos( 0.f, 0.f);
	setSize( (REAL32)pCell->getWidth(), (REAL32)pCell->getHeight());

	INT32 nDummyChildIdx = 0;
	i3GameNode* pNode = getFirstChild();
	while( pNode)
	{
		i3UIControl* pControl = i3::kind_cast<i3UIControl*>(pNode);
		if (pControl == nullptr)
		{
			I3ASSERT(pControl != nullptr);
			break;
		}

		if( pControl->isEnable())
		{
			i3::pack::RECT rt;
			GetChildRect( nDummyChildIdx, &rt);

			pControl->setPos( rt.left, rt.top );
			pControl->setSize( rt.Width() - 1.0f , rt.Height() - 1.0f );
		}

		pNode = pControl->getNext();
		nDummyChildIdx++;
	}
}

void i3UIControl_Dummy::CalcControlPosAndSize( REAL32 fx, REAL32 fy)
{
	i3UIListView_Cell* pCell = (i3UIListView_Cell*)getParent();

	//setPos( 0.f, 0.f);
	//setSize( (REAL32)pCell->getWidth(), (REAL32)pCell->getHeight());

	i3::pack::RECT rect;
	INT32 nDummyChildIdx = 0;
	i3GameNode* pNode = getFirstChild();
	while( pNode)
	{
		i3UIControl* pControl = i3::kind_cast<i3UIControl*>(pNode);

		if (pControl == nullptr)
		{
			I3ASSERT(pControl != nullptr);
			break;
		}

		if( pControl->isEnable())
		{
			//size 설정
			if( i3::same_of<i3UIStaticText*>(pControl))
			{
				VEC2D* vSize = getSize();
				rect.left = 0; rect.top = 0;
				rect.right = vSize->x; rect.bottom = vSize->y;
			}
			else
				pControl->getTemplate()->GetControlRect( &rect);


			pControl->setSize( rect.right, rect.bottom);

			//pos 설정
			REAL32 centerWidth = 0;
			REAL32 centerHeight = 0;
			ud::ALING_STYLE eStyle = GetChildAlignStyle( nDummyChildIdx);
			if( eStyle == ud::ALIGN_NONE)
			{
				VEC3D* pPos = pControl->getPos();
				centerWidth = pPos->x;
				centerHeight = pPos->y;
			}
			else if( eStyle == ud::ALIGN_CENTER) 
			{
				centerWidth = fx * 0.5f;
				centerWidth -= rect.right * 0.5f;
				centerWidth = max( centerWidth, 0.f);

				centerHeight = fy * 0.5f;
				centerHeight -= rect.bottom * 0.5f;
				centerHeight = max( centerHeight, 0.f);

				pControl->setPos( centerWidth, centerHeight);
			}

			//Cell의 Dummy 설정은 항상 마지막 템플릿의 사이즈로 설정.
			pCell->setChildRect( (INT32) centerWidth, (INT32) centerHeight, (INT32) rect.right, (INT32) rect.bottom);
			SetChildRect( nDummyChildIdx, centerWidth, centerHeight, rect.right, rect.bottom);

		}

		pNode = pControl->getNext();
		nDummyChildIdx++;
	}
}