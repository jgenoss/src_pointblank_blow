#include "i3FrameworkPCH.h"
#include "i3UIListView_Item.h"
#include "i3UITemplate_LVItem.h"

#include "i3UIListView_Box.h"
#include "i3UIListView_Cell.h"
#include "i3UIListView_Sep.h"
#include "i3UIListView_Data.h"
#include "i3UITemplate_LVCell.h"
#include "i3UITemplate_LVSep.h"

#include "i3UIManager.h"
#include "i3UIRenderer.h"
#include "i3UISprite.h"

#include "i3UIStaticText.h"
#include "i3UIComboBase.h"
#include "i3UIListBox.h"

#include "i3UIControl_User.h"
#include "i3UIButtonImageSet.h"
#include "i3UIDoubleText.h"
#include "i3UIImageBoxEx.h"
#include "i3UIButton.h"
#include "i3UIButtonComposed3.h"
#include "i3UICheckBox.h"
#include "i3UIControl_Dummy.h"

#include "i3Base/string/compare/generic_is_iequal.h"
#include "i3Base/string/ext/contain_string.h"
#include "i3Base/string/ext/atoi.h"
#include "i3Base/string/ext/utf16_to_utf8.h"
#include "i3Base/string/ext/utf8_to_utf16.h"

//#define i3_profile_enable
#include "i3Base/profile/profile.h"

#if defined( I3_DEBUG)
static i3::vector<i3::rc_string> s_CellTypeList;
static i3::vector<i3::rc_string> s_strings;
#endif


I3_CLASS_INSTANCE( i3UIListView_Item);

i3UIListView_Item::i3UIListView_Item( INT32 nCellCount) : m_nCellCount(nCellCount)
{
	m_CurShape = I3UI_LVITEM_SHAPE_NORMAL;
	
	// Prepare StringList for PropertyGrid
#if defined( I3_DEBUG)
	if( s_CellTypeList.size() == 0)
	{
		for( INT32 i=0; i<ECELLTYPE_COUNT; i++)
		{
			s_CellTypeList.push_back( s_pszCellType[i]);
		}
	}
#endif
}

i3UIListView_Item::~i3UIListView_Item()
{
	I3_SAFE_RELEASE(m_currLVITEM_DATA);

	for(size_t i=0; i< m_CellList.size(); i++)
	{
		i3UIListView_Cell* pCell = m_CellList[i];
		RemoveControl( pCell);
		I3_SAFE_RELEASE( pCell);
	}

	for(size_t i=0; i< m_SepList.size(); i++)
	{
		i3UIListView_Sep* pSep = m_SepList[i];
		RemoveControl( pSep);
		I3_SAFE_RELEASE( pSep);
	}

	for(size_t i=0; i< m_CellInfoList.size(); i++)
	{
		CELLINFO* pInfo = m_CellInfoList[i];
		I3MEM_SAFE_FREE( pInfo);
	}

	for(size_t i = 0; i < m_SeperatorInfoList.size(); i++)
	{
		SEPERATORINFO * pInfo = m_SeperatorInfoList[ i];
		I3MEM_SAFE_FREE( pInfo);
	}
}

REAL32 i3UIListView_Item::getCellWidth( INT32 nIdx)
{
	if( nIdx >= 0 && nIdx < (INT32)m_CellList.size())
	{
		i3UIListView_Cell* pCell = m_CellList[nIdx];
		return pCell->getWidth();
	}

	return 0.0f;
}

i3UIListView_Cell* i3UIListView_Item::getCell( INT32 idx)
{
	if( idx >= 0 && idx < (INT32)m_CellList.size())
		return m_CellList[idx];

	return nullptr;
}

i3UIListView_Sep* i3UIListView_Item::getSep( INT32 idx)
{
	if( idx >= 0 && idx < (INT32)m_SepList.size())
		return m_SepList[idx];

	return nullptr;
}

void i3UIListView_Item::_UpdateShapeState(void)
{
	UINT32 newShape = I3UI_LVITEM_SHAPE_NORMAL;

	COLOR* col = nullptr;

	if (m_pBoundBox ) col = m_pBoundBox->getDefaultColor();
	
	if( isActivated())
	{
		if( m_pBoundBox != nullptr && m_pBoundBox->isItemSelected( this))
		{
			newShape = I3UI_LVITEM_SHAPE_SELECTED;
			col = m_pBoundBox->getSelectedTextColor();
		}
		else if( isState( I3UI_STATE_ONMOUSE))	
		{
			newShape = I3UI_LVITEM_SHAPE_ONMOUSE;
			
			if (m_pBoundBox) col = m_pBoundBox->getMouseOverTextColor();
		}
	}
	else
	{
		newShape = I3UI_LVITEM_SHAPE_DISABLED;
	}
	
	if (newShape != m_CurShape )
	{
		// shape 변경이 실제로 이루어질때 텍스트 색상도 알맞게 변경해준다.. 별도 텍스트 처리 함수 마련..
		if (col)
			_UpdateTextColor(col);
		//
		m_CurShape = newShape;
	}


	for( INT32 i = 0; i < I3UI_LVITEM_SHAPE_COUNT; i++)
	{
		setShapeVisible( i, ( i == (INT32)m_CurShape) && (isValidShape( i)));	//C4389
	}
}


void i3UIListView_Item::UpdateCellPosAndSize( REAL32 cx, REAL32 cy, bool bUpdateChildRect)
{
	i3_prof_func();
	// 셀의 포지션은 상대좌표 (첫번째 셀의 LeftTop = (0,0))

	if( m_CellList.size() == 0)
		return;

	// Size
	REAL32 fPrevSizeX = 0.0f;
	for( size_t i=0; i< m_CellList.size(); i++)
	{
		i3UIListView_Cell* pCell = m_CellList[i];
		fPrevSizeX += pCell->getWidth();

		if( i < m_CellList.size() - 1)
		{	
			fPrevSizeX += m_fSepWidth;
		}
	}


	REAL32 fRatio = cx / fPrevSizeX;	// 이전 Size와 지금 Size의 Ratio를 곱한다. 

	// Position
	REAL32 fOffset = 0.0f;
	for( size_t i=0; i< m_CellList.size(); i++)
	{
		i3UIListView_Cell* pCell = m_CellList[i];
		REAL32 fW = pCell->getWidth() * fRatio;

		pCell->setPos( fOffset, 0, 0);

		if( bUpdateChildRect)
			pCell->setSize( fW, cy);
		else
			pCell->SetSizeNoNotify( fW, cy);

		// Cell control update
		i3UIControl * pCellChild = pCell->getCellChild();
		if( pCellChild != nullptr)	//cell 기본 텍스트는 차일드 컨트롤이 없기 때문에 템플릿도 없다.
		{
			i3_prof_scope("UpdateCellPosAndSize if pCellChild");

			i3::pack::RECT rect;
			if( i3UIControl_Dummy* pDummy = i3::same_cast<i3UIControl_Dummy*>(pCellChild))
			{//new ui control dummy
				pDummy->CalcControlPosAndSize( fW, cy);
			}
			else
			{
				pCellChild->getTemplate()->GetControlRect( &rect);

				REAL32 tW = fW;
				REAL32 tH = cy;
				if(fW > rect.right)
					tW = rect.right;
				if(cy > rect.bottom)
					tH = rect.bottom;
					
				REAL32 centerWidth = fW * 0.5f;
				centerWidth -= rect.right * 0.5f;
				centerWidth = max( centerWidth, 0.f);

				REAL32 centerHeight = cy * 0.5f;
				centerHeight -= rect.bottom * 0.5f;
				centerHeight = max( centerHeight, 0.f);

				pCell->setChildRect( (INT32) centerWidth, (INT32) centerHeight, (INT32) tW, (INT32) tH);
				
				//2014-12-23 PB_ENG 김대영 해당 코드로 인해 리셋 디바이스 이후 리스트 뷰 아이템들중 템플릿
				//           계열의 아이템이 사이즈가 변경되는 버그가 있어 일단 주석 처리합니다. 문제 발생시 알려주시면 감사하겠습니다.			
				//pCellChild->setPos( centerWidth, centerHeight);
				//pCellChild->setSize( tW, tH);
			}
		}

		
		if( i< m_CellInfoList.size())
		{
			CELLINFO* pInfo = m_CellInfoList[i];
			pInfo->m_Width = fW;
		}
		
		fOffset += fW;
		
		if( i < m_CellList.size() - 1)
		{
			i3_prof_scope("UpdateCellPosAndSize - EnableCtrl(true or false)");

			i3UIListView_Sep* pSep = m_SepList[i];

			SEPERATORINFO * pSepInfo = m_SeperatorInfoList[ i];

			if( pSepInfo->m_bHide)
			{
				if( pSep->isEnable())
					pSep->EnableCtrl( false);
			}
			else
			{
				if( !pSep->isEnable())
					pSep->EnableCtrl( true);
			}
			
			pSep->setPos( fOffset, 0, 0);
			pSep->setSize( m_fSepWidth, cy);

			fOffset += m_fSepWidth;
		}
	}
}

void i3UIListView_Item::_AddNewCell( i3UIScene * pAncestor)
{
	i3UIListView_Cell* pCell = i3UIListView_Cell::new_object();

	if( m_pTemplate_Cell != nullptr)
	{
		pCell->SetTemplate( m_pTemplate_Cell);
		pCell->EnableCtrl( true);
	}
	pCell->addGNodeStyle( I3_GAMENODE_STYLE_VOLATILE);
	pCell->addStyle( I3UI_STYLE_SLAVE);
	
	AddControl( pCell);

	pCell->setAncestor( pAncestor);

	m_CellList.push_back( pCell);
}

void i3UIListView_Item::_AddNewSep()
{
	i3UIListView_Sep* pSep = i3UIListView_Sep::new_object();
	
	if( m_pTemplate_Sep != nullptr)
	{
#if defined( I3_DEBUG)
		pSep->SetName( "Item_Sep");
#endif
		pSep->SetTemplate( m_pTemplate_Sep);

		I3ASSERT( m_SepList.size() <= m_SeperatorInfoList.size());
		SEPERATORINFO * pSepInfo = m_SeperatorInfoList[ m_SepList.size()];
		pSep->EnableCtrl( pSepInfo->m_bHide?false:true);
	}
	pSep->addGNodeStyle( I3_GAMENODE_STYLE_VOLATILE);
	pSep->addStyle( I3UI_STYLE_SLAVE);

	AddControl( pSep);
	m_SepList.push_back( pSep);
}

