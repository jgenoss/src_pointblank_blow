// i3BillboardAttr.h
#if !defined( I3_BILLBOARD_ATTR_H)
#define I3_BILLBOARD_ATTR_H

#include "i3RenderAttr.h"

enum I3SG_BILLBOARD_SPACE
{
	I3SG_BILLBOARD_SPACE_WORLD		=	0,
	I3SG_BILLBOARD_SPACE_SCREEN	=	1,
};

#define I3SG_BILLBOARD_EDIT_POSITION			0x00000001
#define I3SG_BILLBOARD_EDIT_UV				0x00000002
#define I3SG_BILLBOARD_EDIT_COLOR			0x00000004
#define I3SG_BILLBOARD_EDIT_AXIS			0x00000010
#define I3SG_BILLBOARD_EDIT_ENABLE			0x00000008

struct ALIGN4 I3SG_BILLBOARD_INFO
{
	VEC3D	m_Start;
	VEC3D	m_End;
	REAL32	m_Height = 0.0f;
	VEC2D	m_UV[2];
	COLOR	m_Color;
	bool	m_bEnable = false;
	UINT32	m_EditMask = 0;
};

class I3_EXPORT_SCENE i3BillboardAttr : public i3RenderAttr
{
	I3_EXPORT_CLASS_DEFINE( i3BillboardAttr, i3RenderAttr);
protected:
	// Persistent Members
	I3SG_BILLBOARD_SPACE	m_Space = I3SG_BILLBOARD_SPACE_WORLD;
	UINT32					m_Count = 0;
	UINT32					m_PrimCount = 0;

	// Volatile Members
	i3VertexArray *			m_pVA = nullptr;
	i3IndexArray *			m_pIA = nullptr;
	i3Shader *				m_pShader = nullptr;
	i3VertexFormat			m_Format;

	bool					m_bEditted = true;
	bool					m_bAlwaysUpdate = false;

	I3SG_BILLBOARD_INFO *	m_pBillboards = nullptr;

	VEC3D				m_InvViewAt;
	bool					m_bRotateUV = false;

protected:
	void		_CreateVertex(void);
	void		ApplyAlwaysUpdate( i3RenderContext * pContext);
	void		ApplyEdittedUpdate( i3RenderContext * pContext);

public:
	i3BillboardAttr(void);
	virtual ~i3BillboardAttr(void);

	bool		Create( UINT32 Count, I3SG_BILLBOARD_SPACE Space, bool bUV = true, bool bColor = false, bool bAlwaysUpdate = false);

	bool		getRotateUVState(void)				{ return m_bRotateUV; }
	void		setRotateUVState( bool bState)		{ m_bRotateUV = bState; }

	void		SetEnable( UINT32 Index, bool bFlag)
	{
		I3ASSERT( Index < m_Count);
		m_pBillboards[Index].m_bEnable = bFlag;
		m_pBillboards[Index].m_EditMask |= I3SG_BILLBOARD_EDIT_ENABLE;

		m_bEditted = true;
	}

	bool		GetEnable( UINT32 Index)					{ I3ASSERT( Index < m_Count); return m_pBillboards[Index].m_bEnable; }

	/// Billboard의 시작점을 설정한다.
	void		SetStart(	UINT32 Index,	///< 시작점을 지정할 Billboard 0-based Index
							VEC3D * pVec	///< 시작점 좌표
							)
	{
		I3ASSERT( Index < m_Count);
		i3Vector::Copy( & m_pBillboards[Index].m_Start, pVec);
		m_pBillboards[Index].m_EditMask |= I3SG_BILLBOARD_EDIT_POSITION;

		m_bEditted = true;
	}

	/// Billboard의 시작점을 설정한다.
	void		SetStart(	UINT32 Index,	///< 시작점을 지정할 Billboard 0-based Index
							REAL32 x, REAL32 y, REAL32 z)
	{
		I3ASSERT( Index < m_Count);
		i3Vector::Set( & m_pBillboards[Index].m_Start, x, y, z);
		m_pBillboards[Index].m_EditMask |= I3SG_BILLBOARD_EDIT_POSITION;

		m_bEditted = true;
	}
	VEC3D *		GetStart( UINT32 Index)					{ return & m_pBillboards[Index].m_Start; }

