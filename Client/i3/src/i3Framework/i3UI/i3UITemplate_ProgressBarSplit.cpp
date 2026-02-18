#include "i3FrameworkPCH.h"
#include "i3UITemplate_ProgressBarSplit.h"
#include "i3UIProgressBarSplit.h"

static const char* s_pszProgressBarInfoString[ I3UI_PROGRESSBAR_SPLIT_SHAPE_COUNT] = 
{
	"왼쪽경계",
	"가운데바탕",
	"오른쪽경계",
	"막대기 왼쪽",
	"막대기 중간",
	"막대기 오른쪽"
};

I3_CLASS_INSTANCE( i3UITemplate_ProgressBarSplit); 

i3UIShape* i3UITemplate_ProgressBarSplit::getShape(INT32 nIndex)	
{
	I3_BOUNDCHK( nIndex, getShapeCount());
	return &m_pShape[nIndex];
}

const char * i3UITemplate_ProgressBarSplit::getShapeInfoString(INT32 nIndex)
{
	I3_BOUNDCHK( nIndex, getShapeCount());
	return s_pszProgressBarInfoString[nIndex];
}

const char * i3UITemplate_ProgressBarSplit::getComment(void)
{
	return "Progress Split Bar(Comment)";
}

const char * i3UITemplate_ProgressBarSplit::getTemplateName(void)
{
	return "Progress Split Bar";
}

I3UI_CATEGORY  i3UITemplate_ProgressBarSplit::getTemplateCategory(void)
{
	return I3UI_CATEGORY_PROGRESS_SPLIT;
}

bool i3UITemplate_ProgressBarSplit::GetControlRect( i3::pack::RECT * pRect)
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

void i3UITemplate_ProgressBarSplit::InitializeLayout(void)
{
	AdjustLayout( 200.0f, 50.0f);
}

bool i3UITemplate_ProgressBarSplit::AdjustLayout(REAL32 cx, REAL32 cy, i3::pack::RECT * pRects, INT32 shapeCount)
{
	i3::pack::RECT* pLeft		= _getTargetRect( I3UI_PROGRESSBAR_SPLIT_SHAPE_LEFT, pRects);
	i3::pack::RECT* pCenter		= _getTargetRect( I3UI_PROGRESSBAR_SPLIT_SHAPE_CENTER, pRects);
	i3::pack::RECT* pRight		= _getTargetRect( I3UI_PROGRESSBAR_SPLIT_SHAPE_RIGHT, pRects);

	i3::pack::RECT* pBarLeft	= _getTargetRect( I3UI_PROGRESSBAR_SPLIT_SHAPE_BAR_LEFT, pRects);
	i3::pack::RECT* pBarCenter	= _getTargetRect( I3UI_PROGRESSBAR_SPLIT_SHAPE_BAR_CENTER, pRects);
	i3::pack::RECT* pBarRight	= _getTargetRect( I3UI_PROGRESSBAR_SPLIT_SHAPE_BAR_RIGHT, pRects);

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

	if( pBarCenter->right < 1.f)
		pBarCenter->right = 90.f;
	
	//Center left = BarLeft left
	//Center rigjt = BarRight right
	pBarLeft->left = pCenter->left;
	pBarRight->left = (pCenter->left+pCenter->right) - pBarRight->right;

	pBarCenter->left = pBarLeft->left+pBarLeft->right;
	pBarCenter->right = pBarRight->left - pBarCenter->left;
	
	
	REAL32 fWMax = i3Math::Max(pBarRight->right, pBarLeft->right);
	fWMax = i3Math::Max(fWMax, pBarCenter->right);
	
	REAL32 fHMax = i3Math::Max(pBarRight->bottom, pBarLeft->bottom);
	fHMax = i3Math::Max(fHMax, pBarCenter->bottom);
	
	if (fHMax > pCenter->bottom)
		fHMax = pCenter->bottom;

	REAL32 fTop = pCenter->bottom*0.5f - fHMax*0.5f;
	
	pBarCenter->top = fTop;
	pBarCenter->bottom = fHMax;
	pBarLeft->top = fTop;
	pBarLeft->bottom = fHMax;
	pBarRight->top = fTop;
	pBarRight->bottom = fHMax;

	return true;
}

i3ClassMeta	* i3UITemplate_ProgressBarSplit::GetInstanceMeta()
{
	return i3UIProgressBarSplit::static_meta();
}

void i3UITemplate_ProgressBarSplit::SpreadLayout(REAL32 * pCX, REAL32 * pCY, i3::pack::RECT * pRects, INT32 shapeCount)
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