void i3UIListView_Item::ChangeCellCount( INT32 nCount, i3UIScene * pAncestor, bool bLoading)
{
	INT32 i;
	INT32 nSepCount = max( nCount-1, 0);

	INT32 nPrevCellCount = (INT32)m_CellList.size();
	INT32 nPrevSepCount = (INT32)m_SepList.size();
	INT32 nPrevCellInfoCount = (INT32)m_CellInfoList.size();
	INT32 nPrevSepInfoCount = (INT32)m_SeperatorInfoList.size();

	if( nPrevCellInfoCount < nCount)
	{
		for( i=0; i< nCount - nPrevCellInfoCount; i++)
		{
			CELLINFO* pInfo = (CELLINFO*)i3MemAlloc( sizeof( CELLINFO));
			m_CellInfoList.push_back( pInfo);
		}
	}
	else if( nPrevCellInfoCount > nCount)
	{
		for( i=nPrevCellInfoCount-1; i>= nCount; i--)
		{
			CELLINFO* pInfo = m_CellInfoList[i];
			m_CellInfoList.erase(m_CellInfoList.begin() + i);
//			m_CellInfoList.Delete( i);
			I3MEM_SAFE_FREE( pInfo);
		}
	}

	if( nPrevSepInfoCount < nCount)
	{
		for( i=0; i< nCount - nPrevSepInfoCount; i++)
		{
			SEPERATORINFO * pSepInfo = (SEPERATORINFO*) i3MemAlloc( sizeof( SEPERATORINFO));
			i3Color::Set( &pSepInfo->m_Color, (UINT8)255, (UINT8)255, (UINT8)255, (UINT8)255);
			pSepInfo->m_bHide = 0;
			m_SeperatorInfoList.push_back( pSepInfo);
		}
	}
	else if( nPrevSepInfoCount > nCount)
	{
		for( i=nPrevSepInfoCount-1; i>= nCount; i--)
		{
			SEPERATORINFO * pSepInfo = m_SeperatorInfoList[ i];
			I3MEM_SAFE_FREE( pSepInfo);
			m_SeperatorInfoList.erase(m_SeperatorInfoList.begin() + i);
//			m_SeperatorInfoList.Delete(i);
		}
	}

	// Update Lists
	if( nPrevCellCount < nCount)
	{
		for( i=0; i< nCount - nPrevCellCount; i++)
		{
			_AddNewCell( pAncestor);
		}
		for( i=0; i< nSepCount - nPrevSepCount; i++)
		{
			_AddNewSep();
		}
	}
	else if( nPrevCellCount > nCount)
	{
		for( i = nPrevCellCount-1; i >= nCount ; i--)
		{
			i3UIListView_Cell* pCell = m_CellList[i];
			m_CellList.erase(m_CellList.begin() + i);

			if( pCell->getFirstChild() != nullptr)
			{
				pCell->RemoveCellChild();
			}

			RemoveControl( pCell);
			I3_SAFE_RELEASE( pCell);
		}
		for( i = nPrevSepCount-1; i >= nSepCount ; i--)
		{
			i3UIListView_Sep* pSep = m_SepList[i];
			m_SepList.erase(m_SepList.begin() + i);

			RemoveControl( pSep);
			I3_SAFE_RELEASE( pSep);
		}
	}

	m_nCellCount = nCount;

	I3ASSERT( (INT32)m_CellList.size() == m_nCellCount);
	I3ASSERT( (INT32)m_SepList.size() == max( 0, m_nCellCount-1));

	if( m_nCellCount == 0)
		return;

	//Update Cell
	REAL32 fCellHeight = getHeight();
	REAL32 fOffset = 0.0f;
	if( bLoading)
	{
		for( INT32 i=0; i< m_nCellCount; i++)
		{
			i3UIListView_Cell* pCell = m_CellList[i];
		
			pCell->setPos( fOffset, 0, 0);

			I3ASSERT( i < (INT32) m_CellInfoList.size())
			{
				CELLINFO* pInfo = m_CellInfoList[i];
				pCell->setType( pInfo->m_Type);
				pCell->SetSizeNoNotify( pInfo->m_Width, pCell->getHeight());

			}
			
			fOffset += pCell->getWidth();

			if( i < m_nCellCount-1)
			{
				i3UIListView_Sep* pSep = m_SepList[i];
				
				pSep->setPos( fOffset, 0, 0);

				SEPERATORINFO * pSepInfo = m_SeperatorInfoList[ i];
				pSep->EnableCtrl( pSepInfo->m_bHide?false:true);

				pSep->setSize( m_fSepWidth, fCellHeight);	//temp
				fOffset += m_fSepWidth;
			}
		}
	}
	else
	{
		REAL32 fCellWidth = (getWidth() - (m_nCellCount-1) * m_fSepWidth) / m_nCellCount;
		
		for( INT32 i=0; i< m_nCellCount; i++)
		{
			i3UIListView_Cell* pCell = m_CellList[i];
			
			pCell->setPos( fOffset, 0, 0);
			pCell->SetSizeNoNotify( fCellWidth, fCellHeight);

			I3ASSERT( i < (INT32)m_CellInfoList.size())
			{
				CELLINFO* pInfo = m_CellInfoList[i];
				pInfo->m_Width = fCellWidth;
			}
			
			fOffset += fCellWidth;

			if( i < m_nCellCount-1)
			{
				i3UIListView_Sep* pSep = m_SepList[i];

				SEPERATORINFO * pSepInfo = m_SeperatorInfoList[ i];
				pSep->EnableCtrl( pSepInfo->m_bHide?false:true);
				
				pSep->setPos( fOffset, 0, 0);
				pSep->SetSizeNoNotify( m_fSepWidth, fCellHeight);
				fOffset += m_fSepWidth;
			}
		}
	}

	if( m_pBoundBox != nullptr)
	{
		if( !bLoading)
			m_pBoundBox->OnCellCountChanged( nCount);
	}
}

void i3UIListView_Item::ChangeCellType(INT32 nIdx, const CELLINFO* pCellInfo)
{
	{
		//이전기록 지우기 유독 텍스트만 이러네...
		if (m_pBoundBox)
			m_pBoundBox->OnTextChanged(this, nIdx, (wchar_t*)pCellInfo->m_wstrText, pCellInfo->m_HorizontalIndex, pCellInfo->m_VerticalIndex);
	}

	if( nIdx < (INT32)m_CellList.size())
	{
		i3UIListView_Cell* pCell = m_CellList[nIdx];
		pCell->setType(pCellInfo->m_Type);

		I3ASSERT(nIdx < (INT32)m_CellInfoList.size());
		CELLINFO* pInfo = m_CellInfoList[nIdx];
		pInfo->m_Type = pCellInfo->m_Type;

		if (m_pBoundBox != nullptr)
			m_pBoundBox->OnCellTypeChanged(this);
	}
}

ECELLTYPE i3UIListView_Item::getCellType( INT32 nIdx)
{
	if( nIdx < (INT32)m_CellList.size())
	{
		i3UIListView_Cell* pCell = m_CellList[nIdx];
		return pCell->getType();
	}

	return ECELLTYPE_TEXT;
}

// 아이템의 크기가 변하면 셀들의 크기도 같이 변하게 된다. 
// 셀에 들어있는 정보가 Text냐 Image냐에 따라서 처리를 다르게 해준다.
// Text인 경우: 셀의 크기를 원래 크기대로 유지해주고 CaptionControl::OnUpdateTextPos에서 YAlign, Intersection을 계산한다.
// Image인 경우: Sprite를 셀의 크기에 맞게 조절해 준다. 
void i3UIListView_Item::CalcRect( i3Rect* rtBox, i3Rect* rtItem)
{
	i3Rect rtIntersect;

	if( m_hSprites == nullptr)
		return;
	
	TOUCH_TYPE touch = rtBox->Intersect( rtItem, &rtIntersect);
	//I3TRACE("%d: %d, %d, %d, %d\n", touch, rtIntersect.left, rtIntersect.top, rtIntersect.right, rtIntersect.bottom);

	if( touch == TOUCH_NONE)
	{
		SetPosNoNotify( (REAL32)rtItem->left, (REAL32)rtItem->top, 0.0f);
		SetSizeNoNotify( (REAL32)(rtItem->getWidth()-1), 0.0f);
		
		for( size_t i=0; i< m_CellList.size(); i++)
		{
			i3UIListView_Cell* pCell = m_CellList[i];
			ECELLTYPE eCellType = pCell->getType();

			if( eCellType == ECELLTYPE_TEXT)
			{
				//pCell->setPos( pCell->getPos()->x, (REAL32)rtItem->top, 0);
				//pCell->SetSizeNoNotify( pCell->getWidth(), (REAL32)rtItem->getHeight());
			}
			else if( eCellType == ECELLTYPE_IMAGE)
			{
				i3UIControl* pChild = pCell->getCellChild();
				if( pChild != nullptr)
				{
					pChild->setSize( pCell->getWidth(), 0);
				}
			}
		}
	}
	else
	{
		//Item
		SetPosNoNotify( (REAL32)rtIntersect.left, (REAL32)rtIntersect.top, 0.0f);
		SetSizeNoNotify( (REAL32)(rtIntersect.getWidth()-1), (REAL32)(rtIntersect.getHeight()-1));
		CalcCustomTexCoord( &rtIntersect, rtItem);		// Calc Sprites' Coordinates

		//Cells
		for( size_t i=0; i< m_CellList.size(); i++)
		{
			i3UIListView_Cell* pCell = m_CellList[i];

			i3Rect rtCell;
			rtCell.left = (INT32)(rtItem->left + pCell->getPos()->x);
			rtCell.top = (INT32)(rtItem->top + pCell->getPos()->y);
			rtCell.right = (INT32)(rtCell.left + pCell->getWidth());
			rtCell.bottom = (INT32)(rtCell.top + rtItem->getHeight()-1);

			i3Rect _inter;
			TOUCH_TYPE touchCell = rtBox->Intersect( &rtCell, &_inter);

			if( touchCell == TOUCH_NONE)
			{
				pCell->setSize( pCell->getWidth(), 0.0f);
			}
			else if( touchCell == TOUCH_PARTIAL)
			{
				pCell->CalcCustomTexCoord( &_inter, &rtCell);
			}
			else	//TOUCH_WHOLE
			{
				pCell->m_bUseCustomTexCoord = false;
			}
		}

		//Seps
		for( size_t i=0; i< m_SepList.size(); i++)
		{
			i3UIListView_Sep* pSep = m_SepList[i];

			i3Rect rtSep;
			rtSep.left = (INT32)(rtItem->left + pSep->getPos()->x);
			rtSep.top = (INT32)(rtItem->top + pSep->getPos()->y);
			rtSep.right = (INT32)(rtSep.left + pSep->getWidth());
			rtSep.bottom = (INT32)(rtSep.top + rtItem->getHeight()-1);

			i3Rect _inter;
			TOUCH_TYPE touchSep = rtBox->Intersect( &rtSep, &_inter);

			if( touchSep == TOUCH_NONE)
			{
				pSep->setSize( pSep->getWidth(), 0.0f);
			}
			else if( touchSep == TOUCH_PARTIAL)
			{
				pSep->CalcCustomTexCoord( &_inter, &rtSep);
			}
			else	//TOUCH_WHOLE
			{
				pSep->m_bUseCustomTexCoord = false;
			}
		}

		// Cell's TexCoord ( 원래는 제대로 동작하면 안되는 코드인 것 같은데..)
		for( size_t i=0; i< m_CellList.size(); i++)
		{
			i3UIListView_Cell* pCell = m_CellList[i];
			ECELLTYPE eCellType = pCell->getType();

			if( eCellType == ECELLTYPE_IMAGE)
			{
				i3UIControl* pChild = pCell->getCellChild();
				if( i3::same_of<i3UIImageBox*>(pChild))
				{
					i3Rect* prtChild = pCell->getChildRect();
					INT32 w = prtChild->getWidth();
					INT32 h = prtChild->getHeight();

					i3Rect rtImage;
					rtImage.left = (INT32)(rtItem->left + pCell->getPos()->x + prtChild->left);
					rtImage.top = (INT32)(rtItem->top + pCell->getPos()->y + prtChild->top);
					rtImage.right = rtImage.left + w;
					rtImage.bottom = rtImage.top + h;

					i3Rect intersect;
					TOUCH_TYPE _touch = rtBox->Intersect( &rtImage, &intersect);

					if( _touch == TOUCH_NONE)
					{
						pChild->setSize( pCell->getWidth(), 0);
					}
					else if( _touch == TOUCH_PARTIAL)
					{
						INT32 left = prtChild->left;
						INT32 top = prtChild->top;
						
						if( rtBox->top - rtItem->top > 0)
							top -= (rtBox->top - rtItem->top);
						
						if( rtBox->left - rtItem->left > 0)
							left -= (rtBox->left - rtItem->left);

						pChild->setPos( (REAL32)max( 0, left), (REAL32)max( 0, top), 0);
						pChild->setSize( (REAL32)(intersect.getWidth()-1), (REAL32)(intersect.getHeight()-1));

						pChild->CalcCustomTexCoord( &intersect, &rtImage);	// Calc Sprites' Coordinates
						
					}
					else if( _touch == TOUCH_WHOLE)
					{
						INT32 left = prtChild->left;
						INT32 top = prtChild->top;
						
						if( rtBox->top - rtItem->top > 0)
							top -= (rtBox->top - rtItem->top);
						
						if( rtBox->left - rtItem->left > 0)
							left -= (rtBox->left - rtItem->left);

						pChild->setPos( (REAL32)max( 0, left), (REAL32)max( 0, top), 0);
						pChild->setSize( (REAL32)(intersect.getWidth()-1), (REAL32)(intersect.getHeight()-1));

						pChild->m_bUseCustomTexCoord = false;
					}
				}
			}
			else if( eCellType == ECELLTYPE_TEXT)
			{
				//pCell->setPos( pCell->getPos()->x, (REAL32)(rtItem->top - rtIntersect.top), 0);
				//pCell->SetSizeNoNotify( pCell->getWidth(), (REAL32)(rtItem->getHeight()));
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
INT32 i3UIListView_Item::_isPointInSep( POINT point)
{
	const size_t nSepCount = m_SepList.size();

	{
		//point를 상대좌표로 변환해서 넘겨준다.
		VEC3D* vPos = getAccumulatedPos();
	
		point.x = (LONG)(point.x - vPos->x);
		point.y = (LONG)(point.y - vPos->y);
	}

	for( size_t i=0; i<nSepCount; i++)
	{
		i3UIListView_Sep* pSep = m_SepList[i];

		if( pSep->isPtInRect( point))
			return i;
	}

	return -1;
}

/*virtual*/ void i3UIListView_Item::_ForceVisible( bool bFlag)
{
	i3UIControl::_ForceVisible( bFlag);

	
	for(INT32 i = 0; i < getCellCount(); i++)
	{
		i3UIListView_Cell* pCell = getCell( i);
		
		if( pCell != nullptr)
		{
			pCell->_ForceVisible( bFlag);
		}
	}

	for(size_t i = 0; i < m_SepList.size(); i++)
	{
		i3UIListView_Sep* pSep = m_SepList[ i];

		I3ASSERT( pSep);
		pSep->_ForceVisible( bFlag);
	}
}

bool i3UIListView_Item::_SetDataForUserControl(i3UIControl* pCellChild, const char * pszData)
{
	i3_prof_func();

	I3ASSERT( pCellChild != nullptr);
	I3ASSERT( pszData != nullptr);

	if( i3UIUserControl * pUser = i3::same_cast< i3UIUserControl* >(pCellChild) )
	{
		INT32 idx = i3::atoi( pszData);

		if( idx == -1)
			pUser->EnableCtrl( false);
		else
		{
			pUser->SetCurrentShape( idx);
			pUser->EnableCtrl( true);
			pUser->SetVisible(true);
		}

		return true;
	}

	return false;
}

bool i3UIListView_Item::_SetDataForButtonImageSet(i3UIControl* pCellChild, const char * pszData)
{
	i3_prof_func();

	I3ASSERT( pCellChild != nullptr);
	I3ASSERT( pszData != nullptr);

	if( i3UIButtonImageSet* pCtrl = i3::same_cast<i3UIButtonImageSet*>(pCellChild) )
	{
	
		INT32 idxShape =	::atoi( pszData);
		
		if( idxShape == -1)
		{
			pCtrl->EnableCtrl( false, true);
			pCtrl->SetVisible(false);
		}
		else
		{
			pCtrl->SetShapeIdx( idxShape );
			pCtrl->SetViewOnly(true);
			pCtrl->EnableCtrl( true, true);
			pCtrl->SetVisible(true);
		}

		return true;
	}

	return false;
}

bool i3UIListView_Item::_SetDataForDoubleText( i3UIControl* pCellChild, const char * pszData)
{
	i3_prof_func();


	I3ASSERT( pCellChild != nullptr);
	I3ASSERT( pszData != nullptr);

	if( i3::same_of<i3UIDoubleText*>(pCellChild))
	{
		i3UIDoubleText * pCtrl = (i3UIDoubleText*) pCellChild;

		char szText1[ MAX_PATH] = {0,};
		char szText2[ MAX_PATH] = {0,};

		char * data = (char*) &pszData[0];

		bool bTextStart = false;
		bool bSubTextStart = false;

		INT32 cnt = 0;

		while( data[cnt] )
		{
			if( bTextStart)
			{
				if( bSubTextStart == false)
					szText1[cnt] = data[cnt];
				else
					szText2[cnt] = data[cnt];
			}

			if( data[cnt] == '[')
				bTextStart = true;
			else if( data[cnt] == ']')
			{
				bTextStart = false;
				bSubTextStart = true;
			}

			cnt++;
		}		

		pCtrl->SetText( szText1, szText2);

		return true;
	}

	return false;
}

bool i3UIListView_Item::_SetDataForStaticText( i3UIControl* pCellChild, const char * pszData)
{
	i3_prof_func();


	I3ASSERT( pCellChild != nullptr);
	I3ASSERT( pszData != nullptr);

	if( i3::same_of<i3UIStaticText*>(pCellChild))
	{
		i3UIStaticText * pCtrl = (i3UIStaticText*) pCellChild;
		char * data = (char*) &pszData[0];


		pCtrl->SetText( data);

		return true;
	}

	return false;
}

bool i3UIListView_Item::_SetDataForButton( i3UIControl* pCellChild, const char * pszData)
{
	i3_prof_func();

	I3ASSERT( pCellChild != nullptr);
	I3ASSERT( pszData != nullptr);

	if( i3::same_of< i3UIButton* >(pCellChild))
	{
		i3UIButton * pButton = (i3UIButton*) pCellChild;
		I3ASSERT( pButton != nullptr);

		INT32 idx = i3::atoi( pszData);

		if( idx == -1)
			pButton->EnableCtrl( false);
		else
			pButton->EnableCtrl( true);

		return true;
	}

	return false;
}

bool i3UIListView_Item::_SetDataForComposed3Button( i3UIControl* pCellChild, const char * pszData)
{
	i3_prof_func();

	I3ASSERT( pCellChild != nullptr);
	I3ASSERT( pszData != nullptr);

	if( i3::same_of<i3UIButtonComposed3*>(pCellChild))
	{
		i3UIButtonComposed3 * pButton = (i3UIButtonComposed3*) pCellChild;
		I3ASSERT( pButton != nullptr);

		INT32 idx = i3::atoi( pszData);

		if( idx == -1)
			pButton->EnableCtrl( false);
		else
			pButton->EnableCtrl( true);

		return true;
	}

	return false;
}

bool i3UIListView_Item::_SetDataForCheckBox( i3UIControl* pCellChild, const char * pszData)
{
	i3_prof_func();

	I3ASSERT( pCellChild != nullptr);
	I3ASSERT( pszData != nullptr);

	if( i3::same_of<i3UICheckBox*>(pCellChild))
	{
		i3UICheckBox * pCheck = (i3UICheckBox*)pCellChild;
		I3ASSERT( pCheck != nullptr);

		INT32 idx = i3::atoi( pszData);

		if(idx == -1)
		{
			pCheck->EnableCtrl(false);
		}
		else if(idx == 1)
		{
			pCheck->EnableCtrl(true);
			pCheck->addState(I3UI_STATE_SELECTED);
		}
		else if(idx == 0)
		{
			pCheck->EnableCtrl(true);
			pCheck->removeState(I3UI_STATE_SELECTED);
		}
		return true;
	}
	return false;
}

bool i3UIListView_Item::_SetDataForImageBox( i3UIControl* pCellChild, const char * pszData)
{
	i3_prof_func();

	I3ASSERT( pCellChild != nullptr);
	I3ASSERT( pszData != nullptr);

	if( i3::same_of<i3UIImageBox*>(pCellChild))
	{
		i3UIImageBox * pBox = (i3UIImageBox*) pCellChild;
		I3ASSERT( pBox != nullptr);

		INT32 idx = i3::atoi( pszData);

		if(idx == -1)
		{
			pBox->EnableCtrl(false);
		}
		else
		{
			pBox->EnableCtrl(true);
		}

		return true;
	}

	return false;
}

namespace
{
	template<class COMBOCTRL> 
	static void _SetDataForComboBoxImp( COMBOCTRL* pComboBox, const char* pszData)
	{
		I3ASSERT( pComboBox != nullptr);

		INT32 idx = i3::atoi( pszData);

		if( idx == -1)
		{
			pComboBox->EnableCtrl( false);
		}
		else
		{
			pComboBox->EnableCtrl( true);
			if(idx < pComboBox->getListBox()->GetItemCount()) 
			{
				pComboBox->SetCurSel(idx);
			}
			else
			{
				pComboBox->SetCurSel(0);
			}
		}
	}
	
	
}


bool i3UIListView_Item::_SetDataForComboBox( i3UIControl* pCellChild, const char * pszData)
{
	i3_prof_func();

	I3ASSERT( pCellChild != nullptr);
	I3ASSERT( pszData != nullptr);

	if( i3::kind_of<i3UIComboBase*>(pCellChild) )
	{
		_SetDataForComboBoxImp(static_cast<i3UIComboBase*>(pCellChild), pszData);
		return true;
	}

	return false;
}

bool i3UIListView_Item::_SetDataForControlDummy( i3UIControl* pCellChild, const i3::vector<i3::string>& DataList,  INT32 nParam1, INT32 nParam2, COLOR* pCol, I3UI_FONTINFO * pFontInfo)
{
	i3_prof_func();

	if( i3::same_of<i3UIControl_Dummy*>( pCellChild))
	{
		i3GameNode* pNode = pCellChild->getFirstChild();
		i3UIControl* pControl = static_cast<i3UIControl*>(pNode);		// 기존 dynamic_cast를 쓴 이유를 모르겠음..
		size_t nDataIdx = 0;											// static_cast 혹은 i3::kind_cast로 변경함..
		while( pControl)
		{					
			//pszData 설정.
			char* pszData;
			if( nDataIdx < DataList.size())
			{
				pszData = const_cast<char*>(DataList[nDataIdx].c_str());
				pControl->EnableCtrl(true);
				pControl->SetVisible(true);
			}
			else
			{
				pszData = nullptr;
				pControl->EnableCtrl(false);
				pControl->SetVisible(false);
			}

			if( pszData != nullptr)
			{
				//StaticText
				if( i3UIStaticText* pStaticText = i3::same_cast<i3UIStaticText*>(pControl))
				{
					pStaticText->addGNodeStyle( I3_GAMENODE_STYLE_VOLATILE);
					pStaticText->addStyle( I3UI_STYLE_SLAVE);

					if( pFontInfo != nullptr)
					{
						LOGFONTA lf;

						i3mem::FillZero( &lf, sizeof(lf));

						i3mem::Copy( lf.lfFaceName, pFontInfo->m_strName.c_str(), sizeof( lf.lfFaceName));
						lf.lfHeight = pFontInfo->m_Size;
						lf.lfItalic = (pFontInfo->m_Style & I3UI_FONT_STYLE_ITALIC)?1:0;
						lf.lfUnderline = (pFontInfo->m_Style & I3UI_FONT_STYLE_UNDERLINE)?1:0;
						lf.lfStrikeOut = (pFontInfo->m_Style & I3UI_FONT_STYLE_STRIKEOUT)?1:0;
						lf.lfQuality = (BYTE) pFontInfo->m_Quality;

						if( pFontInfo->m_Style & I3UI_FONT_STYLE_BOLD)
							lf.lfWeight = 400;

						pStaticText->SetFont( &lf);
					}
					else
					{
						// nullptr이면 Font 설정을 못하므로 Text가 출력이 안된다.
						I3ASSERT( m_pBoundBox != nullptr);
						pStaticText->SetFont( m_pBoundBox->getDefaultFont());
					}


					//I3UI_TEXT_ALIGN_X_CENTER, I3UI_TEXT_ALIGN_Y_MIDDLE
					pStaticText->SetText( pszData);
					pStaticText->SetTextAlign( (I3UI_TEXT_ALIGN_X)nParam1, (I3UI_TEXT_ALIGN_Y)nParam2);
					pStaticText->SetTextColor( pCol);
				} 

				_SetDataForUserControl( pControl, (const char*) pszData);

				_SetDataForButtonImageSet( pControl, (const char*) pszData);

				_SetDataForDoubleText( pControl, (const char*) pszData);

				_SetDataForButton( pControl, (const char*) pszData);

				_SetDataForComposed3Button( pControl, (const char*) pszData);

				_SetDataForCheckBox( pControl, (const char*) pszData);

				_SetDataForImageBox( pControl, (const char*) pszData);

				_SetDataForComboBox(pControl, (const char *) pszData);

			}

			pNode = pControl->getNext();
			pControl = static_cast<i3UIControl*>(pNode);

			nDataIdx++;
		}


		return true;
	}


	return false;
}

void i3UIListView_Item::CopyMembersFrom( i3UIListView_Item* pSrcItem, i3UIScene * pAncestor)
{
	m_nCellCount = pSrcItem->m_nCellCount;
	m_fSepWidth = pSrcItem->m_fSepWidth;

	ChangeCellCount( m_nCellCount, pAncestor, true);

	for( size_t i=0; i< m_CellInfoList.size(); i++)
	{
		CELLINFO* pInfoDest = m_CellInfoList[i];
		CELLINFO* pInfoSrc = pSrcItem->m_CellInfoList[i];

		pInfoDest->m_Type = pInfoSrc->m_Type;
		pInfoDest->m_Width = pInfoSrc->m_Width;
		i3::generic_string_copy(pInfoDest->m_wstrText, pInfoSrc->m_wstrText);
		pInfoDest->m_HorizontalIndex = pInfoSrc->m_HorizontalIndex;
		pInfoDest->m_VerticalIndex = pInfoSrc->m_VerticalIndex;
		pInfoDest->m_nTemplateWidth = pInfoSrc->m_nTemplateWidth;
		pInfoDest->m_nTemplateHeight = pInfoSrc->m_nTemplateHeight;


		if( i < m_CellInfoList.size() - 1)
		{
			SEPERATORINFO * pDestSepInfo = m_SeperatorInfoList[ i];
			SEPERATORINFO * pSrcSepInfo = pSrcItem->m_SeperatorInfoList[ i];

			pDestSepInfo->m_bHide = pSrcSepInfo->m_bHide;
			i3Color::Copy( &pDestSepInfo->m_Color, &pSrcSepInfo->m_Color);
		}
	}

	for( INT32 i=0; i< m_nCellCount; i++)
	{
		i3UIListView_Cell* pDest = m_CellList[i];
		i3UIListView_Cell* pSrc = pSrcItem->m_CellList[i];

		pDest->setType( pSrc->getType());
		pDest->setSize( pSrc->getSize());
	}

	m_pTemplate_Cell = pSrcItem->m_pTemplate_Cell;
	m_pTemplate_Sep = pSrcItem->m_pTemplate_Sep;

	m_pBoundBox = pSrcItem->m_pBoundBox;
}


void i3UIListView_Item::_SetCellData( INT32 nCellIdx, const i3::vector<i3::string>& DataList, INT32 nParam1, INT32 nParam2,
									 COLOR* pCol, I3UI_FONTINFO * pFontInfo, I3UI_LISTVIEW_CELL_CALLBACK func)
{
	i3_prof_func();

	I3ASSERT( nCellIdx < (INT32)m_CellList.size());

	i3UIListView_Cell* pCell = m_CellList[nCellIdx];

	ECELLTYPE eCellType = pCell->getType();

	//Default Data
	char* szData;
	if( DataList.size())
		szData = const_cast<char*>(DataList[0].c_str());
	else 
		szData = nullptr;

	// temp
	switch( eCellType)
	{
	case ECELLTYPE_TEXT:
		{
			i3_prof_scope("case ECELLTYPE_TEXT:");

			if( pFontInfo != nullptr)
			{
				i3_prof_scope("SetFont with pFontInfo");
				LOGFONTA lf;

				i3mem::FillZero( &lf, sizeof(lf));

				i3mem::Copy( lf.lfFaceName, pFontInfo->m_strName.c_str(), sizeof( lf.lfFaceName));
				lf.lfHeight = pFontInfo->m_Size;
				lf.lfItalic = (pFontInfo->m_Style & I3UI_FONT_STYLE_ITALIC)?1:0;
				lf.lfUnderline = (pFontInfo->m_Style & I3UI_FONT_STYLE_UNDERLINE)?1:0;
				lf.lfStrikeOut = (pFontInfo->m_Style & I3UI_FONT_STYLE_STRIKEOUT)?1:0;
				lf.lfQuality = (BYTE) pFontInfo->m_Quality;
				
				if( pFontInfo->m_Style & I3UI_FONT_STYLE_BOLD)
					lf.lfWeight = 400;

				pCell->SetFont( &lf);
			}
			else
			{
				i3_prof_scope("SetFont with default FontInfo");
				// nullptr이면 Font 설정을 못하므로 Text가 출력이 안된다.
				I3ASSERT( m_pBoundBox != nullptr);
				pCell->SetFont( m_pBoundBox->getDefaultFont());
			}

			if( szData == nullptr)
			{
				i3_prof_scope("SetText with EmptyString");
				pCell->SetText( "");
			}
			else
			{
				i3_prof_start("SetText szData");
				pCell->SetText( szData);	//const WCHAR16
				i3_prof_end();
				
				i3_prof_start("SetTextAlign");
				pCell->SetTextAlign( (I3UI_TEXT_ALIGN_X)nParam1, (I3UI_TEXT_ALIGN_Y)nParam2);
				i3_prof_end();
				
				i3_prof_start("SetTextColor");
				pCell->SetTextColor( pCol);
				i3_prof_end();
			}
		}
		break;
	case ECELLTYPE_IMAGE:
		{
			i3_prof_scope("case ECELLTYPE_IMAGE:");
			if( szData == nullptr)
			{
				pCell->setChildTemplate( nullptr);
				pCell->RemoveCellChild();
			}
			else
			{
				bool bSetChild = false;
				i3UITemplate* pChild = pCell->getChildTemplate();

				if( pChild == nullptr)
				{
					bSetChild = true;
				}
				else
				{
					if( i3::generic_is_iequal( pChild->GetNameString(), szData) == false) 
					{
						bSetChild = true;
					}
				}

				if( bSetChild)
				{
					i3UITemplate* pTemplate = i3UI::FindTemplateByName( szData);
					if( pTemplate != nullptr)
					{
						pCell->setChildTemplate( pTemplate);
						pCell->AddCellChild( pTemplate);
						pCell->SetSizeNoNotify( pCell->getWidth(), pCell->getHeight());

						i3UIControl* pCellChild = pCell->getCellChild();
						I3ASSERT( pCellChild != nullptr);

						i3Rect* pRt = pCell->getChildRect();
						if( i3::same_of< i3UIImageBox* >(pCellChild))
						{
							pCellChild->SetPosNoNotify( (REAL32)pRt->left, (REAL32)pRt->top);
							pCellChild->SetSizeNoNotify( (REAL32)pRt->getWidth(), (REAL32)pRt->getHeight());
						}
						else
						{
							i3::pack::RECT rect;

							pCellChild->getTemplate()->GetControlRect( &rect);

							REAL32 centerWidth = (REAL32)pRt->getWidth() * 0.5f;
							centerWidth -= rect.right * 0.5f;

							REAL32 centerHeight = (REAL32) pRt->getHeight() * 0.5f;
							centerHeight -= rect.bottom * 0.5f;

							pCellChild->SetPosNoNotify( centerWidth, centerHeight);
							pCellChild->SetSizeNoNotify( rect.right, rect.bottom);
						}
					}
					else
					{
						pCell->SetSizeNoNotify( pCell->getWidth(), 0);
					}
				}
			}
		}
		break;

	case ECELLTYPE_TEMPLATE:	// 이 경우 Template은 한번 설정되면 바뀔 수 없다..
		{	
			i3_prof_scope("case ECELLTYPE_TEMPLATE:");

			
			char szName[64] = {0,};
			
			if( m_pBoundBox != nullptr)
			{
				m_pBoundBox->GetTemplateName( nCellIdx, szName, sizeof( szName));
			}
			
			if( szName[0] == 0 || szData == nullptr )
			{	
				pCell->setChildTemplate( nullptr);		//사용 하지 않음
				pCell->RemoveCellChild();			
			}
			else
			{
				if (i3UI::getToolDevMode() == true && i3::generic_is_iequal(szData, "NewTemplate"))
				{
					pCell->setChildTemplate(nullptr);		//사용 하지 않음
					pCell->RemoveCellChild();
				}

				//if( pCell->getChildTemplate() == nullptr)
				if( pCell->getCellChild() == nullptr)
				{
					I3ASSERT_RETURN(m_pBoundBox);
					INT32 cnt = m_pBoundBox->GetTemplateCount( nCellIdx);
					if( cnt == 1)
					{
						i3_prof_scope("Find Template & Add Template to Cell");

						i3UITemplate* pTemplate = i3UI::FindTemplateByName( szName);
						
						if (i3UI::getToolDevMode() == true)
						{
							if (!pTemplate)
							{
								pTemplate = i3UI::FindTemplateByName("ImageBox_Null");
							}
						}
						else
						{
							I3ASSERT(pTemplate != nullptr);
						}

						pCell->setChildTemplate(pTemplate);	//사용 하지 않음
						pCell->AddCellChild(pTemplate);
					}
					else
					{//new ui control dummy
						i3_prof_scope("GetTemplateInfo & Set Template To Cell Child");

						pCell->SetCellChild( m_pBoundBox->GetTemplateInfo( nCellIdx));
					}
				}

				pCell->SetSizeNoNotify( pCell->getWidth(), pCell->getHeight());

				i3UIControl* pCellChild = pCell->getCellChild();
				I3ASSERT( pCellChild != nullptr);

				if (i3UI::getToolDevMode() == true)
				{
					VEC2D *Vec2d;
					Vec2d = pCellChild->getSize();
					if (i3UIControl_Dummy* pDummy = i3::same_cast<i3UIControl_Dummy*>(pCellChild))
					{//new ui control dummy
						i3_prof_scope("DummyChild Set Pos & Size");
						pDummy->UpdatePosAndSize();
					}
					else
					{
						i3_prof_start("CellChild Set Pos");
						pCellChild->setPos(0, 0);
						i3_prof_end();

						i3_prof_start("CellChild Set Size");
						if (i3::generic_is_iequal(szData, "TemplateSize"))
						{
							pCellChild->setSize((REAL32)nParam1, (REAL32)nParam2);
						}
						else
						{
							pCellChild->setSize((REAL32)Vec2d->x, (REAL32)Vec2d->y);
						}
						i3_prof_end();
					}

					//툴에서 템플릿은 무조건 중앙정렬
					TemplateAlign(pCell, nParam1, nParam2);
				}
				else
				{
					i3Rect* pRt = nullptr;
					pRt = pCell->getChildRect();
					if (i3UIControl_Dummy* pDummy = i3::same_cast<i3UIControl_Dummy*>(pCellChild))
					{//new ui control dummy
						i3_prof_scope("DummyChild Set Pos & Size");
						pDummy->UpdatePosAndSize();
					}
					else
					{
						i3_prof_start("CellChild Set Pos");
						pCellChild->setPos((REAL32)pRt->left, (REAL32)pRt->top);
						i3_prof_end();

						i3_prof_start("CellChild Set Size");
						pCellChild->setSize((REAL32)pRt->getWidth(), (REAL32)pRt->getHeight());
						i3_prof_end();
					}
				}

				if( i3::kind_of<i3UICaptionControl*>(pCellChild))
				{
					i3_prof_scope("CellChild Caption Set Align & Color");
					i3UICaptionControl* pCaption = (i3UICaptionControl*)pCellChild;
					pCaption->SetTextAlign( (I3UI_TEXT_ALIGN_X)nParam1, (I3UI_TEXT_ALIGN_Y)nParam2);
					pCaption->SetTextColor( pCol);
				}
				
				//new ui control dummy
				if( _SetDataForControlDummy( pCellChild, DataList, nParam1, nParam2, pCol, pFontInfo))
					goto EXIT;

				if( _SetDataForStaticText( pCellChild, (const char*) szData))
					goto EXIT;
		
				if( _SetDataForUserControl( pCellChild, (const char*) szData))
					goto EXIT;

				if( _SetDataForButtonImageSet( pCellChild, (const char*) szData))
					goto EXIT;

				if( _SetDataForDoubleText( pCellChild, (const char*) szData))
					goto EXIT;

				if( _SetDataForButton( pCellChild, (const char*) szData))
					goto EXIT;

				if( _SetDataForComposed3Button( pCellChild, (const char*) szData))
					goto EXIT;

				if( _SetDataForCheckBox( pCellChild, (const char*) szData))
					goto EXIT;

				if(_SetDataForImageBox( pCellChild, (const char*) szData)) 
					goto EXIT;

				_SetDataForComboBox(pCellChild, (const char *) szData);
					

EXIT:
				if( func != nullptr)
					func( pCell, szData);
			}
		}
		break;
	default:
		break;
	}

}

void i3UIListView_Item::_UpdateTextColor( COLOR* clrDefault)
{
	if (m_currLVITEM_DATA == nullptr)		// 널인 경우 그냥 방치..어차피 텍스트도 나오지 않을 것같다..
		return;
	
	const size_t num_cell = m_CellList.size();

	for (size_t i = 0 ; i < num_cell ; ++i)
	{
		if (m_currLVITEM_DATA->isColorSet(i) )		// 별도의 칼라셋이 들어온것은 모두 스킵한다..
			continue;

		i3UIListView_Cell* pCell = m_CellList[i];
		const LVCELL_DATA* pCellData = m_currLVITEM_DATA->GetCellData( i);

		ECELLTYPE eCellType = pCell->getType();
		
		switch(eCellType)
		{
		case ECELLTYPE_TEXT:
			{
				if (!pCellData->m_DataList.empty() )
					pCell->SetTextColor( clrDefault);
			}
		break;

		case ECELLTYPE_TEMPLATE:
			{
				i3UIControl* pCellChild = pCell->getCellChild();
				
				if ( i3::kind_of<i3UICaptionControl*>(pCellChild) )
				{
					static_cast<i3UICaptionControl*>(pCellChild)->SetTextColor(clrDefault);			
				}
				else
				if( i3::same_of<i3UIControl_Dummy*>( pCellChild))
				{
					i3GameNode* pNode = pCellChild->getFirstChild();
					
					if (pNode)
					{
						const size_t num_DataIdx = pCellData->m_DataList.size();
					
						for (size_t nDataIdx = 0; nDataIdx < num_DataIdx ; ++nDataIdx)
						{					
							if ( i3UIStaticText* pStaticCtrl = i3::same_cast<i3UIStaticText*>(pNode) )
								pStaticCtrl->SetTextColor(clrDefault);
																			
							pNode = pNode->getNext();
							if (pNode == nullptr)
								break;
						}
					}
				}
			}
		break;
		}
	
	}
		
	
}

void i3UIListView_Item::SetData( LVITEM_DATA* pData)
{	
	i3_prof_func();
	//char conv[CELLDATA_SIZE+1] = "";

	I3_REF_CHANGE(m_currLVITEM_DATA, pData);

	if( pData != nullptr)
	{
		for(size_t i=0; i< m_CellList.size(); i++)
		{
			i3UIListView_Cell* pCell = m_CellList[i];

			//pData->GetCellData( i, conv, CELLDATA_SIZE);
			//const i3::string* conv = pData->GetCellData( i, 0);
			const LVCELL_DATA* pCellData = pData->GetCellData( i);

			if( m_pBoundBox != nullptr)
			{
				INT32 nParam1 = m_pBoundBox->getCellTextAlignX( i);
				INT32 nParam2 = m_pBoundBox->getCellTextAlignY( i);

				COLOR* pCol = nullptr;

				if( pData->isColorSet(i))	pCol = pData->getCellColor( i);
				else				
				{
					if ( m_pBoundBox->isItemSelected(this) )
						pCol = m_pBoundBox->getSelectedTextColor();
					else
					if ( isState( I3UI_STATE_ONMOUSE) )
						pCol = m_pBoundBox->getMouseOverTextColor();
					else 
						pCol = m_pBoundBox->getDefaultColor();
				}

				//_SetCellData( i, conv, nParam1, nParam2, pCol, pData->getFontInfo( i), pData->getFunc( i) );
				_SetCellData( i, pCellData->m_DataList, nParam1, nParam2, pCol, nullptr, pData->getFunc( i) );

				// tooltip
				//if( pData->getToolTip( i)[0] != 0)
					pCell->SetToolTipMsg( pData->getToolTip( i));
			}
		}

		//disable
		setDisabled( pData->isDisabled());
	}
	else
	{
		for(size_t i=0; i< m_CellList.size(); i++)
		{
			_SetCellData( (INT32)i, i3::vector<i3::string>(), 0, 0, nullptr, nullptr);
		}
	}
		
	addState( I3UI_STATE_NEED_UPDATE);
}

void i3UIListView_Item::UpdateShapes( void)
{
	addState( I3UI_STATE_NEED_UPDATE);

	for( size_t i=0; i< m_CellList.size(); i++)
	{
		i3UIListView_Cell* pCell = m_CellList[i];
		pCell->setModifyState();
	}

	for( size_t i=0; i< m_SepList.size(); i++)
	{
		i3UIListView_Sep* pSep = m_SepList[i];
		pSep->setModifyState();
	}
}

void i3UIListView_Item::SetComboItems( INT32 nCellIdx, const char* pszItems)
{
	if( nCellIdx > -1 && nCellIdx < (INT32)m_CellList.size())
	{
		i3UIListView_Cell* pCell = m_CellList[nCellIdx];
		
		i3UIControl* pChild = (i3UIControl*)pCell->getFirstChild();
		if( i3::kind_of<i3UIComboBase*>(pChild)) 
		{
			i3UIComboBase* pCombo = (i3UIComboBase*)pChild;
			pCombo->SetItems( pszItems);
		}
	}
}

void i3UIListView_Item::SetButtonName( INT32 nCellIdx, const char* pszName)
{
	if( nCellIdx > -1 && nCellIdx < (INT32)m_CellList.size())
	{
		i3UIListView_Cell* pCell = m_CellList[nCellIdx];
		
		i3UIControl* pChild = (i3UIControl*)pCell->getFirstChild();
		if( i3::same_of<i3UIButton* >(pChild))
		{
			i3UIButton* pButton = (i3UIButton*)pChild;
			pButton->SetText( pszName);
			pButton->SetTextAlign( I3UI_TEXT_ALIGN_X_CENTER, I3UI_TEXT_ALIGN_Y_MIDDLE);

			pButton->setModifyState();
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////

void i3UIListView_Item::_OnChangeSepPos( INT32 nIdx, POINT point)
{
	I3ASSERT( nIdx < (INT32)m_CellList.size()-1);

	{
		//point를 상대좌표로 변환
		VEC3D* vPos = getAccumulatedPos();
	
		point.x = (LONG)(point.x - vPos->x);
		point.y = (LONG)(point.y - vPos->y);
	}

	i3UIListView_Sep* pSep = m_SepList[nIdx];
	i3UIListView_Cell* pLeft = m_CellList[nIdx];
	i3UIListView_Cell* pRight = m_CellList[nIdx+1];

	REAL32 fTotalWidth = pLeft->getWidth() + pRight->getWidth() + pSep->getWidth();

	// MINMAX
	REAL32 min = pLeft->getPos()->x;
	REAL32 max = pRight->getPos()->x + pRight->getWidth() - pSep->getWidth();
	REAL32 fX = MINMAX( min, point.x - m_cursorOffset, max);

	// Sep
	pSep->SetPosNoNotify( fX, pSep->getPos()->y, 0.0f);
	
	// Left Cell
	pLeft->SetSizeNoNotify( pSep->getPos()->x - pLeft->getPos()->x, getHeight());
	I3ASSERT( nIdx < (INT32)m_CellInfoList.size())
	{
		CELLINFO* pInfo = m_CellInfoList[nIdx];
		pInfo->m_Width = pLeft->getWidth();
	}
	
	// Right Cell
	pRight->SetSizeNoNotify( fTotalWidth - pLeft->getWidth() - pSep->getWidth(), getHeight());
	I3ASSERT( nIdx + 1 < (INT32)m_CellInfoList.size())
	{
		CELLINFO* pInfo = m_CellInfoList[nIdx + 1];
		pInfo->m_Width = pRight->getWidth();
	}
	pRight->SetPosNoNotify( pSep->getPos()->x + pSep->getWidth(), pRight->getPos()->y, 0.0f);

	_UpdateShapeState();
	
	if( m_pBoundBox != nullptr)
		m_pBoundBox->OnCellSizeChanged( this);
}

bool i3UIListView_Item::OnMouseMove_EditMode( UINT32 nFlag, POINT point)
{
	if( m_nSelectedSep != -1)
		_OnChangeSepPos( m_nSelectedSep, point);

	if( m_nSelectedSep != -1 || _isPointInSep( point) != -1)
		return true;

	return false;
}

void i3UIListView_Item::OnLButtonDown_EditMode( UINT32 nFlag, POINT point)
{
	m_nSelectedSep = _isPointInSep( point);

	if( m_nSelectedSep != -1)
	{
		i3UIListView_Sep* pSep = m_SepList[m_nSelectedSep];
		
		m_cursorOffset = point.x - pSep->getAccumulatedPos()->x;
	}
}

void i3UIListView_Item::OnLButtonUp_EditMode( UINT32 nFlag, POINT point)
{
	m_nSelectedSep = -1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void i3UIListView_Item::OnBindTemplate(void)
{
	i3UIControl::OnBindTemplate();

	if( m_pTemplate == nullptr)
		return;

	m_pTemplate_Cell = (i3UITemplate_LVCell*)m_pTemplate->getSubTemplate( I3UI_LVITEM_SUBTEMPLATE_CELL);
	m_pTemplate_Sep = (i3UITemplate_LVSep*)m_pTemplate->getSubTemplate( I3UI_LVITEM_SUBTEMPLATE_SEP);

	for( size_t i=0; i< m_CellList.size(); i++)
	{
		i3UIListView_Cell* pCell = m_CellList[i];
		pCell->SetTemplate( m_pTemplate_Cell);
		pCell->EnableCtrl( true);

		if( i < m_CellList.size()-1)
		{
			i3UIListView_Sep* pSep = m_SepList[i];
			pSep->SetTemplate( m_pTemplate_Sep);
			pSep->EnableCtrl( true);
		}
	}

	if( m_pBoundBox != nullptr)
		m_pBoundBox->OnItemTemplateChanged( this);	// 일단 Box에 알린다. 
}

void i3UIListView_Item::OnNotify( i3UIControl * pCtrl, I3UI_EVT event, INT32 param1, INT32 param2)
{
	switch( event)
	{
	case I3UI_EVT_BTN_PUSHED:
		{
			INT32 cellIdx = -1;
			for( size_t i=0; i< m_CellList.size(); i++)
			{
				i3UIControl* pCell = m_CellList[i];
				if( pCell == pCtrl)
				{
					cellIdx = i;
					break;
				}
			}

			I3ASSERT( cellIdx > -1);
			
			ParentNotify( event, param1, cellIdx);

			// 리스트뷰 체크박스 복수처리 기능 - Dongik - 여기가 맞나 모르겠다.
			i3UIListView_Cell* pCell = (i3UIListView_Cell*)pCtrl;
			i3UIControl * p_Control = pCell->getCellChild();
//			if(p_Control == nullptr)
//				break;
			if(i3::same_of<i3UICheckBox*>(p_Control))
			{
				INT32 nItemIdx = m_pBoundBox->getItemIndex();		// 눈에 보여지는 리스트에서 선택된 아이템 인덱스를 가져온다.
				INT32 nDataIdx = m_pBoundBox->getDataIndex();		// 실제 리스트의 데이터 갯수에서 가져온다.
				LVITEM_DATA * pItemData = m_pBoundBox->getData(nDataIdx); 

				INT32 n_cellCount = m_pBoundBox->getCellCount();
				for(INT32 i=0; i<n_cellCount; i++)					// 굳이 순회안하고 체크박스 상태를 갱신할 수 있지만, 체크박스가 데이터1개당 여러개일 경우 대비해서 작성.
				{
					i3UIControl * p_target_Control = m_pBoundBox->getCellChild(nItemIdx,i);
					if(p_Control == p_target_Control)
					{
						if(p_Control->isSelected() == false)		// AddState가 뒤에 호출되므로 반대로 처리하였습니다.
						{
							pItemData->SetCellData( i, "0");
						}
						else
						{
							pItemData->SetCellData( i, "1");
						}
						break;
					}
				}
			}
		}
		break;
	case I3UI_EVT_MOUSEWHEEL:
		{
			//I3TRACE("i3UIListView_Item::OnNotify(I3UI_EVT_MOUSEWHEEL)\n");
			ParentNotify( event, param1, param2);
		}
		break;
	case I3UI_EVT_ENTERMOUSE:
		if( m_pBoundBox != nullptr)
		{
			addState( I3UI_STATE_ONMOUSE);
			Notify( I3UI_EVT_ENTERMOUSE, 0, 0);
			
			i3UIListView_Cell* pCell = (i3UIListView_Cell*)pCtrl;
//			if( pCell->getCellChild() != nullptr)
			{
				((i3UIListView_Box*)getParent())->setIsMouseOverValidItem( true);

		// 아이템이 리스트박스보다 늦게 삭제되서인지...리스트박스를 지우면서 마우스가 들어오는 순간...m_pBoundBox가 nullptr이 됩니다.
		// 수정해야함.난중에. 11.06.01.수빈.

				m_pBoundBox->setCurrentOverCell( pCell);
			}

			addState( I3UI_STATE_NEED_UPDATE);
		}
		break;
	case I3UI_EVT_LEAVEMOUSE:
		if( m_pBoundBox != nullptr)
		{
			removeState( I3UI_STATE_ONMOUSE);
			Notify( I3UI_EVT_LEAVEMOUSE, 0, 0);

			i3UIListView_Cell* pCell = (i3UIListView_Cell*)pCtrl;
			if( pCell->getCellChild() != nullptr)
			{
				((i3UIListView_Box*)getParent())->setIsMouseOverValidItem( false);
				m_pBoundBox->setCurrentOverCell( nullptr);
			}

			addState( I3UI_STATE_NEED_UPDATE);
		}
		break;
	case I3UI_EVT_CLICKED :
		{

		}
	case I3UI_EVT_SELITEM:
	case I3UI_EVT_COMBOBOX_SELITEM:
	case I3UI_EVT_DBL_CLICKED :
	case I3UI_EVT_R_CLICKED :
	case I3UI_EVT_KILLFOCUS:
		{
			ParentNotify( event, param1, param2);
		}
		break;

	default:
		break;
	}

	i3UIControl::OnNotify( pCtrl, event, param1, param2);
}

void i3UIListView_Item::OnEnabled( bool bEnable)
{
	i3UIControl::OnEnabled( bEnable);

	for( size_t i=0; i< m_CellList.size(); i++)
	{
		i3UIListView_Cell* pCell = m_CellList[i];
		pCell->EnableCtrl( bEnable);

		i3GameNode* pChild = pCell->getFirstChild();
		if(  i3::kind_of<i3UIControl*>(pChild))
		{
			i3UIControl* pCtrl = (i3UIControl*)pChild;
			pCtrl->EnableCtrl( bEnable);
		}
	}

	for( size_t i=0; i< m_SepList.size(); i++)
	{
		i3UIListView_Sep* pSep = m_SepList[i];
		pSep->EnableCtrl( bEnable);
	}
}

void i3UIListView_Item::prepareSprites( void)
{
	i3UIControl::prepareSprites();

	//// cell
	//for( UINT32 i = 0; i < m_CellList.size(); i++)
	//{
	//	m_CellList.at(i)->prepareSprites();
	//}

	//// cell child
	//for( UINT32 i = 0; i < m_CellList.size(); i++)
	//{
	//	i3GameNode* pChild = m_CellList.at(i)->getFirstChild();
	//	if(i3::kind_of<i3UIControl*>(pChild))
	//	{
	//		i3UIControl* pCtrl = static_cast<i3UIControl*>(pChild);
	//		pCtrl->prepareSprites();
	//	}
	//}

	//// seperator
	//for( size_t i=0; i< m_SepList.size(); i++)
	//{
	//	m_SepList.at(i)->prepareSprites();
	//}
}

void i3UIListView_Item::OnSize( REAL32 cx, REAL32 cy)
{
	i3UIControl::OnSize( cx, cy);
	
	UpdateCellPosAndSize( cx, cy, true);

	if( m_pBoundBox != nullptr && m_pBoundBox->isEnable() )
		m_pBoundBox->OnItemSizeChanged( cx, cy);
}

void i3UIListView_Item::SetSizeNoNotify( REAL32 cx, REAL32 cy)
{
	i3_prof_func();

	i3Vector::Set( &m_Size, cx, cy);

	_recsetModifyState( this, true);

	if( m_pTemplate != nullptr)
	{
		m_pTemplate->AdjustLayout( cx, cy, m_pRects, m_nShapeCount);
	}

	UpdateCellPosAndSize( cx, cy);
}

void i3UIListView_Item::Reset( i3UIListView_Item* pItem)
{
	//if( pItem != nullptr)
	//	CopyMembersFrom( pItem);
	if( pItem == nullptr)
		return;

	SetSizeNoNotify( pItem->getSize()->x, pItem->getSize()->y);

	for( size_t i=0; i< m_CellList.size(); i++)
	{
		_SetCellData( i, i3::vector<i3::string>(), 0, 0, nullptr, nullptr);
		
		i3UIListView_Cell* pCell = m_CellList[i];
		i3UIListView_Cell* pCellFrom = pItem->getCell( i);
		
		pCell->SetPosNoNotify( pCellFrom->getPos());
		pCell->SetSizeNoNotify( pCellFrom->getSize());

		if( i < m_CellList.size()-1)
		{
			i3UIListView_Sep* pSep = m_SepList[i];
			i3UIListView_Sep* pSepFrom = pItem->getSep( i);

			pSep->SetPosNoNotify( pSepFrom->getPos());
			pSep->SetSizeNoNotify( pSepFrom->getSize());
		}
	}

	EnableCtrl( true);
}

void i3UIListView_Item::setDisabled( bool bDisable)
{
	if( bDisable)
	{
		addState( I3UI_STATE_DEACTIVATED);
	}
	else
	{
		removeState( I3UI_STATE_DEACTIVATED);
	}
}

void i3UIListView_Item::OnLButtonUp( UINT32 nFlag, POINT point)
{
	i3UIControl::OnLButtonUp( nFlag, point);

	if( isActivated())
	{
		// select corresponding data 
		ParentNotify( I3UI_EVT_SELITEM, point.x, point.y);
	}
}

//아래 코드는 실질적으로 Parent Control에 두번의 Notify를 보낸다. 
//i3UIControl에서 보내지는 Notify는 param1, param2가 모두 0이므로 relevant한 정보를 전달할 수 없다.
//따라서 직접 ParentNotify를 호출하여 좌표를 전달한다. 
//정상적인 경우 ListView_Item의 Parent인 ListView_Box로 Notify가 전달된다. 
void i3UIListView_Item::OnLButtonDblClk( UINT32 nFlag, POINT point)
{
	i3UIControl::OnLButtonDblClk( nFlag, point);

	if( isActivated())
	{
		ParentNotify( I3UI_EVT_DBL_CLICKED, point.x, point.y);
	}
}

/*virtual*/ void i3UIListView_Item::OnRButtonDown( UINT32 nFlag, POINT point)
{
	i3UIControl::OnRButtonDown( nFlag, point);

	if( isActivated())
	{
		// select corresponding data 
		ParentNotify( I3UI_EVT_R_CLICKED, point.x, point.y);
	}
}

void i3UIListView_Item::OnEnterMouse( REAL32 x, REAL32 y, UINT32 keys)
{
	i3UIControl::OnEnterMouse( x, y, keys);

	addState( I3UI_STATE_NEED_UPDATE);
}

void i3UIListView_Item::OnLeaveMouse( void)
{
	i3UIControl::OnLeaveMouse();

	addState( I3UI_STATE_NEED_UPDATE);
}


#if defined( I3_COMPILER_VC)
#pragma pack(push, 4)
#endif

namespace pack
{
	struct ALIGN4 UILISTVIEW_ITEM
	{
		INT8		m_ID[4] = { 'U', 'L', 'V', 'I' };
		INT32		m_nCellCount = 0;
		REAL32		m_rSepWidth = 0.0f;
		UINT32		pad[8] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack(pop)
#endif

UINT32 i3UIListView_Item::OnSave( i3ResourceFile * pResFile)
{
	
	UINT32	Rc, Result= 0;
	pack::UILISTVIEW_ITEM data;

	Result = i3UIControl::OnSave( pResFile );
	I3_CHKIO( Result);

	data.m_nCellCount = m_nCellCount;
	data.m_rSepWidth = m_fSepWidth;

	Rc = pResFile->Write( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	for(size_t i=0; i< m_CellInfoList.size(); i++)
	{
		CELLINFO* pInfo = m_CellInfoList[ i];
		
		Rc = pResFile->Write( pInfo, sizeof( CELLINFO));
		I3_CHKIO( Rc);
		Result += Rc;
	}

	for(size_t i = 0; i < m_SeperatorInfoList.size(); i++)
	{
		SEPERATORINFO * pSepInfo = m_SeperatorInfoList[ i];

		Rc = pResFile->Write( pSepInfo, sizeof( SEPERATORINFO));
		I3_CHKIO( Rc);
		Result += Rc;
	}

	return Result;
}

UINT32 i3UIListView_Item::OnLoad( i3ResourceFile * pResFile)
{
	i3_prof_func();

	INT32 i;
	UINT32	Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::UILISTVIEW_ITEM data;

	Result = i3UIControl::OnLoad(pResFile);
	I3_CHKIO(Result);

	Rc = pStream->Read(&data, sizeof(data));
	I3_CHKIO(Rc);
	Result += Rc;

	m_nCellCount = data.m_nCellCount;
	m_fSepWidth = data.m_rSepWidth;

	for (i = 0; i < m_nCellCount; i++)
	{
		CELLINFO* pInfo = (CELLINFO*)i3MemAlloc(sizeof(CELLINFO));
		//CELLINFO* pInfo = new CELLINFO;

		// 		pInfo->m_Type = ECELLTYPE_TEXT;
		// 		pInfo->m_Width = 0;
		// 		pInfo->m_wstrCaption = L"";

		DWORD dataSize = 0;

		if (pResFile->GetVersionMinor() > 8 && pResFile->GetVersionMajor() == 1)
		{
			dataSize = sizeof(CELLINFO);
		}
		else
		{
			dataSize = sizeof(pInfo->m_Type) + sizeof(pInfo->m_Width);
		}

		Rc = pStream->Read(pInfo, dataSize);
		I3_CHKIO(Rc);
		Result += Rc;

		m_CellInfoList.push_back(pInfo);
	}

	for (i = 0; i < m_nCellCount - 1; i++)
	{
		SEPERATORINFO * pSepInfo = (SEPERATORINFO*)i3MemAlloc(sizeof(SEPERATORINFO));

		Rc = pStream->Read(pSepInfo, sizeof(SEPERATORINFO));
		I3_CHKIO(Rc);
		Result += Rc;

		m_SeperatorInfoList.push_back(pSepInfo);
	}

	//ChangeCellCount( m_nCellCount, nullptr, true);

	return Result;
}

bool i3UIListView_Item::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{

	char conv[MAX_PATH];
	bool bResult = i3UIControl::OnSaveXML( pFile, pXML);

	if( bResult == false)
		return false;

	pXML->addAttr( "CellCount", m_nCellCount);
	pXML->addAttr( "SepWidth", m_fSepWidth);

	for(size_t i=0; i< m_CellInfoList.size(); i++)
	{
		CELLINFO* pInfo = m_CellInfoList[i];
		ECELLTYPE eType = pInfo->m_Type;
		REAL32 fWidth = pInfo->m_Width;
		
		i3::snprintf( conv, sizeof( conv), "%s_%d", "Cell", i+1);
		pXML->addAttr( conv, (INT32)eType);

		i3::snprintf( conv, sizeof( conv), "%s_%d%s", "Cell", i+1, "X");
		pXML->addAttr( conv, fWidth);

		i3::snprintf(conv, sizeof(conv), "%s_%d%s", "Cell", i + 1, "Text");

		i3::string utf8;
		i3::utf16_to_utf8(pInfo->m_wstrText, utf8);
		pXML->addAttr(conv, utf8.c_str());

		i3::snprintf(conv, sizeof(conv), "%s_%d%s", "Cell", i + 1, "HAlign");
		pXML->addAttr(conv, pInfo->m_HorizontalIndex);

		i3::snprintf(conv, sizeof(conv), "%s_%d%s", "Cell", i + 1, "VAlign");
		pXML->addAttr(conv, pInfo->m_VerticalIndex);

		i3::snprintf(conv, sizeof(conv), "%s_%d%s", "Cell", i + 1, "TemplateWidth");
		pXML->addAttr(conv, pInfo->m_nTemplateWidth);

		i3::snprintf(conv, sizeof(conv), "%s_%d%s", "Cell", i + 1, "TemplateHeight");
		pXML->addAttr(conv, pInfo->m_nTemplateHeight);
	}

	for(size_t i = 0; i < m_SeperatorInfoList.size(); i++)
	{
		SEPERATORINFO * pSepInfo = m_SeperatorInfoList[i];

		i3::snprintf( conv, sizeof( conv), "SeperatorHide_%d", i+1);
		pXML->addAttr( conv, (INT32) pSepInfo->m_bHide);

		i3::snprintf( conv, sizeof( conv), "SeperatorColor_%d", i+1);
		i3Color::SetToXML( pXML, conv, &pSepInfo->m_Color);
	}

	return bResult;
}

bool i3UIListView_Item::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	INT32 i;
	char conv[MAX_PATH];
	bool bResult = i3UIControl::OnLoadXML( pFile, pXML);

	if( bResult == false)
		return false;

	pXML->getAttr( "CellCount", &m_nCellCount);
	pXML->getAttr( "SepWidth", &m_fSepWidth);

	for (i = 0; i < m_nCellCount; i++)
	{
		ECELLTYPE eType = ECELLTYPE_TEXT;
		REAL32 fWidth = 10.0f;

		i3::snprintf(conv, sizeof(conv), "%s_%d", "Cell", i + 1);
		pXML->getAttr(conv, (INT32*)&eType);

		i3::snprintf(conv, sizeof(conv), "%s_%d%s", "Cell", i + 1, "X");
		pXML->getAttr(conv, &fWidth);


		CELLINFO* pInfo = (CELLINFO*)i3MemAlloc(sizeof(CELLINFO));

		pInfo->m_Type = eType;
		pInfo->m_Width = fWidth;

		if (pFile->GetVersionMinor() > 0 && pFile->GetVersionMajor() >= 2)
		{
			i3::snprintf(conv, sizeof(conv), "%s_%d%s", "Cell", i + 1, "Text");

			const char* buf = pXML->getAttr(conv);
			if (buf != nullptr)
			{
				i3::rc_wstring wstrText;
				i3::utf8_to_utf16(i3::string(buf), wstrText);
				i3::generic_string_copy(pInfo->m_wstrText, wstrText);
			}

			i3::snprintf(conv, sizeof(conv), "%s_%d%s", "Cell", i + 1, "HAlign");
			pXML->getAttr(conv, &pInfo->m_HorizontalIndex);

			i3::snprintf(conv, sizeof(conv), "%s_%d%s", "Cell", i + 1, "VAlign");
			pXML->getAttr(conv, &pInfo->m_VerticalIndex);

			i3::snprintf(conv, sizeof(conv), "%s_%d%s", "Cell", i + 1, "TemplateWidth");
			pXML->getAttr(conv, &pInfo->m_nTemplateWidth);

			i3::snprintf(conv, sizeof(conv), "%s_%d%s", "Cell", i + 1, "TemplateHeight");
			pXML->getAttr(conv, &pInfo->m_nTemplateHeight);
		}
		else
		{
			i3::generic_string_copy(pInfo->m_wstrText, L"");
			pInfo->m_HorizontalIndex = 0;
			pInfo->m_VerticalIndex = 0;
			pInfo->m_nTemplateWidth = 0;
			pInfo->m_nTemplateHeight = 0;
		}

		m_CellInfoList.push_back(pInfo);
	}


	for( i = 0; i < m_nCellCount - 1; i++)
	{
		INT32 hide = 0;
		SEPERATORINFO * pSepInfo = (SEPERATORINFO*) i3MemAlloc( sizeof( SEPERATORINFO));
		i3::snprintf( conv, sizeof( conv), "SeperatorHide_%d", i+1);
		pXML->getAttr( conv, &hide);
		pSepInfo->m_bHide = (UINT8)hide;

		i3::snprintf( conv, sizeof( conv), "SeperatorColor_%d", i+1);
		i3Color::GetFromXML( pXML, conv, &pSepInfo->m_Color);

		m_SeperatorInfoList.push_back( pSepInfo);
	}

	return bResult;
}

void i3UIListView_Item::OnInitAfterLoad( void)
{
	i3UIControl::OnInitAfterLoad();

	i3UIScene * pAncestor = nullptr;
	i3GameNode * pParent = getParent();

	while( pParent)
	{
		if( i3::same_of<i3UIScene*>(pParent))
		{
			pAncestor = (i3UIScene*) pParent;
			break;
		}

		pParent = pParent->getParent();
	}

	ChangeCellCount( m_nCellCount, pAncestor, true);
}

bool i3UIListView_Item::isOnMouse(I3UI_OVERLAP_INFO * pInfo)
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

void i3UIListView_Item::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3UIControl::CopyTo( pDest, method );

	I3ASSERT( pDest->kind_of( meta()));

//	i3UIListView_Item* pLVItem = (i3UIListView_Item*)pDest;

	
}


#if defined( I3_DEBUG)
const char* _GetString( const char* pStr)
{
	for( size_t i=0; i< s_strings.size(); i++)
	{
		const char* pTemp = s_strings[i].c_str();
		
		if( i3::generic_is_iequal( pStr, pTemp) )
			return pTemp;
	}

	INT32 idx = (INT32)s_strings.size();
	s_strings.push_back( pStr);	//지역변수로 하지 않기 위함

	I3ASSERT( idx >= 0);
	
	return s_strings[idx].c_str();
}

void i3UIListView_Item::GetTDKGridProperties( i3::vector<PROPERTYINFO*>& List)
{

	char conv[64];

	i3UIControl::GetTDKGridProperties(List);

	AddMFCProperty_BeginGroup(List, "i3UIListView_Item");

	//if( m_pBoundBox)
	//	m_pBoundBox->GetTDKGridProperties( List);

	AddMFCProperty_Real32(List, &m_fSepWidth, "Seperator Width", "", 0.0f, 30.0f);
	AddMFCProperty_Int32(List, &m_nCellCount, "Number of Cells", "");

	for (INT32 i = 0; i < m_nCellCount; i++)
	{
		i3::snprintf(conv, sizeof(conv), "%s%d", "CellType_", i + 1);

		const char* pStr = _GetString(conv);

		I3ASSERT(i < (INT32)m_CellInfoList.size());

		CELLINFO* pInfo = m_CellInfoList[i];
		AddMFCProperty_Combo(List, s_CellTypeList, (INT32*)&(pInfo->m_Type), (const char*)pStr, "");
	}

	AddMFCProperty_BeginGroup(List, "i3UIListView_Sep");
	for (size_t i = 0; i < m_SeperatorInfoList.size(); i++)
	{
		SEPERATORINFO * pInfo = m_SeperatorInfoList[i];
		i3::snprintf(conv, sizeof(conv), "SepHide_%d\0", i + 1);
		const char* pStr = _GetString(conv);
		AddMFCProperty_Bool(List, (bool*)&pInfo->m_bHide, (const char*)pStr);
	}

	m_ListBoxString.clear();
	for (INT32 i = 0; i < m_nCellCount; i++)
	{
		CELLINFO* pInfo = m_CellInfoList[i];
		m_ListBoxString.push_back(pInfo->m_wstrText);
	}

	for (INT32 i = 0; i < m_nCellCount; i++)
	{
		CELLINFO* pInfo = m_CellInfoList[i];

		if (pInfo->m_Type == ECELLTYPE_TEXT)
		{
			i3::snprintf(conv, sizeof(conv), "TextComponent_%d\0", i + 1);
			const char* pStr = _GetString(conv);
			AddMFCProperty_BeginGroup(List, pStr);

			i3::snprintf(conv, sizeof(conv), "TextValue_%d\0", i + 1);
			pStr = _GetString(conv);

			AddMFCProperty_RcStringW(List, m_ListBoxString[i], pStr);

			i3::snprintf(conv, sizeof(conv), "TextAlignX_%d\0", i + 1);
			pStr = _GetString(conv);
			AddMFCProperty_Combo(List, m_HorizontalAlignList, &pInfo->m_HorizontalIndex, pStr, "X Align style");

			i3::snprintf(conv, sizeof(conv), "TextAlignY_%d\0", i + 1);
			pStr = _GetString(conv);
			AddMFCProperty_Combo(List, m_VerticalAlignList, &pInfo->m_VerticalIndex, pStr, "Y Align style");

			AddMFCProperty_EndGroup(List);
		}
		else if (pInfo->m_Type == ECELLTYPE_TEMPLATE)
		{
			i3::snprintf(conv, sizeof(conv), "TemplateComponent_%d\0", i + 1);
			const char* pStr = _GetString(conv);
			AddMFCProperty_BeginGroup(List, pStr);

			i3::snprintf(conv, sizeof(conv), "TemplateName_%d\0", i + 1);
			pStr = _GetString(conv);
			AddMFCProperty_RcStringW(List, m_ListBoxString[i], pStr, "...", true);

			i3::snprintf(conv, sizeof(conv), "TemplateWidth_%d\0", i + 1);
			pStr = _GetString(conv);
			AddMFCProperty_Int32(List, &pInfo->m_nTemplateWidth, pStr, "");

			i3::snprintf(conv, sizeof(conv), "TemplateHeight_%d\0", i + 1);
			pStr = _GetString(conv);
			AddMFCProperty_Int32(List, &pInfo->m_nTemplateHeight, pStr, "");

			i3::snprintf(conv, sizeof(conv), "TemplateAlignX_%d\0", i + 1);
			pStr = _GetString(conv);
			AddMFCProperty_Combo(List, m_HorizontalAlignList, &pInfo->m_HorizontalIndex, pStr, "X Align style");

			i3::snprintf(conv, sizeof(conv), "TemplateAlignY_%d\0", i + 1);
			pStr = _GetString(conv);
			AddMFCProperty_Combo(List, m_VerticalAlignList, &pInfo->m_VerticalIndex, pStr, "Y Align style");

			AddMFCProperty_EndGroup(List);
		}

	}

	AddMFCProperty_EndGroup(List);

	AddMFCProperty_EndGroup(List);
}

INT32 _ExtractFirstInteger( const char* szString)
{
	char conv[32];
	INT32 j=0;
	
	INT32 i=0;
	bool bInt = false;

	while( szString[i] != 0)
	{
		if( szString[i] >= '0' && szString[i] <= '9')
		{
			conv[j++] = szString[i];
			bInt = true;
		}
		else if( bInt == true)
			break;
		
		i++;
	}

	conv[j] = 0;

	if( bInt == false)
		return -1;

	return i3::atoi( conv);
}

void i3UIListView_Item::OnTDKPropertyChanged(const char *szName)
{
	if (i3::generic_is_iequal(szName, "Number of Cells"))
	{
		ChangeCellCount(m_nCellCount, getAncestor(), true);

		if (m_pBoundBox != nullptr)
			m_pBoundBox->OnCellCountChanged(m_nCellCount);

		i3UI::setTDKPropertyUpdate(true);
	}
	else if (i3::generic_is_iequal(szName, "Seperator Width"))
	{
		if (m_pBoundBox != nullptr)
		{
			//Bind된 상태에서는 Box에 알려서 다른 Child Item에도 변화를 적용
			m_pBoundBox->OnSeperatorChanged(m_fSepWidth);
		}
		else
		{
			//Unbind 상태에서는 자기 자신에 대해서만 처리
			UpdateCellPosAndSize(m_Size.x, m_Size.y, true);
		}
	}
	else if (i3::contain_string(szName, "CellType_") >= 0)
	{
		INT32 nIdx = _ExtractFirstInteger(szName) - 1;

		I3ASSERT(nIdx < (INT32)m_CellInfoList.size());
		CELLINFO* pInfo = m_CellInfoList[nIdx];

		i3::vector<i3::string> DataList;

		i3::generic_string_copy(pInfo->m_wstrText, L"");

		_SetCellData(nIdx, DataList, 1, 1, nullptr, nullptr);

		ChangeCellType(nIdx, pInfo);

		i3UI::setTDKPropertyUpdate(true);
	}
	else if (i3::contain_string(szName, "SepHide_") >= 0)
	{
		if (m_pBoundBox)
			m_pBoundBox->OnSeperatorChanged(m_fSepWidth);
	}
	else if (i3::contain_string(szName, "TextValue_") >= 0)
	{
		INT32 nIdx = _ExtractFirstInteger(szName) - 1;
		CELLINFO* pInfo = m_CellInfoList[nIdx];
		i3::generic_string_copy(pInfo->m_wstrText, m_ListBoxString[nIdx]);

		if (m_pBoundBox)
			m_pBoundBox->OnTextChanged(this, nIdx, pInfo->m_wstrText, pInfo->m_HorizontalIndex, pInfo->m_VerticalIndex);
	}
	else if (i3::contain_string(szName, "TextAlignX_") >= 0 || i3::contain_string(szName, "TextAlignY_") >= 0)
	{
		INT32 nIdx = _ExtractFirstInteger(szName) - 1;
		CELLINFO* pInfo = m_CellInfoList[nIdx];
		if (m_pBoundBox)
			m_pBoundBox->OnTextChanged(this, nIdx, pInfo->m_wstrText, pInfo->m_HorizontalIndex, pInfo->m_VerticalIndex);
	}
	else if (i3::contain_string(szName, "TemplateAlignX_") >= 0 || i3::contain_string(szName, "TemplateAlignY_") >= 0)
	{
		INT32 nIdx = _ExtractFirstInteger(szName) - 1;
		CELLINFO* pInfo = m_CellInfoList[nIdx];

		i3::vector<i3::string> DataList;
		DataList.push_back("0");
		_SetCellData(nIdx, DataList, pInfo->m_HorizontalIndex, pInfo->m_VerticalIndex, nullptr, nullptr);
	}
	else if (i3::contain_string(szName, "TemplateWidth_") >= 0 || i3::contain_string(szName, "TemplateHeight_") >= 0)
	{
		INT32 nIdx = _ExtractFirstInteger(szName) - 1;
		CELLINFO* pInfo = m_CellInfoList[nIdx];

		i3::vector<i3::string> DataList;
		DataList.push_back("TemplateSize");
		_SetCellData(nIdx, DataList, pInfo->m_nTemplateWidth, pInfo->m_nTemplateHeight, nullptr, nullptr);
	}
	else if (i3::contain_string(szName, "TemplateName_") >= 0)
	{
		INT32 nIdx = _ExtractFirstInteger(szName) - 1;
		CELLINFO* pInfo = m_CellInfoList[nIdx];
		i3::generic_string_copy(pInfo->m_wstrText, m_ListBoxString[nIdx]);

		i3::string utf8;
		i3::utf16_to_utf8(pInfo->m_wstrText, utf8);

		m_pBoundBox->DeleteTemplateList(nIdx);
		m_pBoundBox->SetTemplateInfo(nIdx, utf8.c_str());

		i3UITemplate* pTemplate = i3UI::FindTemplateByName(utf8.c_str());

		pInfo->m_HorizontalIndex = pInfo->m_VerticalIndex = 0;

		i3::vector<i3::string> DataList;
		DataList.push_back("NewTemplate");
		_SetCellData(nIdx, DataList, pInfo->m_HorizontalIndex, pInfo->m_VerticalIndex, nullptr, nullptr);

		pInfo->m_nTemplateWidth = (INT32)pTemplate->getShape(0)->getWidth();
		pInfo->m_nTemplateHeight = (INT32)pTemplate->getShape(0)->getHeight();

		DataList.clear();
		DataList.push_back("TemplateSize");
		_SetCellData(nIdx, DataList, pInfo->m_nTemplateWidth, pInfo->m_nTemplateHeight, nullptr, nullptr);

		i3UI::setTDKPropertyUpdate(true);
	}
	else
	{
		if (m_pBoundBox)
			m_pBoundBox->OnTDKPropertyChanged(szName);

		i3UIControl::OnTDKPropertyChanged(szName);
	}

}
#endif	// I3_DEBUG

void i3UIListView_Item::Init( void)
{
	//Drop Object..
	i3UIControl::Init();
	I3ASSERT( m_nCellCount == 0);
	ChangeCellCount( m_nCellCount, getAncestor() );
}

//////////////////////////////////////////
// Test Automation
INT32	i3UIListView_Item::getCellIndex(const i3UIListView_Cell & cell)
{
	//return i3::vu::int_index_of( m_CellList, cell);
	for (UINT32 i = 0; i < m_CellList.size(); i++)
	{
		i3UIListView_Cell * pTemp = m_CellList[i];

		if (pTemp == &cell)
			return i;
	}

	return -1;
}

void	i3UIListView_Item::TA_GetPathID(char * pszID, INT32 limit)
{
	// Item은 상위 부모인 ListView_Box에서의 Index로 표현한다.
	i3UIListView_Box * pParent = (i3UIListView_Box *)getParent();
	I3ASSERT(pParent != nullptr);
	//I3ASSERT(pParent->kind_of(i3UIListView_Box::static_meta()));

	INT32 idx = pParent->GetItemIndex(*this);
	if (idx == -1)
	{
		i3UIControl::TA_GetPathID( pszID, limit);
		return;
	}

	sprintf_s(pszID, limit, "LVItem(%d)", idx);
}

i3UIControl * i3UIListView_Item::TA_FindCtrlByPath(const char * pszPathToken)
{
	return nullptr;
}

i3UIControl * i3UIListView_Item::TA_ScanText(const wchar_t * pszText, bool bMustBeSame, bool bSubControl)
{
	for (UINT32 i = 0; i < m_CellList.size(); i++)
	{
		i3UIListView_Cell * pCell = m_CellList[i];

		i3UIControl * pFind = pCell->TA_ScanText(pszText, bMustBeSame, bSubControl);
		if ( pFind != nullptr)
			return pFind;
	}

	return i3UIControl::TA_ScanText( pszText, bMustBeSame, bSubControl);
}

void i3UIListView_Item::TemplateAlign(i3UIListView_Cell* pCell, int AlignX, int AlignY)
{
	i3UIControl* pCellChild = pCell->getCellChild();

	i3::pack::RECT parantRect, childRect;
	REAL32 centerWidth = 0;
	REAL32 centerHeight = 0;

	pCell->getRect(&parantRect);
	pCellChild->getRect(&childRect);

	if (AlignX == I3UI_TEXT_ALIGN_X_CENTER)
	{
		centerWidth = parantRect.Width() * 0.5f;
		centerWidth -= childRect.right * 0.5f;
	}
	else if (AlignX == I3UI_TEXT_ALIGN_X_RIGHT)
	{
		centerWidth = parantRect.Width();
		centerWidth -= childRect.right;
	}

	if (AlignY == I3UI_TEXT_ALIGN_Y_MIDDLE)
	{
		centerHeight = parantRect.Height() * 0.5f;
		centerHeight -= childRect.bottom * 0.5f;
	}
	else if (AlignY == I3UI_TEXT_ALIGN_Y_BOTTOM)
	{
		centerHeight = parantRect.Height();
		centerHeight -= childRect.bottom;
	}

	pCellChild->SetPosNoNotify(centerWidth, centerHeight);
	//pCellChild->SetSizeNoNotify(childRect.right, childRect.bottom);
}

void i3UIListView_Item::InitItem()
{
	for (INT32 i = 0; i < (INT32)m_CellInfoList.size(); i++)
	{
		CELLINFO* pInfo = m_CellInfoList[i];

		i3::string utf8;
		i3::utf16_to_utf8(pInfo->m_wstrText, utf8);

		if (pInfo->m_Type == ECELLTYPE_TEMPLATE)
		{
			m_pBoundBox->DeleteTemplateList(i);
			m_pBoundBox->SetTemplateInfo(i, utf8.c_str());

			i3::vector<i3::string> DataList;
			DataList.push_back("NewTemplate");
			_SetCellData(i, DataList, pInfo->m_HorizontalIndex, pInfo->m_VerticalIndex, nullptr, nullptr);

			DataList.clear();
			DataList.push_back("TemplateSize");
			_SetCellData(i, DataList, pInfo->m_nTemplateWidth, pInfo->m_nTemplateHeight, nullptr, nullptr);

			DataList.clear();
			DataList.push_back("0");
			_SetCellData(i, DataList, pInfo->m_HorizontalIndex, pInfo->m_VerticalIndex, nullptr, nullptr);
		}
		else if (pInfo->m_Type == ECELLTYPE_TEXT)
		{
			m_pBoundBox->OnTextChanged(this, i, pInfo->m_wstrText, pInfo->m_HorizontalIndex, pInfo->m_VerticalIndex);
		}
	}
}

void i3UIListView_Item::UpdateCell()
{
	for (INT32 i = 0; i < (INT32)m_CellInfoList.size(); i++)
	{
		CELLINFO* pInfo = m_CellInfoList[i];

		if (pInfo->m_Type == ECELLTYPE_TEXT)
		{
			if (m_pBoundBox)
				m_pBoundBox->OnTextChanged(this, i, pInfo->m_wstrText, pInfo->m_HorizontalIndex, pInfo->m_VerticalIndex);
		}
		else if (pInfo->m_Type == ECELLTYPE_TEMPLATE)
		{

		}
	}
}