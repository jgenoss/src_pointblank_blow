#include "i3FrameworkPCH.h"
#include "i3UIListView_Box.h"
#include "i3UITemplate_LVBox.h"
#include "i3Framework.h"
#include "i3UITemplate_LVSep.h"
#include "i3UITemplate_LVItem.h"

#include "i3UIScrollBar.h"
#include "i3UIListView_Item.h"
#include "i3UIListView_Header.h"
#include "i3UITemplate_LVHeader.h"
#include "i3UIListView_Data.h"
#include "i3UIListView_Cell.h"
#include "i3UIListView_Sep.h"
#include "i3UIManager.h"
#include "i3UIImageBoxEx.h"
#include "i3Base/string/compare/generic_icompare.h"
#include "i3Base/string/compare/generic_is_iequal.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"
#include "i3Base/string/ext/atoi.h"
#include "i3Base/string/ext/utf8_to_utf16.h"
#include "i3Base/string/ext/utf16_to_utf8.h"

#include "i3VTFileFormat.h"

//#define i3_profile_enable
#include "i3Base/profile/profile.h"

I3_CLASS_INSTANCE( i3UIListView_Box);

i3UIListView_Box::i3UIListView_Box()
{
	m_lfDefault.lfHeight = _PointToHeight( 10);
    m_lfDefault.lfWidth = 0;
    m_lfDefault.lfEscapement = 0;
    m_lfDefault.lfOrientation = 0;
    m_lfDefault.lfWeight = 400;	// bold => 700
    m_lfDefault.lfItalic = FALSE;
    m_lfDefault.lfUnderline = FALSE;
    m_lfDefault.lfStrikeOut = FALSE;
    m_lfDefault.lfCharSet = DEFAULT_CHARSET;
    m_lfDefault.lfOutPrecision = OUT_CHARACTER_PRECIS;
    m_lfDefault.lfClipPrecision = CLIP_CHARACTER_PRECIS;
    m_lfDefault.lfQuality = DEFAULT_QUALITY;
    m_lfDefault.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
    i3::string_ncopy_nullpad( m_lfDefault.lfFaceName, "새굴림", sizeof(m_lfDefault.lfFaceName));
}

i3UIListView_Box::~i3UIListView_Box()
{
	I3_SAFE_RELEASE( m_pScrollBar);

	for( size_t i=0; i< m_ItemList.size(); i++)
	{
		i3UIListView_Item* pItem = m_ItemList[i];
		RemoveControl( pItem);
		I3_SAFE_RELEASE( pItem);
	}
	m_ItemList.clear();

	for( size_t i=0; i< m_DataList.size(); i++)
	{
		LVITEM_DATA* pData = m_DataList[i];
		I3_SAFE_RELEASE( pData);
	}
	m_DataList.clear();

	for( size_t i = 0;i < m_TemplateInfo.size(); ++i)
	{
		TEMPLATE_INFO * pInfo = m_TemplateInfo[i];
		I3_MUST_DELETE( pInfo);
	}
	m_TemplateInfo.clear();

	for( size_t i = 0; i < m_TextInfo.size(); ++i)
	{
		I3UI_LV_TEXTINFO * pInfo = m_TextInfo[i];
		I3MEM_SAFE_FREE( pInfo);
	}
	m_TextInfo.clear();

	I3_SAFE_RELEASE( m_pBoundItem);

	I3_SAFE_RELEASE( m_pCurOverCell);

	I3_SAFE_RELEASE(m_pTSepDefault);
}

void i3UIListView_Box::_UpdateShapeState(void)
{
	for( INT32 i = 0; i < I3UI_LVBOX_SHAPE_COUNT; ++i)
	{
		setShapeVisible( i, isValidShape(i));
	}
}

void i3UIListView_Box::_OnScroll( void)
{
	// 스크롤 됐을 때 처리해 줘야할 것들

	if( m_bClipItem)
		_RepositionItems();		// 1. Item 재배치

	_RepositionData();		// 2. Data 재배치
	
	if( m_bClipItem)
		_RepositionItems();		// 한번 더 콜

	addState( I3UI_STATE_NEED_UPDATE);
}

void i3UIListView_Box::Redraw( void)
{
	_OnScroll();
}

void i3UIListView_Box::_RecalcItemCount( void)
{
	INT32 nCount = _getNumberOfItemsNeeded();

	_AdjustItemCountTo( nCount);	// Item Instance 추가
}

void i3UIListView_Box::_RepositionItems( void)
{
	if( m_pBoundItem == nullptr)
		return;

	INT32 nScrollPos = 0;
	if( m_pScrollBar != nullptr)
		nScrollPos = m_pScrollBar->getScrollPos();

	REAL32 itemWidth = m_pBoundItem->getWidth();
	REAL32 itemHeight = m_pBoundItem->getHeight();

	REAL32 offY = m_rtMargin.top;
	if( m_pHeader != nullptr)
		offY += m_pHeader->getHeight();


	i3Rect rtBox;
	rtBox.set( (INT32)m_rtMargin.left, (INT32)offY, 
		(INT32)(m_Size.x - m_rtMargin.right), 
		(INT32)(m_Size.y - m_rtMargin.bottom));

	if( m_bClipItem && nScrollPos != 0)
		offY -= ( nScrollPos % (INT32)(itemHeight + m_vItemGap.y));			
																			// 디테일한 스크롤바의 경우, 아이템이 상하로 클립될때 쓰이는듯 한데...
																			// 현재 스크롤바에 대한 아이템 이동은 대체로 슬롯위치가 고정된 편입니다..(11.10.25.수빈)
	for( size_t i=0; i< m_ItemList.size(); i++)
	{
		INT32 nCol = i % m_nColumnCount;
		
		REAL32 offX = m_rtMargin.left + (itemWidth + m_vItemGap.x) * nCol;

		i3Rect rtItem;
		rtItem.set( (INT32)offX, (INT32)offY, (INT32)(offX + itemWidth), (INT32)(offY + itemHeight));
		
		i3UIListView_Item* pItem = m_ItemList[i];

		if( !pItem->isEnable() || i3UI::getToolDevMode())
			pItem->EnableCtrl( true);

		INT32 nDataIdx = (INT32)i + m_nFirstVisibleDataIndex;
		
		if( nDataIdx >= (INT32) m_DataList.size())
		{
			if( m_bFillWithEmptyItems /*&& !m_bClipItem*/)
			{
				if( m_bDisableEmptyItems)
					pItem->setDisabled(true);
			}
			else if( m_bPreviewMode)
			{
				//pItem->EnableCtrl( false);	//이걸로 하면 스크롤하다가 갑자기 i3Framework::SetFocus( nullptr)이 되어버림..
				// 아래의 사이즈를 0으로... 완전 꽁수임...
				pItem->SetSizeNoNotify( pItem->getWidth(), 0.f);

				if( nCol == m_nColumnCount-1)
					offY += itemHeight + m_vItemGap.y;

				continue;
			}
		}
		
		pItem->CalcRect( &rtBox, &rtItem);
		pItem->UpdateShapes();

		if( nCol == m_nColumnCount-1)
			offY += itemHeight + m_vItemGap.y;
	}

	addState( I3UI_STATE_NEED_UPDATE);
}


void i3UIListView_Box::_RepositionData( void)
{
	if( m_pBoundItem == nullptr)
		return;

	// 맨 위에 있는 Item과 연결되어 있는 Data의 Index 구하기
	if( m_pScrollBar != nullptr)
	{
		INT32 nScrollPos = m_pScrollBar->getScrollPos();
		REAL32 fItemHeight = m_BoundHeight;
		
		m_nFirstVisibleDataIndex = (nScrollPos / (INT32)( fItemHeight + m_vItemGap.y)) * m_nColumnCount;
	}

	
	for( size_t i=0; i< m_ItemList.size(); i++)
	{
		INT32 nDataIdx = i + m_nFirstVisibleDataIndex;
		i3UIListView_Item* pItem = m_ItemList[i];

		if( nDataIdx < (INT32)m_DataList.size())
		{
			LVITEM_DATA* pData = m_DataList[nDataIdx];
			
			if( pData != nullptr)
				pItem->SetSizeNoNotify( pItem->getWidth(), m_BoundHeight);
			else
				pItem->SetSizeNoNotify( pItem->getWidth(), 0.f);
			pItem->SetData( pData);
		}
		else
		{
			if( m_bFillWithEmptyItems)
			{
				pItem->SetData( nullptr);
				
				if( m_bDisableEmptyItems)
					pItem->setDisabled( true);	
				else
					pItem->setDisabled( false);
			}
			else
			{
				pItem->SetData( nullptr);
				pItem->SetSizeNoNotify( pItem->getWidth(), 0.0f);
			}
		}
	}

	addState( I3UI_STATE_NEED_UPDATE);
}


void i3UIListView_Box::_PlaceScrollBar( void)
{
	if( m_pScrollBar == nullptr || m_pBoundItem == nullptr)
		return;

	// Data 개수
	INT32 nDataCount = (INT32)m_DataList.size();
	
	// 한 Column에 들어가는 Data 개수
	INT32 nDataPerColumn = nDataCount / m_nColumnCount; 
	if( nDataCount % m_nColumnCount != 0)
		nDataPerColumn ++;

	REAL32 fItemHeight = m_pBoundItem->getHeight();
	
	// ScrollBar의 Range
	REAL32 fScrollRange = nDataPerColumn * fItemHeight + (nDataPerColumn - 1) * m_vItemGap.y;
	
	// Item이 들어가는 영역의 height
	REAL32 fItemBoxY = m_Size.y - m_rtMargin.top - m_rtMargin.bottom;
	if( m_pHeader != nullptr)
		fItemBoxY -= m_pHeader->getHeight();
	
	if( fScrollRange > fItemBoxY || 
		m_nScrollStyle == I3UI_LVBOX_SCROLL_STYLE_SHOW)
	{
		m_pScrollBar->EnableCtrl( true);

		if(m_UserFilterFunc != nullptr)
			m_UserFilterFunc(this);

		//Size
		m_pScrollBar->setSize( m_fWidthScrollBar, fItemBoxY);

		//Pos
		REAL32 fPosX = m_Size.x - m_rtMargin.right - m_fWidthScrollBar + m_fScrollBarMargin;
		REAL32 fPosY = m_Size.y - m_rtMargin.bottom - fItemBoxY;
		m_pScrollBar->setPos( fPosX, fPosY);
		
		//Adjust scroll range
		if( !m_bClipItem)
		{
			//마지막 아이템이 보이기 위해서는 Range를 더 늘려야 한다. 
			INT32 nItemCountPerPage = 0;

			{
				REAL32 fTemp = fItemBoxY;
				while( fTemp >= fItemHeight)
				{
					nItemCountPerPage++;
					fTemp = fTemp - fItemHeight - m_vItemGap.y;
				}
			}

			REAL32 fTemp = nItemCountPerPage * fItemHeight + (nItemCountPerPage - 1) * m_vItemGap.y;
			fScrollRange += (fItemBoxY - fTemp);
		}

		m_pScrollBar->setScrollRange( 0, (INT32)fScrollRange);

		//Page Size
		m_pScrollBar->setPageSize( (INT32)fItemBoxY);

		//Tick Size
		if( m_bClipItem)
			m_pScrollBar->setTickSize( 10);
		else
		{
			//m_pScrollBar->setTickSize( 3);
			m_pScrollBar->setTickSize( (INT32)fItemHeight);
		}

		//Cur Pos
		REAL32 rScrollMax = fScrollRange - fItemBoxY;
		if( rScrollMax < 0.0f )	rScrollMax = 0.0f;

		if( m_pScrollBar->getScrollPos() > rScrollMax )
			m_pScrollBar->setScrollPos( (INT32)(fScrollRange-1));

		UpdateAllComponents();

		addState( I3UI_STATE_NEED_UPDATE);
	}
}

void i3UIListView_Box::_HideScrollBar( void)
{

	if( m_pScrollBar == nullptr)
		return;

	if( m_pScrollBar->isEnable())
	{
		m_pScrollBar->setScrollPos( 0);
		m_pScrollBar->EnableCtrl( false);

		if(m_UserFilterFunc != nullptr)
			m_UserFilterFunc(this);


	}
	UpdateAllComponents();			

}

void i3UIListView_Box::_ReposHeader( void)
{
	if( m_pHeader != nullptr)
	{
		m_pHeader->SetPosNoNotify( m_rtMargin.left, m_rtMargin.top, 0.0f);

		REAL32 fWidth = m_Size.x - m_rtMargin.left - m_rtMargin.right;

		m_pHeader->SetSizeNoNotify( fWidth, m_pHeader->getHeight());	//?

		m_pHeader->UpdateShape( m_pBoundItem, m_pHeader->getHeight());
	}
}

void i3UIListView_Box::_ReposBoundItem( void)
{
	if( m_pBoundItem != nullptr)
	{
		//Pos
		REAL32 fX = m_rtMargin.left;
		REAL32 fY = m_rtMargin.top;
		if( m_pHeader != nullptr)
			fY += m_pHeader->getHeight();
		
		m_pBoundItem->setPos( fX, fY, 0.0f );
		
		//Size
		REAL32 w = m_Size.x - m_rtMargin.left - m_rtMargin.right;
		
		if( m_pScrollBar != nullptr  && m_pScrollBar->isEnable())
			w -= m_pScrollBar->getWidth();
		
		w -= (m_nColumnCount-1) * m_vItemGap.x;
		w /= m_nColumnCount;
		
		m_pBoundItem->SetSizeNoNotify( w, m_pBoundItem->getHeight());
	}
}

void i3UIListView_Box::SetTemplateInfo( INT32 nCol, const char* pszName)
{
	const size_t num_info = m_TemplateInfo.size();

	TEMPLATE_INFO* pFind = nullptr;

	for(size_t i=0; i< num_info  ; ++i) 
	{
		if (m_TemplateInfo[i]->m_nCol == nCol)
			pFind = m_TemplateInfo[i];
	}
	
	if ( pFind == nullptr )
	{
		pFind = new TEMPLATE_INFO; 
		NetworkDump_Malloc( pFind, sizeof( TEMPLATE_INFO), __FILE__, __LINE__);
		pFind->m_nCol = nCol;
		m_TemplateInfo.push_back( pFind);
	}
	
	pFind->m_TemplateList.push_back( pszName);
}

void i3UIListView_Box::SwapTemplateInfo( INT32 nCol, i3::vector<i3::rc_string>& List)
{
	const size_t num_info = m_TemplateInfo.size();

	TEMPLATE_INFO* pFind = nullptr;

	for(size_t i=0; i< num_info  ; ++i) 
	{
		if (m_TemplateInfo[i]->m_nCol == nCol)
			pFind = m_TemplateInfo[i];
	}

	if ( pFind == nullptr )
	{
		pFind = new TEMPLATE_INFO;
		NetworkDump_Malloc( pFind, sizeof( TEMPLATE_INFO), __FILE__, __LINE__);
		pFind->m_nCol = nCol;
		m_TemplateInfo.push_back( pFind);
	}

	pFind->m_TemplateList.clear();
	List.swap( pFind->m_TemplateList);
}

void i3UIListView_Box::GetTemplateName( INT32 nCol, char* pBuf, INT32 nBufSize)
{
	return GetTemplateName( nCol, 0, pBuf, nBufSize);
}

void i3UIListView_Box::GetTemplateName( INT32 nCol, INT32 nTemplateIdx, char* pBuf, INT32 nBufSize)
{
	for( size_t i=0; i< m_TemplateInfo.size(); i++)
	{
		TEMPLATE_INFO* pInfo = m_TemplateInfo[i];
		if( pInfo->m_nCol == nCol )
		{
			if( nTemplateIdx >= 0 && nTemplateIdx < (INT32)pInfo->m_TemplateList.size())
			{
				pInfo->m_TemplateList[nTemplateIdx].copy( pBuf, nBufSize);	return;
			}
			break;
		}
	}

	pBuf[0] = 0;
}

const i3::rc_string* i3UIListView_Box::GetTemplateName( INT32 nCol, INT32 nTemplateIdx) const
{
	for( size_t i=0; i< m_TemplateInfo.size(); i++)
	{
		TEMPLATE_INFO* pInfo = m_TemplateInfo[i];
		if( pInfo->m_nCol == nCol )
		{
			if( nTemplateIdx >= 0 && nTemplateIdx < (INT32)pInfo->m_TemplateList.size())
				return &pInfo->m_TemplateList[nTemplateIdx];
			break;
		}
	}
	return nullptr;
}

const TEMPLATE_INFO* i3UIListView_Box::GetTemplateInfo( INT32 nCol) const
{ 
	for( size_t i=0; i< m_TemplateInfo.size(); i++)
	{
		TEMPLATE_INFO* pInfo = m_TemplateInfo[i];
		if( pInfo->m_nCol == nCol )
			return pInfo;
	}
	return nullptr;
}

INT32 i3UIListView_Box::GetTemplateCount( INT32 nCol)
{ 
	for( size_t i=0; i< m_TemplateInfo.size(); i++)
	{
		TEMPLATE_INFO* pInfo = m_TemplateInfo[i];
		if( pInfo->m_nCol == nCol)
		{
			return pInfo->m_TemplateList.size();
		}	
	}

	return 0;
}

void i3UIListView_Box::BindHeader( i3UIListView_Header* pLVHeader)
{
	// Header가 Box의 Child로 들어올 때 호출된다.
	I3ASSERT( pLVHeader != nullptr);

	if( m_pHeader == nullptr)
	{
		m_pHeader = pLVHeader;
		m_pHeader->SetBoundBox( this);

		m_pHeader->EnableCtrl( true);

		m_nColumnCount = 1;

		UpdateAllComponents();
	}
}

void i3UIListView_Box::UnBindHeader( void)
{
	if( m_pHeader != nullptr)
	{
		VEC3D vOffset, vNewPos;
		i3Vector::Set( &vOffset, 10.0f, 10.0f, 0.0f);
		i3Vector::Add( &vNewPos, &vOffset, getAccumulatedPos());

		m_pHeader->setPos( &vNewPos);

		m_pHeader->UpdateShape( nullptr, m_pHeader->getHeight());

		m_pHeader->SetBoundBox( nullptr);
		m_pHeader = nullptr;

		UpdateAllComponents();
	}
}

bool i3UIListView_Box::isScrollBarEnable( void)
{
	if( m_pScrollBar != nullptr && m_pScrollBar->isEnable())
		return true;

	return false;
}

INT32 i3UIListView_Box::_getNumberOfItemsNeeded( void)
{
	m_nVisibleItemCount = 0;

	if( m_pBoundItem == nullptr)
		return 0;

	// Box를 구성하는데 필요한 최소 Item 개수를 구한다.
	REAL32 fHeight = m_Size.y - m_rtMargin.top - m_rtMargin.bottom;
	if( m_pHeader != nullptr)
	{
		fHeight -= m_pHeader->getHeight();
		fHeight = max( m_pHeader->getHeight(), fHeight);	//temp
	}
	else
	{
		fHeight = max( 10.0f, fHeight);		//temp
	}


	REAL32 fItemHeight = m_pBoundItem->getHeight();
	fItemHeight = MINMAX( 5.0f, fItemHeight, fHeight);	//temp

	m_nRowCount = 0;
	
	REAL32 fTemp = fHeight;
	while( fTemp >= fItemHeight)
	{
		m_nRowCount++;
		fTemp = fTemp - fItemHeight - m_vItemGap.y;
	}
			
	if( m_bClipItem)
	{
		if( fHeight / fItemHeight > m_nRowCount)
			m_nVisibleItemCount = m_nRowCount + 2;
		else
			m_nVisibleItemCount = m_nRowCount + 1; 

		m_nVisibleItemCount *= m_nColumnCount;
	}
	else
	{
		m_nVisibleItemCount = m_nRowCount * m_nColumnCount;
	}

	return m_nVisibleItemCount;
}

void i3UIListView_Box::_AdjustItemCountTo( INT32 nCount)
{
	// Box를 구성할 Item의 instance들을 nCount에 맞추어 생성/소멸한다.
	if( nCount > (INT32)m_ItemList.size())
	{
		for( INT32 i= (INT32)m_ItemList.size(); i< nCount; i++)
		{
			i3UIListView_Item* pLVItem = _NewObjectFrom( m_pBoundItem);	// Create Instances ( Clone)
#if defined( I3_DEBUG)
			pLVItem->m_ItemID = (INT32)m_ItemList.size();
#endif
			
			AddControl( pLVItem);
			m_ItemList.push_back( pLVItem);
		}
	}
	else if( nCount < (INT32)m_ItemList.size())
	{
		for( INT32 i= (INT32)m_ItemList.size()-1; i>= nCount; i--)
		{
			i3UIListView_Item* pLVItem = m_ItemList[i];

			pLVItem->ChangeCellCount( 0, getAncestor());

		//	m_ItemList.Delete( i);	
			m_ItemList.erase(m_ItemList.begin() + i);
			RemoveControl( pLVItem);

			I3_SAFE_RELEASE( pLVItem);
		}
	}
}

void i3UIListView_Box::ApplyChanges( void)
{

	// 1. Item 개수 결정 및 Add

	INT32 nDataCount = (INT32)m_DataList.size();

	INT32 nCount = _getNumberOfItemsNeeded();

	_AdjustItemCountTo( nCount);


	// 2. ScrollBar 배치( Show or Hide)
	{

		switch(m_nScrollStyle)
		{	
			case I3UI_LVBOX_SCROLL_STYLE_SHOW:	_PlaceScrollBar();	break;
			case I3UI_LVBOX_SCROLL_STYLE_HIDE:	_HideScrollBar();	break;
			default:
				{
					if( m_nRowCount * m_nColumnCount < nDataCount)
						_PlaceScrollBar();
					else
						_HideScrollBar();

				}
				break;
		}

		if( m_pHeader != nullptr)
			m_pHeader->ResizeLastButton();
		
	}


	// 2. Item 크기 및 위치 조정

	if( m_bClipItem)
		_RepositionItems();


	// 3. Data Update
	_RepositionData();


	if( m_bClipItem)
		_RepositionItems();


	// Sprite Layer가 많은관계로 non-Instance들을 한곳으로 묶어주자

	_ReOrderingForLayer();

	m_bApplyChangeData = false;

}

// Script 가 연결되지 않은 상태에서는 제대로 동작하지 않을 수 있습니다. 
bool i3UIListView_Box::OnPreviewMode( void)
{
	// Data는 이미 추가된 상태
	if( m_pBoundItem == nullptr)
		return false;

	// 안전성 검사( 같은 인덱스를 가진 cell끼리는 template category도 같아야함)
	/*for( INT32 i=0; i< m_pBoundItem->getCellCount(); i++)
	{
		ECELLTYPE eType = m_pBoundItem->getCellType(i);
		if( eType == ECELLTYPE_TEMPLATE)
		{
			if( m_DataList.GetCount() > 1)
			{
				LVITEM_DATA* pFirstData = (LVITEM_DATA*)m_DataList.GetItem(0);
				char conv[MAX_PATH];
				pFirstData->GetCellData( i, conv);
				i3UITemplate * pFirstTemplate = i3UI::FindTemplateByName( conv);
				I3UI_CATEGORY category = pFirstTemplate->getTemplateCategory();
				
				for( INT32 j=1; j< m_DataList.GetCount(); j++)
				{
					LVITEM_DATA* pData = (LVITEM_DATA*)m_DataList.GetItem(j);
					char buf[MAX_PATH];
					pData->GetCellData( i, buf);
					i3UITemplate* pTemplate = i3UI::FindTemplateByName( buf);
					if( pTemplate->getTemplateCategory() != category)
					{
						return false;
					}
				}
			}
		}
	}*/

	// bClip 여부 결정
	for( INT32 i=0; i< m_pBoundItem->getCellCount(); i++)
	{
		ECELLTYPE eType = m_pBoundItem->getCellType(i);
		
		//Text, Image 타입이 아닌 Cell이 하나라도 있으면 bClipItem은 강제로 False가 된다.
		if( eType != ECELLTYPE_TEXT && eType != ECELLTYPE_IMAGE)
		{
			m_bClipItem = false;
			break;
		}
	}

	// Header
	_CheckResizeHeader();

	// ScrollBar
	if( m_pScrollBar != nullptr)
	{
		m_pScrollBar->EnableCtrl( false);	//ApplyChange에서 결정됨
		if(m_UserFilterFunc != nullptr)
			m_UserFilterFunc(this);
	}

	m_bPreviewMode = true;

	ApplyChanges();

	if( m_pBoundItem != nullptr)
		m_pBoundItem->EnableCtrl( false);
	
	_RepositionItems();
	return true;
}

