#include "i3FrameworkPCH.h"
#include "i3UITemplate_StaticText.h"
#include "i3UIStaticText.h"

const char * s_pszStaticTextInfoString[I3UI_STATICTEXT_SHAPE_COUNT] = 
{
	"바탕 이미지 (크기 변화)",
};


I3_CLASS_INSTANCE( i3UITemplate_StaticText);


i3UIShape* i3UITemplate_StaticText::getShape(INT32 nIndex)
{
	I3_BOUNDCHK( nIndex, getShapeCount());

	return & m_pShape[nIndex];
}


const char * i3UITemplate_StaticText::getShapeInfoString(INT32 nIndex)
{
	I3_BOUNDCHK( nIndex, getShapeCount());

	return s_pszStaticTextInfoString[nIndex];
}

const char * i3UITemplate_StaticText::getComment(void)
{
	return "Static Text(Comment)";
}

const char * i3UITemplate_StaticText::getTemplateName(void)
{
	return "Static Text";
}

I3UI_CATEGORY i3UITemplate_StaticText::getTemplateCategory(void)
{
	return I3UI_CATEGORY_STATIC_TEXT;
}

i3ClassMeta * i3UITemplate_StaticText::GetInstanceMeta()
{
	return i3UIStaticText::static_meta();
}

void i3UITemplate_StaticText::InitializeLayout(void)
{
	AdjustLayout( 200.0f, 20.0f);
}

bool i3UITemplate_StaticText::GetControlRect( i3::pack::RECT * pRect)
{
	i3UIShape * pShape = getShape( I3UI_STATICTEXT_SHAPE_IMAGE);

	*pRect = *(pShape->getRect());

	pRect->Normalize();

	return true;
}

bool i3UITemplate_StaticText::AdjustLayout(REAL32 cx, REAL32 cy, i3::pack::RECT * pRects, INT32 shapeCount)
{
	i3::pack::RECT * pRect = _getTargetRect( I3UI_STATICTEXT_SHAPE_IMAGE, pRects);

	pRect->left		= 0.0f;
	pRect->top		= 0.0f;
	pRect->right	= cx;
	pRect->bottom	= cy;

	return true;
}

void i3UITemplate_StaticText::SpreadLayout(REAL32 * pCX, REAL32 * pCY, i3::pack::RECT * pRects, INT32 shapeCount)
{
	//i3::pack::RECT * pOldRect = nullptr; // C4189

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
		
		if( i%3 == 2)
		{
			left = 0.0f;
			top += pRect->bottom + I3UI_SPREAD_MARGIN;
		}
		
		*pCY = i3Math::Max( pRect->bottom, *pCX);
	}
}

bool i3UITemplate_StaticText::GetTextArea( i3::pack::RECT* pIn, i3::pack::RECT* pOut)
{
	const i3::pack::RECT* pRect;

	if( pIn != nullptr)
	{
		pRect = &pIn[I3UI_STATICTEXT_SHAPE_IMAGE];
	}
	else
	{
		pRect = getShape( I3UI_STATICTEXT_SHAPE_IMAGE)->getRect();
	}

	*pOut = *pRect;
		
	return true;
}