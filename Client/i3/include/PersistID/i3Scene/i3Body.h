#if !defined( __I3_LODSET_H)
#define __I3_LODSET_H

#include "i3Skeleton.h"
#include "i3Shape.h"
#include "i3LOD.h"
#include "i3Animation.h"
#include "i3AnimationContext.h"

#define		I3SG_LOD_AUTO				0x00000001
#define		I3SG_MANUAL_ANIM_UPDATE		0x00000002
#define		I3SG_MANUAL_TRANSFORM		0x00000004

class I3_EXPORT_SCENE i3Body : public i3Node
{
	I3SG_NODE_CLASS_DEFINE( i3Body);
protected:
	i3List			m_LODList;
	UINT32			m_Style;
	
	REAL32			m_Slope;
	REAL32			m_Const;

	INT32			m_CurLevel;

	i3Animation *	m_pCurAnim;

	// Volatile Members
	i3LOD *				m_pBoneFrameLOD;
	i3AnimationContext *	m_pAnimCtx;

protected:
	virtual void		_GetWrappingBound( I3_GETBOUND_INFO * pInfo);

	void				_PrepareAnimContext(void);

public:
	i3Body(void);
	virtual ~i3Body(void);

	UINT32				getStyle(void)						{ return m_Style; }
	void				setStyle( UINT32 style)				{ m_Style = style; }
	void				addStyle( UINT32 style)				{ m_Style |= style; }
	void				removeStyle( UINT32 style)			{ m_Style &= ~style; }

	void				AddLOD( i3LOD * pLOD);
	void				RemoveLOD( i3LOD * pSkel);
	void				RemoveAllLOD(void);
	UINT32				getLODCount(void)					{ return (UINT32) m_LODList.GetCount(); }
	i3LOD *				getLOD( UINT32 idx)					{ return (i3LOD *) m_LODList.Items[idx]; }

	void				setBoneTimeScale( INT32 idxBone, REAL32 scale)
	{
		getCombiner( idxBone)->SetTimeScale( scale);
	}

	void				Prepare(void)						{ _PrepareAnimContext(); }

	// LOD Level = Slope * (d^2) + Const
	REAL32				getSlope(void)									{ return m_Slope; }
	void				setSlope( REAL32 slope)							{ m_Slope = slope; }

	REAL32				getConst(void)									{ return m_Const; }
	void				setConst( REAL32 cst)							{ m_Const = cst; }

	INT32				getCurrentLevel(void)							{ return m_CurLevel; }
	void				setCurrentLevel( INT32 l)						{ m_CurLevel = l; }
	void				CalcLOD( i3SceneTracer * pTracer);
	void				CalcLOD( VEC3D * pPos, VEC3D * pCamPos, REAL32 fFOVRatio = 1.0f);
	i3LOD *				getCurrentLOD(void)								{ return (i3LOD *) m_LODList.Items[ m_CurLevel]; }

	MATRIX		*		getCacheMatrix( INT32 nIdx)						
	{ 
		if( m_pAnimCtx == NULL)
			return NULL;

		return m_pAnimCtx->getCacheMatrix( nIdx);
	}

	void				CalcWorldMatrix( INT32 nBoneIndex, MATRIX * pMtx);

	INT32				FindBoneByName( const char * pszName)			
	{ 
		if( m_pBoneFrameLOD == NULL)
			return -1;

		return m_pBoneFrameLOD->getSkeleton()->FindBoneByName( (char *) pszName); 
	}

	INT32				getParentBoneIndex( INT32 idxBone)
	{
		return m_pBoneFrameLOD->getSkeleton()->getParentBoneIndex( idxBone);
	}

	i3Bone *			getBone( INT32 idx)
	{ 
		I3ASSERT( m_pBoneFrameLOD != NULL);
		return m_pBoneFrameLOD->getSkeleton()->getBone( idx); 
	}

	i3LOD *				getFrameLOD(void)								{ return m_pBoneFrameLOD; }

	void				PlayAnim( i3Animation * pAnim, 
								RT_REAL32 rStartTime = __RT_0,
								RT_REAL32 interval = __RT_0,
								RT_REAL32 timeScale = __RT_1,
								I3_ANIM_BLEND_MODE blendMode = I3_ANIM_BLEND_MODE_REPLACE, 
								I3_ANIM_PLAY_MODE destPlayMode = I3_ANIM_PLAY_MODE_REPEAT, 
								I3_ANIM_PLAY_MODE srcPlayMode = I3_ANIM_PLAY_MODE_STOP );
	void				StopAnim(void);
	void				SetPlayAnim( RT_REAL32 tm);
	RT_REAL32			GetDuration(void);
	RT_REAL32			GetActualDuration( void);
	i3Animation *		GetCurrentAnim(void)							{ return m_pCurAnim; }
	i3AnimationContext *getAnimationContext(void)						{ return m_pAnimCtx; }
	i3TransformSourceCombiner *	getCombiner( INT32 idx)					{ return m_pAnimCtx->getCombiner( idx); }

	UINT32				getAnimPlayCount( INT32 idx)					{ return m_pAnimCtx->getCombiner( idx)->getPlayCount(); }

	REAL32				getPlayTime( INT32 idxBone)						{ return m_pAnimCtx->getCombiner( idxBone)->getAnimPlayTime(); }

	void				Traverse( i3SceneTracer * pTracer);
	void				UpdateAnim( i3SceneTracer * pTracer);

	virtual void		OnUpdate( i3SceneTracer * pTracer);			// Manual Update인 경우에만 호출하면 된다.
	
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	OnChangeTime( REAL32 tm);
	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
	virtual void	OnInstancing( void * pUserDefault = NULL);
};

#endif
