#include "i3FrameworkPCH.h"
#include "i3UITemplate_LVItem.h"
#include "i3UIListView_Item.h"

#include "i3UITemplate_LVCell.h"
#include "i3UITemplate_LVSep.h"

static const char * s_pszLVItemInfoString[I3UI_LVITEM_SHAPE_COUNT] = 
{
	"Disabled",
	"Normal",
	"Mouse On",
	"Selected"
};


I3_CLASS_INSTANCE( i3UITemplate_LVItem);

i3UITemplate_LVItem::i3UITemplate_LVItem()
{
	m_pTCell = i3UITemplate_LVCell::new_object();
	m_pTSep = i3UITemplate_LVSep::new_object();
}

i3UITemplate_LVItem::~i3UITemplate_LVItem()
{
	I3_SAFE_RELEASE(m_pTCell);
	I3_SAFE_RELEASE(m_pTSep);
}

i3UITemplate * i3UITemplate_LVItem::getSubTemplate( INT32 idx)
{
	I3_BOUNDCHK( idx, I3UI_LVITEM_SUBTEMPLATE_COUNT);

	i3UITemplate* pTemplate = nullptr;

	switch( idx) 
	{
	case I3UI_LVITEM_SUBTEMPLATE_CELL:		pTemplate = m_pTCell;	break;
	case I3UI_LVITEM_SUBTEMPLATE_SEP:		pTemplate = m_pTSep;		break;
	default:	break;
	}

	return pTemplate;
}

const char * i3UITemplate_LVItem::getSubTemplateComment( INT32 idx)
{
	I3_BOUNDCHK( idx, I3UI_LVITEM_SUBTEMPLATE_COUNT);

	switch( idx) 
	{
	case I3UI_LVITEM_SUBTEMPLATE_CELL:		return "LV Cell(Comment)";
	case I3UI_LVITEM_SUBTEMPLATE_SEP:		return "LV Sep(Comment)";
	default:								return "";
	}
}

void i3UITemplate_LVItem::setSubTemplate( INT32 idx, i3UITemplate * pTemplate)
{
	I3_BOUNDCHK( idx, I3UI_LVITEM_SUBTEMPLATE_COUNT);

	if( pTemplate != nullptr)
	{
		switch( idx) 
		{
		case I3UI_LVITEM_SUBTEMPLATE_CELL:		
			I3ASSERT( i3::same_of<i3UITemplate_LVCell* >(pTemplate));
			I3_REF_CHANGE( m_pTCell, (i3UITemplate_LVCell*) pTemplate);
			break;
		case I3UI_LVITEM_SUBTEMPLATE_SEP:		
			I3ASSERT( i3::same_of<i3UITemplate_LVSep*>(pTemplate));
			I3_REF_CHANGE( m_pTSep, (i3UITemplate_LVSep*) pTemplate);
			break;
		default:
			break;
		}
	}
}

const char * i3UITemplate_LVItem::getShapeInfoString(INT32 nIndex)
{
	I3_BOUNDCHK( nIndex, I3UI_LVITEM_SHAPE_COUNT);	
	return s_pszLVItemInfoString[nIndex];
}

i3ClassMeta	* i3UITemplate_LVItem::GetInstanceMeta()
{
	return i3UIListView_Item::static_meta();	
}

bool i3UITemplate_LVItem::GetControlRect( i3::pack::RECT * pRect)
{
	if( getShapeCount() <= 0)
		return false;

	INT32 i;
	for( i = 0;i < getShapeCount(); ++i)
	{
		i3UIShape * pShape = getShape( i);

		if( i == 0)
		{
			*pRect = *pShape->getRect();
			pRect->right += pShape->getPosX();
			pRect->bottom += pShape->getPosY();
		}
		else
		{
			pRect->left		= i3Math::Min( pRect->left, pShape->getPosX());
			pRect->top		= i3Math::Min( pRect->top, pShape->getPosY());
			pRect->right	= i3Math::Max( pRect->right, pShape->getPosX() + pShape->getWidth());
			pRect->bottom	= i3Math::Max( pRect->bottom, pShape->getPosY() + pShape->getHeight());
		}
	}

	pRect->Normalize();

	return true;
}

void i3UITemplate_LVItem::InitializeLayout(void)
{
	AdjustLayout( 300.0f, 50.0f);
}

bool i3UITemplate_LVItem::AdjustLayout(REAL32 cx, REAL32 cy, i3::pack::RECT * pRects, INT32 shapeCount)
{
	INT32 i;
	INT32 totalCount = getShapeCount();
	
	if( shapeCount > 0)
		totalCount = MIN( totalCount, shapeCount);

	for( i = 0; i < totalCount; ++i)
	{
		i3::pack::RECT * pRect = _getTargetRect( i, pRects);

		pRect->left = 0.0f;
		pRect->top	= 0.0f;

		pRect->right = cx;
		pRect->bottom = cy;
	}

	return true;
}

void i3UITemplate_LVItem::SpreadLayout( REAL32 * pCX, REAL32 * pCY, i3::pack::RECT * pRects, INT32 shapeCount)
{
	//i3::pack::RECT * pOldRect = nullptr;	// C4189

	*pCX = 0.0f;
	*pCY = 0.0f;

	REAL32 left = 0.0f;
	REAL32 top = 0.0f;

	INT32 i;
	INT32 totalCount = getShapeCount();

	if( shapeCount > 0)
		totalCount = MIN( totalCount, shapeCount);

	for( i = 0;i < totalCount; ++i)
	{
		i3::pack::RECT * pRect = _getTargetRect( i, pRects);

		pRect->left = left;
		pRect->top	= top;

		left += pRect->right + I3UI_SPREAD_MARGIN;

		*pCX = i3Math::Max( left, *pCX);
		*pCY = i3Math::Max( pRect->bottom, *pCY);
	}
}

bool i3UITemplate_LVItem::GetTextArea( i3::pack::RECT* pIn, i3::pack::RECT* pOut)
{
	const i3::pack::RECT* pRect;

	if( pIn != nullptr)
	{
		pRect = &pIn[I3UI_LVITEM_SHAPE_NORMAL];
	}
	else
	{
		pRect = getShape( I3UI_LVITEM_SHAPE_NORMAL)->getRect();
	}

	*pOut = *pRect;
		
	return true;
}