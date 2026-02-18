#if !defined( __I3_LODSET_H)
#define __I3_LODSET_H

#include "i3Skeleton.h"
#include "i3Shape.h"
#include "i3LOD.h"
#include "i3AttrSet.h"
#include "i3Transform.h"

typedef UINT32 I3_BODY_STYLE;
#define		I3BS_LOD_AUTO				0x00000001
#define		I3BS_MANUAL_ANIM_UPDATE		0x00000002
#define		I3BS_MANUAL_TRANSFORM		0x00000004
#define		I3BS_HUMAN					0x00000008

/** \brief
	\desc leaf class
	*/

class I3_EXPORT_SCENE i3Body : public i3Node
{
	I3SG_NODE_CLASS_DEFINE( i3Body, i3Node);
protected:
	// Persistant members
	i3::vector<i3LOD*>	m_LODList;
	I3_BODY_STYLE	m_Style = I3BS_LOD_AUTO;
	
	REAL32			m_Slope = 1.0f;
	REAL32			m_Const = 0.0f;

	i3Animation *	m_pCurAnim = nullptr;

	// Volatile Members
	INT32			m_CurLevel = 0;

	i3LOD *				m_pBoneFrameLOD = nullptr;
	i3AnimationContext *	m_pAnimCtx = nullptr;

protected:
	virtual void		_GetWrappingBound( I3_GETBOUND_INFO * pInfo);

	void				_PrepareAnimContext( bool bRecreateAnimCtx = true);
	i3TransformSourceCombiner *	getCombiner( INT32 idx)					{ return m_pAnimCtx->getCombiner( idx); }

public:
	i3Body(void);
	virtual ~i3Body(void);

	I3_BODY_STYLE		getStyle(void)						{ return m_Style; }
	void				setStyle( I3_BODY_STYLE style)		{ m_Style = style; }
	void				addStyle( I3_BODY_STYLE style)		{ m_Style |= style; }
	void				removeStyle( I3_BODY_STYLE style)	{ m_Style &= ~style; }

	bool				isHumanType( void)					{ return (bool) ((m_Style & I3BS_HUMAN) == I3BS_HUMAN); }

	void				AddLOD( i3LOD * pLOD);
	void				RemoveLOD( i3LOD * pSkel);
	void				RemoveAllLOD(void);
	UINT32				getLODCount(void)					{ return (UINT32) m_LODList.size(); }
	i3LOD *				getLOD( UINT32 idx)					{ if( idx < getLODCount())	return m_LODList[idx];	else return nullptr; }

	void				setBoneTimeScale( INT32 idxBone, REAL32 scale)
	{
		getCombiner( idxBone)->SetTimeScale( scale);
	}

	void				Prepare( bool bRecreateAnimCtx = true)			{ _PrepareAnimContext( bRecreateAnimCtx); }

	// LOD Level = Slope * (d^2) + Const
	REAL32				getSlope(void)									{ return m_Slope; }
	void				setSlope( REAL32 slope)							{ m_Slope = slope; }

	REAL32				getConst(void)									{ return m_Const; }
	void				setConst( REAL32 cst)							
	{
		m_Const = cst; 
	}

	INT32				getCurrentLevel(void)							{ return m_CurLevel; }
	void				setCurrentLevel( INT32 l)						
	{
		if( l >= (INT32)m_LODList.size() && m_LODList.size() > 0)
			m_CurLevel = m_LODList.size()-1;
		else
			m_CurLevel = l; 
	}
	void				CalcLOD( i3SceneTracer * pTracer);
	void				CalcLOD( VEC3D * pPos, VEC3D * pCamPos, REAL32 fFOVRatio = 1.0f);
	i3LOD *				getCurrentLOD(void)
	{
		I3_BOUNDCHK( m_CurLevel, (INT32)m_LODList.size());
		return m_LODList[ m_CurLevel];
	}

	MATRIX		*		getCacheMatrix( INT32 nIdx)						
	{ 
		if( m_pAnimCtx == nullptr)
			return nullptr;

		return m_pAnimCtx->getCacheMatrix( nIdx);
	}

	void				CalcWorldMatrix( INT32 nBoneIndex, MATRIX * pMtx);