void i3UIListView_Box::OnEditMode( void)
{
	m_bPreviewMode = false;

	if( m_pBoundItem == nullptr)
		return;

	// Data 
	if (!m_DataList.empty())
	{
		for( INT32 i= (INT32)m_DataList.size()-1; i>=0 ; i--)
		{
			LVITEM_DATA* pData = m_DataList[i];
			m_DataList.erase(m_DataList.begin() + i);
		//	m_DataList.Delete(i);
			I3_SAFE_RELEASE( pData);
		}
	}

	// Items
	for( size_t i=0; i< m_ItemList.size(); i++)
	{
		i3UIListView_Item* pItem = m_ItemList[i];
		pItem->Reset( m_pBoundItem);
	}

	if (i3UI::getToolDevMode() == true)
	{
		m_pBoundItem->InitItem();
	}

	// ScrollBar
	if(m_nScrollStyle == I3UI_LVBOX_SCROLL_STYLE_SHOW)
		_PlaceScrollBar();
	else
		_HideScrollBar();

	// FocusCtrl
	i3GameNode* pParent = getParent();
	while( pParent != nullptr && !i3::same_of<i3UIFrameWnd*>(pParent))
		pParent = pParent->getParent();
	
	if( pParent != nullptr)
	{
		i3UIFrameWnd* pFrameWnd = (i3UIFrameWnd*)pParent;
		pFrameWnd->SetFocusCtrlNULL();
	}

	// Header
	if( m_pHeader != nullptr)
	{
		m_pHeader->Reset();
	}

	// Box
	for( size_t i=0; i< m_TextInfo.size(); i++)
	{
		I3UI_LV_TEXTINFO* pInfo = m_TextInfo[i];
		pInfo->m_nTextAlignX = 0;
		pInfo->m_nTextAlignY = 0;
	}
	//i3Color::Set( &m_colDefault, (UINT8)255, (UINT8)255, (UINT8)255, (UINT8)255);

}

void i3UIListView_Box::BindItem( i3UIListView_Item* pLVItem)
{
	// Item이 Box의 Child로 들어올 때 호출된다.
	I3ASSERT( pLVItem != nullptr);

	if( m_pBoundItem == nullptr)
	{
		I3_SAFE_ADDREF( pLVItem);

		// UI에 붙어 있는 것을 띄어낸다.
		if( i3UI::getManager()->isEditMode() == false)
		{
			i3GameNode * pParent = pLVItem->getParent();
			if( pParent)
			{	
				pParent->RemoveChild( pLVItem);
			}
		}

		m_pBoundItem = pLVItem;
		m_pBoundItem->SetBoundBox( this);

		UpdateAllComponents();

		m_pBoundItem->EnableCtrl( false);
		
		m_pBoundItem->_ForceVisible( false);

		m_BoundHeight = m_pBoundItem->getHeight();
	}
}

void i3UIListView_Box::UnBindItem( void)
{
	if( m_pBoundItem != nullptr)
	{
		_AdjustItemCountTo( 0);		// Item Instance 제거

		VEC3D vOffset, vNewPos;
		i3Vector::Set( &vOffset, 20.0f, 20.0f, 0.0f);
		i3Vector::Add( &vNewPos, &vOffset, getAccumulatedPos());

		m_pBoundItem->SetBoundBox( nullptr);

		m_pBoundItem->setPos( &vNewPos);
		m_pBoundItem->setSize( m_pBoundItem->getSize());

		m_pBoundItem->EnableCtrl( true);
	
		I3_SAFE_RELEASE( m_pBoundItem);
	}
}

void i3UIListView_Box::SetHeaderText( const i3::vector<i3::rc_wstring>& StrList)
{
	if( m_pHeader != nullptr)
		m_pHeader->SetHeaderText( StrList);
}

i3UIListView_Item* i3UIListView_Box::_NewObjectFrom( i3UIListView_Item* pSrcItem)
{
	I3ASSERT( pSrcItem != nullptr);

	i3UITemplate* pTemplate = pSrcItem->getTemplate();
	VEC2D* vSize = pSrcItem->getSize();
	VEC3D* vPos = pSrcItem->getPos();

	i3UIListView_Item* pNewItem = i3UIListView_Item::new_object();

	if(pTemplate != nullptr)
		pNewItem->SetTemplate( pTemplate);
	pNewItem->SetSizeNoNotify( vSize->x, vSize->y);
	pNewItem->setPos( vPos);
	pNewItem->EnableCtrl( true);

	pNewItem->addGNodeStyle( I3_GAMENODE_STYLE_VOLATILE);
	pNewItem->addStyle( I3UI_STYLE_SLAVE);
	
	pNewItem->CopyMembersFrom( pSrcItem, getAncestor());	// Copy Cells and Seps

	return pNewItem;
}

void i3UIListView_Box::_ReleaseObject( i3UIListView_Item* pItem)
{
	I3ASSERT( pItem != nullptr);

	pItem->ChangeCellCount( 0, getAncestor());	
	
	I3_SAFE_RELEASE( pItem);
}

void i3UIListView_Box::OnItemSizeChanged( REAL32 cx, REAL32 cy)
{
	/*
	12029 : 인게임 중 탭키를 누르면 크래시가 발생하는 문제 관련하여 인게임중 리셋디바이스시
		    스프라이트 카운트가 증가하여 예외처리 합니다. 

	static bool bFirst = true;

	if( bFirst)
	{
		bFirst = false;

		if( m_pBoundItem != nullptr)
			m_pBoundItem->SetSizeNoNotify( cx, cy);

		_RecalcItemCount();
		_RepositionItems();

		//Child Rect
		for( size_t i=0; i< m_ItemList.size(); i++)
		{
			i3UIListView_Item* pItem = m_ItemList[i];
			i3::vector<i3UIListView_Cell*>& Cells = pItem->getCellList();
			
			for( size_t j=0; j< Cells.size(); j++)
			{
				i3UIListView_Cell* pCell = Cells[j];
				pCell->setChildRect( 0, 0, (INT32)pCell->getWidth(), (INT32)pCell->getHeight());
			}
		}

		bFirst = true;
	}
	*/
}

void i3UIListView_Box::OnCellTypeChanged( i3UIListView_Item* pFrom)
{
	OnCellSizeChanged( pFrom);
}

void i3UIListView_Box::OnCellSizeChanged( i3UIListView_Item* pFrom)
{
	if( m_pBoundItem != nullptr)
		m_pBoundItem->CopyMembersFrom( pFrom, getAncestor());

	for( size_t i=0; i< m_ItemList.size(); i++)
	{
		i3UIListView_Item* pItem = m_ItemList[i];
		pItem->CopyMembersFrom( pFrom, getAncestor());
	}

	_RepositionItems();
	_ReposHeader();
}

void i3UIListView_Box::OnCellCountChanged( INT32 n)
{
	static bool bFirst = true;

	if( bFirst)
	{
		bFirst = false;

		if( m_pBoundItem != nullptr)
			m_pBoundItem->ChangeCellCount( n, getAncestor());

		for( size_t i=0; i< m_ItemList.size(); i++)
		{
			i3UIListView_Item* pItem = m_ItemList[i];
			pItem->ChangeCellCount( n, getAncestor());
		}

		_ReposHeader();

		bFirst = true;
	}
}

void i3UIListView_Box::OnUpdate( REAL32 rDeltaSeconds)
{
	i3UIControl::OnUpdate( rDeltaSeconds);

	if( m_bApplyChangeData)
	{
		ApplyChanges();
	}
}

void i3UIListView_Box::OnSeperatorChanged( REAL32 w)
{
	INT32 j;

	//이 함수는 Child Item에서만 호출되므로..
	I3ASSERT( m_pBoundItem != nullptr);
	REAL32 width = m_pBoundItem->getSize()->x;
	REAL32 height = m_pBoundItem->getSize()->y;

	if( m_pBoundItem != nullptr)
	{
		m_pBoundItem->setSepWidth( w);
		m_pBoundItem->UpdateCellPosAndSize( width, height, true);
	}

	for(size_t i=0; i< m_ItemList.size(); i++)
	{
		i3UIListView_Item* pItem = m_ItemList[i];
		pItem->setSepWidth( w);

		if( m_pBoundItem != nullptr)
		{
			I3ASSERT( pItem->getCellCount() == m_pBoundItem->getCellCount());
			for( j = 0; j < m_pBoundItem->getCellCount() -1; j++)
				pItem->setSeperatorHideState( j, m_pBoundItem->getSeperatorHideState( j));
		}

		pItem->UpdateCellPosAndSize( width, height, true);
	}

	_ReposHeader();
}

void i3UIListView_Box::OnTextChanged(i3UIListView_Item* pFrom, int nIndex, wchar_t* wstrText, int nParam1, int nParam2)
{
	if (pFrom)
	{
		if (m_pBoundItem != nullptr)
			m_pBoundItem->CopyMembersFrom(pFrom, getAncestor());

		for (size_t i = 0; i < m_ItemList.size(); i++)
		{
			i3UIListView_Item* pItem = m_ItemList[i];
			pItem->CopyMembersFrom(pFrom, getAncestor());
		}
	}

	int nSize = 0;
	if (m_ItemList.size() > 0)
		nSize = 1;

	for (int i = 0; i < nSize; i++)
	{
		i3UIListView_Item* pItem = m_ItemList[i];

		i3UIListView_Cell* pCell = (i3UIListView_Cell*)pItem->getCell(nIndex);
		if (pCell->getType() == ECELLTYPE_TEXT)
		{
			pCell->SetText(wstrText);
			pCell->SetTextAlign((I3UI_TEXT_ALIGN_X)nParam1, (I3UI_TEXT_ALIGN_Y)nParam2);
		}
		else if (pCell->getType() == ECELLTYPE_TEMPLATE)
		{
			pCell->SetText(L"");
		}
	}

}

INT32 i3UIListView_Box::GetCellIndexByPos( INT32 itemIdx, POINT point)
{
	INT32 i;

	if( m_pBoundItem == nullptr)
		return -1;

	if( (INT32)m_ItemList.size() <= itemIdx)
		return -1;

	if( itemIdx < 0 )
		return -1;

	// 상대좌표로 변환
	VEC3D* vPos = getAccumulatedPos();
	REAL32 x = point.x - vPos->x;

	i3UIListView_Item* pItem = m_ItemList[itemIdx];

	if( pItem == nullptr)
		return -1;

	// 검사 
	for( i=0; i< pItem->getCellCount(); i++)
	{
		i3UIListView_Cell* pCell = (i3UIListView_Cell*) pItem->getCell( i);

		VEC3D* vCellPos = pCell->getPos();
		VEC2D* vCellize = pCell->getSize();
		
		if( x > vCellPos->x && x < vCellPos->x + vCellize->x)
		{
			return i;
		}
	}

	return -1;
}

////////////////////////////////////////////////////////////////////////////////////////////
LVITEM_DATA* i3UIListView_Box::NewItem( void)
{
	if( m_pBoundItem != nullptr)
	{
		LVITEM_DATA* pItemData = LVITEM_DATA::new_object();

		INT32 nCellCount = m_pBoundItem->getCellCount();
		pItemData->Init( nCellCount);
		
		return pItemData;
	}

	return nullptr;
}

void i3UIListView_Box::AddItem( LVITEM_DATA* pData)
{
	if( pData != nullptr)
	{
		m_DataList.push_back( pData);
	}

	if( m_bPreviewMode)
		ApplyChanges();
}

void i3UIListView_Box::InsertItem( INT32 nIdx, LVITEM_DATA* pData)
{
	if( pData != nullptr)
	{
		if( nIdx >= 0 && nIdx < (INT32)m_DataList.size())
		{
			m_DataList.insert(m_DataList.begin() + nIdx, pData);
		//	m_DataList.Insert( nIdx, pData);
		}
		else
			m_DataList.push_back( pData);
	}
	
	if( m_bPreviewMode)
		ApplyChanges();
}

void i3UIListView_Box::DeleteItem( INT32 nIdx)
{
	if( nIdx >= 0 && nIdx < (INT32)m_DataList.size())
	{
		LVITEM_DATA* pData = m_DataList[nIdx];
		I3_SAFE_RELEASE( pData);

	//	m_DataList.Delete( nIdx);
		m_DataList.erase(m_DataList.begin() + nIdx);

		if((UINT32)nIdx < m_ItemList.size())
		{
			i3UIListView_Item* pitem = m_ItemList[nIdx];
			i3UIListView_Cell* pcell = pitem->getCellList()[0];
			pcell->SetText("");
		}
	}

	if( m_bPreviewMode)
		ApplyChanges();
}

void	i3UIListView_Box::DeleteAllItems()
{
	const size_t num = m_DataList.size();
	LVITEM_DATA* pData;
	for (size_t i = 0 ; i < num ; ++i )
	{
		pData = m_DataList[i];
		I3_SAFE_RELEASE(pData);
	}
	m_DataList.clear();

	for( INT32 i=0;  i< (INT32)m_ItemList.size(); i++)
	{
		i3UIListView_Item* pitem = m_ItemList[i];
		i3UIListView_Cell* pcell = pitem->getCellList()[0];
		pcell->SetText("");
	}

	if (m_bPreviewMode)
		ApplyChanges();
}

void i3UIListView_Box::SetItemCell( INT32 nItemIdx, INT32 nCellIdx, INT32 pData)
{
	if( nItemIdx >= 0 && nItemIdx < (INT32)m_DataList.size())
	{
		LVITEM_DATA* pItemData = m_DataList[nItemIdx];

		i3::string strData;
		i3::sprintf(strData, "%d", pData);

		//char szCurData[CELLDATA_SIZE] = {0};
		const i3::string* strCurData = pItemData->GetCellData( nCellIdx, 0);

		//if( i3String::NCompare( szCurData, pData, CELLDATA_SIZE) != 0)
		if( strCurData->compare( strData) != 0)
		{
			pItemData->SetCellData( nCellIdx, strData.c_str());

			if( m_bPreviewMode)
			{
				//실제로 Cell의 Data가 바뀌었을 경우에만 ApplyChanges를 호출한다.
				m_bApplyChangeData = true;
			}
		}
	}
}

void i3UIListView_Box::SetItemCell( INT32 nItemIdx, INT32 nCellIdx, const char* pData)
{
	if( nItemIdx >= 0 && nItemIdx < (INT32)m_DataList.size())
	{
		LVITEM_DATA* pItemData = m_DataList[nItemIdx];

		//char szCurData[CELLDATA_SIZE] = {0};
		const i3::string* strCurData = pItemData->GetCellData( nCellIdx, 0);
		
		//if( i3String::NCompare( szCurData, pData, CELLDATA_SIZE) != 0)
		if( strCurData->compare( pData) != 0)
		{
			pItemData->SetCellData( nCellIdx, pData);

			if( m_bPreviewMode)
			{
				//실제로 Cell의 Data가 바뀌었을 경우에만 ApplyChanges를 호출한다.
				m_bApplyChangeData = true;
			}
		}
	}
}

void i3UIListView_Box::SetItemCell( INT32 nItemIdx, INT32 nCellIdx, const wchar_t* pData)
{
	if( nItemIdx >= 0 && nItemIdx < (INT32)m_DataList.size())
	{
		LVITEM_DATA* pItemData = m_DataList[nItemIdx];

		//char szCurData[CELLDATA_SIZE] = {0};
		const i3::string* strCurData = pItemData->GetCellData( nCellIdx, 0);

		i3::string str_utf8;
		i3::utf16_to_utf8(pData, str_utf8);

		//if( i3String::NCompare( szCurData, pData, CELLDATA_SIZE) != 0)
		if( strCurData->compare( str_utf8) != 0)
		{
			pItemData->SetCellData( nCellIdx, str_utf8.c_str());

			if( m_bPreviewMode)
			{
				//실제로 Cell의 Data가 바뀌었을 경우에만 ApplyChanges를 호출한다.
				m_bApplyChangeData = true;
			}
		}
	}
}

void i3UIListView_Box::SetItemCell( INT32 nItemIdx, INT32 nCellIdx, i3::vector<i3::string>& List)
{
	if( nItemIdx >= 0 && nItemIdx < (INT32)m_DataList.size())
	{
		LVITEM_DATA* pItemData = m_DataList[nItemIdx];

		if( pItemData->SetCellData( nCellIdx, List))
		{
			if( m_bPreviewMode)
			{
				//실제로 Cell의 Data가 바뀌었을 경우에만 ApplyChanges를 호출한다.
				m_bApplyChangeData = true;
			}
		}
	}
}

void i3UIListView_Box::SetItemCellTextColor( INT32 nItemIdx, INT32 nCellIndex, COLOR * pCol, bool bRedraw)
{

	if( nItemIdx >= 0 && nItemIdx < (INT32)m_DataList.size())
	{
		LVITEM_DATA* pItemData = m_DataList[nItemIdx];

		if( nCellIndex > -1 && nCellIndex < pItemData->getCellCount())
		{
			pItemData->setCellColor( nCellIndex, pCol);
			if (bRedraw)
				Redraw();
		}
	}
}

void	i3UIListView_Box::RestoreItemCellTextColor(INT32 nItemIdx, INT32 nCellIndex, bool bRedraw)
{
	if( nItemIdx >= 0 && nItemIdx < (INT32)m_DataList.size())
	{
		LVITEM_DATA* pItemData = m_DataList[nItemIdx];

		if( nCellIndex > -1 && nCellIndex < pItemData->getCellCount())
		{
			pItemData->restoreCellColor( nCellIndex);
			if (bRedraw)
				Redraw();
		}
	}
}

LVITEM_DATA* i3UIListView_Box::getData( INT32 nIdx)
{
	if( nIdx >= 0 && nIdx < (INT32)m_DataList.size())
		return m_DataList[nIdx];

	return nullptr;
}

void i3UIListView_Box::getDataFromCell(INT32 nItemIdx, INT32 nCellIdx, i3::wstring& wstrOut)
{
	if( nItemIdx >= 0 && nItemIdx < (INT32)m_DataList.size())
	{

		const LVCELL_DATA* data = m_DataList[nItemIdx]->GetCellData( nCellIdx);
		
		if (data)
		{
			const i3::string& str_utf8 = data->m_DataList[0];
			i3::utf8_to_utf16(str_utf8, wstrOut);
		}
		else
		{
			wstrOut.clear();
		}
	}
}


bool i3UIListView_Box::isSelectedData( INT32 nDataIdx)
{
	if( nDataIdx >= 0 && nDataIdx < (INT32)m_DataList.size())
	{
		LVITEM_DATA* pItemData = m_DataList[nDataIdx];
		return pItemData->isSelected();
	}

	return false;
}

bool i3UIListView_Box::isCheckedData(INT32 nDataIdx, INT32 nCellIdx)
{
	//nDataIdx -> 실제 데이터 리스트의 인덱스
	//nCellIdx -> 몇번째 셀인지...일단은 소스상에서 수동적으로 입력
	if( nDataIdx >= 0 && nDataIdx < (INT32)m_DataList.size())
	{
		char conv[CELLDATA_SIZE+1] = "";
		LVITEM_DATA* pItemData = m_DataList[nDataIdx];
		pItemData->GetCellData(nCellIdx,conv, CELLDATA_SIZE);
		INT32 n_Check = i3::atoi(conv);
		if(n_Check == 1)
			return true;
		else
			return false;
	}
	return false;
}

void i3UIListView_Box::UnSelectAllData( void)
{
	for( size_t i=0; i< m_DataList.size(); i++)
	{
		LVITEM_DATA* pItemData = m_DataList[i];
		pItemData->SetSelected( false);
	}

	for( size_t i=0; i< m_ItemList.size(); i++)
	{
		i3UIListView_Item* pItem = m_ItemList[i];
		pItem->setModifyState();
	}
}

void i3UIListView_Box::SelectData( INT32 nDataIdx, bool bSelect)
{
	if( m_bSingleSelect && bSelect)
		UnSelectAllData();

	if( nDataIdx > -1 && nDataIdx < (INT32)m_DataList.size())
	{
		LVITEM_DATA* pItemData = m_DataList[nDataIdx];
		pItemData->SetSelected( bSelect);
	}
}

void i3UIListView_Box::ScrollToIndex( INT32 nDataIdx)
{
	if( m_pScrollBar != nullptr && m_pScrollBar->isEnable())
	{
		REAL32 fItemHeight = m_pBoundItem->getHeight();
		REAL32 fItemCur = m_vItemGap.y;

		INT32 nPos = (INT32)((fItemHeight + fItemCur) * nDataIdx);
		
		m_pScrollBar->setScrollPos( nPos);
		//_OnScroll();
	}
}

bool i3UIListView_Box::isItemSelected( i3UIListView_Item* pItem)
{
	I3ASSERT( pItem != nullptr);

	for( size_t i=0; i< m_ItemList.size(); i++)
	{
		i3UIListView_Item* pCurItem = m_ItemList[i];

		if( pCurItem == pItem)
		{
			return isSelectedData( i + m_nFirstVisibleDataIndex);
		}
	}

	return false;
}

void i3UIListView_Box::SortByCellIndex( INT32 nBtnIdx, bool bAscending)
{
	// 직접 정렬... ㅜㅜ
	if (!m_DataList.empty())
	{

		for( INT32 i=0; i< (INT32)m_DataList.size()-1; i++)
		{
			LVITEM_DATA* p1 = m_DataList[i];
			char d1[MAX_PATH +1] = "";
			p1->GetCellData( nBtnIdx, d1, MAX_PATH);
			
			for( INT32 j=i+1; j< (INT32)m_DataList.size(); j++)
			{
				LVITEM_DATA* p2 = m_DataList[j];
				char d2[MAX_PATH +1] = "";
				p2->GetCellData( nBtnIdx, d2, MAX_PATH);
				
				bool bSwap = false;

				INT32 nComp = i3::generic_icompare( d1, d2);

				// d1 < d2 이면 negative
				if( nComp < 0)
				{
					bSwap = !bAscending;
				}
				else if( nComp > 0)
				{
					bSwap = bAscending;
				}
				
				if( bSwap)
				{
					i3::vu::set_value_force(m_DataList, i, p2);
					i3::vu::set_value_force(m_DataList, j, p1);
				//	m_DataList.SetItem( i, p2);
				//	m_DataList.SetItem( j, p1);
					
					p1 = p2;
					p2->GetCellData( nBtnIdx, d1, MAX_PATH);
				}
			}
		}
	}

	_OnScroll();
	//_RepositionData();
}

i3UIControl* i3UIListView_Box::getCellChild( INT32 nRow, INT32 nCol)
{
	if( nRow >= 0 && nRow < (INT32)m_ItemList.size())
	{
		i3UIListView_Item* pItem = m_ItemList[nRow];
		
		i3UIListView_Cell* pCell = pItem->getCell( nCol);
		
		if( pCell != nullptr)
		{
//			return pCell->getCellChild();					// 텍스트만 있는 경우엔 셀 그자체가 캡션 콘트롤로 작용되므로, 우선 널값인 경우 pCell을 리턴하게 변경..
			i3UIControl* pResult = pCell->getCellChild();	// (11.07.06.수빈)
			return (pResult) ? pResult : pCell;
		}
	}

	return nullptr;
}

i3UIControl*	 i3UIListView_Box::getHeaderButton( INT32 nCol)
{
	if (!m_pHeader) return nullptr;

	i3::vector<i3UIButton*>& button_list = m_pHeader->getButtonList();

	const INT32 num_button = (INT32)button_list.size();

	if ( nCol >= 0 && nCol < num_button )
	{
		return button_list[nCol];
	}

	return nullptr;
}


////////////////////////////////////////////////////////////////////////////////////////////
bool i3UIListView_Box::CreateBaseSceneObjects()
{
	// sprite 할당은 UpdatePos에서 처리한다.
	// 여기서는 Item Count를 알 수 없으므로 렌더링 순서를 조절할 수 없음.
	return i3UIControl::CreateBaseSceneObjects();
}

void i3UIListView_Box::OnBindTemplate( void)
{
	i3UIControl::OnBindTemplate();

	if( m_pTemplate == nullptr)
		return;

	if( m_pScrollBar == nullptr)
	{
		m_pScrollBar = i3UIScrollBar::new_object();
		AddControl( m_pScrollBar);
		m_pScrollBar->addGNodeStyle( I3_GAMENODE_STYLE_VOLATILE);
		m_pScrollBar->addStyle( I3UI_STYLE_SLAVE);
	}

	m_pScrollBar->SetTemplate( m_pTemplate->getSubTemplate( I3UI_LVBOX_SUBTEMPLATE_VSCROLL));
}

INT32 i3UIListView_Box::GetItemIndexByPos( POINT point)
{
	if( m_pBoundItem == nullptr)
		return -1;

	// 상대좌표로 변환
	VEC3D* vPos = getAccumulatedPos();
	REAL32 x = point.x - vPos->x;
	REAL32 y = point.y - vPos->y;

	// 검사 
	for( size_t i=0; i< m_ItemList.size(); i++)
	{
		i3UIListView_Item* pItem = m_ItemList[i];

		VEC3D* vItemPos = pItem->getPos();
		VEC2D* vItemSize = pItem->getSize();
		
		if( x > vItemPos->x && x < vItemPos->x + vItemSize->x)
		{
			if( y > vItemPos->y && y < vItemPos->y + vItemSize->y)
			{
				return i;
			}
		}
	}

	return -1;
}

INT32 i3UIListView_Box::GetDataIndexByPos( POINT point)
{
	INT32 nItemIndex = GetItemIndexByPos( point);

	if( nItemIndex != -1)
		return nItemIndex + m_nFirstVisibleDataIndex;

	return -1;
}


void i3UIListView_Box::setCurrentOverCell( i3UIListView_Cell* pCell )
{
	if( m_pCurOverCell != nullptr) {
		if( m_pCurOverCell != pCell) {
			I3_SAFE_RELEASE(m_pCurOverCell);
		}
		else {
			return;
		}
	}

	m_pCurOverCell = pCell;
	I3_SAFE_ADDREF( pCell);
}


void i3UIListView_Box::getItemBox( i3Rect* pRt)
{
	REAL32 l = m_AccPos.x + m_rtMargin.left;
	REAL32 t = m_AccPos.y + m_rtMargin.top;
	REAL32 w = m_Size.x - m_rtMargin.left - m_rtMargin.right;
	REAL32 h = m_Size.y - m_rtMargin.top - m_rtMargin.bottom;

	if( m_pHeader != nullptr)
	{
		t += m_pHeader->getHeight();
		h -= m_pHeader->getHeight();
	}

	if( m_pScrollBar != nullptr && m_pScrollBar->isEnable())
		w -= (m_fWidthScrollBar + m_fScrollBarMargin);

	pRt->set( (INT32)l+1, (INT32)t+1, (INT32)(l+w-1), (INT32)(t+h-1));
}

void i3UIListView_Box::getFullRect( i3UIListView_Cell* pCellToFind, i3Rect* pRt)
{
	for( size_t i=0; i< m_ItemList.size(); i++)
	{
		i3UIListView_Item* pItem = m_ItemList[i];
		i3::vector<i3UIListView_Cell*>& CellList = pItem->getCellList();

		for( size_t j=0; j< CellList.size(); j++)
		{
			i3UIListView_Cell* pCell = CellList[j];

			if( pCell == pCellToFind)
			{
				REAL32 l = pCell->getAccumulatedPos()->x;
				REAL32 t = pCell->getAccumulatedPos()->y;
				REAL32 w = pCell->getWidth();
				REAL32 h = pCell->getHeight();

				i3UIListView_Cell* pBoundCell = m_pBoundItem->getCell( j);

				if( pBoundCell->getHeight() > h)	// 잘렸는지 검사
				{
					i3Rect rtBox;
					getItemBox( &rtBox);

					if( t <= rtBox.top)
					{
						t = t + h - pBoundCell->getHeight();
						h = pBoundCell->getHeight();
					}
					else
					{
						h = pBoundCell->getHeight();
					}
				}

				pRt->set( (INT32)l, (INT32)t, (INT32)(l+w), (INT32)(t+h));

				return;
			}
		}
	}
}

INT32 i3UIListView_Box::getCellCount( void)
{
	if( m_pBoundItem != nullptr)
	{
		return m_pBoundItem->getCellCount();
	}

	return 0;
}

INT32 i3UIListView_Box::getCellCount( INT32 nIdx)
{
	if( nIdx >= 0 && nIdx < (INT32)m_DataList.size())
	{
		return m_DataList[nIdx]->getCellCount();

	}
	
	return 0;
}

void i3UIListView_Box::OnNotify( i3UIControl * pCtrl, I3UI_EVT event, INT32 param1, INT32 param2)
{
	switch( event)
	{
	case I3UI_EVT_SCROLL:
		{
			_OnScroll();			// 내부의 repositiondata()함수 호출에 의해 m_nFirstVisibleDataIndex가 변하며, 이것을 루아쪽으로 반영할수 있어야함...
			Notify( I3UI_EVT_SCROLL, m_nFirstVisibleDataIndex, 0);
		}
		break;

		// 현재 I3UI_EVT_CLICKED, I3UI_EVT_DBL_CLICKED의 좌표체크는 동작하지 않습니다.
		// 이유는 param1,param2에 마우스좌표를 요구하지만, 실제로 처리된 OnClick()은 UIControl함수부이고,
		// 그곳에서는 단순버튼처럼 인식하여 마우스좌표 대신 0,0이 들어가게 됩니다. 11.06.03.수빈..
	case I3UI_EVT_CLICKED:
		{
			if( i3::same_of<i3UIListView_Item*>(pCtrl))
			{
				POINT point;
				point.x = param1;
				point.y = param2;

				INT32 nDataIndex = -1;
				//INT32 nDataIndex = GetDataIndexByPos( point);
				INT32 nItemIndex = GetItemIndexByPos( point);

				if( nItemIndex != -1)
					nDataIndex = nItemIndex + m_nFirstVisibleDataIndex;

				INT32 nCellIndex = GetCellIndexByPos( nItemIndex, point);

				I3TRACE("i3UIListView_Box::OnNotify(Clicked) %d\n", nDataIndex);

				if( nDataIndex != -1)
				{
					Notify( I3UI_EVT_CLICKED, nDataIndex, nCellIndex);

					// 아이템콘트롤 클릭의 경우 여기가 아니라..
					// SELITEM이벤트가 쓰이고, 그외 CellChild는 여기 들어옵니다.
					// 셀렉션 처리가 여기없는데, 이것을 넣어서 일단 셀렉션이 되도록 추가..
					// Notify도 SELITEM에 맞춰서 처리합니다..(11.10.26.수빈)
					if( m_bNoSelection == false)
					{
						bool bSel = true;
						if( !m_bSingleSelect)
						{
							if( isSelectedData( nDataIndex))
								bSel = false;
						}
						SelectData( nDataIndex, bSel);
						pCtrl->setModifyState(true);
					}

					//Notify( I3UI_EVT_SELITEM, nDataIndex, 0);		//row

				}
			}
		}
		break;
	case I3UI_EVT_DBL_CLICKED:
		{
			if( i3::same_of<i3UIListView_Item*>(pCtrl) )
			{
				POINT point;
				point.x = param1;
				point.y = param2;

				INT32 nDataIndex = GetDataIndexByPos( point);

				I3TRACE("i3UIListView_Box::OnNotify(DblClicked) %d\n", nDataIndex);

				if( nDataIndex != -1)
				{
					Notify( I3UI_EVT_DBL_CLICKED, nDataIndex, 0);
				}
			}
		}
		break;
	case I3UI_EVT_SELITEM:
		{
			POINT point;
			point.x = param1;
			point.y = param2;
			
			INT32 nDataIndex = GetDataIndexByPos( point);
			I3TRACE("i3UIListView_Box::OnNotify(I3UI_EVT_SELITEM) --- %d\n", nDataIndex);
			I3TRACE("POINT( %d, %d)\n", param1, param2);
			
			if( nDataIndex != -1)
			{
				if( m_bNoSelection == false)
				{
					bool bSel = true;
					if( !m_bSingleSelect)
					{
						if( isSelectedData( nDataIndex))
							bSel = false;
					}
					SelectData( nDataIndex, bSel);
					pCtrl->setModifyState(true);
				}

				Notify( event, nDataIndex, 0);		//row
			}
		}
		break;
	case I3UI_EVT_BTN_PUSHED:
		{
			INT32 dataIdx = -1;
			INT32 itemIdx = -1;

			for( size_t i=0; i< m_ItemList.size(); i++)
			{
				i3UIControl* pItem = m_ItemList[i];
				if( pItem == pCtrl)
				{
					itemIdx = i;
					break;
				}
			}

			dataIdx = itemIdx + m_nFirstVisibleDataIndex;
			setItemIndex(itemIdx);
			setDataIndex(dataIdx);

			I3ASSERT( dataIdx > -1);

			Notify( event, dataIdx, param2);		// row, col
		}
		break;
	case I3UI_EVT_MOUSEWHEEL:
		{
			if( m_pScrollBar != nullptr)
			{
				if( m_pScrollBar->isEnable() )
				{
					INT32 nTickSize = m_pScrollBar->getTickSize();
					INT32 nCurPos = m_pScrollBar->getScrollPos();
					INT32 sign = param1 >= 0 ? -1 : 1;

					//INT32 nNewPos = nCurPos + sign * nTickSize * 3;
					INT32 nNewPos = nCurPos + sign * nTickSize;

					m_pScrollBar->setScrollPos( nNewPos);
				}
				else
					Notify(event, param1, param2);
			}
		}
		break;
	case I3UI_EVT_SIZE:
		{
			if( pCtrl != nullptr && pCtrl == m_pHeader )
			{
				m_pHeader->UpdateShape( m_pBoundItem, (REAL32)param2);
	
				_ReposBoundItem();

				_RecalcItemCount();
				_RepositionItems();
			}
		}
		break;

	case I3UI_EVT_ENTERMOUSE:
		{
			//m_bIsMouseOver = true;
		}
		break;

	case I3UI_EVT_LEAVEMOUSE:
		{
			//m_bIsMouseOver = false;
		}
		break;

	case I3UI_EVT_R_CLICKED :
		{
			POINT point;
			point.x = param1;
			point.y = param2;
			
			INT32 nDataIndex = GetDataIndexByPos( point);
			I3TRACE("i3UIListView_Box::OnNotify(I3UI_EVT_R_CLICKED) --- %d\n", nDataIndex);
			I3TRACE("POINT( %d, %d)\n", param1, param2);
			
			if( nDataIndex != -1)
			{
				// 우측 클릭은 Highlight를 하지 않는다.
				/*bool bSel = true;

				if( !m_bSingleSelect)
				{
					if( isSelectedData( nDataIndex))
						bSel = false;
				}*/

				//SelectData( nDataIndex, bSel);
				//pCtrl->setModifyState(true);

				Notify( event, nDataIndex, 0);		//row
			}
		}
		break;
	case I3UI_EVT_LVH_PUSHED:
		{
			Notify( event, param1, param2);
		}
		break;

	case I3UI_EVT_KILLFOCUS :		Notify( event, param1, param2);		break;

	case I3UI_EVT_COMBOBOX_SELITEM:
		{
			INT32 dataIdx = -1;
			INT32 itemIdx = -1;

			for( size_t i=0; i< m_ItemList.size(); i++)
			{
				i3UIControl* pItem = m_ItemList[i];
				if( pItem == pCtrl)
				{
					itemIdx = i;
					break;
				}
			}

			dataIdx = itemIdx + m_nFirstVisibleDataIndex;
			setItemIndex(itemIdx);
			setDataIndex(dataIdx);

			I3ASSERT( dataIdx > -1);

			Notify( event, dataIdx, param2);	// LV Item idx, Combo Item Idx
		}
		break;
	default:
		break;
	}

	i3UIControl::OnNotify( pCtrl, event, param1, param2);
}

void i3UIListView_Box::UpdateRenderObjects()
{
	if( i3UI::getToolDevMode() == true)
		_RepositionItems();

	i3UIControl::UpdateRenderObjects();
}

