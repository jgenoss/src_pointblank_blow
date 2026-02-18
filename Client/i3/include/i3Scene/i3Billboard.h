#if !defined( __I3_BILLBOARD_H)
#define __I3_BILLBOARD_H

#include "i3Gfx.h"
#include "i3AttrSet.h"
#include "i3TextureEnableAttr.h"
#include "i3TextureBindAttr.h"
#include "i3BlendEnableAttr.h"
#include "i3BlendModeAttr.h"
#include "i3TextureWrapAttr.h"
#include "i3BillboardAttr.h"
#include "i3AlphaTestEnableAttr.h"
#include "i3Geometry.h"

//
// i3BillboardAttr을 손쉽게 쓰기위해서 SceneGraph를 미리 구성하여둔다.
// 
//		i3Billboard
//			|
//			+- i3BlendModeAttr
//			|
//			+- i3TextureBindAttr
//			|
//			+- i3Geometry
//				|
//				+- i3BillboardAttr
//		


class I3_EXPORT_SCENE i3Billboard : public i3AttrSet
{
	I3_EXPORT_CLASS_DEFINE( i3Billboard, i3AttrSet);
protected:

	INT32					m_Count = 1;
	I3G_BLEND				m_SrcBlend = I3G_BLEND_SRCALPHA;
	I3G_BLEND				m_DestBlend = I3G_BLEND_INVSRCALPHA;

	i3BillboardAttr*		m_pBillboard = nullptr;
	i3Texture*				m_pTexture = nullptr;
	i3Geometry*				m_pGeomery;		// [initialize at constructor]
	i3TextureEnableAttr *	m_pTexEnableAttr;	// [initialize at constructor]
	i3TextureBindAttr*		m_pTextureBindAttr;	// [initialize at constructor]
	i3BlendEnableAttr *		m_pBlendEnableAttr;	// [initialize at constructor]
	i3BlendModeAttr*		m_pBlendModeAttr;	// [initialize at constructor]
	i3AlphaTestEnableAttr *	m_pAlphaTestAttr;	// [initialize at constructor]

public:
	i3Billboard(void);
	virtual ~i3Billboard(void);

	void		SetCount(INT32 count);
	void		SetTexture(i3Texture * pTex);
	void		SetTexture(const char * TextureName);

	void		SetEnable( UINT32 Index, bool bFlag)					{ m_pBillboard->SetEnable( Index, bFlag);}
	bool		GetEnable( UINT32 Index)								{ return m_pBillboard->GetEnable( Index);}

	void		SetStart( UINT32 Index, VEC3D * pVec)					{ m_pBillboard->SetStart( Index, pVec);}
	void		SetStart( UINT32 Index, REAL32 x, REAL32 y, REAL32 z)	{ m_pBillboard->SetStart( Index, x, y, z);}
	VEC3D *	GetStart( UINT32 Index)									{ return m_pBillboard->GetStart( Index);}

	void		SetEnd( UINT32 Index, VEC3D * pVec)						{ m_pBillboard->SetEnd( Index, pVec);}
	void		SetEnd( UINT32 Index, REAL32 x, REAL32 y, REAL32 z)		{ m_pBillboard->SetEnd( Index, x, y, z);}
	VEC3D *	GetEnd( UINT32 Index)									{ return m_pBillboard->GetEnd( Index);}

	void		SetSize( UINT32 Index, REAL32 Height)					{ m_pBillboard->SetSize( Index, Height);}
	REAL32	GetSize( UINT32 Index)									{ return m_pBillboard->GetSize( Index);}

	void		SetTextureCoord( UINT32 Index, VEC2D * pMin, VEC2D * pMax)	{ m_pBillboard->SetTextureCoord( Index, pMin, pMax);}
	void		SetTextureCoord( UINT32 Index, REAL32 u1, REAL32 v1, REAL32 u2, REAL32 v2)	{ m_pBillboard->SetTextureCoord( Index, u1, v1, u2, v2);}
	VEC2D *	GetTextureCoord( UINT32 Index, UINT32 which)			{ return m_pBillboard->GetTextureCoord( Index, which);}

	void		SetColor( UINT32 Index, COLOR * pColor)					{ m_pBillboard->SetColor( Index, pColor);}
	void		SetColor( UINT32 Index, UINT8 r,UINT8 g,UINT8 b,UINT8 a){ m_pBillboard->SetColor( Index, r,g,b,a);}
	COLOR *		GetColor( UINT32 Index)									{ return m_pBillboard->GetColor( Index);}

	void		SetSrcBlend( I3G_BLEND blend);
	I3G_BLEND	GetSrcBlend(void)								{ return m_SrcBlend; }

	void		SetDestBlend( I3G_BLEND blend);
	I3G_BLEND	GetDestBlend(void)								{ return m_DestBlend; }

	bool		getRotateUVState(void)							{ return m_pBillboard->getRotateUVState(); }
	void		setRotateUVState( bool bState)					{ m_pBillboard->setRotateUVState( bState); }

	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
};

#endif
