#include "i3FrameworkPCH.h"
#include "i3UIListView_Header.h"
#include "i3UITemplate_LVHeader.h"

#include "i3UIButton.h"
#include "i3UIListView_Sep.h"
#include "i3UIListView_Cell.h"
#include "i3UITemplate_Button.h"
#include "i3UITemplate_LVSep.h"

#include "i3UIListView_Item.h"
#include "i3UIListView_Box.h"

#include "i3Framework.h"

#include "i3Base/string/compare/generic_is_iequal.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"

//#define i3_profile_enable
#include "i3Base/profile/profile.h"

#if defined( I3_DEBUG)
static bool	s_bHeaderFontLoad = false;
static i3::vector<i3::rc_string> s_strFontQuality;
#endif


I3_CLASS_INSTANCE( i3UIListView_Header);

i3UIListView_Header::i3UIListView_Header()
{
	m_defFontInfo.m_Size = 10;

#if defined( I3_DEBUG)
	if( s_bHeaderFontLoad == false)
	{
		s_strFontQuality.push_back( "Anti-Aliased");
		s_strFontQuality.push_back( "CleaerType");
		s_strFontQuality.push_back( "Proof");

		s_bHeaderFontLoad = true;
	}

	//i3UICaptionControl의 m_lf와 동일하게 초기화 합니다.	
	m_lf.lfHeight = _PointToHeight( 10);
    m_lf.lfWidth = 0;
    m_lf.lfEscapement = 0;
    m_lf.lfOrientation = 0;
    m_lf.lfWeight = 400;	// bold => 700
    m_lf.lfItalic = FALSE;
    m_lf.lfUnderline = FALSE;
    m_lf.lfStrikeOut = FALSE;
    m_lf.lfCharSet = DEFAULT_CHARSET;
    m_lf.lfOutPrecision = OUT_CHARACTER_PRECIS;
    m_lf.lfClipPrecision = CLIP_CHARACTER_PRECIS;
    m_lf.lfQuality = DEFAULT_QUALITY;
    m_lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
    i3::string_ncopy_nullpad( m_lf.lfFaceName, "새굴림", sizeof(m_lf.lfFaceName));
#endif
}

i3UIListView_Header::~i3UIListView_Header()
{
	for( size_t i = 0; i < m_ButtonList.size(); i++)
	{
		i3UIButton* pButton = m_ButtonList[i];

		RemoveControl( pButton);
		I3_SAFE_RELEASE( pButton);
	}
	m_ButtonList.clear();

	for( size_t i= 0; i < m_SepList.size(); i++)
	{
		i3UIListView_Sep* pSep = m_SepList[i];

		RemoveControl( pSep);
		I3_SAFE_RELEASE( pSep);
	}
	m_SepList.clear();

	m_IsIndividualHeaderAlign.clear();

	I3_SAFE_DELETE_ARRAY( m_pbNextSortAscending);
}

void i3UIListView_Header::_UpdateShapeState(void)
{
	setShapeVisible( I3UI_LVHEADER_SHAPE_HEADER, true);
}

bool i3UIListView_Header::_isResizable( void)
{
	if( m_pBoundBox != nullptr)
		return m_pBoundBox->isHeaderResizable();

	return false;
}

/*virtual*/ void i3UIListView_Header::_ForceVisible( bool bFlag)
{
	

	i3UIControl::_ForceVisible( bFlag);

	for(size_t i = 0; i < m_ButtonList.size(); i++)
	{
		i3UIButton* pButton = m_ButtonList[ i];
		I3ASSERT( pButton);

		pButton->_ForceVisible( bFlag);
	}

	for(size_t i = 0; i < m_SepList.size(); i++)
	{
		i3UIListView_Sep* pSep = m_SepList[ i];
		I3ASSERT( pSep);

		pSep->_ForceVisible( bFlag);
	}
}


