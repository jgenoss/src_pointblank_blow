#if !defined( __I3_PARTICLE_RENDER_H)
#define __I3_PARTICLE_RENDER_H

#include "i3ParticleGroup.h"
#include "i3SceneTracer.h"
#include "i3TimeSequence.h"

class i3DecalManager;
struct I3SG_DECAL_HITBOX;


struct i3ParticleSubsetForSort;
struct i3AlphaSortSubset;

class I3_EXPORT_SCENE i3ParticleRender : public i3EventReceiver
{
	I3_EXPORT_CLASS_DEFINE( i3ParticleRender, i3EventReceiver);
protected:
	i3::vector_multiset<i3ParticleGroup*>	m_GroupList;

	i3ModelViewMatrixAttr *		m_pWorldMtxAttr;		// [initialize at constructor]
	i3FogEnableAttr*			m_pFogEnableAttr;		// [initialize at constructor]
	i3DecalManager *			m_pDecalManager;		// [initialize at constructor]

#if defined( I3_DEBUG)
	i3AttrSet *					m_pHitBoxAttrSet = nullptr;
#endif

	i3::vector<I3PARTICLEOBJECT*>	m_currAccObjectList;		// 렌더링될 리스트의 집합..
	i3::vector<I3PARTICLEOBJECT*>	m_currAccObjectList_AddBlend;	// 가산 블랜딩은 따로 처리..

																// 노드가 다 수집될때까지는 모으는데 집중..
																// 일단 다 모으고 난뒤, 소팅때리고 렌더링쪽으로 돌림...

	i3::vector<i3ParticleTransformMatrix*>		m_ParticleMatrixListForDelete;	// 나중에 계산이 끝나면 일괄삭제해야한다..
																				// 일괄 삭제방식을 쓰면 레퍼런스 카운팅은 필요가 없음..
	
	i3::vector<i3ParticleSubsetForSort*>	m_ParticleSubsetPool;		// 일괄삭제방식을 쓰도록 한다. (스택처럼 운용)	
	i3::vector<i3ParticleSubsetForSort*>	m_ParticleSubsetListForDelete;	// 삭제를 위해서만 쓴다..

	i3::vector<i3AlphaSortSubset*>			m_AlphaSortSubsetPool;
	i3::vector<i3AlphaSortSubset*>			m_AlphaSortSubsetList;

	i3::vector<i3RenderAttr*>				m_vecWorldRenderAttrForEffect;

public:
	i3ParticleRender(void);
	virtual ~i3ParticleRender(void);

	bool				Create(void);

	void				CreatePhysXShapes( void);
	void				ReleasePhysXShapes( void);

	void				RemoveAllGroups(void);
	void				RemoveGroup( i3ParticleGroup * pGroup);

	bool				AddParticleScene( i3TimeSequence * pNode, INT32 maxCount, INT32 priority = 50, bool bDecal = false);
	i3ParticleGroup *	AddParticle( i3Particle * pParticle, INT32 maxCount);
	i3ParticleGroup *	FindParticleGroup( i3Particle * pParticle);

	I3SG_DECAL_HITBOX * EmitDecal( i3SceneTracer * pTracer, i3Particle * pParticle, I3PARTICLEOBJECT * pObj);

	void				FreeDecal( i3Decal * pDecal);

	void				FreeDrawDecal( i3DecalDrawInfo * pInfo);

	i3ParticleTransformMatrix*	 AllocParticleTransformMatrix(MATRIX * pWorldMtx, MATRIX * pViewMtx);
	void						AddParticleObject(I3PARTICLEOBJECT* obj);
	
	void				Render( i3SceneTracer * pTracer);
	void				ClearParticles();
	void				PostTrace();
	void				PostRender();

	void				ArrangeRenderOrder(void);

	void			AddWorldRenderAttrToEffect(i3RenderAttr* worldRenderAttr);
	void			RemoveWorldRenderAttrsFromEffect();
	void			PushAttr_FogDisable(i3AttrStackManager* pStackManager );
	void			PopAttr_FogDisable(i3AttrStackManager* pStackManager);




#if defined( I3_DEBUG)
	void				Dump(void);
#endif

	virtual bool		OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE) override;

	virtual void		OnLostDevice( bool bLostDevice);
	virtual void		OnRevive( i3RenderContext * pCtx);

};

extern I3_EXPORT_SCENE i3ParticleRender *		g_pParticleRender;

#endif
