#if !defined( __I3_SPRITE2D_H)
#define __I3_SPRITE2D_H

#include "i3Gfx.h"
#include "i3AttrSet.h"
#include "i3TextureBindAttr.h"
#include "i3BlendEnableAttr.h"
#include "i3BlendModeAttr.h"
#include "i3TextureWrapAttr.h"
#include "i3TextureEnableAttr.h"
#include "i3TextureFilterAttr.h"
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
	I3_EXPORT_CLASS_DEFINE( i3Sprite2D, i3AttrSet);
protected:

	INT32					m_Count = 0;
	I3G_BLEND				m_SrcBlend = I3G_BLEND_ZERO;
	I3G_BLEND				m_DestBlend = I3G_BLEND_ZERO;

	i3Sprite2DAttr*			m_pSprite = nullptr;
	i3Texture*				m_pTexture = nullptr;
	i3Geometry*				m_pGeomery;				// [initialize at constructor]
	i3TextureEnableAttr *	m_pTexEnableAttr;		// [initialize at constructor]
	i3TextureBindAttr*		m_pTextureBindAttr;		// [initialize at constructor]
	i3BlendEnableAttr *		m_pBlendEnableAttr;		// [initialize at constructor]
	i3BlendModeAttr*		m_pBlendModeAttr;		// [initialize at constructor]
	i3AlphaTestEnableAttr *	m_pAlphaTestAttr;		// [initialize at constructor]
	i3TextureFilterAttr *	m_pTexFilterAttr = nullptr;

public:
	i3Sprite2D(void);
	virtual ~i3Sprite2D(void);

	void		Create( UINT32 Count = 0, bool bUV = true, bool bColor = true, bool bUseVertexBank = false);
	void		SetRect( UINT32 Index, REAL32 x, REAL32 y, REAL32 width, REAL32 height);

	void		SetCount(INT32 count);
	INT32		GetCount(void)												{ return m_Count;				}
	void		SetTexture(i3Texture * pTex);
	void		SetTexture(const char * TextureName);

	void		SetEnable( bool bFlag)										{ m_pSprite->SetEnable( bFlag);}
	void		SetEnable( UINT32 Index, bool bFlag)						{ m_pSprite->SetEnable( Index, bFlag);}
	bool		GetEnable( UINT32 Index)									{ return m_pSprite->GetEnable( Index);}

	void		SetCenter( UINT32 Index, VEC3D * pVec)					{ m_pSprite->SetCenter( Index, pVec);}
	void		SetCenter( UINT32 Index, REAL32 x, REAL32 y, REAL32 z)	{ m_pSprite->SetCenter( Index, x, y, z);}
	VEC3D *	GetCenter( UINT32 Index)									{ return m_pSprite->GetCenter( Index);}

	void		GetLeftTop( UINT32 Index, VEC3D * pVec)					{ m_pSprite->GetLeftTop( Index, pVec);}
	void		SetLeftTop( UINT32 Index, VEC3D * pVec)					{ m_pSprite->SetLeftTop( Index, pVec);}
	void		GetRightBottom( UINT32 Index, VEC3D * pVec)				{ m_pSprite->GetRightBottom( Index, pVec);}

	void		SetRotation( UINT32 Index,REAL32 rotation)				{ m_pSprite->SetRotation( Index,rotation);}
	void		SetFlip( UINT32 Index, bool FlipX, bool FlipY )				{ m_pSprite->SetFlip( Index, FlipX, FlipY ); }
	bool		IsFlipX( UINT32 Index )										{ return m_pSprite->IsFlipX( Index ); }
	bool		IsFlipY( UINT32 Index )										{ return m_pSprite->IsFlipY( Index ); }

	void		SetSize( UINT32 Index, VEC2D* pVec)						{ m_pSprite->SetSize( Index, pVec);}
	void		SetSize( UINT32 Index, REAL32 Width, REAL32 Height)	{ m_pSprite->SetSize( Index, Width, Height);}
	VEC2D *	GetSize( UINT32 Index)										{ return m_pSprite->GetSize( Index);}

	void		SetTextureCoord( UINT32 Index, VEC2D * pMin, VEC2D * pMax);
	void		SetTextureCoord( UINT32 Index, REAL32 u1, REAL32 v1, REAL32 u2, REAL32 v2);
	void		SetTextureCoord2( UINT32 Index, REAL32 u1, REAL32 v1, REAL32 width, REAL32 height);
	VEC2D *	GetTextureCoord( UINT32 Index, UINT32 which)				{ return m_pSprite->GetTextureCoord( Index, which);}

	void		SetTextureFilter( I3G_TEXTURE_FILTER minFilter, I3G_TEXTURE_FILTER magFilter);

	void		SetColor( COLOR * pColor)								{ m_pSprite->SetColor( pColor ); }
	void		SetColor( UINT32 Index, COLOR * pColor)					{ m_pSprite->SetColor( Index, pColor);}
	void		SetColor( UINT32 Index, UINT8 r,UINT8 g,UINT8 b,UINT8 a){ m_pSprite->SetColor( Index, r,g,b,a);}
	COLOR *		GetColor( UINT32 Index)									{ return m_pSprite->GetColor( Index);}

	void		Set( INT32 idx, INT32 x, INT32 y, INT32 w, INT32 h, INT32 u, INT32 v);

	void		SetSrcBlend( I3G_BLEND blend);
	I3G_BLEND	GetSrcBlend(void)								{ return m_SrcBlend; }

	void		SetDestBlend( I3G_BLEND blend);
	I3G_BLEND	GetDestBlend(void)								{ return m_DestBlend; }

	I3_SPRITE2D_COORD_TYPE			getCoordType(void)			{ return m_pSprite->getCoordType(); }
	void							setCoordType( I3_SPRITE2D_COORD_TYPE type)
	{
		m_pSprite->setCoordType( type);
	}

	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;

	virtual void	OnInitAfterLoad() override;

#if defined (I3_DEBUG)
	void	DumpVB(void)												{ m_pSprite->DumpVB();	}
#endif
};

#endif