void i3UIListView_Header::_AddNewButton()
{
	i3UIButton* pButton = i3UIButton::new_object();

	if( m_pTemplate_Button != nullptr)
	{
#if defined( I3_DEBUG)
		pButton->SetName( "Head_Button");
#endif
		pButton->SetTemplate( m_pTemplate_Button);
		pButton->EnableCtrl( true);
	}
	
	if( m_pBoundBox)
	{
		if(	m_defFontInfo.m_strName.empty() )	
		{
			pButton->SetFont(m_pBoundBox->getDefaultFont() );			// 툴에 헤더의 폰트설정이 아직 없어, 박스의 폰트를 사용합니다. (11.07.06.수빈)
		}
		else
		{
			UINT32 weight = 0;
			if( m_defFontInfo.m_Style & I3UI_FONT_STYLE_BOLD)
				weight = 400;

			pButton->SetFont( m_defFontInfo.m_strName.c_str(), m_defFontInfo.m_Size, weight);
		}
	}

	pButton->addGNodeStyle( I3_GAMENODE_STYLE_VOLATILE);
	pButton->addStyle( I3UI_STYLE_SLAVE);
	
	AddControl( pButton);
	m_ButtonList.push_back( pButton);
	m_IsIndividualHeaderAlign.push_back(false);
}

void	i3UIListView_Header::SetBoundBox( i3UIListView_Box* pBox)
{	
	m_pBoundBox = pBox;	
	
	if( m_defFontInfo.m_strName.empty() )
	{
		const size_t numButton = m_ButtonList.size();
		for (size_t i = 0 ; i < numButton ; ++i)
		{
			i3UIButton* pb = m_ButtonList[i];
			pb->SetFont(m_pBoundBox->getDefaultFont());			// 툴에 헤더의 폰트설정이 아직 없어, 박스의 폰트를 사용합니다. (11.07.06.수빈)
		}

		m_defFontInfo.m_strName = m_pBoundBox->getDefaultFont()->lfFaceName;
	}
}


void i3UIListView_Header::_AddNewSep()
{
	i3UIListView_Sep* pSep = i3UIListView_Sep::new_object();

	if( m_pTemplate_Sep != nullptr)
	{
#if defined( I3_DEBUG)
		pSep->SetName( "Head_Sep");
#endif
		pSep->SetTemplate( m_pTemplate_Sep);
		pSep->EnableCtrl( true);
	}
	pSep->addGNodeStyle( I3_GAMENODE_STYLE_VOLATILE);
	pSep->addStyle( I3UI_STYLE_SLAVE);

	AddControl( pSep);
	m_SepList.push_back( pSep);
}

void i3UIListView_Header::_AdjustButtonCount( INT32 nCount)
{
	INT32 nSepCount = max( nCount-1, 0);

	INT32 nPrevButtonCount = (INT32)m_ButtonList.size();
	INT32 nPrevSepCount = (INT32)m_SepList.size();

	// Update Lists
	if( nPrevButtonCount < nCount)
	{
		for( INT32 i=0; i< nCount - nPrevButtonCount; i++)
		{
			_AddNewButton();
		}
		for( INT32 i=0; i< nSepCount - nPrevSepCount; i++)
		{
			_AddNewSep();
		}
		
		bool* pbAscending = new bool[nCount];
		NetworkDump_Malloc( pbAscending, sizeof( bool) * nCount, __FILE__, __LINE__);

		for ( INT32 i = 0 ; i < nCount ; ++i) pbAscending[i] = true;

		I3_SAFE_DELETE_ARRAY( m_pbNextSortAscending);
		m_pbNextSortAscending = pbAscending; 
				
	}
	else if( nPrevButtonCount > nCount)
	{
		for( INT32 i = nPrevButtonCount-1; i >= nCount ; i--)
		{
			i3UIButton* pButton = m_ButtonList[i];
			m_ButtonList.erase(m_ButtonList.begin() + i);
			//m_ButtonList.Delete(i);
			RemoveControl( pButton);
			I3_SAFE_RELEASE( pButton);

			m_IsIndividualHeaderAlign.erase(m_IsIndividualHeaderAlign.begin() + i);
		}
		for( INT32 i = nPrevSepCount-1; i >= nSepCount ; i--)
		{
			i3UIListView_Sep* pSep = m_SepList[i];
			m_SepList.erase(m_SepList.begin() + i);
			//m_SepList.Delete(i);
			RemoveControl( pSep);
			I3_SAFE_RELEASE( pSep);
		}

		bool* pbAscending = new bool[nCount];
		NetworkDump_Malloc( pbAscending, sizeof( bool) * nCount, __FILE__, __LINE__);

		for ( INT32 i = 0 ; i < nCount ; ++i) pbAscending[i] = true;
		I3_SAFE_DELETE_ARRAY( m_pbNextSortAscending);
		m_pbNextSortAscending = pbAscending; 
	}

	I3ASSERT( nCount == (INT32)m_ButtonList.size());
}

void i3UIListView_Header::_AdjustButtonRect( i3UIListView_Item* pBoundItem, REAL32 fHeight)
{
	if( pBoundItem == nullptr)
		return;

	INT32 nButtonCount = (INT32)m_ButtonList.size();
	INT32 nCellCount = pBoundItem->getCellCount();
	I3ASSERT( nButtonCount == nCellCount);

	REAL32 fSepWidth = pBoundItem->getSepWidth();
	REAL32 offX = 0.0f;
	REAL32 offY = 0.0f;

	for( INT32 i=0; i< nButtonCount; i++)
	{
		REAL32 fCellWidth = pBoundItem->getCellWidth(i);

		i3UIButton* pButton = m_ButtonList[i];
		pButton->setSize( fCellWidth, fHeight);
		pButton->SetPosNoNotify( offX, offY, 0);
		offX += fCellWidth;

		if( i < (INT32)m_HeaderTexts.size())
			pButton->SetText( m_HeaderTexts[i]);
		
		if( !m_IsIndividualHeaderAlign[i] )
			pButton->SetTextAlign( (I3UI_TEXT_ALIGN_X)m_defXAlign, (I3UI_TEXT_ALIGN_Y)m_defYAlign);

		pButton->SetButtonTextColorAll( m_defColor);


		if( i < nButtonCount-1)
		{
			i3UIListView_Sep* pSep = m_SepList[i];
			pSep->setSize( fSepWidth, fHeight);
			pSep->SetPosNoNotify( offX, offY, 0);
			offX += fSepWidth;
		}
	}
}

void i3UIListView_Header::UpdateShape( i3UIListView_Item* pBoundItem, REAL32 fHeight)
{
	//setSize 시에 OnSize가 먼저 호출되고 나중에 m_Size가 업데이트 되는 것 때문에
	m_Size.y = fHeight;	

	if( pBoundItem == nullptr)
	{
		_AdjustButtonCount( 0);
		_AdjustButtonRect( nullptr, fHeight);
	}
	else
	{
		INT32 nButtonCount = pBoundItem->getCellCount();
		
		_AdjustButtonCount( nButtonCount);
		_AdjustButtonRect( pBoundItem, fHeight);
	}
}

void i3UIListView_Header::SetHeaderText( const i3::vector<i3::rc_wstring>& StrList)
{
	I3ASSERT( StrList.size() == m_ButtonList.size());
	
	m_HeaderTexts = StrList;				// 대입만으로 간단이 끝낼수 있다..

//	m_HeaderTexts.Clear();
//	for( size_t i=0; i< StrList.size(); i++)
//	{
//		m_HeaderTexts.push_back( StrList[i]);
//	}

	for( size_t i=0; i< m_ButtonList.size(); i++)
	{
		i3UIButton* pButton = m_ButtonList[i];
		pButton->SetText( m_HeaderTexts[i]);

		pButton->setModifyState();
	}
}

void i3UIListView_Header::SetHeaderColor( COLOR* pCol)
{
	i3Color::Copy( &m_defColor, pCol);

	for( size_t i=0; i< m_ButtonList.size(); i++)
	{
		i3UIButton* pButton = m_ButtonList[i];
		pButton->SetButtonTextColorAll( *pCol);
	}
}

void i3UIListView_Header::SetHeaderAlign( INT32 xAlign, INT32 yAlign)
{
	m_defXAlign = xAlign;
	m_defYAlign = yAlign;

	for( size_t i=0; i< m_ButtonList.size(); i++)
	{
		i3UIButton* pButton = m_ButtonList[i];
		pButton->SetTextAlign( (I3UI_TEXT_ALIGN_X)xAlign, (I3UI_TEXT_ALIGN_Y)yAlign);
	}
}

void i3UIListView_Header::SetIndividualHeaderAlign( INT32 Cloumn, INT32 xAlign, INT32 yAlign)
{
	size_t i = Cloumn;
	if( i >= m_IsIndividualHeaderAlign.size() )
		return;

	i3UIButton* pButton = m_ButtonList[Cloumn];
	pButton->SetTextAlign( (I3UI_TEXT_ALIGN_X)xAlign, (I3UI_TEXT_ALIGN_Y)yAlign);

	m_IsIndividualHeaderAlign[Cloumn] = true;
}