void i3UIListView_Box::prepareSprites(void)
{
	//UINT32 i;
	// Box의 Sprite Render
	i3UIControl::prepareSprites();

	//// Header render
	//if( m_pHeader != nullptr)
	//	m_pHeader->prepareSprites();

	//// Item은 세로순서로 draw해야한다. 
	//{
	//	//UINT32 cellCnt = 0;
	//	// Item render
	//	for( i = 0; i < m_ItemList.size(); i++)
	//	{
	//		m_ItemList.at( i)->prepareSprites();

	//		/*if( cellCnt < m_ItemList.at( i)->getCellList().size())
	//			cellCnt = m_ItemList.at( i)->getCellList().size();*/

	//		// Seperate render
	//		/*for( j = 0; j < m_ItemList.at( i)->getSepList().size(); j++)
	//		{
	//			m_ItemList.at( i)->getSep( j)->prepareSprites();
	//		}*/
	//	}

	//	// cell render
	//	//for( j = 0; j < cellCnt; j++)
	//	//{
	//	//	for( i = 0; i < m_ItemList.size(); i++)
	//	//	{
	//	//		m_ItemList.at( i)->getCell( j)->prepareSprites();
	//	//	}
	//	//}

	//	//// cell child
	//	//for( j = 0; j < cellCnt; j++)
	//	//{
	//	//	for( i = 0; i < m_ItemList.size(); i++)
	//	//	{
	//	//		i3UIControl * pChild = m_ItemList.at( i)->getCell( j)->getCellChild();
	//	//		if( pChild != nullptr)
	//	//		{
	//	//			pChild->prepareSprites();
	//	//		}
	//	//	}
	//	//}
	//}

	//if( m_pBoundItem != nullptr)
	//	m_pBoundItem->prepareSprites();

	if (i3UI::getToolDevMode() == true)
	{
		if (m_pScrollBar != nullptr)
		{
			m_pScrollBar->prepareSprites();
			i3UIControl * pChild = (i3UIControl *)m_pScrollBar->getFirstChild();
			while (pChild)
			{
				pChild->prepareSprites();

				pChild = (i3UIControl *)pChild->getNext();
			}
		}
	}
}

bool i3UIListView_Box::OnRevive( i3RenderContext * pCtx)
{
	if( i3UIControl::OnRevive( pCtx) == false)
		return false;

	_RepositionData();

	//골때리네... RepositionData에서 Text내용을 지운다...
	//기존에 가지고 있는 내용을 다시 업데이트하게 TOOL에만 추가
	if (i3UI::getToolDevMode() == true)
	{
		for (size_t i = 0; i < m_ItemList.size(); i++)
		{
			m_ItemList[i]->UpdateCell();
		}
	}

	return true;
}

void i3UIListView_Box::OnDraw( i3UIRenderer * pRenderer)
{
	INT32 hSprite;
	if( this->isEnable() == false)
		return;

	// Box의 Sprite Render
 	if( isAllocedSprite() )
 	{
 		for( INT32 i = 0; i < getShapeCount(); i++)
 		{
 			hSprite = getSpriteHandle( i);
 			if( hSprite != -1)
 				pRenderer->Draw_Kick( hSprite);
 		}
 	}
 
 	// Header render
 	if( m_pHeader != nullptr)
 		m_pHeader->OnDraw( pRenderer);
 
 	// Item은 세로순서로 draw하자
 	{
 		i3UIListView_Item * pItem;
 		UINT32 itemCount = m_ItemList.size();
 
 		if( itemCount > 0)
 		{
 			pItem = m_ItemList.at( 0);
 			INT32 cellCount = pItem->getCellCount();
 
 			// Item render
 			for( UINT32 i = 0; i < m_ItemList.size(); i++)
 			{
 				pItem = m_ItemList.at( i);
 
 				if( pItem->isEnable())
 				{
 					if( pItem->isAllocedSprite() )
 					{
 						for( INT32 j = 0; j < pItem->getShapeCount(); j++)
 						{
 							hSprite = pItem->getSpriteHandle( j);
 
 							if( hSprite != -1)
 								pRenderer->Draw_Kick( hSprite);
 						}
 					}
 				}
 
 				// Seperate render
 				for (UINT32 j = 0; j < pItem->getSepList().size(); j++)
 				{
 					i3UIListView_Sep * pSep = pItem->getSep(j);
 					
 					if (pSep->isEnable())
 					{
 						pSep->OnDraw(pRenderer);
 					}
 				}
 			}
 
			if (m_pBoundItem != nullptr)
				m_pBoundItem->OnDraw(pRenderer);
 
			m_pImageBoxList.clear();

 			// cell render
 			for( INT32 i = 0; i < cellCount; i++)
 			{
 				for( UINT32 j = 0; j < itemCount; j++)
 				{
 					pItem = m_ItemList.at( j);
 
 					if( pItem->isEnable())
 					{
 						i3UIListView_Cell * pCell = pItem->getCell( i);
 
						if (pCell != nullptr && pCell->isEnable())
						{
							//ImageBox부류 Control은 랜더링 순서를 늦춘다
							if (i3::kind_of<i3UIImageBox*>(pCell->getCellChild()))
							{
								m_pImageBoxList.push_back(pCell);
							}
							else
							{
								pCell->OnDraw(pRenderer);
							}
						}
 					}
 				}
 			}

			for (INT32 i = 0; i < (INT32)m_pImageBoxList.size(); i++)
			{
				m_pImageBoxList[i]->OnDraw(pRenderer);
			}
 		}
 	}
	
	if (m_pScrollBar != nullptr)
		m_pScrollBar->OnDraw(pRenderer);
}

void i3UIListView_Box::SetDummyLine(bool bState)
{ 
	m_bDefaultTemplateState = bState; 

	if (m_bDefaultTemplateState)
	{
		for (UINT32 i = 0; i < m_ItemList.size(); i++)
		{
			i3UIListView_Item * pItem = m_ItemList.at(i);
		
			//childitem
			for (UINT32 j = 0; j < pItem->getSepList().size(); j++)
			{
				i3UIListView_Sep * pSep = pItem->getSep(j);
				if (m_pTSepDefault == nullptr)
				{
					m_pTSepDefault = i3UITemplate_LVSep::new_object();
				}

				INT32 offX = 0, offY = 0;
				i3Texture* pEmptyImg = i3Framework::getCurrentFramework()->GetResourceManager()->LoadTexture("res/SelectImage.i3i");

				i3UI::getVirtualTexture()->GetFileFormat()->PlaceTexture(pEmptyImg, &offX, &offY);

				for (int i = 0; i < m_pTSepDefault->getShapeCount(); ++i)
				{
					i3UIShape * pShape = m_pTSepDefault->getShape(i);
					if (pShape->hasTexture() == false)
					{
						i3::pack::RECT rt;
						rt.left = rt.top = 0.00001f;
						rt.right = rt.bottom = 127.0f;

						pShape->setTextureResPath(pEmptyImg->getResourceLoadPath());
						pShape->SetTextureCoord(&rt);
						pShape->setTexcoordOffset(offX, offY);
					}
				}
				I3_SAFE_RELEASE(pEmptyImg);

				pSep->SetTemplate(m_pTSepDefault);
			}
		}

		//m_pBoundItem 더미텍스쳐 적용
		for (UINT32 j = 0; j < m_pBoundItem->getSepList().size(); j++)
		{
			i3UIListView_Sep * pSep = m_pBoundItem->getSep(j);
			pSep->SetTemplate(m_pTSepDefault);
		}
	}
	else
	{
		i3UIListView_Item * pItem = nullptr;
		for (UINT32 i = 0; i < m_ItemList.size(); i++)
		{
			pItem = m_ItemList.at(i);

			//childitem
			for (UINT32 j = 0; j < pItem->getSepList().size(); j++)
			{
				i3UIListView_Sep * pSep = pItem->getSep(j);
				pSep->SetTemplate(pItem->m_pTemplate_Sep);
			}
		}

		//m_pBoundItem 오리지널텍스쳐 적용
		if (m_pBoundItem)
		{
			for (UINT32 j = 0; j < m_pBoundItem->getSepList().size(); j++)
			{
				i3UIListView_Sep * pSep = m_pBoundItem->getSep(j);
				pSep->SetTemplate(m_pBoundItem->m_pTemplate_Sep);
			}
		}
	}
}

void i3UIListView_Box::OnEnabled( bool bEnable)
{
	i3UIControl::OnEnabled( bEnable);

	if( m_pScrollBar != nullptr && (i3UI::getToolDevMode() == false || m_bPreviewMode))
		m_pScrollBar->EnableCtrl( bEnable);

	for( size_t i=0; i< m_ItemList.size(); i++)
	{
		i3UIListView_Item* pItem = m_ItemList[i];
		pItem->EnableCtrl( bEnable);
	}

	if( m_pHeader != nullptr)
	{
		m_pHeader->EnableCtrl( bEnable);
	}
}

void i3UIListView_Box::OnSize( REAL32 cx, REAL32 cy)
{
	i3UIControl::OnSize( cx, cy);

	m_Size.x = cx;
	m_Size.y = cy;

	UpdateAllComponents();

	if (i3UI::getToolDevMode() == true)
	{
		_PlaceScrollBar();
		SetDummyLine(m_bDefaultTemplateState);
	}
}

void i3UIListView_Box::OnMouseWheel( UINT32 nFlag, short zDelta, POINT point)
{
	i3UIControl::OnMouseWheel( nFlag, zDelta, point);

	if( m_pScrollBar != nullptr && m_pScrollBar->isEnable())
	{
		INT32 nTickSize = m_pScrollBar->getTickSize();
		INT32 nCurPos = m_pScrollBar->getScrollPos();
		INT32 sign = zDelta >= 0 ? -1 : 1;

		INT32 nNewPos = nCurPos + sign * nTickSize * 3;

		m_pScrollBar->setScrollPos( nNewPos);
	}
}

void i3UIListView_Box::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	if( m_pBoundItem != nullptr)
		m_pBoundItem->OnBuildObjectList(List);

	if( m_pHeader != nullptr)
		m_pHeader->OnBuildObjectList(List);

	i3UIControl::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack(push, 4)
#endif

namespace pack
{
	struct UILISTVIEW_BOX
	{
		INT8		m_ID[4] = { 'U', 'L', 'V', 'B' };
		COLOR		m_TextColor_MouseOver;
		COLOR		m_TextColor_Selected;
		UINT32		pad[6] = { 0 };
	};

	struct UILISTVIEW_BOX_FLAG
	{
		INT8	m_bClipItem = 0;
		INT8	m_bSingleSelect = 0;
		INT8	m_bFillWithEmptyItems = 0;
		INT8	m_bDisableEmptyItems = 0;
		INT8	m_bNoSelection = 0;
		INT32	m_nColumnCount = 0;
		UINT32	m_pad[4] = { 0 };
	};

