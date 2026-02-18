#include "i3FrameworkPCH.h"
#include "i3UITemplate_LVHeader.h"
#include "i3UIListView_Header.h"

#include "i3UITemplate_Button.h"
#include "i3UITemplate_LVSep.h"

static const char * s_pszLVHeaderInfoString[I3UI_LVHEADER_SHAPE_COUNT] = 
{
	"Header Image",
};


I3_CLASS_INSTANCE( i3UITemplate_LVHeader);

i3UITemplate_LVHeader::i3UITemplate_LVHeader()
{
	m_pTButton = i3UITemplate_Button::new_object();
	m_pTSep = i3UITemplate_LVSep::new_object();
}

i3UITemplate_LVHeader::~i3UITemplate_LVHeader()
{
	I3_SAFE_RELEASE(m_pTButton);
	I3_SAFE_RELEASE(m_pTSep);
}

i3UITemplate * i3UITemplate_LVHeader::getSubTemplate( INT32 idx)
{
	I3_BOUNDCHK( idx, I3UI_LVHEADER_SUBTEMPLATE_COUNT);

	i3UITemplate* pTemplate = nullptr;

	switch( idx) 
	{
	case I3UI_LVHEADER_SUBTEMPLATE_BUTTON:		pTemplate = m_pTButton;		break;
	case I3UI_LVHEADER_SUBTEMPLATE_SEP:			pTemplate = m_pTSep;		break;
	default:	break;
	}

	return pTemplate;
}

const char * i3UITemplate_LVHeader::getSubTemplateComment( INT32 idx)
{
	I3_BOUNDCHK( idx, I3UI_LVHEADER_SUBTEMPLATE_COUNT);

	switch( idx) 
	{
	case I3UI_LVHEADER_SUBTEMPLATE_BUTTON:	return "Button(Comment)";
	case I3UI_LVHEADER_SUBTEMPLATE_SEP:		return "LV Sep(Comment)";
	default:								return "";
	}
}

void i3UITemplate_LVHeader::setSubTemplate( INT32 idx, i3UITemplate * pTemplate)
{
	I3_BOUNDCHK( idx, I3UI_LVHEADER_SUBTEMPLATE_COUNT);

	if( pTemplate != nullptr)
	{
		switch( idx) 
		{
		case I3UI_LVHEADER_SUBTEMPLATE_BUTTON:
			I3ASSERT( i3::same_of<i3UITemplate_Button* >(pTemplate));
			I3_REF_CHANGE( m_pTButton, (i3UITemplate_Button*) pTemplate);
			break;
		case I3UI_LVHEADER_SUBTEMPLATE_SEP:		
			I3ASSERT( i3::same_of<i3UITemplate_LVSep* >(pTemplate));
			I3_REF_CHANGE( m_pTSep, (i3UITemplate_LVSep*) pTemplate);
			break;
		default:
			break;
		}
	}
}

const char * i3UITemplate_LVHeader::getShapeInfoString(INT32 nIndex)
{
	I3_BOUNDCHK( nIndex, I3UI_LVHEADER_SHAPE_COUNT);	
	return s_pszLVHeaderInfoString[nIndex];
}

i3ClassMeta	* i3UITemplate_LVHeader::GetInstanceMeta()
{
	return i3UIListView_Header::static_meta();	
}

bool i3UITemplate_LVHeader::GetControlRect( i3::pack::RECT * pRect)
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

void i3UITemplate_LVHeader::InitializeLayout(void)
{
	AdjustLayout( 300.0f, 50.0f);
}

bool i3UITemplate_LVHeader::AdjustLayout(REAL32 cx, REAL32 cy, i3::pack::RECT * pRects, INT32 shapeCount)
{
	INT32 i;

	INT32 totalCount = getShapeCount();
	
	if( shapeCount > 0)
		totalCount = MIN( totalCount, shapeCount);

	for( i = 0;i < totalCount; ++i)
	{
		i3::pack::RECT * pRect = _getTargetRect( i, pRects);

		pRect->left = 0.0f;
		pRect->top	= 0.0f;

		pRect->right = cx;
		pRect->bottom = cy;
	}

	return true;
}

void i3UITemplate_LVHeader::SpreadLayout( REAL32 * pCX, REAL32 * pCY, i3::pack::RECT * pRects, INT32 shapeCount)
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

bool i3UITemplate_LVHeader::GetTextArea( i3::pack::RECT* pIn, i3::pack::RECT* pOut)
{
	const i3::pack::RECT* pRect;

	if( pIn != nullptr)
	{
		pRect = &pIn[I3UI_LVHEADER_SHAPE_HEADER];
	}
	else
	{
		pRect = getShape( I3UI_LVHEADER_SHAPE_HEADER)->getRect();
	}

	*pOut = *pRect;
		
	return true;
}