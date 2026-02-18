#if !defined( __I3_SPRITE2D_H)
#define __I3_SPRITE2D_H

#include "i3Gfx.h"
#include "i3AttrSet.h"
#include "i3TextureBindAttr.h"
#include "i3BlendEnableAttr.h"
#include "i3BlendModeAttr.h"
#include "i3TextureWrapAttr.h"
#include "i3TextureEnableAttr.h"
#include "i3AlphaTestEnableAttr.h"
#include "i3Sprite2DAttr.h"
#include "i3Geometry.h"

//
// i3SpriteAttr을 손쉽게 쓰기위해서 SceneGraph를 미리 구성하여둔다.
// 
//		i3Sprite2D
//			|
//			+- i3BlendModeAttr
//			|
//			+- i3TextureBindAttr
//			|
//			+- i3Geometry
//				|
//				+- i3SpriteAttr
//		


class I3_EXPORT_SCENE i3Sprite2D : public i3AttrSet
{
	I3_CLASS_DEFINE( i3Sprite2D);
protected:

	INT32					m_Count;
	I3G_BLEND				m_SrcBlend;
	I3G_BLEND				m_DestBlend;

	i3Sprite2DAttr*			m_pSprite;
	i3Texture*				m_pTexture;
	i3Geometry*				m_pGeomery;
	i3TextureEnableAttr *	m_pTexEnableAttr;
	i3TextureBindAttr*		m_pTextureBindAttr;
	i3BlendEnableAttr *		m_pBlendEnableAttr;
	i3BlendModeAttr*		m_pBlendModeAttr;
	i3AlphaTestEnableAttr *	m_pAlphaTestAttr;

public:
	i3Sprite2D(void);
	virtual ~i3Sprite2D(void);

	void		Create( UINT32 Count = 0, BOOL bUV = TRUE, BOOL bColor = TRUE, BOOL bUseVertexBank = FALSE);
	void		SetRect( UINT32 Index, RT_REAL32 x, RT_REAL32 y, RT_REAL32 width, RT_REAL32 height);

	void		SetCount(INT32 count);
	INT32		GetCount(void)												{ return m_Count;				}
	void		SetTexture(i3Texture * pTex);
	void		SetTexture(const char * TextureName);

	void		SetEnable( BOOL bFlag)										{ m_pSprite->SetEnable( bFlag);}
	void		SetEnable( UINT32 Index, BOOL bFlag)						{ m_pSprite->SetEnable( Index, bFlag);}
	BOOL		GetEnable( UINT32 Index)									{ return m_pSprite->GetEnable( Index);}

	void		SetCenter( UINT32 Index, RT_VEC3D * pVec)					{ m_pSprite->SetCenter( Index, pVec);}
	void		SetCenter( UINT32 Index, RT_REAL32 x, RT_REAL32 y, RT_REAL32 z)	{ m_pSprite->SetCenter( Index, x, y, z);}
	RT_VEC3D *	GetCenter( UINT32 Index)									{ return m_pSprite->GetCenter( Index);}

	void		GetLeftTop( UINT32 Index, RT_VEC3D * pVec)					{ m_pSprite->GetLeftTop( Index, pVec);}
	void		SetLeftTop( UINT32 Index, RT_VEC3D * pVec)					{ m_pSprite->SetLeftTop( Index, pVec);}
	void		GetRightBottom( UINT32 Index, RT_VEC3D * pVec)				{ m_pSprite->GetRightBottom( Index, pVec);}

	void		SetRotation( UINT32 Index,RT_REAL32 rotation)				{ m_pSprite->SetRotation( Index,rotation);}
	void		SetFlip( UINT32 Index, BOOL FlipX, BOOL FlipY )				{ m_pSprite->SetFlip( Index, FlipX, FlipY ); }
	BOOL		IsFlipX( UINT32 Index )										{ return m_pSprite->IsFlipX( Index ); }
	BOOL		IsFlipY( UINT32 Index )										{ return m_pSprite->IsFlipY( Index ); }

	void		SetSize( UINT32 Index, RT_VEC2D* pVec)						{ m_pSprite->SetSize( Index, pVec);}
	void		SetSize( UINT32 Index, RT_REAL32 Width, RT_REAL32 Height)	{ m_pSprite->SetSize( Index, Width, Height);}
	RT_VEC2D *	GetSize( UINT32 Index)										{ return m_pSprite->GetSize( Index);}

	void		SetTextureCoord( UINT32 Index, RT_VEC2D * pMin, RT_VEC2D * pMax);
	void		SetTextureCoord( UINT32 Index, RT_REAL32 u1, RT_REAL32 v1, RT_REAL32 u2, RT_REAL32 v2);
	RT_VEC2D *	GetTextureCoord( UINT32 Index, UINT32 which)				{ return m_pSprite->GetTextureCoord( Index, which);}

	void		SetColor( COLOR * pColor)								{ m_pSprite->SetColor( pColor ); }
	void		SetColor( UINT32 Index, COLOR * pColor)					{ m_pSprite->SetColor( Index, pColor);}
	void		SetColor( UINT32 Index, UINT8 r,UINT8 g,UINT8 b,UINT8 a){ m_pSprite->SetColor( Index, r,g,b,a);}
	COLOR *		GetColor( UINT32 Index)									{ return m_pSprite->GetColor( Index);}

	void		SetSrcBlend( I3G_BLEND blend);
	I3G_BLEND	GetSrcBlend(void)								{ return m_SrcBlend; }

	void		SetDestBlend( I3G_BLEND blend);
	I3G_BLEND	GetDestBlend(void)								{ return m_DestBlend; }

	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif
