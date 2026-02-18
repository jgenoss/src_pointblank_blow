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

typedef struct ALIGN4 _tagBillboardInfo
{
	RT_VEC3D	m_Start;
	RT_VEC3D	m_End;
	RT_REAL32	m_Height;
	RT_VEC2D	m_UV[2];
	COLOR		m_Color;
	BOOL		m_bEnable;
	UINT32		m_EditMask;
} I3SG_BILLBOARD_INFO;

class I3_EXPORT_SCENE i3BillboardAttr : public i3RenderAttr
{
	I3_CLASS_DEFINE( i3BillboardAttr);
protected:
	// Persistent Members
	I3SG_BILLBOARD_SPACE	m_Space;
	UINT32					m_Count;
	UINT32					m_PrimCount;

	// Volatile Members
	i3VertexArray *			m_pVA;
	i3IndexArray *			m_pIA;
	i3Shader *				m_pShader;

	BOOL					m_bEditted;
	BOOL					m_bAlwaysUpdate;

	I3SG_BILLBOARD_INFO *	m_pBillboards;
	//INT32					m_BillboardsIdx; 

	RT_VEC3D				m_InvViewAt;

protected:

	void		ApplyAlwaysUpdate( i3RenderContext * pContext);
	void		ApplyEdittedUpdate( i3RenderContext * pContext);

public:
	i3BillboardAttr(void);
	virtual ~i3BillboardAttr(void);

	BOOL		Create( UINT32 Count, I3SG_BILLBOARD_SPACE Space, BOOL bUV = TRUE, BOOL bColor = FALSE, BOOL bAlwaysUpdate = FALSE);

	void		SetEnable( UINT32 Index, BOOL bFlag)
	{
		I3_BOUNDCHK( Index, m_Count);
		m_pBillboards[Index].m_bEnable = bFlag;
		m_pBillboards[Index].m_EditMask |= I3SG_BILLBOARD_EDIT_ENABLE;

		m_bEditted = TRUE;
	}

	BOOL		GetEnable( UINT32 Index)					{ I3_BOUNDCHK( Index, m_Count); return m_pBillboards[Index].m_bEnable; }

	/// Billboard의 시작점을 설정한다.
	void		SetStart(	UINT32 Index,	///< 시작점을 지정할 Billboard 0-based Index
							RT_VEC3D * pVec	///< 시작점 좌표
							)
	{
		I3_BOUNDCHK( Index, m_Count);
		i3Vector::Copy( & m_pBillboards[Index].m_Start, pVec);
		m_pBillboards[Index].m_EditMask |= I3SG_BILLBOARD_EDIT_POSITION;

		m_bEditted = TRUE;
	}

	/// Billboard의 시작점을 설정한다.
	void		SetStart(	UINT32 Index,	///< 시작점을 지정할 Billboard 0-based Index
							RT_REAL32 x, RT_REAL32 y, RT_REAL32 z)
	{
		I3_BOUNDCHK( Index, m_Count);
		i3Vector::Set( & m_pBillboards[Index].m_Start, x, y, z);
		m_pBillboards[Index].m_EditMask |= I3SG_BILLBOARD_EDIT_POSITION;

		m_bEditted = TRUE;
	}
	RT_VEC3D *		GetStart( UINT32 Index)					{ return & m_pBillboards[Index].m_Start; }

	void		SetEnd( UINT32 Index, RT_VEC3D * pVec)
	{
		I3_BOUNDCHK( Index, m_Count);
		i3Vector::Copy( &m_pBillboards[Index].m_End, pVec);
		m_pBillboards[Index].m_EditMask |= I3SG_BILLBOARD_EDIT_POSITION;

		m_bEditted = TRUE;
	}
	
	void		SetEnd( UINT32 Index, RT_REAL32 x, RT_REAL32 y, RT_REAL32 z)
	{
		I3_BOUNDCHK( Index, m_Count);
		i3Vector::Set( &m_pBillboards[Index].m_End, x, y, z);
		m_pBillboards[Index].m_EditMask |= I3SG_BILLBOARD_EDIT_POSITION;

		m_bEditted = TRUE;
	}

