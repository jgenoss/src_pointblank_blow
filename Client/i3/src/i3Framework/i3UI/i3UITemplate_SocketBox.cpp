#include "i3FrameworkPCH.h"
#include "i3UITemplate_SocketBox.h"
#include "i3UISocketBox.h"

static const char * s_pszSocketBoxInfoString[I3UI_SOCKET_SHAPE_COUNT] = 
{
	"0: 선택되지 않은 상태",
	"1: 선택된 상태",
	"2: OnMouse 상태",
	"4: Disable 상태"
};

I3_CLASS_INSTANCE( i3UITemplate_SocketBox);


const char * i3UITemplate_SocketBox::getShapeInfoString(INT32 nIndex)
{
	I3_BOUNDCHK( nIndex, I3UI_SOCKET_SHAPE_COUNT);

	return s_pszSocketBoxInfoString[nIndex];
}

const char * i3UITemplate_SocketBox::getComment(void)
{
	return "SocketBox";
}

const char * i3UITemplate_SocketBox::getTemplateName(void)
{
	return "SocketBox";
}

I3UI_CATEGORY i3UITemplate_SocketBox::getTemplateCategory(void)
{
	return I3UI_CATEGORY_SOCKET;
}

i3ClassMeta * i3UITemplate_SocketBox::GetInstanceMeta()
{
	return i3UISocketBox::static_meta();
}

bool i3UITemplate_SocketBox::GetControlRect( i3::pack::RECT * pRect)
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

void i3UITemplate_SocketBox::InitializeLayout(void)
{
	AdjustLayout( 200.0f, 50.0f);
}

bool i3UITemplate_SocketBox::AdjustLayout(REAL32 cx, REAL32 cy, i3::pack::RECT * pRects, INT32 shapeCount)
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

void i3UITemplate_SocketBox::SpreadLayout(REAL32 * pCX, REAL32 * pCY, i3::pack::RECT * pRects, INT32 shapeCount)
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
		*pCY = i3Math::Max( pRect->bottom, *pCX);
	}
}
