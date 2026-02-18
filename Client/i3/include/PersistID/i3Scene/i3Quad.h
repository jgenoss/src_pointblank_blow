#if !defined( __I3_QUAD_H)
#define __I3_QUAD_H

#include "i3Gfx.h"
#include "i3AttrSet.h"
#include "i3TextureEnableAttr.h"
#include "i3TextureBindAttr.h"
#include "i3BlendEnableAttr.h"
#include "i3BlendModeAttr.h"
#include "i3TextureWrapAttr.h"
#include "i3AlphaTestEnableAttr.h"
#include "i3QuadAttr.h"
#include "i3Geometry.h"

//
// i3QuadAttr을 손쉽게 쓰기위해서 SceneGraph를 미리 구성하여둔다.
// 
//		i3Quad
//			|
//			+- i3BlendModeAttr
//			|
//			+- i3TextureBindAttr
//			|
//			+- i3Geometry
//				|
//				+- i3QuadAttr
//		


class I3_EXPORT_SCENE i3Quad : public i3AttrSet
{
	I3_CLASS_DEFINE( i3Quad);
protected:

	INT32					m_Count;
	I3SG_QUAD_SPACE			m_Space;
	BOOL					m_bAlwaysUpdate;
	I3G_BLEND				m_SrcBlend;
	I3G_BLEND				m_DestBlend;

	i3QuadAttr*				m_pQuad;
	i3Texture*				m_pTexture;
	i3Geometry*				m_pGeomery;
	i3TextureEnableAttr *	m_pTexEnableAttr;
	i3TextureBindAttr*		m_pTextureBindAttr;
	i3BlendEnableAttr *		m_pBlendEnableAttr;
	i3BlendModeAttr*		m_pBlendModeAttr;
	i3AlphaTestEnableAttr *	m_pAlphaTestAttr;

public:
	i3Quad(void);
	virtual ~i3Quad(void);

	void		Create( UINT32 Count, I3SG_QUAD_SPACE Space, BOOL bUV = TRUE, BOOL bColor = TRUE, BOOL bAlwaysUpdate = TRUE);
	void		SetTexture(i3Texture * pTex);
	void		SetTexture(const char * TextureName);
	i3Texture*	GetTexture()											{ return m_pTexture; }

	void		SetEnable( UINT32 Index, BOOL bFlag)					{ m_pQuad->SetEnable( Index, bFlag);}
	BOOL		GetEnable( UINT32 Index)								{ return m_pQuad->GetEnable( Index);}

	void		SetRect( UINT32 Index, RT_REAL32 x, RT_REAL32 y, RT_REAL32 width, RT_REAL32 height);
	void 		GetRect( UINT32 Index, RT_VEC4D * pRect);
	void		SetCenter( UINT32 Index, RT_VEC3D * pVec)						{ m_pQuad->SetCenter( Index, pVec);}
	void		SetCenter( UINT32 Index, RT_REAL32 x, RT_REAL32 y, RT_REAL32 z)	{ m_pQuad->SetCenter( Index, x, y, z);}
	RT_VEC3D *	GetCenter( UINT32 Index)								{ return m_pQuad->GetCenter( Index);}

	void		SetRotation( UINT32 Index, QUAD_ROTATION Rot )			{ m_pQuad->SetRotation( Index, Rot); }
	void		SetFlip( UINT32 Index, BOOL FlipX, BOOL FlipY )			{ m_pQuad->SetFlip( Index, FlipX, FlipY ); }

	void		GetLeftTop( UINT32 Index, RT_VEC3D * pVec)				{ m_pQuad->GetLeftTop( Index, pVec);}
	void		GetRightBottom( UINT32 Index, RT_VEC3D * pVec)			{ m_pQuad->GetRightBottom( Index, pVec);}

	void		SetSize( UINT32 Index, RT_VEC2D* pVec)						{ m_pQuad->SetSize( Index, pVec);}
	void		SetSize( UINT32 Index, RT_REAL32 Width, RT_REAL32 Height)	{ m_pQuad->SetSize( Index, Width, Height);}
	void 		GetSize( UINT32 Index, RT_VEC2D * pVec)						{ m_pQuad->GetSize( Index, pVec);}

	void		SetTextureCoord( UINT32 Index, RT_VEC2D * pMin, RT_VEC2D * pMax);
	void		SetTextureCoord( UINT32 Index, RT_REAL32 u1, RT_REAL32 v1, RT_REAL32 u2, RT_REAL32 v2);
	RT_VEC2D *	GetTextureCoord( UINT32 Index, UINT32 which)			{ return m_pQuad->GetTextureCoord( Index, which);}

	void		SetColor( UINT32 Index, COLOR * pColor)					{ m_pQuad->SetColor( Index, pColor);}
	void		SetColor( UINT32 Index, UINT8 r,UINT8 g,UINT8 b,UINT8 a){ m_pQuad->SetColor( Index, r,g,b,a);}
	COLOR *		GetColor( UINT32 Index)									{ return m_pQuad->GetColor( Index);}

	void		SetInverseViewMatrix( RT_MATRIX * pMatrix)					{ m_pQuad->SetInverseViewMatrix( pMatrix); }

	void		SetSrcBlend( I3G_BLEND blend);
	I3G_BLEND	GetSrcBlend(void)								{ return m_SrcBlend; }

	void		SetDestBlend( I3G_BLEND blend);
	I3G_BLEND	GetDestBlend(void)								{ return m_DestBlend; }

	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif
