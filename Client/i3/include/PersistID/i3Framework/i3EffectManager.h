#ifndef __I3_EFFECT_MANAGER_H
#define __I3_EFFECT_MANAGER_H

#include "i3GameObj.h"
#include "i3EffectTypeInfo.h"
#include "i3SgLayer.h"
#include "i3Object.h"

class i3Framework;
class i3ActionEmitEffect;

class I3_EXPORT_FRAMEWORK i3EffectManager :	public i3GameObj
{
	I3_CLASS_DEFINE( i3EffectManager );

protected:
	i3BinList			m_EffectList;

	bool				m_bForceEmit;
	REAL32				m_timeLocal;

	i3Framework *		m_pFramework;
	
	i3ParticleRender *	m_pRenderer;

	i3List				m_EmitterList;

	//UINT32				m_nContactReportCount;
	//UINT32				m_nContactReportReadPos;
	//i3PhysixShapeSet *	m_ppContactShapeSet[ 64 ];
	//i3Decal *			m_ppContactDecal[ 64 ];

protected:
	i3TimeStamp		*	_FindFreeEffectByType( INT32 nType);

	//void				_QueContactReport( i3PhysixShapeSet * pShapeSet, i3Decal * pDecal );
	//void				_ParseContactReport(void);
public:
	i3EffectManager(void);
	virtual ~i3EffectManager(void);

	void			BindFramework( i3Framework * pFramework)	{ m_pFramework = pFramework; }
	void			AttachToLayer( i3SgLayer * pSgLayer);

	void			AddEffectType( i3EffectTypeInfo * pInfo);
	void			RemoveEffectType( i3EffectTypeInfo * pInfo);
	void			RemoveAllEffect(void);

	void			RemoveAllEmitter(void);

	BOOL			Create( void);
	BOOL			Load( const char * pszIni, bool bCommon);

	INT32			FindEffect( const char * pszName);
	INT32			FindEffect( UINT32 hash);

	INT32			getEffectTypeCount(void)				{ return m_EffectList.GetCount(); }
	i3EffectTypeInfo *	getEffectType( INT32 idx)			{ return (i3EffectTypeInfo *) m_EffectList.Items[idx]; }

	// 
	i3TimeStamp *	EmitEffect( INT32 nType, MATRIX * pEmitTx, MATRIX * pBoneMatrix, MATRIX * pLocalOffset, VEC3D * pSrcPos);
	i3TimeStamp *	AddEffect( INT32 nType, VEC3D * pPos, VEC3D * pNormal = NULL, MATRIX * pMatrix = NULL, REAL32 fDeg = 0.f, i3GameObjBase * pObj = NULL);	// 3
	i3TimeStamp *	AddEffect( INT32 nType, VEC3D * pPos, VEC3D * pNormal, REAL32 fDeg);
	i3TimeStamp *	AddEffect( i3ActionEmitEffect * pAction, MATRIX * pMatrix);
	i3TimeStamp *	AddEffect( INT32 nType, VEC3D * pPos, VEC3D * pNormal, REAL32 fDeg, VEC2D * pUVs, INT32 nCount, UINT16 * pIndices = NULL, INT32 nIndexCount = 0 );
	
	virtual void	OnUpdate( RT_REAL32 rDeltaSeconds);

	virtual	void	Reset();

	virtual void	OnCreate(void);

	void			RenderEffect( i3SceneTracer * pTracer)					{ m_pRenderer->Render( pTracer); }

	void			RemoveAllNonCommonEffects(void);

#if defined( I3_DEBUG)
	void			Dump(void);
#endif
};

#endif //ifndef __I3_EFFECT_MANAGER_H