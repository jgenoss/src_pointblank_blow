#if !defined( __I3_TRANSFORM_H)
#define __I3_TRANSFORM_H

#include "i3Base.h"
#include "i3Math.h"
#include "i3Node.h"
#include "i3ModelViewMatrixAttr.h"

class i3Transform;

class I3_EXPORT_SCENE i3Transform : public i3Node
{
	I3SG_NODE_CLASS_DEFINE( i3Transform, i3Node);

	friend class i3SceneObject;
	friend class i3TransformRef;

protected:
	// Persistant members
	i3MatrixObject *		m_pMatrixObject;			// [initialize at constructor]
	i3TransformSourceCombiner *		m_pTransCombiner = nullptr;

	// volatile members
	MATRIX					m_CacheMatrix;
	UINT32					m_UpdateFrameID = 0xFFFFFFFF;

protected:
	virtual void		_GetWrappingBound( I3_GETBOUND_INFO * pInfo) override;
	virtual void	CalcOccMeshSet(i3::occ_mesh_set* occMeshSet, i3MatrixStack* matStack, bool twoSideEnabled) override;

	//////////////////////////////////////
	// Internal Use Only
	UINT32			GetUpdateFrameID(void)				{ return m_UpdateFrameID; }
	void			SetUpdateFrameID( UINT32 id)		{ m_UpdateFrameID = id; }

public:
	i3Transform(void);
	virtual ~i3Transform(void);
	
	void			SetMatrixObject( i3MatrixObject * pObj);
	i3MatrixObject *GetMatrixObject(void)								{ return m_pMatrixObject; }

	void			SetMatrix( MATRIX * pMatrix);
	MATRIX *		GetMatrix(void)							{ return m_pMatrixObject->GetMatrix(); }

	void			SetPos( const VEC3D & pos);

	MATRIX *		GetCacheMatrix(void)					{ return &m_CacheMatrix;}

	// 자신을 포함한 모든 Parent들의 모든 Transform을 Concaternation해,
	// 최신 Transform을 구한다.
	// 
	virtual void	CalcUpdatedMatrix( MATRIX * pOut, i3SceneTracer * pTracer);

	bool			IsAnimated(void)									
	{ 
		return (m_pTransCombiner != nullptr) && (m_pTransCombiner->IsEmpty() == false); 
	}

	void			GetAnimatedMatrix( I3MATRIXINFO * pEnv);
	void			SetAnimation( i3Animation * pAnim, INT32 idxBone,
							REAL32 rStartTime = 0.0f,
							REAL32 Interval = 0.0f,
							REAL32 timeScale = 1.0f,
							REAL32 rManualClampTime = 1.0f,
							I3_ANIM_BLEND_MODE BlendMode = I3_ANIM_BLEND_MODE_REPLACE,
							I3_ANIM_PLAY_MODE destPlayMode = I3_ANIM_PLAY_MODE_REPEAT,
							I3_ANIM_PLAY_MODE srcPlayMode = I3_ANIM_PLAY_MODE_STOP
							);

	REAL32			getPlayTime(void)
	{
		if( m_pTransCombiner == nullptr)
			return 0.0f;

		return m_pTransCombiner->getAnimPlayTime();
	}
	
	void			SetPlayCount(UINT32 count);
	void			SetPlayTime( REAL32 tm);
	void			AddPlayTime( REAL32 tm);
	void			SetPlayMode( I3_ANIM_PLAY_MODE mode)				{ m_pTransCombiner->SetPlayMode( mode); }
	REAL32			GetDuration(void)
	{
		if( m_pTransCombiner != nullptr)
			return m_pTransCombiner->GetDuration();

		return 0.0f;
	}

	void			RemoveAllAnimation(void)	
	{
		m_pTransCombiner->RemoveAllAnimation();
	}

	UINT32			getPlayCount(void)									
	{ 
		if( m_pTransCombiner == nullptr)
			return 0;

		return m_pTransCombiner->getPlayCount(); 
	}

	INT32			getAnimationCount(void)							{ return m_pTransCombiner->getAnimationCount(); }
	i3Animation *	getAnimation( INT32 idx)						{ return m_pTransCombiner->getAnimation( idx); }
	INT32			getAnimationBoneIndex( INT32 idx)				{ return m_pTransCombiner->getAnimationBoneIndex( idx); }

	virtual void	OnUpdate( i3SceneTracer * pTracer) override;
	virtual void	PreTrace(i3SceneTracer*  pTracer) override;

	virtual void	OnChangeTime( REAL32 tm) override;
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;

#ifdef I3_CHECK_FLOAT_VALUE_FOR_PHYSICX
	bool			isValidMatrix(MATRIX * pMat);
#endif

	virtual void	OnInstancing( I3_ONINSTANCING_INFO * pInfo) override;
};

#endif
