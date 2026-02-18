#include "i3FrameworkPCH.h"
#include "i3UITemplate_DoubleText.h"
#include "i3UIDoubleText.h"

#include "i3UITemplate_StaticText.h"

const char * s_pszDoubleTextInfoString[I3UI_DOUBLETEXT_SHAPE_COUNT] = 
{
	"¹è°æ",
};

//I3_CLASS_INSTANCE( i3UITemplate_DoubleText, i3UITemplate);
I3_CLASS_INSTANCE( i3UITemplate_DoubleText);

i3UITemplate_DoubleText::i3UITemplate_DoubleText()
{
	m_pTStaticText1 = i3UITemplate_StaticText::new_object();
	m_pTStaticText2 = i3UITemplate_StaticText::new_object();
}

i3UITemplate_DoubleText::~i3UITemplate_DoubleText()
{
	I3_SAFE_RELEASE( m_pTStaticText1);
	I3_SAFE_RELEASE( m_pTStaticText2);
}


i3UIShape* i3UITemplate_DoubleText::getShape(INT32 nIndex)
{
	I3_BOUNDCHK( nIndex, getShapeCount());

	return & m_pShape[nIndex];
}
	
const char * i3UITemplate_DoubleText::getShapeInfoString(INT32 nIndex)
{
	I3_BOUNDCHK( nIndex, getShapeCount());

	return s_pszDoubleTextInfoString[nIndex];
}

INT32 i3UITemplate_DoubleText::getSubTemplateCount(void)
{
	return I3UI_DOUBLETEXT_SUBTEMPLATE_COUNT;
}

i3UITemplate * i3UITemplate_DoubleText::getSubTemplate( INT32 idx)
{
	I3_BOUNDCHK( idx, getSubTemplateCount());

	i3UITemplate* pTemplate = nullptr;

	switch( idx) 
	{
	case I3UI_DOUBLETEXT_SUBTEMPLATE_TEXT0 :	pTemplate = m_pTStaticText1;	break;
	case I3UI_DOUBLETEXT_SUBTEMPLATE_TEXT1:		pTemplate = m_pTStaticText2;	break;
	default:	break;
	}

	return pTemplate;
}

const char * i3UITemplate_DoubleText::getSubTemplateComment( INT32 idx)
{
	I3_BOUNDCHK( idx, getSubTemplateCount());

	switch( idx) 
	{
	case I3UI_DOUBLETEXT_SUBTEMPLATE_TEXT0 :	return "Static Text 1";	break;
	case I3UI_DOUBLETEXT_SUBTEMPLATE_TEXT1:		return "Static Text 2";	break;
	default:	break;
	}

	return "";
}

void i3UITemplate_DoubleText::setSubTemplate( INT32 idx, i3UITemplate * pTemplate)
{
	I3_BOUNDCHK( idx, getSubTemplateCount());

	switch( idx) 
	{
	case I3UI_DOUBLETEXT_SUBTEMPLATE_TEXT0 :
		I3ASSERT( i3::kind_of<i3UITemplate_StaticText* >(pTemplate));
		I3_REF_CHANGE( m_pTStaticText1, (i3UITemplate_StaticText*) pTemplate);
		break;

	case I3UI_DOUBLETEXT_SUBTEMPLATE_TEXT1:
		I3ASSERT( i3::kind_of<i3UITemplate_StaticText* >(pTemplate));
		I3_REF_CHANGE( m_pTStaticText2, (i3UITemplate_StaticText*) pTemplate);
		break;
	default:	break;
	}
}

I3UI_CATEGORY i3UITemplate_DoubleText::getTemplateCategory(void)
{
	return I3UI_CATEGORY_DOUBLETEXT;
}

bool i3UITemplate_DoubleText::GetControlRect( i3::pack::RECT * pRect)
{
	i3UIShape * pShape = getShape( I3UI_DOUBLETEXT_SHAPE_BKGND);

	*pRect = *(pShape->getRect());

	pRect->Normalize();

	return true;
}

void i3UITemplate_DoubleText::InitializeLayout(void)
{
	AdjustLayout( 200.0f, 80.0f);
}

bool i3UITemplate_DoubleText::AdjustLayout(REAL32 cx, REAL32 cy, i3::pack::RECT * pRects /*= nullptr*/, INT32 shapeCount)
{
	i3::pack::RECT* pRect = _getTargetRect( I3UI_DOUBLETEXT_SHAPE_BKGND, pRects);

	pRect->left = 0.0f;
	pRect->top = 0.0f;
	pRect->right = cx;
	pRect->bottom = cy;

	return true;
}

void i3UITemplate_DoubleText::SpreadLayout(REAL32 * pCX, REAL32 * pCY, i3::pack::RECT * pRects /*= nullptr*/, INT32 shapeCount)
{
}

i3ClassMeta	* i3UITemplate_DoubleText::GetInstanceMeta()
{
	return i3UIDoubleText::static_meta();
}