	struct UILISTVIEW_BOX_SPACING
	{
		i3::pack::RECT		m_rtMargin;
		i3::pack::VEC2D m_vItemGap;
		REAL32		m_fWidthScrollBar = 0.0f;
		REAL32		m_fScrollBarMargin = 0.0f;
		UINT32		m_pad[4] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack(pop)
#endif

UINT32 i3UIListView_Box::OnSave( i3ResourceFile * pResFile)
{
	UINT32	Rc, Result= 0;
	pack::UILISTVIEW_BOX data;
	pack::UILISTVIEW_BOX_FLAG flag;
	pack::UILISTVIEW_BOX_SPACING	spacing;

	Result = i3UIControl::OnSave( pResFile );
	I3_CHKIO( Result);
		
	// m_colDefault가 COLOR를 그대로 저장하는 듯 하여 여기서도 마찬가지로 그대로 저장..
	data.m_TextColor_MouseOver = m_colText_MouseOver;
	data.m_TextColor_Selected  = m_colText_Selected;
	//

	Rc = pResFile->Write( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	//default font
	Rc = pResFile->Write( &m_colDefault, sizeof(m_colDefault));
	I3_CHKIO( Rc);
	Result += Rc;

	Rc = pResFile->Write( &m_lfDefault, sizeof(m_lfDefault));
	I3_CHKIO( Rc);
	Result += Rc;

	flag.m_bClipItem = (INT8)m_bClipItem;
	flag.m_bSingleSelect = (INT8)m_bSingleSelect;
	flag.m_bFillWithEmptyItems = (INT8)m_bFillWithEmptyItems;
	flag.m_bDisableEmptyItems = (INT8)m_bDisableEmptyItems;
	flag.m_bNoSelection = (INT8)m_bNoSelection;
	flag.m_nColumnCount = m_nColumnCount;
	flag.m_pad[0] = (UINT32)m_nScrollStyle;

	Rc = pResFile->Write( &flag, sizeof(flag));
	I3_CHKIO( Rc);
	Result += Rc;

	
	memcpy( &spacing.m_rtMargin, &m_rtMargin, sizeof(m_rtMargin));

	spacing.m_vItemGap.x = m_vItemGap.x;
	spacing.m_vItemGap.y = m_vItemGap.y;
	spacing.m_fWidthScrollBar = m_fWidthScrollBar;
	spacing.m_fScrollBarMargin = m_fScrollBarMargin;

	Rc = pResFile->Write( &spacing, sizeof( spacing));
	I3_CHKIO( Rc);
	Result += Rc;
	
	// Don't save clones
	OBJREF ref = pResFile->GetObjectPersistID( m_pHeader);
	Rc = pResFile->Write( &ref, sizeof(ref));
	I3_CHKIO( Rc);
	Result += Rc;

	ref = pResFile->GetObjectPersistID( m_pLoadedItem);
	Rc = pResFile->Write( &ref, sizeof(ref));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32 i3UIListView_Box::OnLoad( i3ResourceFile * pResFile)
{
i3_prof_func();

	UINT32	Rc, Result =0;
	i3Stream * pStream = pResFile->GetStream();
	pack::UILISTVIEW_BOX data;
	pack::UILISTVIEW_BOX_FLAG flag;
	pack::UILISTVIEW_BOX_SPACING	spacing;

	Result = i3UIControl::OnLoad( pResFile );
	I3_CHKIO( Result);

	Rc = pStream->Read( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	//default font
	Rc = pStream->Read( &m_colDefault, sizeof(m_colDefault));
	I3_CHKIO( Rc);
	Result += Rc;

	Rc = pStream->Read( &m_lfDefault, sizeof(m_lfDefault));
	I3_CHKIO( Rc);
	Result += Rc;

	Rc = pStream->Read( &flag, sizeof(flag));
	I3_CHKIO( Rc);
	Result += Rc;

	if (reinterpret_cast<UINT32&>(data.m_TextColor_MouseOver) != 0)
	{
		m_colText_MouseOver = data.m_TextColor_MouseOver;
		m_colText_Selected  = data.m_TextColor_Selected;
	}
	else
	{
		m_colText_MouseOver = m_colDefault;
		m_colText_Selected  = m_colDefault;
	}

	m_bClipItem = flag.m_bClipItem?true:false;
	m_bSingleSelect = flag.m_bSingleSelect?true:false;
	m_bFillWithEmptyItems = flag.m_bFillWithEmptyItems?true:false;
	m_bDisableEmptyItems = flag.m_bDisableEmptyItems?true:false;
	m_bNoSelection = flag.m_bNoSelection?true:false;
	m_nColumnCount = flag.m_nColumnCount;
	m_nScrollStyle = (INT32)flag.m_pad[0];

	Rc = pResFile->Read( &spacing, sizeof( spacing));
	I3_CHKIO( Rc);
	Result += Rc;
	
	memcpy( &m_rtMargin, &spacing.m_rtMargin, sizeof(m_rtMargin));

	m_vItemGap.x = spacing.m_vItemGap.x;
	m_vItemGap.y = spacing.m_vItemGap.y;
	m_fWidthScrollBar = spacing.m_fWidthScrollBar;
	m_fScrollBarMargin = spacing.m_fScrollBarMargin;
	
	OBJREF ref;
	
	Rc = pStream->Read( &ref, sizeof(ref));
	I3_CHKIO(Rc);
	Result += Rc;

	if( ref != 0)
	{
		m_pHeader = (i3UIListView_Header*) pResFile->FindObjectByID( ref);
	}

	Rc = pStream->Read( &ref, sizeof(ref));
	I3_CHKIO( Rc);
	Result += Rc;

	if( ref != 0)
	{
		m_pLoadedItem = (i3UIListView_Item*) pResFile->FindObjectByID( ref);
	}
	
	return Result;
}

bool i3UIListView_Box::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = i3UIControl::OnSaveXML( pFile, pXML);

	if( bResult == false)
		return false;

	//default font
	i3Color::SetToXML( pXML, "TextColor", &m_colDefault);
	i3Color::SetToXML( pXML, "TextColor_MouseOver", &m_colText_MouseOver);
	i3Color::SetToXML( pXML, "TextColor_Selected", &m_colText_Selected);

	pXML->addAttr( "FontName",		m_lfDefault.lfFaceName);
	pXML->addAttr( "FontSize",		_HeightToPoint( m_lfDefault.lfHeight));
	pXML->addAttr( "FontWeight",	m_lfDefault.lfWeight);
	pXML->addAttr( "Italic",		m_lfDefault.lfItalic);
	pXML->addAttr( "Underline",		m_lfDefault.lfUnderline);
	pXML->addAttr( "StrikeOut",		m_lfDefault.lfStrikeOut);
	//default font

	pXML->addAttr( "ClipItem", (INT32)m_bClipItem);
	pXML->addAttr( "SingleSelect", (INT32)m_bSingleSelect);
	pXML->addAttr( "FillWithEmptyItems", (INT32)m_bFillWithEmptyItems);
	pXML->addAttr( "DisableEmptyItems", (INT32)m_bDisableEmptyItems);
	pXML->addAttr( "NoSelection", (INT32)m_bNoSelection);
	pXML->addAttr( "ColumnCount", m_nColumnCount);

	pXML->addAttr( "MarginLeft", m_rtMargin.left);
	pXML->addAttr( "MarginTop", m_rtMargin.top);
	pXML->addAttr( "MarginRight", m_rtMargin.right);
	pXML->addAttr( "MarginBottom", m_rtMargin.bottom);
	pXML->addAttr( "ItemGapX", m_vItemGap.x);
	pXML->addAttr( "ItemGapY", m_vItemGap.y);
	pXML->addAttr( "ScrollBarWidth", m_fWidthScrollBar);
	pXML->addAttr( "ScrollBarMargin", m_fScrollBarMargin);
	pXML->addAttr( "ScrollBarStyle", m_nScrollStyle);
	
	// Don't save clones
	pXML->addAttr( "HeaderItem", pFile->GetObjectPersistID( m_pHeader));
	pXML->addAttr( "BindedItem", pFile->GetObjectPersistID( m_pBoundItem));

	return bResult;
}

bool i3UIListView_Box::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = i3UIControl::OnLoadXML( pFile, pXML);

	if( bResult == false)
		return false;

	//default font
	i3Color::GetFromXML( pXML, "TextColor", &m_colDefault);
	if( m_colDefault.a == 0)
		i3Color::Set( &m_colDefault, (UINT8)255,(UINT8)255,(UINT8)255,(UINT8)255);


	if (pXML->FirstChild("TextColor_MouseOver"))
	{
		i3Color::GetFromXML(pXML, "TextColor_MouseOver", &m_colText_MouseOver);
		i3Color::GetFromXML(pXML, "TextColor_Selected", &m_colText_Selected);		
	}
	else
	{
		m_colText_MouseOver = m_colDefault;
		m_colText_Selected = m_colText_MouseOver;
	}

	const char* pszText = pXML->getAttr( "FontName");
	if( pszText != nullptr)
		strncpy( m_lfDefault.lfFaceName, pszText, sizeof( m_lfDefault.lfFaceName) - 1);
	
	INT32 nSizeInPoint = 0;
	pXML->getAttr( "FontSize",	&nSizeInPoint);
	m_lfDefault.lfHeight = _PointToHeight( nSizeInPoint);

	pXML->getAttr( "FontWeight",	(INT32*)&m_lfDefault.lfWeight);
	pXML->getAttr( "Italic",	(INT32*)&m_lfDefault.lfItalic);
	pXML->getAttr( "Underline", (INT32*)&m_lfDefault.lfUnderline);
	pXML->getAttr( "StrikeOut", (INT32*)&m_lfDefault.lfStrikeOut);
	//default font

	pXML->getAttr( "ClipItem", (INT32*)&m_bClipItem);
	pXML->getAttr( "SingleSelect", (INT32*)&m_bSingleSelect);
	pXML->getAttr( "FillWithEmptyItems", (INT32*)&m_bFillWithEmptyItems);
	pXML->getAttr( "DisableEmptyItems", (INT32*)&m_bDisableEmptyItems);
	pXML->getAttr( "NoSelection", (INT32*)&m_bNoSelection);
	pXML->getAttr( "ColumnCount", &m_nColumnCount);

	pXML->getAttr( "MarginLeft", &m_rtMargin.left);
	pXML->getAttr( "MarginTop", &m_rtMargin.top);
	pXML->getAttr( "MarginRight", &m_rtMargin.right);
	pXML->getAttr( "MarginBottom", &m_rtMargin.bottom);
	pXML->getAttr( "ItemGapX", &m_vItemGap.x);
	pXML->getAttr( "ItemGapY", &m_vItemGap.y);

	pXML->getAttr( "ScrollBarWidth", &m_fWidthScrollBar);
	pXML->getAttr( "ScrollBarMargin", &m_fScrollBarMargin);
	pXML->getAttr( "ScrollBarStyle", &m_nScrollStyle);

	INT32 objref;

	pXML->getAttr( "BindedItem", &objref);
	if( objref > 0)
	{
		m_pLoadedItem = (i3UIListView_Item*)pFile->FindObjectByID( objref);
		I3ASSERT( objref != 0);
	}

	pXML->getAttr( "HeaderItem", &objref);
	if( objref > 0)
	{
		m_pHeader = (i3UIListView_Header*)pFile->FindObjectByID( objref);
		I3ASSERT( objref != 0);
	}

	return bResult;
}

void i3UIListView_Box::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3UIControl::CopyTo( pDest, method );

	I3ASSERT( pDest->kind_of( meta()))

//	i3UIListView_Box* pLVBox = (i3UIListView_Box*)pDest;

	
}

void i3UIListView_Box::OnInitAfterLoad( void)
{
	i3UIControl::OnInitAfterLoad();

	if( m_pLoadedItem != nullptr)
	{
		BindItem( m_pLoadedItem);
		m_pLoadedItem->EnableCtrl(false, true);
	}

	if( m_pHeader != nullptr)
	{
		i3UIListView_Header* pTemp = m_pHeader;
		m_pHeader = nullptr;

		BindHeader( pTemp);
	}

	if( m_pHeader != nullptr && m_pBoundItem != nullptr)
		m_pHeader->UpdateShape( m_pBoundItem, m_pHeader->getHeight());
}

bool i3UIListView_Box::isOnMouse(I3UI_OVERLAP_INFO * pInfo)
{
	// 컨트롤 내 있으면 true 리턴
	if( i3UIControl::isOnMouse(pInfo))
		return true;

	i3UIControl * pTemp = (i3UIControl *) getFirstChild();
	
	while(pTemp != nullptr)
	{
		if(pTemp->isOnMouse(pInfo))
			return true;

		pTemp = (i3UIControl *) pTemp->getNext();
	}

	return false;
}

void i3UIListView_Box::_CheckResizeHeader( void)
{
	bool bResizeHeader = false;

	if( m_pHeader != nullptr && m_pBoundItem != nullptr)
	{
		i3::vector<i3UIListView_Cell*>& List = m_pBoundItem->getCellList();
		for( size_t i=0; i< List.size(); i++)
		{
			i3UIListView_Cell* pCell = List[i];
			if( pCell->getType() != ECELLTYPE_TEXT)
			{
				bResizeHeader = false;
				break;
			}
			else
				bResizeHeader = true;
		}
	}
		
	m_bResizeHeader = bResizeHeader;
}

/*virtual*/ void i3UIListView_Box::_ForceVisible( bool bFlag)
{
	i3UIControl::_ForceVisible( bFlag);

	

	if( m_pHeader)
		m_pHeader->_ForceVisible( bFlag);

	/*if( m_pBoundItem)
		m_pBoundItem->_ForceVisible( bFlag);*/

	for( size_t i = 0; i < m_ItemList.size(); i++)
	{
		i3UIListView_Item* pItem = m_ItemList[i];
		
		pItem->_ForceVisible( bFlag);
	}
}

void i3UIListView_Box::_ReOrderingForLayer( void)
{
	INT32  j;
	INT32 endHandler = -1;

	endHandler = getEndSpriteHandle();

	// 마지막 Sprite의 뒤에 다른 layer용 Sprite를 붙여준다.
	for(size_t i = 0; i < m_ItemList.size(); i++)
	{
		i3UIListView_Item * pItem = m_ItemList[ i];

		for( j = 0; j < pItem->getCellCount(); j++)
		{
			i3UIListView_Cell * pCell = pItem->getCell( j);

			if( pCell == nullptr)
				continue;

			if( i3::same_of<i3UIImageBoxEx*>(pCell->getCellChild()))
			{
				i3UIImageBoxEx * pImgBoxEx = (i3UIImageBoxEx*) pCell->getCellChild();

				INT32 idx = pImgBoxEx->AdjustSpriteOrder( endHandler, false);

				if( idx > -1)
					endHandler = idx;
			}

		}
	}
}

#if defined( I3_DEBUG)
void i3UIListView_Box::GetTDKGridProperties( i3::vector<PROPERTYINFO*>& List)
{
	_CheckResizeHeader();

	

	i3UIControl::GetTDKGridProperties( List);	

	AddMFCProperty_BeginGroup( List, "i3UIListView_Box");

	AddMFCProperty_Font( List, &m_lfDefault, &m_colDefault, "Default Font", "Text타입의 Cell들에 적용될 Font");
	AddMFCProperty_Color( List, &m_colDefault, "TextColor Normal", "노멀 상태의 디폴트 텍스트 색상");
	AddMFCProperty_Color( List, &m_colText_MouseOver, "TextColor MouseOver", "마우스오버시 디폴트 텍스트 색상");
	AddMFCProperty_Color( List, &m_colText_Selected, "TextColor Selected", "셀렉션된 디폴트 텍스트 색상");
	
	AddMFCProperty_Bool( List, &m_bClipItem, "Clip Item", 
		"Item이 잘려진 상태로 표시되는 것을 허용합니다.(단, Text와 Image 타입에 한해서만 적용됩니다.)", m_bPreviewMode);
	AddMFCProperty_Bool( List, &m_bNoSelection, "NO Selection", "선택표시를 하지 않는다.", m_bPreviewMode);
	AddMFCProperty_Bool( List, &m_bSingleSelect, "Single Select", 
		"한 번에 한 개의 아이템만 선택되도록 합니다.", m_bPreviewMode);
	AddMFCProperty_Bool( List, &m_bFillWithEmptyItems, "Fill With Empty Items", 
		"빈 아이템으로 Box의 나머지 부분을 채웁니다.",  m_bPreviewMode);
	AddMFCProperty_Bool( List, &m_bDisableEmptyItems, "Disable Empty Items", 
		"빈 아이템들을 Disable 상태로 만듭니다.", m_bPreviewMode);
	AddMFCProperty_Int32( List, &m_nColumnCount, "Column Count", "Header가 있는 경우 무조건 1이 됩니다.", 1, 3, m_bPreviewMode);
	
	AddMFCProperty_Bool( List, &m_bResizeHeader, "Resize Header", "Header의 크기를 변경할 수 있게 합니다.");
	
	AddMFCProperty_Real32( List, &m_rtMargin.left, "Left Margin", 0, 0.0f, 100.0f, m_bPreviewMode);
	AddMFCProperty_Real32( List, &m_rtMargin.top, "Top Margin", 0, 0.0f, 100.0f, m_bPreviewMode);
	AddMFCProperty_Real32( List, &m_rtMargin.right, "Right Margin", 0, 0.0f, 100.0f, m_bPreviewMode);
	AddMFCProperty_Real32( List, &m_rtMargin.bottom, "Bottom Margin", 0, 0.0f, 100.0f, m_bPreviewMode);
	AddMFCProperty_Real32( List, &m_vItemGap.x, "Item Gap X", 0, 0.0f, 100.0f, m_bPreviewMode);
	AddMFCProperty_Real32( List, &m_vItemGap.y, "Item Gap Y", 0, 0.0f, 100.0f, m_bPreviewMode);
	AddMFCProperty_Real32( List, &m_fWidthScrollBar, "ScrollBar Width", 0, 0.0f, 100.0f, m_bPreviewMode);
	AddMFCProperty_Real32( List, &m_fScrollBarMargin, "ScrollBar Margin", 0, 0.0f, 100.0f, m_bPreviewMode);

	{
		m_ScrollStyleList.clear();
		m_ScrollStyleList.push_back( "Auto");
		m_ScrollStyleList.push_back( "Show");
		m_ScrollStyleList.push_back( "Hide");

		AddMFCProperty_Combo( List, m_ScrollStyleList, &m_nScrollStyle, "ScrollBar View", "ScrollBar View Style");
	}
	
	AddMFCProperty_EndGroup( List);
}

void i3UIListView_Box::OnTDKPropertyChanged(const char *szName)
{
	//m_bFillWithEmptyItems와 m_bClipItem이 동시에 true가 될 수 없다.  왜였지?? 

	if( i3::generic_is_iequal( szName, "Clip Item") )
	{
		if( m_bClipItem == true)
			m_bFillWithEmptyItems = false;

		_RecalcItemCount();
		_RepositionItems();

	}
	else if( i3::generic_is_iequal( szName, "Fill With Empty Items") )
	{
		if( m_bFillWithEmptyItems == true)
			m_bClipItem = false;
	}
	else if( i3::generic_is_iequal(  szName, "Column Count") )
	{
		if( m_pHeader != nullptr)
			m_nColumnCount = 1;
		
		_ReposBoundItem();
		_RecalcItemCount();
		_RepositionItems();
	}
	else if( i3::generic_is_iequal( szName, "Left Margin")  ||
			i3::generic_is_iequal( szName, "Top Margin")  ||
			 i3::generic_is_iequal( szName, "Right Margin")  ||
			i3::generic_is_iequal( szName, "Bottom Margin") ||
			 i3::generic_is_iequal( szName, "Item Gap X") ||
			 i3::generic_is_iequal( szName, "Item Gap Y") ||
			 i3::generic_is_iequal( szName, "ScrollBar Width") ||
			i3::generic_is_iequal( szName, "ScrollBar Margin") )
	{
		ApplyChanges();
	}
	else if(i3::generic_is_iequal( szName, "ScrollBar View") )
	{
		ApplyChanges();
	}
	else if( i3::generic_is_iequal( szName, "Resize Header") )
	{
	}
	else
	{
		i3UIControl::OnTDKPropertyChanged( szName);
	}

}
#endif	// I3_DEBUG

void i3UIListView_Box::SetCellTextAlign( INT32 nColumn, INT32 nXAlign, INT32 nYAlign)
{
	//I3UI_LV_TEXTINFO*
	for( size_t i=0; i< m_TextInfo.size(); i++)
	{
		I3UI_LV_TEXTINFO* pInfo = m_TextInfo[i];
		
		if( pInfo->m_nColumn == nColumn)
		{
			pInfo->m_nTextAlignX = nXAlign;
			pInfo->m_nTextAlignY = nYAlign;

			return;
		}
	}

	I3UI_LV_TEXTINFO* pInfo = (I3UI_LV_TEXTINFO*)i3MemAlloc( sizeof( I3UI_LV_TEXTINFO));
	
	if( pInfo != nullptr)
	{
		pInfo->m_nColumn = nColumn;
		pInfo->m_nTextAlignX = nXAlign;
		pInfo->m_nTextAlignY = nYAlign;
	}

	m_TextInfo.push_back( pInfo);
}


//void i3UIListView_Box::SetCellTextColor( INT32 nColumn, COLOR color)
//{
//	for( INT32 i=0; i< m_TextInfo.GetCount(); i++)
//	{
//		I3UI_LV_TEXTINFO* pInfo = (I3UI_LV_TEXTINFO*)m_TextInfo.GetItem( i);
//		
//		if( pInfo->m_nColumn == nColumn)
//		{
//			pInfo->m_cTextColor = color;	//copy
//		}
//	}
//
//	I3UI_LV_TEXTINFO* pInfo = (I3UI_LV_TEXTINFO*)i3MemAlloc( sizeof( I3UI_LV_TEXTINFO));
//	
//	if( pInfo != nullptr)
//	{
//		pInfo->m_nColumn = nColumn;
//		pInfo->m_cTextColor = color;	//copy
//	}
//}

INT32 i3UIListView_Box::getCellTextAlignX( INT32 nColumn)
{
	for( size_t i=0; i< m_TextInfo.size(); i++)
	{
		I3UI_LV_TEXTINFO* pInfo = m_TextInfo[i];
		
		if( pInfo->m_nColumn == nColumn)
		{
			return pInfo->m_nTextAlignX;
		}
	}

	return 0;
}

INT32 i3UIListView_Box::getCellTextAlignY( INT32 nColumn)
{
	for( size_t i=0; i< m_TextInfo.size(); i++)
	{
		I3UI_LV_TEXTINFO* pInfo = m_TextInfo[i];
		
		if( pInfo->m_nColumn == nColumn)
		{
			return pInfo->m_nTextAlignY;
		}
	}

	return 0;
}

//COLOR i3UIListView_Box::getCellTextColor( INT32 nColumn)
//{
//	for( INT32 i=0; i< m_TextInfo.GetCount(); i++)
//	{
//		I3UI_LV_TEXTINFO* pInfo = (I3UI_LV_TEXTINFO*)m_TextInfo.GetItem( i);
//		
//		if( pInfo->m_nColumn == nColumn)
//		{
//			return pInfo->m_cTextColor;
//		}
//	}
//
//	COLOR col;
//	i3Color::SetColor( &col, (UINT8)255, (UINT8)255, (UINT8)255);
//	
//	return col;
//}

void i3UIListView_Box::UpdateAllComponents( void)
{
	_ReposHeader();
	
	_ReposBoundItem();
	
	_RecalcItemCount();
	
	_RepositionItems();
}

void i3UIListView_Box::OnItemTemplateChanged( i3UIListView_Item* pFrom)
{
	// BoundItem으로부터 온 메시지만 처리
	if( pFrom != m_pBoundItem)		
		return;

	for( size_t i=0; i< m_ItemList.size(); i++)
	{
		i3UIListView_Item* pItem = m_ItemList[i];

		pItem->SetTemplate( pFrom->getTemplate());
	}

	UpdateAllComponents();
}

void i3UIListView_Box::SetCellCallback(INT32 nItemIdx, INT32 nCellIdx, void * pFunc)
{
	I3UI_LISTVIEW_CELL_CALLBACK pCbFunc = (I3UI_LISTVIEW_CELL_CALLBACK) pFunc;

	if( (pCbFunc != nullptr) && (nItemIdx != -1))
	{
		LVITEM_DATA * pData = getData( nItemIdx);
		if( pData != nullptr)
			pData->setFunc( nCellIdx, pCbFunc);
	}

}

////////////////////////////////////////////////////
// Test Automation용 함수
// i3UIControl의 함수 정의 참조
INT32 i3UIListView_Box::GetItemIndex(const i3UIListView_Item & item)
{
	//return i3::vu::int_index_of(m_ItemList, item);
	for (UINT32 i = 0; i < m_ItemList.size(); i++)
	{
		i3UIListView_Item * pTemp = m_ItemList[i];

		if (pTemp == &item)
			return i;
	}

	return -1;
}

i3UIControl * i3UIListView_Box::TA_FindCtrlByPath(const char * pszPathToken)
{
	return nullptr;
}

i3UIControl *	i3UIListView_Box::TA_ScanText(const wchar_t * pszText, bool bMustBeSame, bool bSubControl)
{
	if (isActivated() && isVisible())
	{
		for (UINT32 i = 0; i < m_ItemList.size(); i++)
		{
			i3UIListView_Item * pTemp = m_ItemList[i];

			if (pTemp->isSelected())
			{
				i3UIControl * pFind = pTemp->TA_ScanText(pszText, bMustBeSame, bSubControl);
				if (pFind != nullptr)
					return pFind;
			}
		}
	}

	return i3UIControl::TA_ScanText( pszText, bMustBeSame, bSubControl);
}

bool i3UIListView_Box::TA_Select(const wchar_t * pszText, bool bMustBeSame)
{
	for (UINT32 i = 0; i < m_ItemList.size(); i++)
	{
		i3UIListView_Item * pItem = m_ItemList[i];

		if( pItem->TA_ScanText( pszText, bMustBeSame, true) != nullptr)
		{
			SelectData( i, true);
			return true;
		}
	}

	return false;
}

void i3UIListView_Box::DeleteTemplateList(int nCol)
{
	const size_t num_info = m_TemplateInfo.size();

	TEMPLATE_INFO* pFind = nullptr;

	for (size_t i = 0; i < num_info; ++i)
	{
		if (m_TemplateInfo[i]->m_nCol == nCol)
			pFind = m_TemplateInfo[i];
	}

	if (pFind)
		pFind->m_TemplateList.clear();
}