void i3UIListView_Header::SetHeaderFont( const char * pszFontName, INT32 size /*= 10*/,
		I3UI_FONT_QUALITY quality /*= I3UI_FONT_QUALITY_ANTIALIASED*/, I3UI_FONT_STYLE style /*= 0*/)
{
	
	i3UIButton * pButton;

	if( pszFontName != nullptr)
	{
		m_defFontInfo.m_strName = pszFontName;
	}

	m_defFontInfo.m_Size = size;
	m_defFontInfo.m_Quality = quality;
	m_defFontInfo.m_Style = style;

	INT32 weight = 0;
	if( style & I3UI_FONT_STYLE_BOLD)
		weight = 400;
	for( size_t i = 0; i < m_ButtonList.size(); i++)
	{
		pButton = m_ButtonList[ i];

		pButton->SetFont( m_defFontInfo.m_strName.c_str(), size, weight);
	}
}

void i3UIListView_Header::SetHeaderFont( LOGFONTA * lf)
{
	
	i3UIButton * pButton;

	m_defFontInfo.m_strName = lf->lfFaceName;
	m_defFontInfo.m_Size = lf->lfHeight;
	m_defFontInfo.m_Quality = (I3UI_FONT_QUALITY) lf->lfQuality;

	I3UI_FONT_STYLE style = 0;
	if( lf->lfWeight >= 400)
		style |= I3UI_FONT_STYLE_BOLD;
	if( lf->lfItalic)
		style |= I3UI_FONT_STYLE_ITALIC;
	if( lf->lfUnderline)
		style |= I3UI_FONT_STYLE_UNDERLINE;
	if( lf->lfStrikeOut)
		style |= I3UI_FONT_STYLE_STRIKEOUT;

	m_defFontInfo.m_Style = style;

	for(size_t i = 0; i < m_ButtonList.size(); i++)
	{
		pButton = m_ButtonList[ i];

		pButton->SetFont( lf);
	}
}

void i3UIListView_Header::SetHeaderFontName( const char * pszFontName)
{
	
	i3UIButton * pButton;

	m_defFontInfo.m_strName = pszFontName;

	INT32 weight = 0;
	if( m_defFontInfo.m_Style & I3UI_FONT_STYLE_BOLD)
		weight = 400;

	for( size_t i = 0; i < m_ButtonList.size(); i++)
	{
		pButton = m_ButtonList[ i];

		pButton->SetFont( pszFontName, m_defFontInfo.m_Size, weight);
	}
}

void i3UIListView_Header::Reset( void)
{
	for( size_t i=0; i< m_ButtonList.size(); i++)
	{
		i3UIButton* pButton = m_ButtonList[i];
		pButton->SetText( "");	
		pButton->setModifyState();
		m_IsIndividualHeaderAlign[i] = false;
	}

	m_defXAlign = 0;
	m_defYAlign = 0;
	i3Color::Set( &m_defColor, (UINT8)255, (UINT8)255, (UINT8)255, (UINT8)255);

	m_HeaderTexts.clear();
}

// ScrollBar의 Enable여부에 따라 Header의 마지막 Button의 Width를 바꿔줍니다. 
void i3UIListView_Header::ResizeLastButton( void)
{
	INT32 nBtnCnt = (INT32)m_ButtonList.size();
	REAL32 fBtnWTot = 0.0f;
	REAL32 fBtnWLast = 0.0f;

	for( INT32 i=0; i< nBtnCnt; i++)
	{
		i3UIButton* pButton = m_ButtonList[i];
		fBtnWTot += pButton->getWidth();

		if( i < nBtnCnt-1)
		{
			i3UIListView_Sep* pSep = m_SepList[i];
			fBtnWTot += pSep->getWidth();
		}
		else	// Last loop
		{
			fBtnWLast = pButton->getWidth();
			REAL32 fDiff = m_Size.x - fBtnWTot;
			pButton->setSize( fBtnWLast + fDiff, pButton->getHeight());
		}
	}
}

void i3UIListView_Header::OnBindTemplate(void)
{
	i3UIControl::OnBindTemplate();

	if( m_pTemplate == nullptr)
		return;

	m_pTemplate_Button = (i3UITemplate_Button*)m_pTemplate->getSubTemplate( I3UI_LVHEADER_SUBTEMPLATE_BUTTON);
	m_pTemplate_Sep = (i3UITemplate_LVSep*)m_pTemplate->getSubTemplate( I3UI_LVHEADER_SUBTEMPLATE_SEP);

	for( size_t i=0; i< m_ButtonList.size(); i++)
	{
		i3UIButton* pButton = m_ButtonList[i];
		pButton->SetTemplate( m_pTemplate_Button);
		pButton->EnableCtrl( true);

		if( i < m_SepList.size())
		{
			i3UIListView_Sep* pSep = m_SepList[i];
			pSep->SetTemplate( m_pTemplate_Sep);
			pSep->EnableCtrl( true);
		}
	}
}

INT32 i3UIListView_Header::_GetButtonIndex( i3UIButton* pButton)
{
	for( size_t i=0; i< m_ButtonList.size(); i++)
	{
		i3UIButton* pBtn = m_ButtonList[i];
		if( pBtn == pButton)
			return (INT32)i;
	}

	return -1;
}

void i3UIListView_Header::OnNotify( i3UIControl * pCtrl, I3UI_EVT event, INT32 param1, INT32 param2)
{
	switch( event)
	{
	case I3UI_EVT_BTN_PUSHED:
		{
			I3TRACE(" i3UIListView_Header::OnNotify -------- I3UI_EVT_BTN_PUSHED\n");

			INT32 nBtnIdx = _GetButtonIndex( (i3UIButton*)pCtrl);		// if문 내부에 있는것을 외부로 노출..
			I3ASSERT( nBtnIdx != -1);

//			bool bAscend = true;	// 기본적으로 오름차순으로 정렬
//			if( nBtnIdx == m_nPressedBtnIndex)
//				bAscend = !m_bSortAscending;	// 두번째로 눌렸다면 
			bool bAscend = m_pbNextSortAscending[nBtnIdx];		// 인덱스가 -1이면 안되는데, 위에 ASSERT가 기존에 존재..
			m_pbNextSortAscending[nBtnIdx] = !bAscend;			

			if( isHeaderStyle( I3UI_LVH_STYLE_AUTOSORT))
			{
				i3GameNode* pParent = getParent();
				I3ASSERT( i3::same_of<i3UIListView_Box*>(pParent));
				i3UIListView_Box* pLVBox = (i3UIListView_Box*)pParent;

				pLVBox->SortByCellIndex( nBtnIdx, bAscend);

			}

//			m_nPressedBtnIndex = nBtnIdx;
//			m_bSortAscending = bAscend;

			ParentNotify(I3UI_EVT_LVH_PUSHED, nBtnIdx, INT32(bAscend) );			// 외부에서 헤더버튼을 다른 용도로 쓰기 위해, 이벤트를 둠..(11.07.18.수빈)
		}
		break;
	case I3UI_EVT_DRAGSTART:
		{
			if( i3::same_of< i3UIListView_Sep* >(pCtrl)
				&& _isResizable())
			{
				I3TRACE("I3UI_EVT_DRAGSTART\n");
				POINT point;
				point.x = param1;
				point.y = param2;
				m_nSelectedSep = _isPointInSep( point);

				if( m_nSelectedSep != -1)
				{
					i3UIListView_Sep* pSep = m_SepList[m_nSelectedSep];
					m_cursorOffset = point.x - pSep->getAccumulatedPos()->x;
				}
			}
		}
		break;
	case I3UI_EVT_DRAGEND:
		{
			if( i3::same_of<i3UIListView_Sep* >(pCtrl)
				&& _isResizable())
			{
				I3TRACE("I3UI_EVT_DRAGEND\n");
				m_nSelectedSep = -1;
			}
		}
		break;
	case I3UI_EVT_DRAGGING:
		{
			if( i3::same_of<i3UIListView_Sep* >(pCtrl)
				&& _isResizable())
			{
				I3TRACE("I3UI_EVT_DRAGGING\n");
				bool bInsideSep = false;

				POINT point;
				point.x = param1;
				point.y = param2;

				if( _isPointInSep( point) != -1)
					bInsideSep = true;
				
				if( m_nSelectedSep != -1)
				{
					_OnChangeSepPos( m_nSelectedSep, point);
				}
			}
		}
		break;
	case I3UI_EVT_ENTERMOUSE:
		{
			if( i3::same_of<i3UIListView_Sep*>(pCtrl))
			{
				ParentNotify( event, param1, param2);
			}
		}
		break;
	case I3UI_EVT_LEAVEMOUSE:
		{
			if( i3::same_of<i3UIListView_Sep*>(pCtrl) )
			{
				ParentNotify( event, param1, param2);
			}
		}
		break;
	case I3UI_EVT_MOUSEWHEEL:
		{
			I3TRACE("i3UIListView_Header::OnNotify(I3UI_EVT_MOUSEWHEEL)\n");
			ParentNotify( event, param1, param2);
		}
		break;
	/*case I3UI_EVT_RESERVED5:
		I3TRACE("I3UI_EVT_RESERVED5\n");
		break;
	case I3UI_EVT_RESERVED6:
		I3TRACE("I3UI_EVT_RESERVED6\n");
		break;*/
	default:
		break;
	}

	i3UIControl::OnNotify( pCtrl, event, param1, param2);
}