	void		SetEnd( UINT32 Index, VEC3D * pVec)
	{
		I3ASSERT( Index < m_Count);
		i3Vector::Copy( &m_pBillboards[Index].m_End, pVec);
		m_pBillboards[Index].m_EditMask |= I3SG_BILLBOARD_EDIT_POSITION;

		m_bEditted = true;
	}
	
	void		SetEnd( UINT32 Index, REAL32 x, REAL32 y, REAL32 z)
	{
		I3ASSERT( Index < m_Count);
		i3Vector::Set( &m_pBillboards[Index].m_End, x, y, z);
		m_pBillboards[Index].m_EditMask |= I3SG_BILLBOARD_EDIT_POSITION;

		m_bEditted = true;
	}

	VEC3D *		GetEnd( UINT32 Index)					{ I3ASSERT( Index < m_Count); return & m_pBillboards[Index].m_End; }

	void		SetSize( UINT32 Index, REAL32 Height)
	{
		I3ASSERT( Index < m_Count);
		m_pBillboards[Index].m_Height = Height;
		m_pBillboards[Index].m_EditMask |= I3SG_BILLBOARD_EDIT_POSITION;

		m_bEditted = true;
	}
	REAL32		GetSize( UINT32 Index)					{ I3ASSERT( Index < m_Count); return m_pBillboards[Index].m_Height; }

	void		SetTextureCoord( UINT32 Index, VEC2D * pMin, VEC2D * pMax)
	{
		I3ASSERT( Index < m_Count);
		i3Vector::Copy( & m_pBillboards[Index].m_UV[0], pMin);
		i3Vector::Copy( & m_pBillboards[Index].m_UV[1], pMax);
		m_pBillboards[Index].m_EditMask |= I3SG_BILLBOARD_EDIT_UV;

		m_bEditted = true;
	}

	void		SetTextureCoord( UINT32 Index, REAL32 u1, REAL32 v1, REAL32 u2, REAL32 v2)
	{
		I3ASSERT( Index < m_Count);
		i3Vector::Set( & m_pBillboards[Index].m_UV[0], u1, v1);
		i3Vector::Set( & m_pBillboards[Index].m_UV[1], u2, v2);
		m_pBillboards[Index].m_EditMask |= I3SG_BILLBOARD_EDIT_UV;

		m_bEditted = true;
	}

	VEC2D *		GetTextureCoord( UINT32 Index, UINT32 which)	{ I3ASSERT( Index < m_Count); return & m_pBillboards[Index].m_UV[which]; }

	void		SetColor( UINT32 Index, COLOR * pColor)
	{
		I3ASSERT( Index < m_Count);
		i3Color::Set( & m_pBillboards[Index].m_Color, pColor);
		m_pBillboards[Index].m_EditMask |= I3SG_BILLBOARD_EDIT_COLOR;

		m_bEditted = true;
	}

	void		SetColor( UINT32 Index, UINT8 r,UINT8 g,UINT8 b,UINT8 a)
	{
		I3ASSERT( Index < m_Count);
		i3Color::Set( & m_pBillboards[Index].m_Color, r, g, b, a);
		m_pBillboards[Index].m_EditMask |= I3SG_BILLBOARD_EDIT_COLOR;

		m_bEditted = true;
	}

	void		SetColor( UINT32 Index, UINT32 rgba)
	{
		I3ASSERT( Index < m_Count);
		((I3UNIONCOLOR *) &m_pBillboards[Index].m_Color)->value = rgba;
		m_pBillboards[Index].m_EditMask |= I3SG_BILLBOARD_EDIT_COLOR;

		m_bEditted = true;
	}

	COLOR *		GetColor( UINT32 Index)							{ I3ASSERT( Index < m_Count); return & m_pBillboards[Index].m_Color; }

	void		SetInverseViewMatrix( MATRIX * pMatrix)
	{
		#if defined( I3_FIXED_POINT)
			VEC3D at;

			i3Vector::Copy( &at, i3Matrix::GetAt( pMatrix));
			i3Vector::Normalize( &m_InvViewAt, &at);
		#else
			i3Vector::Normalize( &m_InvViewAt, i3Matrix::GetAt( pMatrix));
		#endif
	}

	virtual	void	Apply( i3RenderContext * pContext) override;
	virtual void	CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method) override;

	virtual void	OnLostDevice( bool bLostDevice) override;
	virtual void	OnRevive( i3RenderContext * pCtx) override;

	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
};

#endif
