#include "i3FrameworkPCH.h"
#include "i3UITemplate_CheckButton.h"

static const char* s_pszCheckButtonInfoString[ I3UI_CHECKBUTTON_SHAPE_COUNT] = 
{
	"활성, UnChecked",			
	"활성, Checked",			
	
	"활성, UnChecked, OnMouse",			
	"활성, Checked, OnMouse",			

	"비활성, UnChecked",		
	"비활성, Checked",	

	"캡션",
};

I3_CLASS_INSTANCE( i3UITemplate_CheckButton);

i3UIShape* i3UITemplate_CheckButton::getShape(INT32 nIndex)	
{
	I3_BOUNDCHK( nIndex, getShapeCount() );
	return &m_pShape[nIndex];
}

const char * i3UITemplate_CheckButton::getShapeInfoString(INT32 nIndex)
{
	I3_BOUNDCHK( nIndex, getShapeCount() );
	return s_pszCheckButtonInfoString[nIndex];
}

bool i3UITemplate_CheckButton::GetControlRect( i3::pack::RECT * pRect)
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

void i3UITemplate_CheckButton::InitializeLayout(void)
{
}

bool i3UITemplate_CheckButton::AdjustLayout(REAL32 cx, REAL32 cy, i3::pack::RECT * pRects, INT32 shapeCount)
{
	INT32 totalCount = getShapeCount();

	if( shapeCount > 0)
		totalCount = MIN( totalCount, shapeCount);

	for( INT32 i=0; i< totalCount; i++)
	{
		i3::pack::RECT * pRect = _getTargetRect( i, pRects);

		pRect->top = 0.0f;
		pRect->bottom = cy;

		if( i == I3UI_CHECKBUTTON_SHAPE_CAPTION)
		{
			if( m_bRightText)
			{
				pRect->left = cy + m_nCaptionOffset;			
			}
			else
			{
				pRect->left = 0.0f;
			}

			pRect->right = cx - cy - m_nCaptionOffset;	//width
		}
		else
		{
			if( m_bRightText)
			{
				pRect->left = 0.0f;
			}
			else
			{
				pRect->left = cx - cy;
			}

			if( m_bFreeSize == true)
				pRect->right = cx;
			else
				pRect->right = cy;
		}
	}

	//I3TRACE("i3UITemplate_CheckButton::AdjustLayout\n");

	return true;
}

void i3UITemplate_CheckButton::SpreadLayout(REAL32 * pCX, REAL32 * pCY, i3::pack::RECT * pRects, INT32 shapeCount)
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
		
		if( i%2 == 1)
		{
			left = 0.0f;
			top += pRect->bottom + I3UI_SPREAD_MARGIN;
		}
		
		*pCY = i3Math::Max( pRect->bottom, *pCX);
	}
}

bool i3UITemplate_CheckButton::GetTextArea( i3::pack::RECT* pIn, i3::pack::RECT* pOut)
{
	const i3::pack::RECT* pRect;

	if( pIn != nullptr)
	{
		pRect = &pIn[I3UI_CHECKBUTTON_SHAPE_CAPTION];
		//pRect = &pIn[I3UI_CHECKBUTTON_SHAPE_ENABLED_UNCHECKED];
	}
	else
	{
		pRect = getShape( I3UI_CHECKBUTTON_SHAPE_CAPTION)->getRect();
		//pRect = getShape( I3UI_CHECKBUTTON_SHAPE_ENABLED_UNCHECKED)->getRect();
	}

	//i3::pack::RECT	crt;	// Control Rect
	//GetControlRect( &crt);
	//m_nCaptionWidth = crt.right - pRect->right - m_nCaptionOffset;

	*pOut = *pRect;
	
	//pOut->left = pOut->left + pOut->right + m_nCaptionOffset;	
	//pOut->right = (REAL32)m_nCaptionWidth;					
		
	return true;
}

void	i3UITemplate_CheckButton::Create()
{
	i3UITemplate::Create();

	// 랙트값 초기화
	AdjustLayout( (REAL32)(m_nCaptionHeight + m_nCaptionWidth + m_nCaptionOffset), (REAL32)(m_nCaptionHeight) );
}
