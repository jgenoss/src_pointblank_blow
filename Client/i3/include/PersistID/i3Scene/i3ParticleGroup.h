#if !defined( __I3_PARTICLE_GROUP_H)
#define __I3_PARTICLE_GROUP_H

#include "i3GeometryAttr.h"
#include "i3LightingEnableAttr.h"
#include "i3TextureBindAttr.h"
#include "i3BlendEnableAttr.h"
#include "i3BlendModeAttr.h"
#include "i3TextureEnableAttr.h"
#include "i3ZWriteEnableAttr.h"
#include "i3NormalMapEnableAttr.h"
#include "i3NormalMapBindAttr.h"
#include "i3ZBiasAttr.h"
#include "i3Particle.h"

#include "i3SceneTracer.h"

class i3Particle;
class i3ParticleRender;

class I3_EXPORT_SCENE i3ParticleGroup : public i3ElementBase
{
	I3_CLASS_DEFINE( i3ParticleGroup);
public:

protected:
	UINT32						m_Style;
	i3ParticleInfo *			m_pInfo;
	i3Particle *				m_pOrgParticle;

	INT32						m_Priority;

	i3GeometryAttr *			m_pGeoAttr;
	i3IndexArray *				m_pIA;
	i3VertexArray *				m_pVA;
	INT32						m_PrimCount;
	INT32						m_MaxCount;
	i3VertexFormat				m_VtxFmt;
	
	bool						m_bLocked;

	i3TextureBindAttr *			m_pTexBind;
	i3TextureEnableAttr *		m_pTexEnableAttr;
	i3NormalMapBindAttr *		m_pNormalMapBindAttr;
	i3NormalMapEnableAttr *		m_pNormalMapEnableAttr;
	i3BlendEnableAttr *			m_pBlendEnableAttr;
	i3BlendModeAttr *			m_pBlendModeAttr;
	i3ZWriteEnableAttr *		m_pZWriteEnableAttr;
	i3ZBiasAttr *				m_pZBiasAttr;
	i3LightingEnableAttr *		m_pLightingEnableAttr;

	MATRIX						m_WorldTx;
	MATRIX						m_invCamTx;
	
protected:
	virtual void		_Lock(void);
	virtual void		_Unlock(void);

	virtual INT32		GetTriangleCount(void);

public:
	i3ParticleGroup(void);
	virtual ~i3ParticleGroup(void);

	bool				IsSame( i3Particle * pParticle);

	INT32				getPriority(void)					{ return m_Priority; }
	void				setPriority( INT32 prio)			{ m_Priority = prio; }

	virtual bool		Create( i3Particle * pParticle, INT32 maxCount);
	virtual void		SetParticle( i3Particle * pParticle, bool bRecreateGeom = false);
	void				Unbind( i3Particle * pParticle);

	INT32				getCapacity(void)					{ return m_MaxCount; }
	virtual bool		SetCapacity( INT32 Count, bool bForce = false);

	i3VertexFormat *	getVertexFormat(void)				{ return &m_VtxFmt; }
	void				setVertexFormat( i3VertexFormat * pVF)
	{
		m_VtxFmt = *pVF;
	}

	i3Particle *		getParticle(void)					{ return m_pOrgParticle; }

	i3ParticleInfo *	getParticleInfo(void)				{ return m_pInfo; }
	void				setParticleInfo( i3ParticleInfo * pInfo)
	{
		I3_REF_CHANGE( m_pInfo, pInfo);
	}

	bool				getTextureEnable(void)				{ return m_pTexEnableAttr->Get() == TRUE; }
	void				setTextureEnable( bool bStatus)		{ m_pTexEnableAttr->Set( bStatus == true); }

	i3Texture *			getTexture(void)					{ return m_pTexBind->GetTexture(); }
	void				setTexture( i3Texture * pTex)		{ m_pTexBind->SetTexture( pTex); }

	bool				getNormalMapEnable(void)			{ return m_pNormalMapEnableAttr->Get() == TRUE; }
	void				setNormalMapEnable( bool bFlag)		{ m_pNormalMapEnableAttr->Set( bFlag == true); }

	i3Texture *			getNormalMap(void)					{ return m_pNormalMapBindAttr->GetTexture(); }
	void				setNormalMap( i3Texture * pTex)		{ m_pNormalMapBindAttr->SetTexture( pTex); }

	bool				getBlendEnable(void)				{ return m_pBlendEnableAttr->Get() == TRUE; }
	void				setBlendEnable( bool bFlag)			{ m_pBlendEnableAttr->Set( bFlag == true); }

	I3G_BLEND			getSrcBlend(void)					{ return m_pBlendModeAttr->GetSource(); }
	void				setSrcBlend( I3G_BLEND mode)		{ m_pBlendModeAttr->SetSource( mode); }

	I3G_BLEND			getDestBlend(void)					{ return m_pBlendModeAttr->GetDestination(); }
	void				setDestBlend( I3G_BLEND mode)		{ m_pBlendModeAttr->SetDestination( mode); }

	bool				getZWriteEnable(void)				{ return m_pZWriteEnableAttr->Get() == TRUE; }
	void				setZWriteEnable( bool bFlag)		{ m_pZWriteEnableAttr->Set( bFlag == true); }

	REAL32				getZBiasConst(void)					{ return m_pZBiasAttr->getBias(); }
	void				setZBiasConst( REAL32 val)			{ m_pZBiasAttr->setBias( val); }

	REAL32				getZBiasSlope(void)					{ return m_pZBiasAttr->getSlope(); }
	void				setZBiasSlope( REAL32 val)			{ m_pZBiasAttr->setSlope( val); }

	INT32				reserveQuad( INT32 cnt = 1)
	{
		if((m_PrimCount + cnt) > m_MaxCount)
		{
			return -1;
		}

		m_PrimCount += cnt;
		return m_PrimCount - cnt;
	}

	inline
	void				SetCameraSpaceTransform( MATRIX * pWorldMtx, MATRIX * pViewMtx)
	{
		i3Matrix::Copy( &m_WorldTx, pWorldMtx);
		i3Matrix::Mul( &m_invCamTx, pWorldMtx, pViewMtx);
		i3Matrix::Transpose3x3( &m_invCamTx, &m_invCamTx);
	}

	virtual INT32				AddForQuad( VEC3D * pCenter, VEC3D * pSize);
	virtual INT32				AddForSprite( VEC3D * pCenter, VEC3D * pSize, REAL32 rot, bool bFace2Cam);
	virtual INT32				AddForBillboard( VEC3D * pStart, VEC3D * pEnd, VEC3D * pSize);
	virtual void				SetColor( INT32 idx, UINT32 col);
	virtual void				SetTextureCoord( INT32 idx, VEC2D * p1, VEC2D * p2);
	virtual void				SetTextureCoordForBillboard( INT32 idx, VEC2D * p1, VEC2D * p2);

	void				Render( i3SceneTracer * pTracer);
};

#endif
