#include "i3FrameworkPCH.h"
#include "i3UIControl.h"

/*void i3UIControl::ResizeShapeForFixedPixel(	REAL32 cx, REAL32 cy,
											INT32 idxTopLeft, INT32 idxTop, INT32 idxTopRight, 
											INT32 idxLeft, INT32 idxCenter, INT32 idxRight,
											INT32 idxBottomLeft, INT32 idxBottom, INT32 idxBottomRight,
											INT32 idxDisable)
{
	i3UIShape * pTopLeft, *pTop, *pTopRight;
	i3UIShape * pLeft, *pCenter, *pRight;
	i3UIShape * pBottomLeft, *pBottom, *pBottomRight;
	i3UIShape * pDisableCenter;
	REAL32 x2, y2;

	x2 = cx;
	y2 = cy;

	if( idxTopLeft != -1)
		pTopLeft		= getShape( idxTopLeft);
	else
		pTopLeft		= nullptr;

	if( idxTop != -1)
		pTop			= getShape( idxTop);
	else
		pTop			= nullptr;

	if( idxTopRight != -1)
		pTopRight		= getShape( idxTopRight);
	else
		pTopRight		= nullptr;

	if( idxLeft != -1)
		pLeft			= getShape( idxLeft);
	else
		pLeft			= nullptr;

	if( idxCenter != -1)
		pCenter			= getShape( idxCenter);
	else
		pCenter			= nullptr;

	if( idxRight != -1)
		pRight			= getShape( idxRight);
	else
		pRight			= nullptr;


	if( idxBottomLeft != -1)
		pBottomLeft		= getShape( idxBottomLeft);
	else
		pBottomLeft		= nullptr;

	if( idxBottom != -1)
		pBottom			= getShape( idxBottom);
	else
		pBottom			= nullptr;

	if( idxBottomRight != -1)
		pBottomRight	= getShape( idxBottomRight);
	else
		pBottomRight	= nullptr;

	if( idxDisable != -1)
		pDisableCenter	= getShape( idxDisable);
	else
		pDisableCenter	= nullptr;

	// Top-Left (변경될 필요없다.)

	// TopRight (위치만 조정)
	if( pTopRight != nullptr)
	{
		pTopRight->setPosX(	cx - pTopRight->getWidth());
	}

	// Top (수평 크기만 조정)
	if( pTop != nullptr)
	{
		if( pTopRight != nullptr)
			pTop->setWidth( pTopRight->getPosX() - pTop->getPosX());
		else
			pTop->setWidth( cx - pTop->getPosX());
	}

	// Bottom-Left (위치만 조정)
	if( pBottomLeft != nullptr)
	{
		pBottomLeft->setPosY( cy - pBottomLeft->getHeight());
	}

	// Left (수직 크기만 조정)
	if( pLeft != nullptr)
	{
		if( pBottomLeft != nullptr)
			pLeft->setHeight( pBottomLeft->getPosY() - pLeft->getPosY());
		else
			pLeft->setHeight( cy - pLeft->getPosX());
	}

	// Bottom-Right (위치만 조정)
	if( pBottomRight != nullptr)
	{
		pBottomRight->setPosX( cx - pBottomRight->getWidth());
		pBottomRight->setPosY( cy - pBottomRight->getHeight());
	}

	// Right (수직 크기만 조정)
	if( pRight != nullptr)
	{
		pRight->setPosX( cx - pRight->getWidth());

		if( pBottomRight != nullptr)
			pRight->setHeight( pBottomRight->getPosY() - pRight->getPosY());
		else
			pRight->setHeight( cy - pRight->getPosY());

		x2 = pRight->getPosX();
	}

	// Bottom (세로 위치, 수평 크기 조정)
	if( pBottom != nullptr)
	{
		pBottom->setPosY( cy - pBottom->getHeight());

		if( pBottomRight != nullptr)
			pBottom->setWidth( pBottomRight->getPosX() - pBottom->getPosX());
		else
			pBottom->setWidth( cx - pBottom->getPosX());

		y2 = pBottom->getPosY();
	}

	// Center (크기만 조정)
	if( pCenter != nullptr)
	{
		pCenter->setWidth( x2 - pCenter->getPosX());
		pCenter->setHeight( y2 - pCenter->getPosY());

		if( pDisableCenter != nullptr)
		{
			pCenter->setRect( pCenter->getRect());
		}
	}
}
*/