INT32 i3UIListView_Header::_isPointInSep( POINT point)
{
	INT32 nSepCount = (INT32)m_SepList.size();

	{
		//point를 상대좌표로 변환해서 넘겨준다.
		VEC3D* vPos = getAccumulatedPos();
	
		point.x = (LONG)(point.x - vPos->x);
		point.y = (LONG)(point.y - vPos->y);
	}

	for( INT32 i=0; i<nSepCount; i++)
	{
		i3UIListView_Sep* pSep = m_SepList[i];

		if( pSep->isPtInRect( point))
			return i;
	}

	return -1;
}

void i3UIListView_Header::_OnChangeSepPos( INT32 nIdx, POINT point)
{
	I3ASSERT( nIdx < (INT32)m_ButtonList.size()-1);

	{
		//point를 상대좌표로 변환
		VEC3D* vPos = getAccumulatedPos();
	
		point.x = (LONG)(point.x - vPos->x);
		point.y = (LONG)(point.y - vPos->y);
	}

	i3UIListView_Sep* pSep = m_SepList[nIdx];
	i3UIButton* pLeft = m_ButtonList[nIdx];
	i3UIButton* pRight = m_ButtonList[nIdx+1];

	REAL32 fTotalWidth = pLeft->getWidth() + pRight->getWidth() + pSep->getWidth();

	// MINMAX
	REAL32 min = pLeft->getPos()->x;
	REAL32 max = pRight->getPos()->x + pRight->getWidth() - pSep->getWidth();
	REAL32 fX = MINMAX( min, point.x, max);

	// Sep
	pSep->SetPosNoNotify( fX - m_cursorOffset, pSep->getPos()->y, 0.0f);
	
	// Left Cell
	REAL32 leftSize = pSep->getPos()->x - pLeft->getPos()->x;
	pLeft->SetSizeNoNotify( leftSize, getHeight());
	I3PRINTLOG(I3LOG_NOTICE, "_OnChangeSepPos SetSize : %f, %f", leftSize, getHeight());
	
	// Right Cell
	REAL32 rightSize = fTotalWidth - pLeft->getWidth() - pSep->getWidth();
	pRight->SetSizeNoNotify( rightSize, getHeight());
	I3PRINTLOG(I3LOG_NOTICE, "_OnChangeSepPos SetSize : %f, %f", rightSize, getHeight());
	pRight->SetPosNoNotify( pSep->getPos()->x + pSep->getWidth(), pRight->getPos()->y, 0.0f);

	
	// Apply to m_pBoundItem and m_ItemList
	if( m_pBoundBox != nullptr)
	{
		i3::vector<i3UIListView_Item*>& ItemList = m_pBoundBox->getItemList();

		// 마지막 셀인 경우
		if( nIdx + 2 == (INT32)m_ButtonList.size() && m_pBoundBox->isScrollBarEnable())
			rightSize -= m_pBoundBox->getScrollBarWidth();

		for( size_t i=0; i< ItemList.size(); i++)
		{
			i3UIListView_Item* pItem = ItemList[i];
			i3::vector<i3UIListView_Sep*>& SepList = pItem->getSepList();
			i3::vector<i3UIListView_Cell*>& CellList = pItem->getCellList();

			// Sep
			i3UIListView_Sep* pItemSep = SepList[nIdx];
			pItemSep->SetPosNoNotify( fX, pItemSep->getPos()->y, 0.0f);

			// Left
			i3UIListView_Cell* pLeftCell = CellList[nIdx];
			pLeftCell->SetSizeNoNotify( leftSize, pLeftCell->getHeight());
			I3PRINTLOG(I3LOG_NOTICE, "_OnChangeSepPos SetSize : %f, %f", leftSize, pLeftCell->getHeight());

			// Right
			i3UIListView_Cell* pRightCell = CellList[nIdx + 1];

			pRightCell->SetSizeNoNotify( rightSize, pRightCell->getHeight());
			I3PRINTLOG(I3LOG_NOTICE, "_OnChangeSepPos SetSize : %f, %f", rightSize, pRightCell->getHeight());
			pRightCell->SetPosNoNotify( pSep->getPos()->x + pSep->getWidth(), pRightCell->getPos()->y, 0.0f);
		}
	}
}