	INT32				FindBoneByName( const char * pszName)			
	{ 
		if( (m_pBoneFrameLOD != nullptr) &&
			(m_pBoneFrameLOD->getSkeleton() != nullptr) )
			return m_pBoneFrameLOD->getSkeleton()->FindBoneByName( pszName); 

		return -1;
	}

	INT32				getParentBoneIndex( INT32 idxBone)
	{
		I3ASSERT_RETURN( m_pBoneFrameLOD && m_pBoneFrameLOD->getSkeleton(), -1);
		return m_pBoneFrameLOD->getSkeleton()->getParentBoneIndex( idxBone);
	}

	i3Bone *			getBone( INT32 idx)
	{ 
		I3ASSERT_RETURN(m_pBoneFrameLOD && m_pBoneFrameLOD->getSkeleton(), nullptr);
		return m_pBoneFrameLOD->getSkeleton()->getBone( idx); 
	}

	i3LOD *				getFrameLOD(void)								{ return m_pBoneFrameLOD; }
	void				setFrameLOD( i3LOD * pLOD)						{ m_pBoneFrameLOD = pLOD; }

	void				PlayAnim( i3Animation * pAnim, 
								REAL32 rStartTime = 0.0f,
								REAL32 interval = 0.0f,
								REAL32 timeScale = 1.0f,
								REAL32 rManualClampTime = 1.0f,
								I3_ANIM_BLEND_MODE blendMode = I3_ANIM_BLEND_MODE_REPLACE, 
								I3_ANIM_PLAY_MODE destPlayMode = I3_ANIM_PLAY_MODE_REPEAT, 
								I3_ANIM_PLAY_MODE srcPlayMode = I3_ANIM_PLAY_MODE_STOP );
	void				StopAnim(void);
	void				SetPlayAnim( REAL32 tm);
	REAL32			GetDuration(void);
	REAL32			GetActualDuration( void);
	i3Animation *		GetCurrentAnim(void)							{ return m_pCurAnim; }

	i3AnimationContext *getAnimationContext(void)						{ return m_pAnimCtx; }
	void				setAnimationContext( i3AnimationContext * pCtx)	{ I3_REF_CHANGE( m_pAnimCtx, pCtx); }

	UINT32				getAnimPlayCount( INT32 idx)					{ return m_pAnimCtx->getCombiner( idx)->getPlayCount(); }

	REAL32				getPlayTime( INT32 idxBone)						{ return m_pAnimCtx->getCombiner( idxBone)->getAnimPlayTime(); }

	void				Traverse( i3SceneTracer * pTracer);
	void				UpdateAnim( i3SceneTracer * pTracer);

	virtual bool		OnQueryInFrustum( i3SceneTracer * pTracer) override;

	virtual void		PreTrace(i3SceneTracer * pTracer) override;
	virtual void		OnUpdate( i3SceneTracer * pTracer) override;			// Manual Update인 경우에만 호출하면 된다.
	
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void	OnChangeTime( REAL32 tm) override;
	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual void	OnInstancing( I3_ONINSTANCING_INFO * pInfo) override;


	// Render Skeleton Bone
protected:
	i3AttrSet *	m_pRenderableBone = nullptr;
	INT32		m_iRenderableBoneCount = 0;
	bool		m_bEnableRenderBone = false;

	i3::vector<i3Node*> m_RenderableTransformList;

	void		_CreateRenderableTransform( i3Transform * pAttachNode);
	void		_FindAndCreateRenderableBone( i3Node * pRoot);
	void		CreateRenderableBoneRef( void);

	void		CreateRenderableBone( void);
	void		CreateRenderDiamond( i3Node * pAttachNode, VEC3D * pDir, bool bLeaf);
	void		CreateRenderAxis( i3Node * pAttachNode, INT32 AxisDiv = 20, REAL32 Length = 1.0f, REAL32 rad = 0.05f);

public:
	void		EnableRenderBone( bool bEnable);

	void		ToggleRenderBone( void)
	{
		m_bEnableRenderBone = !m_bEnableRenderBone;

		EnableRenderBone( m_bEnableRenderBone);
	}
};

#endif
