#include "i3FrameworkPCH.h"
#include "i3UITemplate_ProgressBarImage.h"
#include "i3UIProgressBarImage.h"

static const char* s_pszProgressBarInfoString[ I3UI_PROGRESSBAR_SHAPE_COUNT] = 
{
	"왼쪽경계",
	"가운데바탕",
	"오른쪽경계",

	"막대기"
};

I3_CLASS_INSTANCE( i3UITemplate_ProgressBarImage);


i3UIShape* i3UITemplate_ProgressBarImage::getShape(INT32 nIndex)	
{
	I3_BOUNDCHK( nIndex, getShapeCount());
	return &m_pShape[nIndex];
}

const char * i3UITemplate_ProgressBarImage::getShapeInfoString(INT32 nIndex)
{
	I3_BOUNDCHK( nIndex, getShapeCount());
	return s_pszProgressBarInfoString[nIndex];
}

const char * i3UITemplate_ProgressBarImage::getComment(void)
{
	return "Progress Image Bar(Comment)";
}

const char * i3UITemplate_ProgressBarImage::getTemplateName(void)
{
	return "Progress Image Bar";
}

I3UI_CATEGORY  i3UITemplate_ProgressBarImage::getTemplateCategory(void)
{
	return I3UI_CATEGORY_PROGRESS_IMAGE;
}

bool i3UITemplate_ProgressBarImage::GetControlRect( i3::pack::RECT * pRect)
{
	for( INT32 i=0; i < getShapeCount(); i++)
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

void i3UITemplate_ProgressBarImage::InitializeLayout(void)
{
	AdjustLayout( 200.0f, 50.0f);
}

bool i3UITemplate_ProgressBarImage::AdjustLayout(REAL32 cx, REAL32 cy, i3::pack::RECT * pRects, INT32 shapeCount)
{
	i3::pack::RECT* pLeft		= _getTargetRect( I3UI_PROGRESSBAR_SHAPE_LEFT, pRects);
	i3::pack::RECT* pCenter		= _getTargetRect( I3UI_PROGRESSBAR_SHAPE_CENTER, pRects);
	i3::pack::RECT* pRight		= _getTargetRect( I3UI_PROGRESSBAR_SHAPE_RIGHT, pRects);
	i3::pack::RECT* pBar		= _getTargetRect( I3UI_PROGRESSBAR_SHAPE_BAR, pRects);

	pLeft->bottom = cy;
	REAL32 w = pLeft->right;
	
	pRight->top = pLeft->top;
	pRight->left = cx - w;
	pRight->right = w;
	pRight->bottom = pLeft->bottom;

	pCenter->top = pLeft->top;
	pCenter->left = pLeft->top + w;
	pCenter->right = cx - w * 2.0f;
	pCenter->bottom = pLeft->bottom;

	if( pBar->right < 1.f)
		pBar->right = 90.f;
	
	pBar->left = pCenter->left;

	if (pBar->bottom > pCenter->bottom)
		pBar->bottom = pCenter->bottom;

	REAL32 fTop = pCenter->bottom*0.5f - pBar->bottom*0.5f;

	pBar->top = fTop;

	return true;
}

i3ClassMeta	* i3UITemplate_ProgressBarImage::GetInstanceMeta()
{
	return i3UIProgressBarImage::static_meta();
}

void i3UITemplate_ProgressBarImage::SpreadLayout(REAL32 * pCX, REAL32 * pCY, i3::pack::RECT * pRects, INT32 shapeCount)
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

		//막대기.. temp
		/*if( i == I3UI_PROGRESSBAR_SHAPE_BAR)
		{
			pRect->right = 90;
		}*/

		left += pRect->right + I3UI_SPREAD_MARGIN;

		*pCX = i3Math::Max( left, *pCX);
		*pCY = i3Math::Max( pRect->bottom, *pCX);
	}
}