void i3UIListView_Header::OnEnabled( bool bEnable)
{
	i3UIControl::OnEnabled( bEnable);

	for( size_t i=0; i< m_ButtonList.size(); i++)
	{
		i3UIButton* pButton = m_ButtonList[i];
		pButton->EnableCtrl( bEnable);
	}

	for( size_t i=0; i< m_SepList.size(); i++)
	{
		i3UIListView_Sep* pSep = m_SepList[i];
		pSep->EnableCtrl( bEnable);
	}
}

void i3UIListView_Header::prepareSprites( void)
{
	i3UIControl::prepareSprites();

	for( UINT32 i = 0; i < m_ButtonList.size(); i++)
	{
		i3UIButton * pButton = m_ButtonList[i];
		pButton->prepareSprites();
	}

	for( UINT32 i = 0; i < m_SepList.size(); i++)
	{
		i3UIListView_Sep * pSep = m_SepList[i];
		pSep->prepareSprites();
	}
}


#if defined( I3_COMPILER_VC)
#pragma pack(push, 4)
#endif

namespace pack
{
	struct UILISTVIEW_HEADER
	{
		INT8		m_ID[4] = { 'U', 'L', 'V', 'H' };
		UINT32		m_HeaderStyle = 0;
		INT8		m_FontName[64] = { 0 };				// 파일에 들어가는 것은 문자배열을 유지해야한다.(12.11.20.수빈)
		UINT32		m_FontSize = 0;
		UINT32		m_FontQuality = 0;
		UINT32		m_FontStyle = 0;
		UINT32		pad[8] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack(pop)
#endif

UINT32 i3UIListView_Header::OnSave( i3ResourceFile * pResFile)
{
	UINT32	Rc, Result= 0;
	pack::UILISTVIEW_HEADER data;

	Result = i3UIControl::OnSave( pResFile );
	I3_CHKIO( Result);
	
	data.m_HeaderStyle = m_HeaderStyle;
	i3::safe_string_copy( (char*)data.m_FontName, m_defFontInfo.m_strName.c_str(), 64);
	data.m_FontSize = m_defFontInfo.m_Size;
	data.m_FontQuality = (UINT32) m_defFontInfo.m_Quality;
	data.m_FontStyle = m_defFontInfo.m_Style;

	Rc = pResFile->Write( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32 i3UIListView_Header::OnLoad( i3ResourceFile * pResFile)
{
i3_prof_func();

	UINT32	Rc, Result =0;
	i3Stream * pStream = pResFile->GetStream();
	pack::UILISTVIEW_HEADER data;

	Result = i3UIControl::OnLoad( pResFile );
	I3_CHKIO( Result);

	Rc = pStream->Read( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	m_HeaderStyle = data.m_HeaderStyle;

	if( data.m_FontSize == 0)
		data.m_FontSize = 10;

	SetHeaderFont( (const char *)data.m_FontName, data.m_FontSize, (I3UI_FONT_QUALITY) data.m_FontQuality, (I3UI_FONT_STYLE) data.m_FontStyle);

#if defined( I3_DEBUG)
	if( isHeaderStyle( I3UI_LVH_STYLE_AUTOSORT))
		m_bAutoSort = true;
	else
		m_bAutoSort = false;
#endif

	return Result;
}

bool i3UIListView_Header::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = i3UIControl::OnSaveXML( pFile, pXML);

	if( bResult == false)
		return false;

	pXML->addAttr( "HeaderStyle", m_HeaderStyle);

	pXML->addAttr( "HeaderFontName", m_defFontInfo.m_strName.c_str());
	pXML->addAttr( "HeaderFontSize", m_defFontInfo.m_Size);
	pXML->addAttr( "HeaderFontQuality", m_defFontInfo.m_Quality);
	pXML->addAttr( "HeaderFontStyle", m_defFontInfo.m_Style);

	return bResult;
}

bool i3UIListView_Header::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = i3UIControl::OnLoadXML( pFile, pXML);

	if( bResult == false)
		return false;

	pXML->getAttr( "HeaderStyle", &m_HeaderStyle);

	const char * pszFontName = pXML->getAttr( "HeaderFontName");
	INT32 size, quality;
	UINT32 style;
	
	pXML->getAttr( "HeaderFontSize", &size);
	pXML->getAttr( "HeaderFontQuality", &quality);
	pXML->getAttr( "HeaderFontStyle", &style);

	if( size == 0)
		size = 10;

	SetHeaderFont( pszFontName, size, (I3UI_FONT_QUALITY) quality, style);

#if defined( I3_DEBUG)
	if( isHeaderStyle( I3UI_LVH_STYLE_AUTOSORT) )
		m_bAutoSort = true;
	else
		m_bAutoSort = false;
#endif

	return bResult;
}

void i3UIListView_Header::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3UIControl::CopyTo( pDest, method );

	I3ASSERT( pDest->kind_of( meta()));

	i3UIListView_Header* pLVHeader = (i3UIListView_Header*)pDest;
	pLVHeader->setHeaderStyle( m_HeaderStyle);

	pLVHeader->SetHeaderText( m_HeaderTexts);
	pLVHeader->SetHeaderAlign( m_defXAlign, m_defYAlign);
	pLVHeader->SetHeaderColor( &m_defColor);
	pLVHeader->SetHeaderFont( m_defFontInfo.m_strName.c_str(), m_defFontInfo.m_Size, m_defFontInfo.m_Quality, m_defFontInfo.m_Style);
}


#if defined( I3_DEBUG)
void i3UIListView_Header::GetTDKGridProperties( i3::vector<PROPERTYINFO*>& List)
{
	i3UIControl::GetTDKGridProperties( List);	

	AddMFCProperty_BeginGroup( List, "i3UIListView_Header");
		AddMFCProperty_Bool( List, &m_bAutoSort, "AutoSort", "자동 Sort의 사용 유무를 설정합니다.");
		AddMFCProperty_Font( List,	&m_lf, &m_defColor, "Font");
		AddMFCProperty_Color( List, &m_defColor, "Font Color");
		AddMFCProperty_Combo( List, s_strFontQuality, (INT32*)&m_FontQuality, "Font Quality");
	AddMFCProperty_EndGroup( List);
}

void i3UIListView_Header::OnTDKPropertyChanged(const char *szName)
{
	if( i3::generic_is_iequal( szName, "AutoSort") )
	{
		if( m_bAutoSort)
		{
			addHeaderStyle( I3UI_LVH_STYLE_AUTOSORT);
		}
		else
		{
			removeHeaderStyle( I3UI_LVH_STYLE_AUTOSORT);
		}
	}
	else if( i3::generic_is_iequal( szName, "Font") )
	{
		I3UI_FONT_STYLE style = 0;
		if( m_lf.lfWeight > 400)	style |= I3UI_FONT_STYLE_BOLD;
		if( m_lf.lfItalic > 0)		style |= I3UI_FONT_STYLE_ITALIC;
		if( m_lf.lfUnderline > 0)	style |= I3UI_FONT_STYLE_UNDERLINE;
		if( m_lf.lfStrikeOut > 0)	style |= I3UI_FONT_STYLE_STRIKEOUT;

		SetHeaderFont( m_lf.lfFaceName, m_lf.lfHeight, m_FontQuality, style);
	}
	else if( i3::generic_is_iequal( szName, "Font Color") )
	{
	}
	else if( i3::generic_is_iequal( szName, "Font Quality") )
	{
	}
	else
	{
		i3UIControl::OnTDKPropertyChanged( szName);
	}
}
#endif	// I3_DEBUG