	RT_VEC3D *		GetEnd( UINT32 Index)					{ I3_BOUNDCHK( Index, m_Count); return & m_pBillboards[Index].m_End; }

	void		SetSize( UINT32 Index, RT_REAL32 Height)
	{
		I3_BOUNDCHK( Index, m_Count);
		m_pBillboards[Index].m_Height = Height;
		m_pBillboards[Index].m_EditMask |= I3SG_BILLBOARD_EDIT_POSITION;

		m_bEditted = TRUE;
	}
	RT_REAL32		GetSize( UINT32 Index)					{ I3_BOUNDCHK( Index, m_Count); return m_pBillboards[Index].m_Height; }

	void		SetTextureCoord( UINT32 Index, RT_VEC2D * pMin, RT_VEC2D * pMax)
	{
		I3_BOUNDCHK( Index, m_Count);
		i3Vector::Copy( & m_pBillboards[Index].m_UV[0], pMin);
		i3Vector::Copy( & m_pBillboards[Index].m_UV[1], pMax);
		m_pBillboards[Index].m_EditMask |= I3SG_BILLBOARD_EDIT_UV;

		m_bEditted = TRUE;
	}

	void		SetTextureCoord( UINT32 Index, RT_REAL32 u1, RT_REAL32 v1, RT_REAL32 u2, RT_REAL32 v2)
	{
		I3_BOUNDCHK( Index, m_Count);
		i3Vector::Set( & m_pBillboards[Index].m_UV[0], u1, v1);
		i3Vector::Set( & m_pBillboards[Index].m_UV[1], u2, v2);
		m_pBillboards[Index].m_EditMask |= I3SG_BILLBOARD_EDIT_UV;

		m_bEditted = TRUE;
	}

	RT_VEC2D *		GetTextureCoord( UINT32 Index, UINT32 which)	{ I3_BOUNDCHK( Index, m_Count); return & m_pBillboards[Index].m_UV[which]; }

	void		SetColor( UINT32 Index, COLOR * pColor)
	{
		I3_BOUNDCHK( Index, m_Count);
		i3Color::Set( & m_pBillboards[Index].m_Color, pColor);
		m_pBillboards[Index].m_EditMask |= I3SG_BILLBOARD_EDIT_COLOR;

		m_bEditted = TRUE;
	}

	void		SetColor( UINT32 Index, UINT8 r,UINT8 g,UINT8 b,UINT8 a)
	{
		I3_BOUNDCHK( Index, m_Count);
		i3Color::Set( & m_pBillboards[Index].m_Color, r, g, b, a);
		m_pBillboards[Index].m_EditMask |= I3SG_BILLBOARD_EDIT_COLOR;

		m_bEditted = TRUE;
	}

	void		SetColor( UINT32 Index, UINT32 rgba)
	{
		I3_BOUNDCHK( Index, m_Count);
		((I3UNIONCOLOR *) &m_pBillboards[Index].m_Color)->value = rgba;
		m_pBillboards[Index].m_EditMask |= I3SG_BILLBOARD_EDIT_COLOR;

		m_bEditted = TRUE;
	}

	COLOR *		GetColor( UINT32 Index)							{ I3_BOUNDCHK( Index, m_Count); return & m_pBillboards[Index].m_Color; }

	void		SetInverseViewMatrix( RT_MATRIX * pMatrix)
	{
		#if defined( I3_FIXED_POINT)
			RT_VEC3D at;

			i3Vector::Copy( &at, i3Matrix::GetAt( pMatrix));
			i3Vector::Normalize( &m_InvViewAt, &at);
		#else
			i3Vector::Normalize( &m_InvViewAt, i3Matrix::GetAt( pMatrix));
		#endif
	}

	virtual	void	Apply( i3RenderContext * pContext);
	virtual void	CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method);
	virtual BOOL	IsSame( i3RenderAttr * pAttr);

	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif
