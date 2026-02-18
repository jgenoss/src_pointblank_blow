#ifndef __I3_EFFECT_MANAGER_H
#define __I3_EFFECT_MANAGER_H

#include "i3GameObj.h"
#include "i3EffectTypeInfo.h"
#include "i3SgLayer.h"
#include "i3Object.h"


class i3Framework;
class i3ActionEmitEffect;


bool	operator<(const i3EffectTypeInfo& a1, const i3EffectTypeInfo& a2);
template<> struct i3::less<i3EffectTypeInfo*> : i3::less_ptr<i3EffectTypeInfo*> {};

class I3_EXPORT_FRAMEWORK i3EffectManager :	public i3GameObj
{
	I3_EXPORT_CLASS_DEFINE( i3EffectManager, i3GameObj );

protected:
	typedef void	(* EffectCancelProc)( i3Node * pNode);

	i3::vector_set<i3EffectTypeInfo*>	m_EffectList;

	bool				m_bForceEmit = false;
	REAL32				m_timeLocal = 0.0f;

	i3Framework *		m_pFramework = nullptr;
	
	i3ParticleRender *	m_pRenderer;				// [initialize at constructor]

	i3::vector<i3ActionEmitEffect*>	m_EmitterList;

	EffectCancelProc	m_pCancelEffectProc = nullptr;

public:
	i3EffectManager(void);
	virtual ~i3EffectManager(void);

	void			BindFramework( i3Framework * pFramework)	{ m_pFramework = pFramework; }
	void			AttachToLayer( i3SgLayer * pSgLayer);

	void			AddEffectType( i3EffectTypeInfo * pInfo);
	void			RemoveEffectType( i3EffectTypeInfo * pInfo);
	void			RemoveAllEffect(void);

	void			RemoveAllEmitter(void);

	bool			Create( void);
	bool			Load( const char * pszIni, bool bCommon);

	INT32			FindEffect( const char * pszName);
	INT32			FindEffect( UINT32 hash);

	INT32			getEffectTypeCount(void)				{ return (INT32)m_EffectList.size(); }
	i3EffectTypeInfo *	getEffectType( INT32 idx)			{ return m_EffectList.get_vector()[idx]; }

	// 
	i3TimeStamp *	EmitEffect( INT32 nType, MATRIX * pEmitTx, MATRIX * pBoneMatrix, MATRIX * pLocalOffset, VEC3D * pSrcPos);
	i3TimeStamp *	AddEffect( INT32 nType, VEC3D * pPos, VEC3D * pNormal = nullptr, MATRIX * pMatrix = nullptr, REAL32 fDeg = 0.f, i3GameObjBase * pObj = nullptr);	// 3
	i3TimeStamp *	AddEffect( INT32 nType, VEC3D * pPos, VEC3D * pNormal, REAL32 fDeg);
	i3TimeStamp *	AddEffect( i3ActionEmitEffect * pAction, MATRIX * pMatrix);
	i3TimeStamp *	AddEffect( INT32 nType, VEC3D * pPos, VEC3D * pNormal, REAL32 fDeg, VEC2D * pUVs, INT32 nCount, UINT16 * pIndices = nullptr, INT32 nIndexCount = 0 );

	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;
	virtual	void	Reset();
	virtual void	OnCreate(void);

	virtual bool	OnLostDevice( bool bLostDevice) override;
	virtual bool	OnRevive( i3RenderContext * pCtx) override;

	void			AddWorldRenderAttrToEffect(i3RenderAttr* worldRenderAttr);
	void			RemoveWorldRenderAttrsFromEffect();

	void			RenderEffect( i3SceneTracer * pTracer)					{ m_pRenderer->Render( pTracer); }
	void			ClearEffect()											{ m_pRenderer->ClearParticles(); }
	void			PostTraceEffect()										{ m_pRenderer->PostTrace(); }
	void			PostRenderEffect()										{ m_pRenderer->PostRender();  }
	void			RemoveAllNonCommonEffects(void);

	void			SetEffectCancelProc( EffectCancelProc pProc)				{ m_pCancelEffectProc = pProc; }

	/** \brief stage 진입시 호출
		\note decal용 PhysX Shape을 생성한다.
		*/
	void			EnterStage( void);
	void			LeaveStage( void);

#if defined( I3_DEBUG)
	void			Dump(void);
#endif
};

#endif //ifndef __I3_EFFECT_MANAGER_H