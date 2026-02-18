// i3QuadAttr.h
#if !defined( I3_QUAD_ATTR_H)
#define I3_QUAD_ATTR_H

#include "i3RenderAttr.h"

enum I3SG_QUAD_SPACE
{
	I3SG_QUAD_SPACE_WORLD		=	0,
	I3SG_QUAD_SPACE_SCREEN	=	1,
};

#define I3SG_QUAD_EDIT_POSITION			0x00000001
#define I3SG_QUAD_EDIT_UV				0x00000002
#define I3SG_QUAD_EDIT_COLOR			0x00000004
#define I3SG_QUAD_EDIT_ROTATION			0x00000010

enum QUAD_ROTATION
{
	QUAD_ROT_0,
	QUAD_ROT_90,
	QUAD_ROT_180,
	QUAD_ROT_270,
};

typedef struct ALIGN4 _tagQUADInfo
{
	BOOL			m_bEnable;
	RT_VEC3D			m_Center;
	RT_REAL32			m_Width;
	RT_REAL32			m_Height;
	COLOR			m_Color;
	QUAD_ROTATION	m_Rotation;
	RT_VEC2D			m_UV[2];
	BOOL			m_bFlipX;
	BOOL			m_bFlipY;
	UINT32			m_EditMask;
} I3SG_QUAD_INFO;

class I3_EXPORT_SCENE i3QuadAttr : public i3RenderAttr
{
	I3_CLASS_DEFINE( i3QuadAttr);
protected:
	I3SG_QUAD_SPACE			m_Space;
	UINT32					m_Count;
	i3VertexArray *			m_pVA;
	i3IndexArray *			m_pIA;
	UINT32					m_PrimCount;
	i3Shader *				m_pShader;

	BOOL					m_bEditted;
	BOOL					m_bAlwaysUpdate;

	//INT32					m_QuadsIdx; 
	I3SG_QUAD_INFO *		m_pQuads;

	RT_MATRIX					m_InvViewMatrix;
	UINT32					m_nBufferIndex;

protected:

	void		ApplyAlwaysUpdate( i3RenderContext * pContext);
	void		ApplyEdittedUpdate( i3RenderContext * pContext);

public:
	i3QuadAttr(void);
	virtual ~i3QuadAttr(void);

	BOOL		Create( UINT32 Count, I3SG_QUAD_SPACE Space, BOOL bUV = TRUE, BOOL bColor = FALSE, BOOL bAlwaysUpdate = FALSE);

	i3VertexArray *	GetVertexArray(void)					{ return m_pVA; }

	void		SetEnable( UINT32 Index, BOOL bFlag);
	BOOL		GetEnable( UINT32 Index)					{ return m_pQuads[Index].m_bEnable; }

	void		SetCenter( UINT32 Index, RT_VEC3D * pVec);
	void		SetCenter( UINT32 Index, RT_REAL32 x, RT_REAL32 y, RT_REAL32 z);
	RT_VEC3D *	GetCenter( UINT32 Index)					{ return & m_pQuads[Index].m_Center; }

	void		GetLeftTop( UINT32 Index, RT_VEC3D * pVec);
	void		GetRightBottom( UINT32 Index, RT_VEC3D * pVec);

	void		SetRotation( UINT32 Index, QUAD_ROTATION Rot );
	void		SetFlip( UINT32 Index, BOOL FlipX, BOOL FlipY );

	void		SetSize( UINT32 Index, RT_VEC2D* pVec);
	void		SetSize( UINT32 Index, RT_REAL32 Width, RT_REAL32 Height);
	void 		GetSize( UINT32 Index, RT_VEC2D * pVec);

	void		SetTextureCoord( UINT32 Index, RT_VEC2D * pMin, RT_VEC2D * pMax);
	void		SetTextureCoord( UINT32 Index, RT_REAL32 u1, RT_REAL32 v1, RT_REAL32 u2, RT_REAL32 v2);
	RT_VEC2D *	GetTextureCoord( UINT32 Index, UINT32 which)	{ return & m_pQuads[Index].m_UV[which]; }

	void		SetColor( UINT32 Index, COLOR * pColor);
	void		SetColor( UINT32 Index, UINT8 r,UINT8 g,UINT8 b,UINT8 a);
	void		SetColor( UINT32 Index, UINT32 rgba);
	COLOR *		GetColor( UINT32 Index)							{ return & m_pQuads[Index].m_Color; }

	void		SetInverseViewMatrix( RT_MATRIX* pMat)
	{
		i3Matrix::Copy( &m_InvViewMatrix, pMat);
	}

	virtual	void	Apply( i3RenderContext * pContext);
	virtual void	CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method);
	virtual BOOL	IsSame( i3RenderAttr * pAttr);

	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif
