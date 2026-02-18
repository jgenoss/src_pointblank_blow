#if !defined( __I3_RECT_COORD_SOURCE_H)
#define __I3_RECT_COORD_SOURCE_H

#include "i3Vector.h"

///
/// 사각 영역에 대한 좌표값을 제공하는 Base Class.
/// virtual GetRect() 함수를 통해, 사각 영역을 반환하는 다양한 Class들의
/// Base가 된다.
class i3RectCoordSource : public i3PersistantElement
{
	I3_CLASS_DEFINE( i3RectCoordSource);
protected:
	VEC2D	m_vStart;
	REAL32	m_Width;
	REAL32	m_Height;

public:
	i3RectCoordSource(void);
	virtual ~i3RectCoordSource(void);

	void	SetStart( VEC2D * pVec)			{ i3Vector::Copy( &m_vStart, pVec); }
	VEC2D *	GetStart(void)					{ return &m_vStart; }

	void	SetX( REAL32 x)					{ i3Vector::SetX( &m_vStart, x); }
	REAL32	GetX(void)						{ return i3Vector::GetX( &m_vStart); }

	void	SetY( REAL32 y)					{ i3Vector::SetY( &m_vStart, y); }
	REAL32	GetY(void)						{ return i3Vector::GetY( &m_vStart); }

	void	SetWidth( REAL32 w)				{ m_Width = w; }
	REAL32	GetWidth(void)					{ return m_Width; }

	void	SetHeight( REAL32 h)			{ m_Height = h; }
	REAL32	GetHeight(void)					{ return m_Height; }

	virtual void	GetRect( I3RECT * pRect); 

	virtual	UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);

};

#endif
